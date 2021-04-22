#include "Element.h"


namespace HT {

	Element::Element(const void* key, int keylength, const void* value, int valuelength) {

		this->key = new char[keylength] { '\0' };
		this->value = new char[valuelength] { '\0' };
		memcpy(this->key, key, keylength);
		memcpy(this->value, value, valuelength);

		this->keylength = keylength;
		this->valuelength = valuelength;
	}

	void Element::Erase(int maxKeyLength, int maxValueLength) {
		int elsize = sizeof(Element) + maxKeyLength + maxValueLength;
		char* removed = new char[elsize] { '\0' };
		memcpy(this, removed, elsize);
	}

	void HT::printCharChar(const Element* element) {
		char* key = new char[element->keylength]{ '\0' };
		char* value = new char[element->valuelength]{ '\0' };
		memcpy(key, element->key, element->keylength);
		memcpy(value, element->value, element->valuelength);
		std::cout << std::endl << "----------------------------------------------" << std::endl;
		std::cout << "Key: " << key << std::endl;
		std::cout << "Key Length: " << element->keylength << std::endl;
		std::cout << "Value: " << value << std::endl;
		std::cout << "Value Length: " << element->valuelength << std::endl;
		std::cout << "----------------------------------------------" << std::endl;
	}

	void HT::printCharInt(const Element* element) {
		char* key = new char[element->keylength]{ '\0' };
		char* value = new char[element->valuelength]{ '\0' };
		memcpy(key, element->key, element->keylength);
		memcpy(value, element->value, element->valuelength);
		std::cout << std::endl << "----------------------------------------------" << std::endl;
		std::cout << "Key: " << key << std::endl;
		std::cout << "Key Length: " << element->keylength << std::endl;
		std::cout << "Value: " << ToInt(value) << std::endl;
		std::cout << "Value Length: " << element->valuelength << std::endl;
		std::cout << "----------------------------------------------" << std::endl;
	}

	char* HT::GetKeyAsChar(const Element* element) {
		char* key = new char[element->keylength]{ '\0' };
		memcpy(key, element->key, element->keylength);
		return key;
	}

	char* HT::GetValueAsChar(const Element* element) {
		char* value = new char[element->valuelength]{ '\0' };
		memcpy(value, element->value, element->valuelength);
		return value;
	}

	int HT::GetValueAsInt(const Element* element) {
		char* value = new char[element->valuelength]{ '\0' };
		memcpy(value, element->value, element->valuelength);
		return ToInt(value);
	}
}

