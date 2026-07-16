#include <iostream>
#include "playfair.h"

int main() {
	Playfair playfair;

	int choice;
	std::string loop = "n";
	std::string encryptText;
	std::string decryptText;
	std::string key;

	while (loop == "n") {
		std::cout << "Do you want to encrypt or decrypt? [1/2]: ";

        try {
            if (!(std::cin >> choice))
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::cout << "Invalid choice. Please enter 1 or 2.\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (choice != 1 && choice != 2) {
                std::cout << "Invalid choice. Please enter 1 or 2.\n";
                continue;
            }

            switch (choice)
            {
            case 1:
                std::cout << "Write your text to encrypt: ";
                std::getline(std::cin, encryptText);

                std::cout << std::endl;

                std::cout << "\nWrite your key: ";
                std::getline(std::cin, key);

                std::cout << '\n' << playfair.encrypt(encryptText, key);
                break;

            case 2:
                std::cout << "Write your text to decrypt: ";
                std::getline(std::cin, decryptText);

				std::cout << std::endl;

                std::cout << "\nWrite your key: ";
                std::getline(std::cin, key);

                std::cout << '\n' << playfair.decrypt(decryptText, key);
                break;
            }
        }
        catch (const std::invalid_argument& e) {
            std::cout << e.what() << '\n';
            continue;
        }

        while (true)
        {
            std::cout << "\nDo you want to leave? [Y/n]: ";
            std::getline(std::cin, loop);

            if (loop == "Y" || loop == "y" ||
                loop == "N" || loop == "n")
            {
                break;
            }

            std::cout << "Invalid choice. Please enter Y or n.\n";
        }
    }

	return 0;
}
