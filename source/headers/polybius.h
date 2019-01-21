#pragma once

#include <string>
#include <vector>

class Polybius {
private:
	/* Method that returns true if duplicate items are found within a vector */
	static bool duplicate_items(std::vector<uint32_t> target) {
		std::vector<uint32_t> item_buffer;
		for (std::size_t i = 0; i < target.size(); i++) {
			uint32_t current_item = target.at(i);
			auto iterator = std::find(item_buffer.begin(), item_buffer.end(), current_item);
			if (iterator != item_buffer.end()) return true;
		}

		return false;
	}

	/* Method that converts lowercase characters to uppercase */
	static std::string convert_uppercase(std::string target) {
		std::string buffer_string;
		for (std::size_t i = 0; i < target.size(); i++) {
			int8_t current_character = target.at(i);
			if (current_character >= 'a' && current_character <= 'z')
				buffer_string.push_back(current_character - 32);
			else 
				buffer_string.push_back(current_character);
		}
		return buffer_string;
	}

	/* Method that removes any character that are not uppercase */
	static std::string remove_specials(std::string target) {
		std::string buffer_string;
		for (std::size_t i = 0; i < target.size(); i++) {
			int8_t current_character = target.at(i);
			if (current_character >= 'A' && current_character <= 'Z')
				buffer_string.push_back(current_character);
		}
		return buffer_string;
	}

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

