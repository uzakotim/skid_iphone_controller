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

#ifndef FUNCTIONSFUNCTION_H
#define FUNCTIONSFUNCTION_H
#include "libraries.h"

// BASE 64 -----------------------------------------------------------------------
typedef unsigned char BYTE;

std::string base64_encode(BYTE const *buf, unsigned int bufLen);
std::vector<BYTE> base64_decode(std::string const &);

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static inline bool is_base64(BYTE c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(BYTE const *buf, unsigned int bufLen)
{
    std::string ret;
    int i = 0;
    int j = 0;
    BYTE char_array_3[3];
    BYTE char_array_4[4];

    while (bufLen--)
    {
        char_array_3[i++] = *(buf++);
        if (i == 3)
        {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; (i < 4); i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while ((i++ < 3))
            ret += '=';
    }

    return ret;
}

std::vector<BYTE> base64_decode(std::string const &encoded_string)
{
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    BYTE char_array_4[4], char_array_3[3];
    std::vector<BYTE> ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
    {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == 4)
        {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret.push_back(char_array_3[i]);
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++)
            ret.push_back(char_array_3[j]);
    }

    return ret;
}
// [END] BASE 64 -----------------------------------------------------------------

std::vector<double> compute_averages(std::vector<std::vector<double>> array, int size)
{
    std::vector<double> averages = {0, 0, 0};
    std::vector<int> sign_sums = {0, 0, 0};
    for (int j = 0; j < 3; j++)
    {
        double sum = 0;
        for (int i = 0; i < size; i++)
        {
            if ((array[i][j]) >= 0)
            {
                // if positive set to 1
                sign_sums[j] += 1;
            }
            sum += std::abs(array[i][j]);
        }
        // number of positive is greater than half
        if (sign_sums[j] >= size / 2)
        {
            averages[j] = sum / size;
        }
        else
        {
            averages[j] = -sum / size;
        }
    }
    // std::cout << sign_sums[0] << " " << sign_sums[1] << " " << sign_sums[2] << std::endl;
    // std::cout << "averages: " << averages[0] << " " << averages[1] << " " << averages[2] << std::endl;
    return averages;
}
// Message functions -----------------------------------------------------------
void init_function(const int &id, const std::string &name, std::mutex &lock)
{
    lock.lock();
    auto time = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(time);
    std::cout << FBLU(<< id << ": Init " <<) << FGRN(<< name <<) << FBLU(<< " function: " << std::ctime(&now_time) <<);
    lock.unlock();
}

void send_closing_message(const int &id, const std::string &name, std::mutex &lock)
{
    lock.lock();
    std::cout << FYEL(<< id <<) << ": " << FRED(<< name <<) << FRED(" thread closing...") << "\n";
    lock.unlock();
}
// [END] Message functions -----------------------------------------------------

// Function to convert string to boolean ---------------------------------------
bool stringToBool(const std::string &str)
{
    if (str == "true" || str == "1" || str == "True")
    {
        return true;
    }
    else if (str == "false" || str == "0" || str == "False")
    {
        return false;
    }
    else
    {
        throw std::invalid_argument("Invalid boolean value: " + str);
    }
}
// [END] Function to convert string to boolean ----------------------------------
void on_press_vel(const int left_speed, const int right_speed, std::fstream &ser_motors)
{
    ser_motors.flush();
    ser_motors << std::to_string(left_speed) << ' ' << std::to_string(right_speed) << '\n';
    ser_motors.flush();
}
// SOCKET FUNCTIONS ------------------------------------------------------------
#define BUFFER_SIZE 4096

struct msg_data
{
    ssize_t numBytesReceived;
    struct sockaddr_in srcAddr;
    char *buffer;
};
int init_socket()
{
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1)
    {
        std::cerr << "socket creation failed\n";
        return 1;
    }
    // Setting to non-blocking

    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    int broadcastEnable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
                   &broadcastEnable, sizeof(broadcastEnable)) == -1)
    {
        std::cerr << "setsockopt failed\n";
        close(sock);
        return 1;
    }
    return sock;
}
struct sockaddr_in init_address(const int &port, const char *address)
{
    struct sockaddr_in destAddr;
    std::memset(&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port); // topic
    destAddr.sin_addr.s_addr = address ? inet_addr(address) : INADDR_ANY;
    return destAddr;
}
void send_message(int sock, const char *message, const struct sockaddr_in &destAddr)
{
    ssize_t numBytesSent = sendto(sock, message, std::strlen(message), 0,
                                  (struct sockaddr *)&destAddr, sizeof(destAddr));
    if (numBytesSent == -1)
    {
        std::cerr << FRED("sendto failed\n");
        close(sock);
        return;
    }
}

msg_data receive_message(const int &sock, const bool &stop_threads, char *buffer, const struct sockaddr_in &srcAddr, socklen_t &srcAddrLen)
{
    msg_data data;
    ssize_t numBytesReceived = recvfrom(sock, buffer, BUFFER_SIZE, MSG_WAITALL,
                                        (struct sockaddr *)&srcAddr, &srcAddrLen);

    while (numBytesReceived < 0 && !stop_threads)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            usleep(1);
            numBytesReceived = recvfrom(sock, buffer, BUFFER_SIZE, MSG_WAITALL,
                                        (struct sockaddr *)&srcAddr, &srcAddrLen);
        }
        else
        {
            std::cerr << "Error receiving data: " << strerror(errno) << "\n";
            break;
        }
    }
    data.numBytesReceived = numBytesReceived;
    data.srcAddr = srcAddr;
    data.buffer = buffer;
    return data;
}
void bind_socket(const int &sock, const struct sockaddr_in &myAddr)
{
    if (bind(sock, (struct sockaddr *)&myAddr, sizeof(myAddr)) == -1)
    {
        std::cerr << "bind failed\n";
        close(sock);
        return;
    }
}
void print_buffer(const char *msg_buffer, const struct sockaddr_in &srcAddr, const ssize_t &numBytesReceived, const char &cur, const int &speed, std::mutex &lock)
{
    lock.lock();
    std::cout << "Received " << numBytesReceived << " bytes from "
              << inet_ntoa(srcAddr.sin_addr) << ": " << cur << " " << speed << "\n";
    std::cout.flush();
    lock.unlock();
}
// [END] SOCKET FUNCTIONS ------------------------------------------------------
#endif