#include "playfair.h"
#include <iostream>
#include <utility>
#include <stdexcept>
#include <cctype>
#include <algorithm>

std::size_t Playfair::getBytes(const unsigned char charByte) {
	int counterOf1 = 0;
	for (size_t i = 7; i > 2; i--) {
		int bit = (charByte >> i) & 1;

		if (bit == 1) {
			counterOf1++;
		}
		else if (bit == 0) {
			break;
		}
	}

	if (counterOf1 == 0) { // 0b0xxxxxxx (1 byte)
		return 1;
	}
	else if (counterOf1 == 2) { // 0b110xxxxx (2 bytes)
		return 2;
	}
	else if (counterOf1 == 3) { // 0b1110xxxx (3 bytes)
		return 3;
	}
	else if (counterOf1 == 4) { // 0b11110xxx (4 bytes)
		return 4;
	}
	else {
		// throw
	}

	return 0;
}

void Playfair::generateMatrix(const std::string& key) {
	std::vector<std::string> updatedAlphabet;
	std::string upperKey;
	std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789¡!\",.;:¿?'%# ";
	int letterIndex = 0;

	for (size_t i = 0; i < key.length(); ++i) {
		std::string actualCharacter = key.substr(i, Playfair::getBytes(key[i]));

		upperKey += actualCharacter;
		i += Playfair::getBytes(key[i]) - 1;
		/*if (std::isalnum() && !std::isdigit()) { // FIX THIS WHEN THE FUNCTION TO GET THE CHAR'S BYTES IS COMPLETED
			std::transform(actualCharacter.begin(), actualCharacter.end(), actualCharacter.begin(), std::toupper);
			upperKey += actualCharacter;
		}
		else { // if actualCharater is a symbol
			upperKey += actualCharacter;
		}*/
	}

	for (size_t i = 0; i < upperKey.length(); i++) {
		std::string keyToAlphabet = upperKey.substr(i, Playfair::getBytes(upperKey[i]));

		auto it = std::find(updatedAlphabet.begin(), updatedAlphabet.end(), keyToAlphabet);

		if (it == updatedAlphabet.end()) { // if the letter is not found in the updatedAlphabet, add it
			updatedAlphabet.push_back(keyToAlphabet);
		}

		i += Playfair::getBytes(upperKey[i]) - 1;
	}
	
	for (size_t letter = 0; letter < alphabet.length(); ++letter) {
		std::string letterNotInAlphabet = alphabet.substr(letter, Playfair::getBytes(alphabet[letter]));

		if (std::find(updatedAlphabet.begin(), updatedAlphabet.end(), letterNotInAlphabet) == updatedAlphabet.end()) {
			updatedAlphabet.push_back(letterNotInAlphabet); // generates a new alphabet with the key letters first and then the rest of the alphabet
		}

		letter += Playfair::getBytes(alphabet[letter]) - 1;
	}

	for (size_t row = 0; row < MATRIX_SIZE; row++) {
		for (size_t column = 0; column < MATRIX_SIZE; column++) {
			matrix[row][column] = updatedAlphabet[letterIndex];
			std::cout << matrix[row][column];
			letterIndex++;
		}
	}
}

PreparedText Playfair::prepareText(const std::string& text) {
	PreparedText preparedText;

	std::string cleanText;
	std::string pair;

	for (size_t i = 0; i < text.length(); ++i) { // verify valid characters
		std::string thisCharacter = text.substr(i, Playfair::getBytes(text[i]));
		auto it = std::find(symbolsList.begin(), symbolsList.end(), thisCharacter);

		if (std::all_of(thisCharacter.begin(), thisCharacter.end(), [](unsigned char c) { return std::isalnum(c); })
			|| std::all_of(thisCharacter.begin(), thisCharacter.end(), [](unsigned char c) { return std::isspace(c); })
			|| it != symbolsList.end())
		{
			cleanText += thisCharacter;
		}
		else {
			throw std::invalid_argument("The text contains invalid characters.");
		}

		i += Playfair::getBytes(text[i]);
	}

	if (cleanText.empty()) {
		throw std::invalid_argument("The text must contain at least one letter or digit.");
	}

	std::transform(cleanText.begin(), cleanText.end(), cleanText.begin(), std::toupper);
	
	size_t i = 0;
	while (i < cleanText.length()) {
		size_t xPosition = preparedText.pairs.size() * 2 + 1; // calculates the position of the 'X' at the end of the pair, which is always at an odd index
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
			if (matrix[row][column] == std::to_string(c)) {
				return { static_cast<int>(row), static_cast<int>(column) };	
			}
		}
	}

	return { -1, -1 };
}

std::string Playfair::serialize(const EncryptionResult& result) {
	std::string format;

	format += "PF2"; // PlayFair Version
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

	if (inputText.substr(0, 4) != "PF2|") { //Checks the first 4 characters which are the first validation.
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
		Position firstPos = findPosition(preparedText.pairs[i][0]); // [H, E] -> (pair) gets H [i, 0]
		Position secondPos = findPosition(preparedText.pairs[i][1]); // [H, E] -> (pair) gets E [i, 1]

		if (firstPos.row == secondPos.row) {
			firstPos.column = (firstPos.column + 1) % MATRIX_SIZE;
			secondPos.column = (secondPos.column + 1) % MATRIX_SIZE;
			
			result.text += matrix[firstPos.row][firstPos.column];
			result.text += matrix[secondPos.row][secondPos.column];
		}
		else if (firstPos.column == secondPos.column) {
			firstPos.row = (firstPos.row + 1) % MATRIX_SIZE;
			secondPos.row = (secondPos.row + 1) % MATRIX_SIZE;

			result.text += matrix[firstPos.row][firstPos.column];
			result.text += matrix[secondPos.row][secondPos.column];
		}
		else {
			std::swap(firstPos.column, secondPos.column);

			result.text += matrix[firstPos.row][firstPos.column];
			result.text += matrix[secondPos.row][secondPos.column];
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

	if (upperText.length() % 2 != 0) { // this is because the encrypted text consists of pairs of characters
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

			decryptedText += matrix[firstPos.row][firstPos.column];
			decryptedText += matrix[secondPos.row][secondPos.column];
		}
		else if (firstPos.column == secondPos.column) {
			firstPos.row = (firstPos.row + MATRIX_SIZE - 1) % MATRIX_SIZE;
			secondPos.row = (secondPos.row + MATRIX_SIZE - 1) % MATRIX_SIZE;

			decryptedText += matrix[firstPos.row][firstPos.column];
			decryptedText += matrix[secondPos.row][secondPos.column];
		}
		else {
			std::swap(firstPos.column, secondPos.column);

			decryptedText += matrix[firstPos.row][firstPos.column];
			decryptedText += matrix[secondPos.row][secondPos.column];
		}
	}

	if (!paddingPositions.empty()) {
		for (auto it = paddingPositions.rbegin(); it != paddingPositions.rend(); ++it) { //delete the padding X
			decryptedText.erase(*it, 1);
		}
	}

	return decryptedText;
}
