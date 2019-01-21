#pragma once

#include <iostream>
#include <vector>

class Xor {
public:
	static std::vector<uint32_t> apply_xor(std::vector<uint32_t> data, std::vector<uint32_t> key) {
		std::vector<uint32_t> encoded_vector = std::vector<uint32_t>();
		uint32_t key_index = 0;

		for (std::size_t i = 0; i < data.size(); i++) {
			const uint32_t& index_value = data.at(i);
			encoded_vector.push_back(index_value ^ key.at(key_index));
			key_index >= key.size() - 1 ? 0 : key_index + 1;
		}

		return encoded_vector;
	}

	static std::string apply_xor(std::string data, std::vector<uint32_t> key) {
		std::vector<uint32_t> encoded = apply_xor(std::vector<uint32_t>(data.begin(), data.end()), key);
		return std::string(encoded.begin(), encoded.end());
	}
	
	static std::string apply_xor(std::string data, uint32_t key) {
		return apply_xor(data, { key });
	}

	static std::string apply_xor(std::string data, std::string key) { 
		return apply_xor(data, std::vector<uint32_t>(key.begin(), key.end()));
	}
};
