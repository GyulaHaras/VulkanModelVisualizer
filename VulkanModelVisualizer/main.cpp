#include <iostream>
// #include "ModelTextParser.h"
extern "C" {
	#include "mock.h"
	#include "ModelTextParser.h"
}

int main() {
	std::cout << "Hola" << std::endl;
	print_objModel();
	greeting();
	return 0;
}