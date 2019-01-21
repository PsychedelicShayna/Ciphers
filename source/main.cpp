#include <iostream>
#include <string>

#include "ciphers.h"

/* Small formatting namesapace, simple methods to make formatting text easier */
namespace fmt {
	std::string string_pad(std::string target, std::string padding, std::size_t target_size, bool backwards = false) {
		std::string buffer = std::string();

		if (!backwards) buffer = target;

		for (std::size_t i = 0; i < (target_size - target.size()); i++) {
			buffer.append(padding);
		}

		if (backwards) buffer.append(target);
		return buffer;
	}

	template <typename vector_type>
	std::string render_vector(std::vector<vector_type> vector, std::string(*converter)(vector_type)=nullptr) {
		std::string buffer = std::string();
		for (std::size_t i = 0; i < vector.size(); i++) {
			const vector_type& index_value = vector.at(i);
			converter == nullptr ? buffer.append(std::to_string(index_value)) : buffer.append(converter(index_value));
			if (i != vector.size() - 1) buffer.append(", ");
		}
		return buffer;
	}
}

inline void test_polybius() {
	Polybius::matrix<uint32_t> matrix_output_sacrificed = Polybius::matrix<uint32_t>();
	Polybius::matrix<uint32_t> matrix_output_extended = Polybius::matrix<uint32_t>();
	
	const std::string input_data_a = "Hello World";
	const std::string input_key_a = "Unique";

	std::vector<std::pair<uint32_t, uint32_t>> output_data_a_sacrifice;
	std::vector<std::pair<uint32_t, uint32_t>> output_data_a_extended;

	output_data_a_sacrifice = Polybius::encode_data(input_data_a, input_key_a, 'Z', &matrix_output_sacrificed);
	output_data_a_extended = Polybius::encode_data(input_data_a, input_key_a, '\0', &matrix_output_extended);

	std::string str_output_a1 = std::string();
	std::string str_output_a2 = std::string();

	for (std::size_t i = 0; i < output_data_a_sacrifice.size(); i++) {
		const std::pair<uint32_t, uint32_t>& index_value = output_data_a_sacrifice.at(i);
		str_output_a1.append(std::to_string(index_value.first));
		str_output_a1.append(std::to_string(index_value.second));
		if (i != output_data_a_sacrifice.size() - 1) str_output_a1.append(", ");
	}

	for (std::size_t i = 0; i < output_data_a_extended.size(); i++) {
		const std::pair<uint32_t, uint32_t>& index_value = output_data_a_extended.at(i);
		str_output_a2.append(std::to_string(index_value.first));
		str_output_a2.append(std::to_string(index_value.second));
		if (i != output_data_a_extended.size() - 1) str_output_a2.append(", ");
	}

	std::vector<uint32_t> decoded_data_s = Polybius::decode_data(output_data_a_sacrifice, matrix_output_sacrificed);
	std::vector<uint32_t> decoded_data_e = Polybius::decode_data(output_data_a_extended, matrix_output_extended);

	std::string str_decoded_data_s = std::string(decoded_data_s.begin(), decoded_data_s.end());
	std::string str_decoded_data_e = std::string(decoded_data_e.begin(), decoded_data_e.end());

	std::cout << "ENCRYPTION" << std::endl << std::endl;
	std::cout << "Encryption Key: " << input_key_a << std::endl;
	std::cout << "Input Data: " << input_data_a << std::endl;
	std::cout << "Output Data (Sacrificed): " << str_output_a1 << std::endl;
	std::cout << "Output Data (Extended): " << str_output_a2 << std::endl << std::endl;

	std::cout << "DECRYPTION" << std::endl << std::endl;
	std::cout << "Encryption Key: " << input_key_a << std::endl;

	std::cout << "Input Data String (Sacrificed): " << str_output_a1 << std::endl;
	std::cout << "Input Data String (Extended): " << str_output_a2 << std::endl;

	std::cout << "Output Data Str (Sacrificed): " << str_decoded_data_s << std::endl;
	std::cout << "Output Data Str (Extended): " << str_decoded_data_e << std::endl;
	std::cout << "Output Data (Sacrificed): " << fmt::render_vector<uint32_t>(decoded_data_s) << std::endl;
	std::cout << "Output Data (Extended): " << fmt::render_vector<uint32_t>(decoded_data_e) << std::endl;


	std::cout << std::endl << std::endl;
	
	std::cout << "Generated Matrix (Sacrificed)" << std::endl;
	std::cout << "===================================================" << std::endl;
	for (std::size_t i = 0; i < matrix_output_sacrificed.size(); i++) {
		std::cout << "| ";
		for (std::size_t c = 0; c < matrix_output_sacrificed.at(i).size(); c++) {
			std::cout << (int8_t)matrix_output_sacrificed.at(i).at(c) << " | ";
		}

		std::cout << "// | ";

		for (std::size_t c = 0; c < matrix_output_sacrificed.at(i).size(); c++) {
			std::cout << matrix_output_sacrificed.at(i).at(c) << " | ";
		}


		std::cout << std::endl;
	}
	std::cout << "===================================================" << std::endl << std::endl;


	std::cout << "Generated Matrix (Extended)" << std::endl;
	std::cout << "============================================================" << std::endl;
	for (std::size_t i = 0; i < matrix_output_extended.size(); i++) {
		std::cout << "| ";
		for (std::size_t c = 0; c < matrix_output_extended.at(i).size(); c++) {
			std::cout << (int8_t)matrix_output_extended.at(i).at(c) << " | ";
		}

		std::cout << "// | ";

		for (std::size_t c = 0; c < matrix_output_extended.at(i).size(); c++) {
			std::cout << fmt::string_pad(std::to_string(matrix_output_extended.at(i).at(c)), "0", 2, 1) << " | ";
		}

		std::cout << std::endl;
	}
	std::cout << "============================================================" << std::endl;
}

