/*
    Copyright (c) 2024 Timur Uzakov

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

#include "../include/node/colors.h"
#include "../include/node/libraries.h"
#include "../include/node/functions.h"
#include "../include/node/kalman.h"

using namespace boost::asio;

std::mutex lock;
std::mutex lock_cur;
static std::atomic<bool> stop_threads;
static std::atomic<bool> prod, is_carpet;
unsigned char cur, prev, stored;
int speed = 0;
char c;

std::fstream ser_motors;

std::string port_sensors;
std::string port_motors;

// Configuration structure
struct Configuration
{
    int SPEED;
    bool PROD;
    std::string SENSORS_PORT;
    std::string MOTORS_PORT;
    int WIFI_PORT;
    bool IS_CARPET;
    // Add more parameters as needed
};
std::pair<int, int> key_to_speeds(char key, int speed)
{
    int speed_rot = 0;
    if (is_carpet)
    {
        speed_rot = speed + 50;
    }
    else
    {
        speed_rot = speed + 10;
    }
    std::map<char, std::pair<int, int>> switch_map = {
        {'w', {speed, speed}},
        {'s', {-speed, -speed}},
        {'q', {0, speed_rot}},
        {'e', {speed_rot, 0}},
        {'a', {0, -speed_rot}},
        {'d', {-speed_rot, 0}},
        {'r', {0, 0}}};
    return switch_map.count(key) ? switch_map.at(key) : std::make_pair(0, 0);
}
// Function to load configuration from a file
bool loadConfiguration(const std::string &filename, Configuration &config)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open configuration file\n";
        return false;
    }

    std::string line;
    int counter = 0;
    while (std::getline(file, line))
    {
        // Skip lines starting with '#' (comment lines)
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        // Use stringstream to parse non-comment lines
        std::istringstream iss(line);
        if (counter == 0)
        {
            iss >> config.SPEED;
        }
        if (counter == 1)
        {
            try
            {
                std::string value;
                iss >> value;
                config.PROD = stringToBool(value);
                prod = config.PROD;
            }
            catch (const std::invalid_argument &e)
            {
                std::cerr << e.what() << std::endl;
                return false;
            }
        }
        if (counter == 2)
        {
            iss >> config.SENSORS_PORT;
        }
        if (counter == 3)
        {
            iss >> config.MOTORS_PORT;
        }
        if (counter == 4)
        {
            iss >> config.WIFI_PORT;
        }
        if (counter == 5)
        {
            try
            {
                std::string value;
                iss >> value;
                config.IS_CARPET = stringToBool(value);
                is_carpet = config.IS_CARPET;
            }
            catch (const std::invalid_argument &e)
            {
                std::cerr << e.what() << std::endl;
                return false;
            }
        }
        // If you have more parameters, parse them here as well

        counter++;
    }

    file.close();
    return true;
}

void commander(const int &id, const std::string &name, const int &delay)
{
    init_function(id, name, lock);
    int stop = 0;
    while (!stop_threads)
    {
        if (prod)
        {
            if (c == '1')
            {
                std::cout << FRED("STOP") << std::endl;
                on_press_vel(0, 0, ser_motors);
            }
            else
            {
                std::pair<int, int> speeds = key_to_speeds(cur, speed);
                on_press_vel(speeds.first, speeds.second, ser_motors);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    send_closing_message(id, name, lock);
    return;
}

void input_thread(const int &id, const std::string &name, const int &speed, const int &speed_rot, const int &delay)
{
    init_function(id, name, lock);

    struct termios old_tio, new_tio;
    /* get the terminal settings for stdin */
    tcgetattr(STDIN_FILENO, &old_tio);
    /* we want to keep the old setting to restore them a the end */
    new_tio = old_tio;
    /* disable canonical mode (buffered i/o) and local echo */
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    /* set the new settings immediately */
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    int left_speed;
    int right_speed;
    size_t counter = 0;
    while (!stop_threads)
    {
        cur = getchar();
        if (cur == 'r')
        {
            counter++;
        }
        if (counter > 2)
        {
            on_press_vel(0, 0, ser_motors);
            ser_motors.close();
            stop_threads = true;
            break;
        }
        prev = stored;
        stored = cur;
        std::cout << cur << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    };

    /* restore the former settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
    stop_threads = true;
    send_closing_message(id, name, lock);
    return;
}
void wifi_thread(const int &id, const std::string &name, const int &port, const int &delay)
{
    init_function(id, name, lock);
    int sock = init_socket();
    struct sockaddr_in myAddr = init_address(port, nullptr);
    bind_socket(sock, myAddr);
    char *buffer = new char[BUFFER_SIZE];
    struct sockaddr_in srcAddr;
    socklen_t srcAddrLen = sizeof(srcAddr);
    while (!stop_threads)
    {
        msg_data msg = receive_message(sock, stop_threads, buffer, srcAddr, srcAddrLen);
        if (stop_threads == true)
            break;
        lock_cur.lock();
        cur = msg.buffer[0];
        speed = std::stoi(std::string(msg.buffer + 2, msg.numBytesReceived - 1));
        prev = stored;
        stored = cur;
        lock_cur.unlock();
        print_buffer(msg.buffer, msg.srcAddr, msg.numBytesReceived, cur, speed, lock);
        // clear buffer
        memset(buffer, 0, BUFFER_SIZE);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    send_closing_message(id, name, lock);
}

void sensor_thread(const int &id, const std::string &name, const int &delay)
{
    init_function(id, name, lock);

    try
    {
        while (!stop_threads)
        {
            io_service io;
            serial_port ser_sensors(io, port_sensors); // Adjust port name as per your setup
            // Set baud rate
            ser_sensors.set_option(serial_port_base::baud_rate(9600)); // Adjust baud rate as needed
            read(ser_sensors, buffer(&c, 1));
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            ser_sensors.close();
        }
    }
    catch (std::exception &e)
    {
        // Check if ser_sensors is open
        std::cout << FRED("Error: Could not open serial port: ") << FYEL(<< port_sensors <<) << FRED("") << std::endl;
    }
    stop_threads = true;
    send_closing_message(id, name, lock);
    return;
}
int main(int argc, char **argv)
{
    std::cout << FGRN("Concurrent controller program started") << std::endl;
    Configuration config;
    std::string projectFolderPath = std::string(PROJECT_FOLDER_PATH);
    std::cout << FGRN("Path: ") << FYEL(<< projectFolderPath <<) << std::endl;

    stop_threads = false;
    // Load configuration from file
    if (!loadConfiguration(projectFolderPath + "/config.txt", config))
        return 1;

    if (prod)
    {
        port_sensors = config.SENSORS_PORT;
        port_motors = config.MOTORS_PORT;
        ser_motors = std::fstream(port_motors);
        // Check if ser_motors is open
        if (!ser_motors.is_open())
        {
            std::cout << FRED("Error: Could not open serial port: ") << FYEL(<< port_motors <<) << FRED("") << std::endl;
        }
    }
    // std::thread ip1(input_thread, 1, "keyboard input", config.SPEED, config.SPEED_ROT, frequency_to_milliseconds(10));
    std::thread wf1(wifi_thread, 1, "wifi input", config.WIFI_PORT, frequency_to_milliseconds(100));
    std::thread cmd(commander, 2, "command thread", frequency_to_milliseconds(200));
    std::thread th1(sensor_thread, 3, "sensor thread", frequency_to_milliseconds(100));

    cur = 'k';
    prev = 'k';
    stored = 'k';

    wf1.join();
    cmd.join();
    th1.join();
    return 0;
}
