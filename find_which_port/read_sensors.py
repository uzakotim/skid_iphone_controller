import serial
import threading

def read_serial(serial_port):
    while True:
        try:
            if serial_port.in_waiting > 0:
                data = serial_port.readline().decode().strip()
                print(f"Data from {serial_port.name}: {data}")
        except serial.SerialException as e:
            print(f"Error reading from {serial_port.name}: {e}")
            break

if __name__ == "__main__":
    try:
        # Open serial ports
        serial_port_0 = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
        serial_port_1 = serial.Serial('/dev/ttyUSB1', 9600, timeout=1)

        # Start threads to read from serial ports
        thread_0 = threading.Thread(target=read_serial, args=(serial_port_0,))
        thread_1 = threading.Thread(target=read_serial, args=(serial_port_1,))
        
        thread_0.start()
        thread_1.start()

        # Keep the main thread alive
        thread_0.join()
        thread_1.join()

    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")

