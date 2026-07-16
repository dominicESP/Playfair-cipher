#pragma once

#include <string>
#include <vector>

struct Position {
	int row;
	int column;
};

struct EncryptionResult {
	std::string text;
	std::vector<size_t> paddingPositions;
};

struct PreparedText {
	std::vector<std::string> pairs;
	std::vector<size_t> paddingPositions;
};

class Playfair {
	
private:
	static constexpr int MATRIX_SIZE = 6;

	char matrix[6][6];
	
	void generateMatrix(const std::string& key);

	PreparedText prepareText(const std::string& text);

	Position findPosition(char c);

	std::string serialize(const EncryptionResult& result);

	EncryptionResult deserialize(const std::string& inputText);

public:
	std::string encrypt(const std::string& text, const std::string& key);
	std::string decrypt(const std::string& message, const std::string& key);

};