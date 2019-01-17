#pragma once

#include <string>

class Caesar {
public:
	static std::string caesar_shift(std::string data, uint32_t amount, bool shift_backwards=false) {
		std::string alphabet_lower = "abcdefghijklmnopqrstuvwxyz";
		std::string alphabet_upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		std::string shifted_data = std::string();

		for (std::size_t i = 0; i < data.size(); i++) {
			uint32_t index_character = data.at(i);
			auto iterator_lower = std::find(alphabet_lower.begin(), alphabet_lower.end(), index_character);
			auto iterator_upper = std::find(alphabet_upper.begin(), alphabet_upper.end(), index_character);

			std::string* target_alphabet = nullptr;
			int32_t character_index = 0;
			
			if (iterator_lower != alphabet_lower.end()) {
				character_index = std::distance(alphabet_lower.begin(), iterator_lower);
				target_alphabet = &alphabet_lower;
			}
			
			else if (iterator_upper != alphabet_upper.end()) {
				character_index = std::distance(alphabet_upper.begin(), iterator_upper);
				target_alphabet = &alphabet_upper;
			}

			int32_t shifted_index;

			if (shift_backwards) {
				shifted_index = character_index - amount;
				while (shifted_index < 0) shifted_index += 26;
			} else {
				shifted_index = character_index + amount;
				while (shifted_index > 25) shifted_index -= 26;
			}

			shifted_data.push_back(target_alphabet->at(shifted_index));
		}

		return shifted_data;
	}

	static std::string rot13(std::string data) { return caesar_shift(data, 13); }

};