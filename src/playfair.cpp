#include "playfair.h"
#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <cctype>
#include <algorithm>

void Playfair::generateMatrix(const std::string& key) {
	std::string updatedKey;
	std::string upperKey = key;
	std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int letterIndex = 0;

	std::transform(upperKey.begin(), upperKey.end(), upperKey.begin(), std::toupper);

	for (size_t i = 0; i < upperKey.length(); i++) {
		auto it = std::find(updatedKey.begin(), updatedKey.end(), upperKey[i]);

		if (it == updatedKey.end()) {
			updatedKey.push_back(upperKey[i]);
		}
	}

	for (const auto& letter : alphabet) {
		if (std::find(updatedKey.begin(), updatedKey.end(), letter) == updatedKey.end()) {
			updatedKey.push_back(letter);
		}
	}

	for (size_t row = 0; row < MATRIX_SIZE; row++) {
		for (size_t column = 0; column < MATRIX_SIZE; column++) {
			matrix[row][column] = updatedKey[letterIndex];
			letterIndex++;
		}
	}

}

PreparedText Playfair::prepareText(const std::string& text) {
	PreparedText preparedText;

	std::string cleanText;
	std::string pair;

	size_t i = 0;

	for (char c : text) {
		if (std::isalnum(static_cast<unsigned char>(c))) {
			cleanText.push_back(c);
		}
	}

	if (cleanText.empty()) {
		throw std::invalid_argument("The text must contain at least one letter or digit.");
	}

	std::transform(cleanText.begin(), cleanText.end(), cleanText.begin(), std::toupper);

	while (i < cleanText.length()) {
		size_t xPosition = preparedText.pairs.size() * 2 + 1;
		if (i == cleanText.length() - 1) {
			pair.push_back(cleanText[i]);
			pair.push_back('X');
			
			preparedText.paddingPositions.push_back(xPosition);
			preparedText.pairs.push_back(pair);
			break;
		}
		else {
			if (cleanText[i] == cleanText[i + 1]) {
				pair.push_back(cleanText[i]);
				pair.push_back('X');
				
				preparedText.paddingPositions.push_back(xPosition);
				preparedText.pairs.push_back(pair);
				i++;
			}
			else {
				pair.push_back(cleanText[i]);
				pair.push_back(cleanText[i + 1]);

				preparedText.pairs.push_back(pair);
				i += 2;
			}
		}
		pair.clear();
	}

	return preparedText;
}

Position Playfair::findPosition(char c) {
	Position position;

	for (size_t row = 0; row < MATRIX_SIZE; row++) {
		for (size_t column = 0; column < MATRIX_SIZE; column++) {
			if (matrix[row][column] == c) {
				return { static_cast<int>(row), static_cast<int>(column) };	
			}
		}
	}

	return { -1, -1 };
}

std::string Playfair::serialize(const EncryptionResult& result) {
	std::string format;

	format += "PF1"; // PlayFair Version
	format.push_back('|'); // Separator
	
	format += result.text; // Add the encrypted text

	format.push_back('|');

	for (size_t p = 0; p < result.paddingPositions.size(); ++p) {
		format += std::to_string(result.paddingPositions[p]);

		if (p != result.paddingPositions.size() - 1) { format += ','; }
	}

	return format;
}

EncryptionResult Playfair::deserialize(const std::string& inputText) {
	EncryptionResult result;
	int checkSeparators = 0;

	for (char c : inputText) {
		if (c == '|') {
			checkSeparators++;
		}
	}
	if (checkSeparators != 2) {
		throw std::invalid_argument("Invalid Playfair message format.");
	}

	if (inputText.substr(0, 4) != "PF1|") { //Checks the first 4 characters which are the first validation.
		throw std::invalid_argument("Invalid Playfair message format.");
	}

	std::string remainingText = inputText.substr(4);
	std::string encryptedText;

	size_t separator = remainingText.find('|');

	if (separator == std::string::npos) { //Second validation, confirm that the second separator exists.
		throw std::invalid_argument("Invalid Playfair message format.");
	}
	else {
		encryptedText = remainingText.substr(0, separator);

		if (encryptedText.empty()) {
			throw std::invalid_argument("Invalid Playfair message format.");
		}

		for (char c : encryptedText)
		{
			if (!std::isalnum(static_cast<unsigned char>(c)))
			{
				throw std::invalid_argument("Invalid Playfair message format.");
			}
		}

		result.text = encryptedText;
	}

	std::string paddingPositionsNum = remainingText.substr(separator + 1);

	if (paddingPositionsNum.empty()) {
		return result;
	}

	std::string number;
	for (size_t i = 0; i < paddingPositionsNum.size(); ++i) {
		if (isdigit(paddingPositionsNum[i])) { //Is a valid number
			number.push_back(paddingPositionsNum[i]);
		}
		else if (paddingPositionsNum[i] == ',') {
			if (i + 1 < paddingPositionsNum.size()) {
				if (paddingPositionsNum[i + 1] == ',') { //Confirm that there isnt double comma (,,)
					throw std::invalid_argument("Invalid Playfair message format.");
				}
				else if (number.empty()) { //Checks if there was not a number before the comma (,n)
					throw std::invalid_argument("Invalid Playfair message format.");
				}

				result.paddingPositions.push_back(std::stoul(number));
				number.clear();
			}
			else { //The last character is the comma
				throw std::invalid_argument("Invalid Playfair message format.");
			}
		}
		else {
			throw std::invalid_argument("Invalid Playfair message format.");
		}
		
	}
	if (!number.empty()) {
		result.paddingPositions.push_back(std::stoi(number));
	}

	return result;
}

