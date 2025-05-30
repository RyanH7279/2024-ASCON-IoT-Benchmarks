import serial

def log_serial_communication(port='COM20', baudrate=9600, end_marker='#'):
    logfile = f'AES-256-GCM-AD-log.txt'
    try:
        # Open the serial port
        ser = serial.Serial(port, baudrate, timeout=1)
        print(f"Listening on {port}...")

        ser.write(f"START".encode())

        # Open the log file
        with open(logfile, 'a', encoding='utf-8') as log:
            while True:
                # Read a line from the serial port
                if ser.in_waiting > 0:
                    line = ser.readline().decode('utf-8').strip()
                    
                    # Log the received line to the text file
                    log.write(line + '\n')
                    print(line)  # Optionally print the line to the console
                    
                    # Check for the end marker
                    if line.endswith(end_marker):
                        print("End marker detected. Stopping logging.")
                        break


    except Exception as e:
        print(f"Error: {e}")


def main():
    # port = find_ardunio()
    port='COM20'
    log_serial_communication(port=port, baudrate=9600, end_marker='#')

if __name__ == "__main__":
    main()
