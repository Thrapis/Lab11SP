#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "Windows.h"

#define FILEDIR "E:\\Study\\SP\\LABS\\HT Space\\HTspaceShare.ht"

using namespace std;

int main() {
	
	try {
		HMODULE lib = LoadLibrary(L"HTAPI.dll");
		if (!lib)
			throw "LoadLibrary filed";

		//----------------------------------
		struct HashTable* (*Create)(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxValueLength, const char FileName[512]);
		(FARPROC&)Create = GetProcAddress(lib, "Create");

		BOOL (*Close)(struct HashTable* table);
		(FARPROC&)Close = GetProcAddress(lib, "Close");

		char* (*InfoOf)(struct HashTable* table);
		(FARPROC&)InfoOf = GetProcAddress(lib, "InfoOf");

		BOOL(*Insert)(struct HashTable* table, const void* key, int keylength, const void* value, int valuelength);
		(FARPROC&)Insert = GetProcAddress(lib, "Insert");
		//----------------------------------

		struct HashTable* HT = Create(2000, 3, 10, 5, FILEDIR);

		/*char key[] = "Key";
		char value[] = "Some Interesting Information ";

		for (int i = 0; i < 10; i++) {
			int kl = strlen(key);
			int vl = strlen(value);
			char integer_string[32];
			sprintf(integer_string, "%d", i);

			char* key_i = new char[kl + strlen(integer_string)]{ '\0' };;
			char* value_i = new char[vl + strlen(integer_string)]{ '\0' };;
			strcpy(key_i, key);
			strcpy(value_i, value);
			strcat(key_i, integer_string);
			strcat(value_i, integer_string);

			Insert(HT, key_i, kl + strlen(integer_string) + 1, value_i, vl + strlen(integer_string) + 1);
		}*/

		cout << "HT-Storage Created. " << InfoOf(HT) << endl;

		system("pause");

		Close(HT);

		if (!FreeLibrary(lib))
			throw "FreeLibrary filed";
	}
	catch (...) {

	}
	system("pause");
}