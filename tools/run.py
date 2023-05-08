import subprocess
import sys
import select
import tty
import termios

# Save the original terminal settings
original_terminal_settings = termios.tcgetattr(sys.stdin)

# Set the terminal to raw mode
tty.setraw(sys.stdin.fileno())

server_process = subprocess.Popen(['./webserv'])

while True:
    # Check if there is anything to read from stdin
    rlist, _, _ = select.select([sys.stdin], [], [], 0.1)
    if rlist:
        key = sys.stdin.read(1)
        if key == 'r':
            print('\r\033[1;33mRestarting server...\033[0m   \r')
            # Terminate the old process and start a new one
            server_process.terminate()
            server_process.wait()
            server_process = subprocess.Popen(['./webserv'])
            print('\r\033[1;32mServer restarted.\033[0m   \r\n')
        elif key == 'q':
            print('\r\033[1;31mStopping server...\033[0m   \r')
            # Terminate the process and exit the loop
            server_process.terminate()
            server_process.wait()
            break

# Restore the original terminal settings
termios.tcsetattr(sys.stdin, termios.TCSADRAIN, original_terminal_settings)

print('\033[1;34mDone.\033[0m')

