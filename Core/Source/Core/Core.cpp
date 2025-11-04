#include "Core.h"
#include "Linux_UART.h"
#include <iostream>

namespace Core {

	void PrintHelloWorld()
	{
		int serialPort;

		serialPort = Linux::Communication::Linux_i_InitCommunication();
		if(serialPort != 0xFF)
		{
			while(1) /* Currently blocking, testing purpose only */
			{
				Linux::Communication::Linux_v_SerialRead(serialPort);
			}
			
		}
	}
}