#pragma once

#define HTDLL_API extern "C" __declspec (dllexport)

#define _CRT_SECURE_NO_WARNINGS
#include "windows.h"
#include <iostream>
#include "string.h"
#include <algorithm>

namespace HT {

	char* Replace(char* text, char oldval, char newval);

	char* Replace(char* text, const std::string from, const std::string to);

	HTDLL_API char* GetFileName(const char* path);

	HTDLL_API wchar_t* ConvertCharArrayToLPCWSTR(const char* charArray);

	int ToInt(const char* buffer);
	
	LPWSTR ToLPWSTR(const char* text);
}