	static matrix<uint32_t> create_matrix(std::vector<uint32_t> base, std::vector<uint32_t> key, uint32_t matrix_size) {
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

	static std::pair<uint32_t, uint32_t> single_encode(uint32_t data, matrix<uint32_t> matrix) {
		for (std::size_t i = 0; i < matrix.size(); i++) {
			for (std::size_t c = 0; c < matrix.at(i).size(); c++) {
				if (matrix.at(i).at(c) == data) 
					return std::make_pair(i, c);
			}
		}
		return std::make_pair(0, 0);
	}
	static uint32_t single_decode(std::pair<uint32_t, uint32_t> location, matrix<uint32_t> matrix) {
		return matrix.at(location.first).at(location.second);
	}

	static std::vector<std::pair<uint32_t, uint32_t>> encode_data(std::string data, std::string key, int8_t sacrifice = '\0', matrix<uint32_t>* matrix_output=nullptr) {
		/* This is the character set that will be used for the encoding */
		std::vector<uint32_t> matrix_base {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

		/* Sanitizes the key: Checks for duplicate characters, checks key length, and converts case.
		Removes non-letter characters from key */
		if (key.size() > matrix_base.size()) throw KeyLengthGreaterThanBaseException();
		if (duplicate_items(std::vector<uint32_t>(key.begin(), key.end()))) throw DuplicateCharInKeyException();
		else { 
			key = convert_uppercase(key);
			key = remove_specials(key);
		}

		/* Checks the presence of the sacrifice in the key */
		auto key_iterator = std::find(key.begin(), key.end(), sacrifice);
		if (key_iterator != key.end()) throw SacrificeAppearsInKeyException();

		/* Checks the presence of the sacrifice in the base, and erases from base 
		this will not run if the sacrifice is a nullbyte, which means the matrix will expand
		beyond 5x5 to 6x6 in order to include the additional characters, so that no 
		characters will have to be sacrificed. */
		if (sacrifice != '\0') {
			auto base_iterator = std::find(matrix_base.begin(), matrix_base.end(), sacrifice);
			if (base_iterator == matrix_base.end()) throw SacrificeNotInBaseException();
			else matrix_base.erase(base_iterator);
		}

		/* Sanitizes input data to uppercase and strips non-letters */
		data = convert_uppercase(data);
		data = remove_specials(data);

		/* Creates the matrix that will be used to encode data */
		matrix<uint32_t> encoder_matrix = create_matrix(matrix_base, std::vector<uint32_t>(key.begin(), key.end()), sacrifice == '\0' ? 6 : 5);
			
		/* Stores the created matrix if user wishes */
		if (matrix_output != nullptr) *matrix_output = encoder_matrix;

		/* This will store the encoded output data. */
		std::vector<std::pair<uint32_t, uint32_t>> encoded_output_data;

		/* Loops over every character in the input data, encodes and writes to output data */
		for (std::size_t i = 0; i < data.size(); i++) {
			int8_t current_character = data.at(i);
			
			/* Encodes the current character */
			std::pair<uint32_t, uint32_t> encoded = single_encode(current_character, encoder_matrix);

			/* Adds the encoded character to the output data */
			encoded_output_data.push_back(encoded);
		}

		return encoded_output_data;
	}
	static std::vector<std::pair<uint32_t, uint32_t>> encode_data(std::vector<uint32_t> data, matrix<uint32_t> encoder_matrix) {
		/* This is where the encoded output data will be stored */
		std::vector<std::pair<uint32_t, uint32_t>> encoded_data;
		
		/* Loops over every item in input data, encodes, and adds to encoded_data vector */
		for (std::size_t i = 0; i < data.size(); i++) {
			uint32_t current_int = data.at(i);
			std::pair<uint32_t, uint32_t> encoded = single_encode(current_int, encoder_matrix);
			encoded_data.push_back(encoded);
		}

		return encoded_data;
	}

	static std::vector<uint32_t> decode_data(std::vector<std::pair<uint32_t, uint32_t>> data, std::string key, int8_t sacrifice = '\0', matrix<uint32_t>* matrix_output=nullptr) {
		/* This is the character set that will be used for the decoding */
		std::vector<uint32_t> matrix_base{ 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z' };

		/* Sanitizes the key: Checks for duplicate characters, checks key length, and converts case.
		Removes non-letter characters from key */
		if (key.size() > matrix_base.size()) throw KeyLengthGreaterThanBaseException();
		if (duplicate_items(std::vector<uint32_t>(key.begin(), key.end()))) throw DuplicateCharInKeyException();
		else { 
			key = convert_uppercase(key);
			key = remove_specials(key);
		}

		/* Checks the presence of the sacrifice in the key */
		auto key_iterator = std::find(key.begin(), key.end(), sacrifice);
		if (key_iterator != key.end()) throw SacrificeAppearsInKeyException();

		/* Checks the presence of the sacrifice in the base, and erases from base
		this will not run if the sacrifice is a nullbyte, which means the matrix will expand
		beyond 5x5 to 6x6 in order to include the additional characters, so that no
		characters will have to be sacrificed. */
		if (sacrifice != '\0') {
			auto base_iterator = std::find(matrix_base.begin(), matrix_base.end(), sacrifice);
			if (base_iterator == matrix_base.end()) throw SacrificeNotInBaseException();
			else matrix_base.erase(base_iterator);
		}

		/* Creates the matrix that will be used to decode data */
		matrix<uint32_t> decoder_matrix = create_matrix(matrix_base, std::vector<uint32_t>(key.begin(), key.end()), sacrifice == '\0' ? 6 : 5);

		/* Stores the created matrix if user wishes */
		if (matrix_output != nullptr) *matrix_output = decoder_matrix;

		/* This will store the decoded output data */
		std::vector<uint32_t> decoded_output_data;


		for (std::size_t i = 0; i < data.size(); i++) {

		}
	}
	static std::vector<uint32_t> decode_data(std::vector<std::pair<uint32_t, uint32_t>> data, matrix<uint32_t> matrix) {
		/* This will hold the decoded data */
		std::vector<uint32_t> decoded_data;

		for (std::size_t i = 0; i < data.size(); i++) {

			/* Reference to the current coordinate pair. */
			std::pair<uint32_t, uint32_t>& current = data.at(i);
			
			/* The current decoded item in the coordinate array */
			uint32_t decoded_item = matrix.at(current.first).at(current.second);

			/* Adds decoded item to vector */
			decoded_data.push_back(decoded_item);
		}

		return decoded_data;
	}
};
