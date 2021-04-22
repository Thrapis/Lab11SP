#pragma once

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#define HTDLL_API extern "C" __declspec (dllexport)

#define _CRT_SECURE_NO_WARNINGS
#include "windows.h"
#include "Additions.h"
#include <iostream>

namespace HT {

	struct DLL_API Element {
		void* key = nullptr;
		int keylength = 0;
		void* value = nullptr;
		int valuelength = 0;

		Element(const void* key, int keylength, const void* value, int valuelength);
		void Erase(int maxKeyLength, int maxValueLength);
	};


	HTDLL_API void printCharChar(
		const Element* element
	);

	HTDLL_API void printCharInt(
		const Element* element
	);

	HTDLL_API char* GetKeyAsChar(
		const Element* element
	);

	HTDLL_API char* GetValueAsChar(
		const Element* element
	);

	HTDLL_API int GetValueAsInt(
		const Element* element
	);
}

