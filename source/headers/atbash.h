#pragma once

#include <string>
#include <vector>

class Atbash {
public:
	template<typename vector_type>
	static std::vector<vector_type> reverse_apply(std::vector<vector_type> data, std::vector<vector_type> base) {
		std::vector<vector_type> reversed_base = std::vector<vector_type>(base.rbegin(), base.rend());
		std::cout << reversed_base.at(0) << std::endl;
		std::cout << base.at(0) << std::endl;

		std::vector<vector_type> encoded_data;

		for (std::size_t i = 0; i < data.size(); i++) {
			vector_type ivalue = data.at(i);
			auto ivalue_iterator = std::find(base.begin(), base.end(), ivalue);

			if (ivalue_iterator == base.end()) {
				encoded_data.push_back(ivalue);
				continue;
			}

			vector_type ivalue_base_index = std::distance(base.begin(), ivalue_iterator);
			encoded_data.push_back(reversed_base.at(ivalue_base_index));
		}

		return encoded_data;
	}

	static std::string atbash_apply(std::string data) {
		std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
		std::string reversed = "zyxwvutsrqponmlkjihgfedcba";
		std::string encoded_data;

		for (std::size_t i = 0; i < data.size(); i++) {
			int8_t current_character = data.at(i);
			int8_t case_difference = 0;

			if (current_character >= 'A' && current_character <= 'Z')
				case_difference = 32;
			
			current_character += case_difference;

			auto alphabet_iterator = std::find(alphabet.begin(), alphabet.end(), current_character);
			
			if (alphabet_iterator == alphabet.end()) {
				encoded_data.push_back(current_character);
				continue;
			}

			uint8_t character_index = std::distance(alphabet.begin(), alphabet_iterator);
			encoded_data.push_back(reversed.at(character_index)-case_difference);
		}

		return encoded_data;
	}
};