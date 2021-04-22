#include "Additions.h"

namespace HT {

	char* Replace(char* text, char oldval, char newval) {
		std::string string_text = text;
		std::replace(string_text.begin(), string_text.end(), oldval, newval);
		char* new_text = new char[strlen(string_text.c_str())];
		strcpy(new_text, string_text.c_str());
		return new_text;
	}

	char* Replace(char* text, const std::string from, const std::string to) {
		std::string str = text;
		size_t start_pos = str.find(from);
		str.replace(start_pos, from.length(), to);
		char* newtext = new char[strlen(str.c_str())];
		strcpy(newtext, str.c_str());
		return newtext;
	}

	char* GetFileName(const char* path) {
		std::string str = path;
		int lastSlash = str.find_last_of('\\');
		int lastDot = str.find_last_of('.');
		char* fileMapName = new char[strlen(path)]{ '\0' };
		strcat(fileMapName, str.substr(lastSlash + 1, lastDot - lastSlash - 1).c_str());
		return fileMapName;
	}

	wchar_t* ConvertCharArrayToLPCWSTR(const char* charArray)
	{
		wchar_t* wString = new wchar_t[4096];
		MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
		return wString;
	}

	int ToInt(const char* buffer) {
		return int((unsigned char)(buffer[0]) << 24 |
			(unsigned char)(buffer[1]) << 16 |
			(unsigned char)(buffer[2]) << 8 |
			(unsigned char)(buffer[3]));
	}

	LPWSTR ToLPWSTR(const char* text) {
		wchar_t buf[513];
		mbstowcs(buf, text, strlen(text) + 1);
		LPWSTR lpwstr = buf;
		return lpwstr;
	}
}