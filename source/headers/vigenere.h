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

