#pragma once

#include <string>
#include <vector>


class Vigenere {
public:
	template <typename matrix_type>
	using matrix = std::vector<std::vector<matrix_type>>;

	class ZeroBaseLengthException : public std::exception {
	public: ZeroBaseLengthException() : std::exception("Length of supplied base vector is zero.") {}
	};
	class ZeroKeyLengthException : public std::exception {
	public: ZeroKeyLengthException() : std::exception("Length of supplied key is zero.") {}
	};

	static matrix<uint32_t> construct_matrix(std::vector<uint32_t> base)  {
		if (base.size() == 0) throw ZeroBaseLengthException();

		std::vector<std::vector<uint32_t>> matrix_buffer;
		std::vector<uint32_t> shift_buffer;
		for (std::size_t i = 0; i < base.size(); i++) {
			for (std::size_t c = 0; c < base.size(); c++) {
				uint32_t shifted_index = c + i;
				while (shifted_index > base.size()-1) shifted_index -= base.size();
				shift_buffer.push_back(base.at(shifted_index));
			}

			matrix_buffer.push_back(shift_buffer);
			shift_buffer.clear();
		}

		return matrix_buffer;
	}

	static std::vector<uint32_t> matrix_encode(matrix<uint32_t>& matrix, std::vector<uint32_t> data, std::vector<uint32_t> key)  {
		if (key.size() == 0) throw ZeroKeyLengthException();
		std::vector<uint32_t> encoded_matrix;
		
		uint32_t key_position = 0;

		for (std::size_t i = 0; i < data.size(); i++) {
			uint32_t data_index;
			uint32_t key_index;

			auto data_iterator = std::find(matrix.at(0).begin(), matrix.at(0).end(), data.at(i));
			auto key_iterator = std::find(matrix.at(0).begin(), matrix.at(0).end(), key.at(key_position));

			if (data_iterator == matrix.at(0).end() || key_iterator == matrix.at(0).end()) {
				encoded_matrix.push_back(data.at(i));
				continue;
			}

			data_index = std::distance(matrix.at(0).begin(), data_iterator);
			key_index = std::distance(matrix.at(0).begin(), key_iterator);

			uint32_t encoded_data = matrix.at(data_index).at(key_index);
			encoded_matrix.push_back(encoded_data);

			key_position++;
			if (key_position > key.size() - 1) key_position = 0;
		}

		return encoded_matrix;
	}

	static std::vector<uint32_t> matrix_decode(matrix<uint32_t>& matrix, std::vector<uint32_t> encoded, std::vector<uint32_t> key)  {
		if (key.size() == 0) throw ZeroKeyLengthException();
		std::vector<uint32_t> decoded_matrix;
		uint32_t key_position = 0;

		for (std::size_t i = 0; i < encoded.size(); i++) {
			uint32_t key_index;

			auto data_iterator = std::find(matrix.at(0).begin(), matrix.at(0).end(), encoded.at(i));
			auto key_iterator = std::find(matrix.at(0).begin(), matrix.at(0).end(), key.at(key_position));
			
			if (key_iterator == matrix.at(0).end() || data_iterator == matrix.at(0).end()) {
				decoded_matrix.push_back(encoded.at(i));
				continue;
			}

			key_index = std::distance(matrix.at(0).begin(), key_iterator);

			for (std::size_t c = 0; c < matrix.size(); c++) {
				if (matrix.at(c).at(key_index) == encoded.at(i)) 
					decoded_matrix.push_back(matrix.at(0).at(c));
			}

			key_position++;
			if (key_position > key.size() - 1) key_position = 0;
		}

		return decoded_matrix;
	}
	
	static std::string vigenere_lookup(std::string data, std::string key, bool decode_lookup = false, bool preserve_case=true) {
		if (key.size() == 0) throw ZeroKeyLengthException();

		std::vector<std::vector<uint32_t>> matrix = 
			construct_matrix({'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'});

		std::vector<uint32_t> uppercases;
		if (preserve_case) {
			for (std::size_t i = 0; i < data.size(); i++) {
				char& ichar = data.at(i);
				if (ichar >= 'A' && ichar <= 'Z') uppercases.push_back(i);
			}
		}

		std::vector<uint32_t> data_vector, key_vector;
		data_vector = std::vector<uint32_t>(data.begin(), data.end());
		key_vector = std::vector<uint32_t>(key.begin(), key.end());

		auto lower_caser = [](std::vector<uint32_t>* target) {
			for (std::size_t i = 0; i < target->size(); i++) {
				uint32_t& index_data = target->at(i);
				if (index_data >= 'A' && index_data <= 'Z') {
					target->at(i) += 32;
				}
			}
		};
		
		lower_caser(&data_vector);
		lower_caser(&key_vector);
		
		std::vector<uint32_t> output_data = decode_lookup ? matrix_decode(matrix, data_vector, key_vector) : matrix_encode(matrix, data_vector, key_vector);
		for (std::size_t i = 0; i < uppercases.size(); i++) output_data.at(uppercases.at(i)) -= 32;
		return std::string(output_data.begin(), output_data.end());
	}
};

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