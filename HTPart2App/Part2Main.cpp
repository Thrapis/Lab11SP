#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "Windows.h"
#include "../HTAPI/HashTable.h"

#define FILEDIR "E:\\Study\\SP\\LABS\\HT Space\\HTspaceShare.ht"

using namespace std;

char* ToCharArray(int num) {
	char* bytes = new char[sizeof(num)]{ '\0' };
	bytes[0] = (num >> 24) & 0xFF;
	bytes[1] = (num >> 16) & 0xFF;
	bytes[2] = (num >> 8) & 0xFF;
	bytes[3] = num & 0xFF;
	return bytes;
}

int ToInt(const char* buffer) {
	return int((unsigned char)(buffer[0]) << 24 |
		(unsigned char)(buffer[1]) << 16 |
		(unsigned char)(buffer[2]) << 8 |
		(unsigned char)(buffer[3]));
}

int main() {

	try {
		HMODULE lib = LoadLibrary(L"HTAPI.dll");
		if (!lib)
			throw "LoadLibrary filed";

		//----------------------------------
		struct HashTable* (*Open)(const char FileName[512]);
		(FARPROC&)Open = GetProcAddress(lib, "Open");

		BOOL(*Close)(struct HashTable* table);
		(FARPROC&)Close = GetProcAddress(lib, "Close");

		BOOL(*Insert)(struct HashTable* table, const void* key, int keylength, const void* value, int valuelength);
		(FARPROC&)Insert = GetProcAddress(lib, "Insert");

		void (*PrintAllElementsCharInt)(struct HashTable* table);
		(FARPROC&)PrintAllElementsCharInt = GetProcAddress(lib, "PrintAllElementsCharInt");

		char* (*InfoOf)(struct HashTable* table);
		(FARPROC&)InfoOf = GetProcAddress(lib, "InfoOf");

		struct Element* (*Get)(struct HashTable * table, const void* key, int valuelength);
		(FARPROC&)Get = GetProcAddress(lib, "Get");

		void (*printCharInt)(const struct Element* element);
		(FARPROC&)printCharInt = GetProcAddress(lib, "printCharInt");
		//----------------------------------

		struct HashTable* HT;

		try {
			HT = Open(FILEDIR);
			cout << "HT-Storage Opened. " << InfoOf(HT) << endl;

			char key[] = "Key";

			for (int i = 0; i < 50; i++) {
				int kl = strlen(key);
				int vl = sizeof i;
				char integer_string[32];
				sprintf(integer_string, "%d", i);

				char* key_i = new char[kl] { '\0' };
				char* value_i = ToCharArray(0);
				memcpy(key_i, key, kl + 1);
				strcat(key_i, integer_string);

				Insert(HT, key_i, kl + strlen(integer_string) + 1, value_i, sizeof(int) + 1);

				cout << "Added new Element: ";
				printCharInt(Get(HT, key_i, kl + strlen(integer_string) + 1));

				Sleep(1000);
			}

			PrintAllElementsCharInt(HT);

			system("pause");
			if (HT != NULL)
				Close(HT);
		}
		catch (...) {}
		

		if (!FreeLibrary(lib))
			throw "FreeLibrary filed";
	}
	catch (...) {

	}
	system("pause");
}