std::string Playfair::encrypt(const std::string& text, const std::string& key) {
	EncryptionResult result;
	PreparedText preparedText;

	generateMatrix(key);
	preparedText = prepareText(text);
	
	result.paddingPositions = preparedText.paddingPositions;

	for (size_t i = 0; i < preparedText.pairs.size(); i++) {
		Position firstPos = findPosition(preparedText.pairs[i][0]);
		Position secondPos = findPosition(preparedText.pairs[i][1]);

		if (firstPos.row == secondPos.row) {
			firstPos.column = (firstPos.column + 1) % MATRIX_SIZE;
			secondPos.column = (secondPos.column + 1) % MATRIX_SIZE;
			
			result.text.push_back(matrix[firstPos.row][firstPos.column]);
			result.text.push_back(matrix[secondPos.row][secondPos.column]);
		}
		else if (firstPos.column == secondPos.column) {
			firstPos.row = (firstPos.row + 1) % MATRIX_SIZE;
			secondPos.row = (secondPos.row + 1) % MATRIX_SIZE;

			result.text.push_back(matrix[firstPos.row][firstPos.column]);
			result.text.push_back(matrix[secondPos.row][secondPos.column]);
		}
		else {
			std::swap(firstPos.column, secondPos.column);

			result.text.push_back(matrix[firstPos.row][firstPos.column]);
			result.text.push_back(matrix[secondPos.row][secondPos.column]);
		}
	}

	std::string returnText = serialize(result);

	return returnText;
}

std::string Playfair::decrypt(const std::string& message, const std::string& key) {
	generateMatrix(key);

	EncryptionResult result = deserialize(message);
	std::string upperText = result.text;
	std::vector<std::string> pairs;
	const auto& paddingPositions = result.paddingPositions;

	std::transform(upperText.begin(), upperText.end(), upperText.begin(), std::toupper);

	if (upperText.empty()) {
		throw std::invalid_argument("The encrypted can't be empty.");
	}

	if (upperText.length() % 2 != 0) {
		throw std::invalid_argument("The encrypted text must have an even number of characters.");
	}
	

	for (size_t i = 0; i < upperText.length(); i+=2) {
		std::string pair;

		pair.push_back(upperText[i]);
		pair.push_back(upperText[i + 1]);

		pairs.push_back(pair);
	}

	std::string decryptedText;

	for (size_t i = 0; i < pairs.size(); ++i) {
		Position firstPos = findPosition(pairs[i][0]);
		Position secondPos = findPosition(pairs[i][1]);

		if (firstPos.row == -1 || secondPos.row == -1) {
			throw std::invalid_argument("The encrypted text contains invalid characters.");
		}

		if (firstPos.row == secondPos.row) {
			firstPos.column = (firstPos.column + MATRIX_SIZE - 1) % MATRIX_SIZE; // adding MATRIX_SIZE - 1 is equivalent to subtracting 1 while avoiding negative index
			secondPos.column = (secondPos.column + MATRIX_SIZE - 1) % MATRIX_SIZE;

			decryptedText.push_back(matrix[firstPos.row][firstPos.column]);
			decryptedText.push_back(matrix[secondPos.row][secondPos.column]);
		}
		else if (firstPos.column == secondPos.column) {
			firstPos.row = (firstPos.row + MATRIX_SIZE - 1) % MATRIX_SIZE;
			secondPos.row = (secondPos.row + MATRIX_SIZE - 1) % MATRIX_SIZE;

			decryptedText.push_back(matrix[firstPos.row][firstPos.column]);
			decryptedText.push_back(matrix[secondPos.row][secondPos.column]);
		}
		else {
			std::swap(firstPos.column, secondPos.column);

			decryptedText.push_back(matrix[firstPos.row][firstPos.column]);
			decryptedText.push_back(matrix[secondPos.row][secondPos.column]);
		}
	}

	if (!paddingPositions.empty()) {
		for (auto it = paddingPositions.rbegin(); it != paddingPositions.rend(); ++it) { //delete the padding X
			decryptedText.erase(*it, 1);
		}
	}

	return decryptedText;
}
