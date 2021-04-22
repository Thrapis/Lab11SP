#include "HashTable.h"

namespace HT {

	DLL_API byte* HashTable::GetMemorySpaceByIndex(int inIndex)
	{
		return (byte*)this->StartPoint + (sizeof(int) * 4 + inIndex * (sizeof(Element) + this->MaxKeyLength + this->MaxValueLength));
	}

	DLL_API Element* HashTable::ConvertPointerToElement(byte* inPointer)
	{
		Element* result = (Element*)inPointer;
		result->key = (char*)(inPointer + sizeof(Element));
		result->value = (char*)(inPointer + sizeof(Element) + this->MaxKeyLength);
		return result;
	}

	DLL_API byte* HashTable::GetFreeMemoryForElement()
	{
		byte* pointer;
		int count_of_elements = 0;
		Element* temp_el;
		while (count_of_elements < this->Capacity)
		{
			pointer = GetMemorySpaceByIndex(count_of_elements);
			temp_el = (Element*)pointer;
			if (temp_el->keylength == SPCFILL)
			{
				return pointer;
			}

			count_of_elements++;
		}
		this->SetLastError("Getting memory for element failed. There is no free memory to allocate");
		return NULL;
	}

	DLL_API byte* HashTable::FindElement(const void* inKey, int inKeyLenght)
	{
		char* charKey = new char[inKeyLenght] { '\0' };;
		memcpy(charKey, inKey, inKeyLenght);

		byte* pointer;
		int count_of_elements = 0;
		Element* tempEl;
		while (count_of_elements < this->Capacity)
		{
			pointer = GetMemorySpaceByIndex(count_of_elements);
			if (pointer == NULL)
				return NULL;
			tempEl = ConvertPointerToElement(pointer);

			char* tempCharKey = new char[inKeyLenght] { '\0' };;
			memcpy(tempCharKey, tempEl->key, tempEl->keylength);

			if (tempEl->keylength != SPCFILL && strcmp(charKey, tempCharKey) == 0)
			{
				return pointer;
			}
			else if (tempEl->keylength == SPCFILL) {
				tempEl->Erase(this->MaxKeyLength, this->MaxValueLength);
			}

			count_of_elements++;
		}
		this->SetLastError("Finding element failed. There is no element with the same key");
		return NULL;
	}

