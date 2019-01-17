#pragma once

#include <string>
#include <vector>

class Polybius {
public:
	template <typename matrix_type>
	using matrix = std::vector<std::vector<matrix_type>>;

	class SacrificeNotInBaseException : public std::exception {
	public: 
		SacrificeNotInBaseException() : std::exception("The specified sacrificed character does not appear in the base vector.") {}
	};
	class KeyLengthGreaterThanBaseException : public std::exception {
	public:
		KeyLengthGreaterThanBaseException() : std::exception("The ,length of the supplied key is greater than the length of the base.") {}
	};
	class SacrificeAppearsInKeyException : public std::exception {
	public: SacrificeAppearsInKeyException() : std::exception("Sacrifice appears in supplied key.") {}
	};
	class DuplicateCharInKeyException : public std::exception {
	public:
		DuplicateCharInKeyException() : std::exception("The key has duplicate characters inside of it.") {}
	};

	static matrix<uint32_t> inscribe_matrix(std::vector<uint32_t> base, std::vector<uint32_t> key, uint32_t matrix_size) {
		matrix<uint32_t> matrix_buffer;
		uint32_t current_key_index = 0;
		uint32_t current_base_index = 0;

		for (std::size_t i = 0; i < matrix_size; i++) {
			std::vector<uint32_t> matrix_row_buffer;

			for (std::size_t c = 0; c < matrix_size; c++) {
				if (current_key_index < key.size()) {
					matrix_row_buffer.push_back(key.at(current_key_index));
					current_key_index++;
				}
				else if (current_base_index < base.size()) {
					uint32_t base_character = base.at(current_base_index);
					auto iterator = std::find(key.begin(), key.end(), base_character);

					while (iterator != key.end()) {
						current_base_index++;
						if (current_base_index > base.size() - 1) {
							matrix_row_buffer.push_back(NULL);
							break;
						}

						base_character = base.at(current_base_index);
						iterator = std::find(key.begin(), key.end(), base_character);
					}

					matrix_row_buffer.push_back(base_character);
					current_base_index++;
				}
				else {
					matrix_row_buffer.push_back(NULL);
				}
			}

			matrix_buffer.push_back(matrix_row_buffer);
			matrix_row_buffer.clear();
		}
		return matrix_buffer;
	}

	static std::pair<uint32_t, uint32_t> matrix_encode(matrix<uint32_t>& matrix, uint32_t data) {
		for (std::size_t i = 0; i < matrix.size(); i++) {
			for (std::size_t c = 0; c < matrix.at(i).size(); c++) {
				if (matrix.at(i).at(c) == data) 
					return std::make_pair(i, c);
			}
		}
		return std::make_pair(0, 0);
	}

	static uint32_t matrix_decode(matrix<uint32_t>& matrix, std::pair<uint32_t, uint32_t> location) {
		return matrix.at(location.first).at(location.second);
	}


	static std::vector<std::pair<uint32_t, uint32_t>> matrix_vector_encode(matrix<uint32_t>& matrix, std::vector<uint32_t> data) {
		std::vector<std::pair<uint32_t, uint32_t>> encoded;
		for (std::size_t i = 0; i < data.size(); i++) {
			encoded.push_back(matrix_encode(matrix, data.at(i)));
		}
		return encoded;
	}

	static std::vector<uint32_t> matrix_vector_decode(matrix<uint32_t>& matrix, std::vector<std::pair<uint32_t, uint32_t>> locations) {
		std::vector<uint32_t> decoded_data;
		for (std::size_t i = 0; i < locations.size(); i++) {
			decoded_data.push_back(matrix.at(locations.at(i).first).at(locations.at(i).second));
		}
		return decoded_data;
	}

	static std::string plaintext_encode(std::string data, std::string key, bool expand=false, bool preserve_special=false, char sacrifice='Z') {
		std::vector<uint32_t> present_characters;
		for (std::size_t i = 0; i < key.size(); i++) {
			auto key_iterator = std::find(present_characters.begin(), present_characters.end(), key.at(i));
			if (key_iterator != present_characters.end()) throw DuplicateCharInKeyException();
			present_characters.push_back(key.at(i));
		}

		for (std::size_t i = 0; i < present_characters.size(); i++) {
			if (present_characters.at(i) >= 'a' && present_characters.at(i) <= 'z') {
				present_characters.at(i) = present_characters.at(i) - 32;
			}
		}

		key = std::string(present_characters.begin(), present_characters.end());


		std::vector<uint32_t> matrix_base{'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
		if (key.size() > matrix_base.size()) throw KeyLengthGreaterThanBaseException();

		if (!expand) {
			auto key_iterator = std::find(key.begin(), key.end(), sacrifice);
			if (key_iterator != key.end()) throw SacrificeAppearsInKeyException();

			auto base_iterator = std::find(matrix_base.begin(), matrix_base.end(), sacrifice);
			if (base_iterator == matrix_base.end()) throw SacrificeNotInBaseException();
			matrix_base.erase(base_iterator);
		}

		matrix<uint32_t> inscribed_matrix = inscribe_matrix(matrix_base, std::vector<uint32_t>(key.begin(), key.end()), expand ? 6 : 5);
		
		std::string encoded_formated;

		for (std::size_t i = 0; i < data.size(); i++) {
			int8_t current_char = data.at(i);
			if (current_char >= 'a' && current_char <= 'z') current_char -= 32;
			else if (!(current_char >= 'A' && current_char <= 'Z')) {
				if(preserve_special) encoded_formated.push_back(current_char);
				continue;
			}

			std::pair<uint32_t, uint32_t> encoded = matrix_encode(inscribed_matrix, current_char);
			encoded_formated.append(std::to_string(encoded.first+1));
			encoded_formated.append(std::to_string(encoded.second+1));
			if(i != data.size()-1) encoded_formated.push_back(',');
		}

		return encoded_formated;
	}

	/* TO-DO: MAKE DECODER FOR ^ */
};