inline void test_xor() {
	std::cout << "Encode ======================" << std::endl;
	const std::string test_input_a = "Hello World";
	const std::string test_key_a = "Foo Bar";
	const std::string test_output_a = Xor::apply_xor(test_input_a, test_key_a);

	std::cout << "Key string A: " << test_key_a << std::endl;
	std::cout << "Key decimal A: ";

	for (std::size_t i = 0; i < test_key_a.size(); i++) {
		const int8_t& index_value = test_key_a.at(i);
		std::cout << +index_value;
		if (i != test_key_a.size() - 1) std::cout << ", ";
	}

	std::cout << std::endl;

	std::cout << "Input A: " << test_input_a << std::endl;
	std::cout << "Output A: " << test_output_a << std::endl;
	std::cout << "=============================" << std::endl;

	const std::string test_input_b = "Hello World";
	const std::vector<uint32_t> test_key_b = { 10,34,77,13,93,47,52 };
	const std::string test_output_b = Xor::apply_xor(test_input_b, test_key_b);

	std::cout << "Key string B: Not Applicable / Not Mappable" << std::endl;
	std::cout << "Key decimal B: ";

	for (std::size_t i = 0; i < test_key_b.size(); i++) {
		const uint32_t& index_value = test_key_b.at(i);
		std::cout << +index_value;
		if (i != test_key_b.size() - 1) std::cout << ", ";
	}

	std::cout << std::endl;

	std::cout << "Input B: " << test_input_b << std::endl;
	std::cout << "Output B: " << test_output_b << std::endl;
	std::cout << "=============================" << std::endl;

	std::cout << std::endl << std::endl;
	std::cout << "Decode ======================" << std::endl;
	std::cout << "Key string A: " << test_key_a << std::endl;
	std::cout << "Key decimal A: ";

	for (std::size_t i = 0; i < test_key_a.size(); i++) {
		const int8_t& index_value = test_key_a.at(i);
		std::cout << +index_value;
		if (i != test_key_a.size() - 1) std::cout << ", ";
	}

	std::cout << std::endl;

	std::cout << "Input A: " << test_output_a << std::endl;
	std::cout << "Output A: " << Xor::apply_xor(test_output_a, test_key_a) << std::endl;
	std::cout << "=============================" << std::endl;
	std::cout << "Key string B: Not Applicable / Not Mappable" << std::endl;
	std::cout << "Key decimal B: ";

	for (std::size_t i = 0; i < test_key_b.size(); i++) {
		const uint32_t& index_value = test_key_b.at(i);
		std::cout << +index_value;
		if (i != test_key_b.size() - 1) std::cout << ", ";
	}

	std::cout << std::endl;
	std::cout << "Input B: " << test_output_b << std::endl;
	std::cout << "Output B: " << Xor::apply_xor(test_output_b, test_key_b) << std::endl;
	std::cout << "=============================" << std::endl;
}

int main() {
	std::cout << "1.) Test Polybius" << std::endl;
	std::cout << "2.) Test Xor Encryption" << std::endl;

	while (true) {
		std::string input;
		std::getline(std::cin, input);

		if (input == "1") {
			system("cls");
			test_polybius();
		}

		if (input == "2") {
			system("cls");
			test_xor();
		}
	}

	std::cin.get();
	return EXIT_SUCCESS;
}