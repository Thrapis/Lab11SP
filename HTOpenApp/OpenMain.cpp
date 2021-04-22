#define DLL_API __declspec(dllexport)

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "Windows.h"
#include "../HTAPI/HashTable.h"

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

		struct HashTable* (*Open)(const char FileName[512]);
		(FARPROC&)Open = GetProcAddress(lib, "Open");

		BOOL (*Close)(struct HashTable* table);
		(FARPROC&)Close = GetProcAddress(lib, "Close");

		/*struct Element* (*Get)(struct HashTable * table, const char* key);
		(FARPROC&)Get = GetProcAddress(lib, "Get");

		void (*print)(const struct Element* element);
		(FARPROC&)print = GetProcAddress(lib, "print");*/

		char* (*InfoOf)(struct HashTable* table);
		(FARPROC&)InfoOf = GetProcAddress(lib, "InfoOf");

		void (*PrintAllElementsCharChar)(struct HashTable* table);
		(FARPROC&)PrintAllElementsCharChar = GetProcAddress(lib, "PrintAllElementsCharChar");
		//----------------------------------

		struct HashTable* HT = Open(FILEDIR);
		
		cout << "HT-Storage Start. " << InfoOf(HT) << endl;
		//PrintAllElementsCharChar(HT);

		system("pause");

		Close(HT);

		if (!FreeLibrary(lib))
			throw "FreeLibrary filed";
	}
	catch (...) {

	}
	system("pause");
}