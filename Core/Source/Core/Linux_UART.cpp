#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <iomanip>
#include <unistd.h>
#include <stdint.h>
namespace Linux
{
    namespace Communication
    {
        int Linux_i_InitCommunication() /* TODO: Add port and baud rate to parameters - port needs to be detected before that */
        {
            int serial_port = open("/dev/ttyACM0", O_RDWR);

            termios tty{};

            if (serial_port < 0) 
            {
                std::cout << "Error while trying to open serial port. Error number: " << errno << '.' << std::endl; /* Create logger class */
                return 0xFF;
            }

            if(tcgetattr(serial_port, &tty) != 0)
            {
                std::cout << "Initilaization error number " << errno << '.' << std::endl;
                return 0xFF;
            }

            tty.c_cflag &= ~PARENB;                 /* Clear parity */
            tty.c_cflag &= ~CSTOPB;                 /* One stop bit */
            tty.c_cflag &= ~CSIZE;                  /* Clear byte size */
            tty.c_cflag |= CS8;                     /* 8 bytes in bit */
            tty.c_cflag &= ~CRTSCTS;                /* Disable RTS/CTS HW flow control */
            tty.c_cflag |= CREAD | CLOCAL;          /* Enable read and disable control lines */
            tty.c_lflag &= ~ISIG;                   /* Disable interpretation of INTR, QUIT and SUSP */
            tty.c_iflag &= ~(IXON | IXOFF | IXANY); /* Disable SW flow control */
            cfmakeraw(&tty);

            cfsetispeed(&tty, B115200);

            tty.c_cc[VTIME] = 5;    /* 10 seconds wait for data*/
            tty.c_cc[VMIN] = 0;

            tcsetattr(serial_port, TCSANOW, &tty);
            
            std::cout << "Serial port /dev/ttyACM0 opened sucsessfully!\n";

            return serial_port;
        } 
        
        void Linux_v_SerialRead(int serial_port)
        {
            uint8_t buffer[256];
            int total_bytes = read(serial_port, buffer, sizeof(buffer));

            if (total_bytes > 0)
            {
                std::cout << "Received " << total_bytes << " bytes: ";
                for (int i = 0; i < total_bytes; ++i)
                {
                    std::printf("%02X ", buffer[i]);
                }
                std::printf("\n");
            }
        }
    }
}
