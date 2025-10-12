#include "Core.h"
#include "Linux_UART.h"
#include <iostream>

namespace Core {

	void PrintHelloWorld()
	{
		int x;

		x = Linux::Communication::Linux_i_Communication();
		std::cout << x << std::endl;
		std::cout << "Hello World!\n";
		std::cin.get();
	}

}