	DLL_API BOOL HashTable::Snap()
	{
		char* mutex_name = Replace((char*)FileName, '\\', '_');
		strcat(mutex_name, "_mutex");

		Mutex = CreateMutex(NULL, FALSE, ConvertCharArrayToLPCWSTR(mutex_name));
		WaitForSingleObject(Mutex, INFINITE);

		int totalHtSize = sizeof(HashTable) + Capacity * (sizeof(Element) + MaxKeyLength + MaxValueLength);
		if (FlushViewOfFile(StartPoint, totalHtSize))
		{
			//getting time
			time(this->LastSnapShotTime);

			//formatting time
			char buffer[80] = "";
			time_t* time_ = new time_t();
			time(time_);
			tm* current_time = localtime(time_);

			char subbuf[30] = { '\0' };
			strcat(buffer, "_");
			sprintf(subbuf, "%d", current_time->tm_mday);
			strcat(buffer, subbuf);
			strcat(buffer, "-");
			sprintf(subbuf, "%d", current_time->tm_mon + 1);
			strcat(buffer, subbuf);
			strcat(buffer, "-");
			sprintf(subbuf, "%d", current_time->tm_year + 1900);
			strcat(buffer, subbuf);
			strcat(buffer, "_");
			sprintf(subbuf, "%d", current_time->tm_hour);
			strcat(buffer, subbuf);
			strcat(buffer, "-");
			sprintf(subbuf, "%d", current_time->tm_min);
			strcat(buffer, subbuf);
			strcat(buffer, "-");
			sprintf(subbuf, "%d", current_time->tm_sec);
			strcat(buffer, subbuf);
			strcat(buffer, ".");

			printf("\nSync Snap at %s\n", buffer);

			//creating new file's path
			char* new_file_snap = Replace((char*)this->FileName, '.', '.');
			new_file_snap = Replace(new_file_snap, ".", buffer); 

			//creating new snap file 
			wchar_t buf[513];
			mbstowcs(buf, new_file_snap, strlen(new_file_snap) + 1);
			LPWSTR Path = buf;
			HANDLE file_handler = NULL;
			file_handler = CreateFile(Path, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (file_handler == INVALID_HANDLE_VALUE)
			{
				this->SetLastError("HashTable snapping failed. File is not opened");
				ReleaseMutex(Mutex);
				return false;
			}
			else
			{
				//writing memory into created snap file 
				DWORD written = -1;
				int ht_size = sizeof(HashTable) + Capacity * (sizeof(Element) + MaxKeyLength + MaxValueLength);
				if (!WriteFile(file_handler, StartPoint, ht_size, &written, NULL))
				{
					this->SetLastError("HashTable snapping failed. Copying is failed");
					ReleaseMutex(Mutex);
					return false;
				}
			}
			ReleaseMutex(Mutex);
			return true;
		}
		else
		{
			ReleaseMutex(Mutex);
			return false;
		}
	}

	DLL_API DWORD WINAPI PeriodicSnapShot(LPVOID inHashTable)
	{
		HashTable* table = (HashTable*)inHashTable;

		while (true) {
			Sleep(table->SecSnapshotInterval * 1000);
			//if (table->BreakToken)
			//	return 0;
			table->Snap();
		}
		return 0;
	}

	DLL_API void HashTable::SetLastError(const char* error) {
		this->LastErrorMessage = new char[strlen(error) + 1];
		strcpy((char*)this->LastErrorMessage, error);
	}

	DLL_API char* HashTable::GetLastError() {
		if (this->LastErrorMessage == NULL) {
			char elseex[] = "No Error Messages";
			return elseex;
		}

		char res[1024] = "";
		strcpy(res, "--- Error: ");
		strcat(res, (char*)this->LastErrorMessage);
		strcat(res, " ---");
		return (char*)res;
	}

	//------------------------------------------------------

	HashTable* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxValueLength, const char FileName[512]) {

		HashTable* table;

		HANDLE fileh;
		HANDLE fileMaph;
		LPVOID mapViewPointer;

		bool mappingExists = false;

		fileMaph = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, ConvertCharArrayToLPCWSTR(GetFileName(FileName)));
		if (!fileMaph)
		{
			fileh = CreateFileA(FileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

			if (fileh == INVALID_HANDLE_VALUE)
			{
				throw "HashTable creation failed. File is not created";
			}

			int fileMaxSize = (sizeof(Element) + MaxKeyLength + MaxValueLength) * Capacity + sizeof(HashTable);

			int i = 0;
			LPDWORD d = 0;
			while (i < fileMaxSize) {
				WriteFile(fileh, "\0", 1, d, NULL);
				i++;
			}

			fileMaph = CreateFileMapping(fileh, nullptr, PAGE_READWRITE, 0, 0, ConvertCharArrayToLPCWSTR(GetFileName(FileName)));
			if (!fileMaph)
			{
				throw "HashTable creation failed. FileMapping is not created";
			}

			mapViewPointer = MapViewOfFile(fileMaph, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (!mapViewPointer)
			{
				throw "HashTable creation failed. MapView is not created";
			}
		}
		else {
			mapViewPointer = MapViewOfFile(fileMaph, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (!mapViewPointer)
			{
				throw "HashTable creation failed. MapView is not created";
			}
			mappingExists = true;
		}
		
		//table = (HashTable*)((byte*)mapViewPointer);

		table = new HashTable();
		table->FileMapping = &fileMaph;
		table->StartPoint = (byte*)mapViewPointer;
		memcpy(table->FileName, FileName, strlen(FileName));

		if (!mappingExists) {
			table->Capacity = Capacity;
			table->MaxKeyLength = MaxKeyLength;
			table->MaxValueLength = MaxValueLength;
			table->SecSnapshotInterval = SecSnapshotInterval;
			table->File = &fileh;

			memcpy((byte*)mapViewPointer, &table->Capacity, sizeof(int));
			memcpy((byte*)mapViewPointer + sizeof(int), &table->SecSnapshotInterval, sizeof(int));
			memcpy((byte*)mapViewPointer + sizeof(int) * 2, &table->MaxKeyLength, sizeof(int));
			memcpy((byte*)mapViewPointer + sizeof(int) * 3, &table->MaxValueLength, sizeof(int));
		}
		else {
			memcpy(&table->Capacity, (byte*)mapViewPointer, sizeof(int));
			memcpy(&table->SecSnapshotInterval, (byte*)mapViewPointer + sizeof(int), sizeof(int));
			memcpy(&table->MaxKeyLength, (byte*)mapViewPointer + sizeof(int) * 2, sizeof(int));
			memcpy(&table->MaxValueLength, (byte*)mapViewPointer + sizeof(int) * 3, sizeof(int));
		}

		DWORD timer_thread_id;
		table->TimerThreadHandler = (HANDLE*)CreateThread(NULL, NULL, PeriodicSnapShot, table, NULL, &timer_thread_id);
		
		std::cout << table->StartPoint << " " << table->FileName << std::endl;

		return table;
	}

	HashTable* Open(const char FileName[512]) {

		HashTable* table;

		HANDLE fileh;
		HANDLE fileMaph;
		LPVOID mapViewPointer;

		bool mappingExists = false;

		fileMaph = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, ConvertCharArrayToLPCWSTR(GetFileName(FileName)));
		if (!fileMaph)
		{
			fileh = CreateFileA(FileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

			if (fileh == INVALID_HANDLE_VALUE)
			{
				throw "HashTable creation failed. File is not created";
			}

			fileMaph = CreateFileMapping(fileh, nullptr, PAGE_READWRITE, 0, 0, ConvertCharArrayToLPCWSTR(GetFileName(FileName)));
			if (!fileMaph)
			{
				throw "HashTable creation failed. FileMapping is not created";
			}

			mapViewPointer = MapViewOfFile(fileMaph, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (!mapViewPointer)
			{
				throw "HashTable creation failed. MapView is not created";
			}
		}
		else {
			mapViewPointer = MapViewOfFile(fileMaph, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (!mapViewPointer)
			{
				throw "HashTable creation failed. MapView is not created";
			}
			mappingExists = true;
		}

		//table = (HashTable*)((byte*)mapViewPointer);
		
		table = new HashTable();
		table->FileMapping = &fileMaph;
		table->StartPoint = (byte*)mapViewPointer;
		memcpy(table->FileName, FileName, strlen(FileName));

		memcpy(&table->Capacity, (byte*)mapViewPointer, sizeof(int));
		memcpy(&table->SecSnapshotInterval, (byte*)mapViewPointer + sizeof(int), sizeof(int));
		memcpy(&table->MaxKeyLength, (byte*)mapViewPointer + sizeof(int) * 2, sizeof(int));
		memcpy(&table->MaxValueLength, (byte*)mapViewPointer + sizeof(int) * 3, sizeof(int));

		if (!mappingExists)
			table->File = &fileh;
		

		DWORD timer_thread_id;
		table->TimerThreadHandler = (HANDLE*)CreateThread(NULL, NULL, PeriodicSnapShot, table, NULL, &timer_thread_id);

		std::cout << table->StartPoint << " " << table->FileName << std::endl;

		return table;
	}

	BOOL Close(HashTable* table) {
		try {
			//table->BreakToken = true;
			DWORD d = 0;
			TerminateThread(table->TimerThreadHandler, d);
			table->Snap();
			UnmapViewOfFile(table->StartPoint);
			return true;
		}
		catch (char* ex) {
			return false;
		}
	}

	BOOL Insert(HashTable* table, const Element* element) {

		if (element->keylength > table->MaxKeyLength || element->valuelength > table->MaxValueLength) {
			table->SetLastError("Insertion failed. Key or value length greater then max border");
			return false;
		}
		
		char* mutex_name = Replace(table->FileName, '\\', '_');
		strcat(mutex_name, "_mutex");
		LPWSTR mutex_name_l = ConvertCharArrayToLPCWSTR(mutex_name);

		table->Mutex = CreateMutex(NULL, FALSE, mutex_name_l);
		WaitForSingleObject(table->Mutex, INFINITE);

		byte* pointer = table->GetFreeMemoryForElement();
		if (pointer == NULL)
			return false;
		Element* new_element = table->ConvertPointerToElement(pointer);

		memcpy(new_element->key, element->key, element->keylength);
		memcpy(new_element->value, element->value, element->valuelength);
		new_element->keylength = element->keylength;
		new_element->valuelength = element->valuelength;

		ReleaseMutex(table->Mutex);

		return true;
	}

	BOOL Insert(HashTable* table, const void* key, int keylength, const void* value, int valuelength) {

		if (keylength > table->MaxKeyLength || valuelength > table->MaxValueLength) {
			table->SetLastError("Insertion failed. Key or value length greater then max border");
			return false;
		}

		char* mutex_name = Replace(table->FileName, '\\', '_');
		strcat(mutex_name, "_mutex");
		LPWSTR mutex_name_l = ConvertCharArrayToLPCWSTR(mutex_name);

		table->Mutex = CreateMutex(NULL, FALSE, mutex_name_l);
		WaitForSingleObject(table->Mutex, INFINITE);
		
		byte* pointer = table->GetFreeMemoryForElement();
		if (pointer == NULL)
			return false;
		Element* new_element = table->ConvertPointerToElement(pointer);

		memcpy(new_element->key, key, keylength);
		memcpy(new_element->value, value, valuelength);
		new_element->keylength = keylength;
		new_element->valuelength = valuelength;

		ReleaseMutex(table->Mutex);

		return true;
	}

	BOOL Delete(HashTable* table, const void* key, int keylength) {

		if (keylength > table->MaxKeyLength) {
			table->SetLastError("Deletion failed. Key length greater then max border");
			return false;
		}

		char* mutex_name = Replace(table->FileName, '\\', '_');
		strcat(mutex_name, "_mutex");
		LPWSTR mutex_name_l = ConvertCharArrayToLPCWSTR(mutex_name);

		table->Mutex = CreateMutex(NULL, FALSE, mutex_name_l);
		WaitForSingleObject(table->Mutex, INFINITE);

		Element* el = Get(table, key, keylength);
		if (el != nullptr) {
			el->Erase(table->MaxKeyLength, table->MaxValueLength);
			ReleaseMutex(table->Mutex);
			return true;
		}

		ReleaseMutex(table->Mutex);
		return false;
	}

	Element* Get(HashTable* table, const void* key, int keylength) {
		
		byte* point = table->FindElement(key, keylength);
		if (point == NULL)
			return NULL;
		Element* el = table->ConvertPointerToElement(point);
		return el;
	}

	BOOL Update(HashTable* table, const void* key, int keylength, const void* newvalue, int newvaluelength) {
		if (newvaluelength > table->MaxValueLength)
			return false;

		char* mutex_name = Replace(table->FileName, '\\', '_');
		strcat(mutex_name, "_mutex");
		LPWSTR mutex_name_l = ConvertCharArrayToLPCWSTR(mutex_name);

		table->Mutex = CreateMutex(NULL, FALSE, mutex_name_l);
		WaitForSingleObject(table->Mutex, INFINITE);
		
		Element* el = Get(table, key, keylength);
		if (el == NULL)
			return false;
		memcpy(el->value, newvalue, newvaluelength);
		el->valuelength = newvaluelength;

		ReleaseMutex(table->Mutex);

		return true;
	}

	HTDLL_API char* InfoOf(HashTable* table)
	{
		char* info = new char[1024] {"Filename={0}, SecSnapshotInterval={1}, Capacity={2}, MaxKeyLength={3}, MaxValueLength={4}"};
		char integer_string[32];
		info = Replace(info, "{0}", table->FileName);
		sprintf(integer_string, "%d", table->SecSnapshotInterval);
		info = Replace(info, "{1}", integer_string);
		sprintf(integer_string, "%d", table->Capacity);
		info = Replace(info, "{2}", integer_string);
		sprintf(integer_string, "%d", table->MaxKeyLength);
		info = Replace(info, "{3}", integer_string);
		sprintf(integer_string, "%d", table->MaxValueLength);
		info = Replace(info, "{4}", integer_string);
		return info;
	}

	void PrintAllElementsCharChar(HashTable* table) {
		byte* pointer;
		int count_of_elements = 0;
		Element* tempEl;
		
		char* mutex_name = Replace(table->FileName, '\\', '_');
		strcat(mutex_name, "_mutex");
		LPWSTR mutex_name_l = ConvertCharArrayToLPCWSTR(mutex_name);

		table->Mutex = CreateMutex(NULL, FALSE, mutex_name_l);
		WaitForSingleObject(table->Mutex, INFINITE);

		std::cout << "-------------------------" << std::endl;
		while (count_of_elements < table->Capacity)
		{
			pointer = table->GetMemorySpaceByIndex(count_of_elements);
			if (pointer == NULL) {
				std::cout << "-------------------------" << std::endl;
				return;
			}

			tempEl = table->ConvertPointerToElement(pointer);
			if (tempEl->keylength != SPCFILL)
			{
				char* key = new char[tempEl->keylength]{ '\0' };;
				char* value = new char[tempEl->valuelength]{ '\0' };;
				memcpy(key, tempEl->key, tempEl->keylength);
				memcpy(value, tempEl->value, tempEl->valuelength);
				std::cout << "Element: " << key << " (" << tempEl->keylength << ") "
					<< "- " << value << " (" << tempEl->valuelength << ") " << std::endl;
			}
			else {
				tempEl->Erase(table->MaxKeyLength, table->MaxValueLength);
			}

			count_of_elements++;
		}
		std::cout << "-------------------------" << std::endl;

		ReleaseMutex(table->Mutex);

		return;
	}

	void PrintAllElementsCharInt(HashTable* table) {
		byte* pointer;
		int count_of_elements = 0;
		Element* tempEl;

		char* mutex_name = Replace(table->FileName, '\\', '_');
		strcat(mutex_name, "_mutex");
		LPWSTR mutex_name_l = ConvertCharArrayToLPCWSTR(mutex_name);

		table->Mutex = CreateMutex(NULL, FALSE, mutex_name_l);
		WaitForSingleObject(table->Mutex, INFINITE);
		
		std::cout << "-------------------------" << std::endl;
		while (count_of_elements < table->Capacity)
		{
			pointer = table->GetMemorySpaceByIndex(count_of_elements);
			if (pointer == NULL) {
				std::cout << "-------------------------" << std::endl;
				return;
			}

			tempEl = table->ConvertPointerToElement(pointer);
			if (tempEl->keylength != SPCFILL)
			{
				char* key = new char[tempEl->keylength]{ '\0' };;
				char* value = new char[tempEl->valuelength]{ '\0' };;
				memcpy(key, tempEl->key, tempEl->keylength);
				memcpy(value, tempEl->value, tempEl->valuelength);
				std::cout << "Element: " << key << " (" << tempEl->keylength << ") "
					<< "- " << ToInt(value) << " (" << tempEl->valuelength << ") " << std::endl;
			}
			else {
				tempEl->Erase(table->MaxKeyLength, table->MaxValueLength);
			}

			count_of_elements++;
		}
		std::cout << "-------------------------" << std::endl;

		ReleaseMutex(table->Mutex);

		return;
	}
}