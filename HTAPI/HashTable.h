#pragma once

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#define HTDLL_API extern "C" __declspec (dllexport)

#define GuardedMutex std::lock_guard<std::mutex>

#define _CRT_SECURE_NO_WARNINGS
#include "windows.h"
#include <iostream>
#include <ctime>
#include "Element.h"
#include "Additions.h"
#include <mutex>

#define SPCFILL 0

namespace HT    // HT API
{
	struct DLL_API HashTable    // блок управления HT
	{
		int     Capacity;
		int     SecSnapshotInterval;
		int     MaxKeyLength;
		int     MaxValueLength; 

		void* StartPoint;
		HANDLE* File;
		HANDLE* FileMapping;
		char FileName[512];
		HANDLE Mutex;
		time_t*  LastSnapShotTime;
		char*	LastErrorMessage;
		HANDLE*	TimerThreadHandler;

		//bool BreakToken = false;

		BOOL Snap();
		void SetLastError(const char* error);
		char* GetLastError();

		Element* ConvertPointerToElement(byte* inPointer);
		byte* GetFreeMemoryForElement();
		byte* FindElement(const void* inKey, int inKeyLenght);
		byte* GetMemorySpaceByIndex(int inIndex);
	};

	HTDLL_API void PrintAllElementsCharChar(HashTable* table);

	HTDLL_API void PrintAllElementsCharInt(HashTable* table);

	HTDLL_API DWORD WINAPI PeriodicSnapShot(LPVOID inHashTable);

	HTDLL_API HashTable* Create
	(
		int	  Capacity,
		int   SecSnapshotInterval,
		int   MaxKeyLength,
		int   MaxValueLength,
		const char  FileName[512]
	); 	// != NULL успешное завершение  

	HTDLL_API HashTable* Open
	(
		const char  FileName[512]
	); 	// != NULL успешное завершение  


	HTDLL_API BOOL Close
	(
		HashTable* table
	);	//  == TRUE успешное завершение   


	BOOL Insert
	(
		HashTable* table,
		const Element* element
	);	//  == TRUE успешное завершение 

	HTDLL_API BOOL Insert
	(
		HashTable* table,
		const void* key,
		int keylength,
		const void* value,
		int valuelength
	);

	HTDLL_API BOOL Delete
	(
		HashTable* table,
		const void* key,
		int keylength
	);	//  == TRUE успешное завершение 

	HTDLL_API Element* Get
	(
		HashTable* table,
		const void* key,
		int keylength
	); 	//  != NULL успешное завершение 


	HTDLL_API BOOL Update
	(
		HashTable* table,
		const void* key,
		int keylength,
		const void* newvalue,
		int newvaluelength
	); 	//  != NULL успешное завершение 

	HTDLL_API char* InfoOf
	(
		HashTable* table
	); 	//  != NULL успешное завершение 
};
