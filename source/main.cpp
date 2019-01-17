#include <iostream>
#include <string>

#include "ciphers.h"



void test_polybius() {
	Polybius::matrix<uint32_t> created_matrix;
	std::vector<std::pair<uint32_t, uint32_t>> polybius_encoded = Polybius::encode_data("Hello World", "Unique", '\0', &created_matrix);
	std::cout << "Encoded: ";
	for (std::size_t i = 0; i < polybius_encoded.size(); i++) {
		std::cout << polybius_encoded.at(i).first << polybius_encoded.at(i).second;
		if (i != polybius_encoded.size() - 1)  std::cout << ", ";
	}

	std::cout << std::endl << std::endl;
	std::cout << "GENERATED MATRIX_____" << std::endl;
	for (std::size_t i = 0; i < created_matrix.size(); i++) {
		std::cout << "| ";
		for (std::size_t c = 0; c < created_matrix.at(i).size(); c++) {
			std::cout << (char)created_matrix.at(i).at(c) << " | ";
		}
		std::cout << std::endl;
	}

	// 5x5 Matrix: 22, 10, 30, 30, 32, 43, 32, 34, 30, 14
	// 6x6 Matrix: 20, 05, 23, 23, 25, 35, 25, 31, 23, 13
}

int main() {	
	test_polybius();

	std::cin.get();
	return EXIT_SUCCESS;
}