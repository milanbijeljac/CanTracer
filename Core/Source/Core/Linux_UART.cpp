#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

namespace Linux
{
    namespace Communication
    {
        int Linux_i_Communication()
        {
            int serial_port = open("/dev/ttyACM0", O_RDWR);
            termios tty{};

            if (serial_port < 0) 
            {
                std::cout << "Error while trying to open serial port. Error number: " << errno << '.' << std::endl;
                return 1;
            }

            if(tcgetattr(serial_port, &tty) != 0)
            {
                std::cout << "Initilaization error number " << errno << '.' << std::endl;
                return 1;
            }

            tty.c_cflag &= ~PARENB;                 /* Clear parity */
            tty.c_cflag &= ~CSTOPB;                 /* One stop bit */
            tty.c_cflag &= ~CSIZE;                  /* Clear byte size */
            tty.c_cflag |= CS8;                     /* 8 bytes in bit */
            tty.c_cflag &= ~CRTSCTS;                /* Disable RTS/CTS HW flow control */
            tty.c_cflag |= CREAD | CLOCAL;          /* Enable read and disable control lines */
            tty.c_lflag &= ~ISIG;                   /* Disable interpretation of INTR, QUIT and SUSP */
            tty.c_iflag &= ~(IXON | IXOFF | IXANY); /* Disable SW flow control */

            cfsetispeed(&tty, B115200);

            tty.c_cc[VTIME] = 5;    /* 10 seconds wait for data*/
            tty.c_cc[VMIN] = 0;

            tcsetattr(serial_port, TCSANOW, &tty);
            
            return 0;
        } 
    }
}
