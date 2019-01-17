#include <iostream>
#include <string>

#include "cipher.h"

int main() {
	std::string encoded = Polybius::plaintext_encode("HELLO WORLD", "fbcdjk");
	std::cout << encoded << std::endl;

	std::cin.get();
	return EXIT_SUCCESS;
}