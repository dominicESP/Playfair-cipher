# Playfair Cipher (PF1)

A C++ implementation of the classical Playfair cipher with encryption, decryption, serialization, deserialization, and input validation.

---

## Requirements

- C++17 or newer
- Visual Studio 2026 (or any C++17 compatible compiler)

---

## Features

- Encrypt messages using the Playfair cipher.
- Decrypt Playfair-encrypted messages.
- Automatic insertion of padding characters (`X`).
- Automatic removal of inserted padding during decryption.
- Serialization and deserialization using the `PF1` format.
- Input validation with exception handling.

---

## How to use

### Encryption

1. Select **Encrypt (1)**.
2. Enter the plaintext.
3. Enter a encryption key.
4. The program generates a serialized Playfair message.

## Decryption

1. Select **Decrypt (2)**.
2. Enter the serialized Playfair message.
3. **Use the same encryption key that was used during encryption.**
4. The original message will be restored (according to PF1's limitations).

---

## Example

## Encryption
- Plaintext: hello world
- Encryption key: pizza
- Encrypted message: PF1|JFKYMQVQSKK4|3,11

  PF1 = Serialization version
  JFKYMQVQSKK4 = Encrypted text
  3,11 = Padding positions

## Decryption
- Encrypted text: PF1|JFKYMQVQSKK4|3,11
- Encryption key: pizza
- Original text: HELLOWORLD

---

## Known Limitations

- Spaces are removed during encryption and are not restored after decryption.
- Punctuation is not preserved.
- The serialized message should not be modified manually. (Even though it has some ways to check if its valid)

---

## Future Improvements (PF2)

- Preserve spaces.
- Preserve punctuation.
- Improve serialization format.
- Improve message validation.
- Additional usability improvements.

---

## What I learned

During the development of this project I practiced:

- Classes and object-oriented programming.
- Serialization and deserialization.
- Input validation.
- Iterators (`rbegin`, `rend`).
- File and project organization.
- Exception handling.
- Advanced use of STL containers (`std::vector`, `std::string`).
- Defensive input validation.
- Designing and implementing a custom serialization format.

---

## Author

Created by Dominic.
