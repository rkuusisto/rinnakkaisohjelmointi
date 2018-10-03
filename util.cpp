//
// Created by kuusri on 30/09/2018.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <ctime>
#include "util.h"
#include "App.h"

namespace util {


    void clrscr(COORD from, COORD to, char chr) {
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE); // gets the window handle
        DWORD count;

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hStdOut, &csbi); // gets the buffer info (screen)

        // fill all characters as ' ' (empty the screen)
        int length;
        if (to.X == 0 && to.Y == 0) {
            length = csbi.dwSize.X * (csbi.dwSize.Y - from.Y) - from.X;
        } else {
            length = csbi.dwSize.X * (to.Y - from.Y + 1) - to.X - from.X;
        }
        FillConsoleOutputCharacter(hStdOut, chr, static_cast<DWORD>(length), from, &count);

        // resets the cursor position
        SetConsoleCursorPosition(hStdOut, from);
    }

    void hr(char chr) {

        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE); // gets the window handle
        DWORD count;
        CONSOLE_SCREEN_BUFFER_INFO csbi = getScreenBufferInfo();
        COORD coord = csbi.dwCursorPosition;

        FillConsoleOutputCharacter(hStdOut, chr, csbi.dwSize.X, coord, &count);
        SetConsoleCursorPosition(hStdOut, {1, static_cast<SHORT>(coord.Y + 1)});
    }

    CONSOLE_SCREEN_BUFFER_INFO getScreenBufferInfo(){
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE); // gets the window handle
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hStdOut, &csbi); // gets the buffer info (screen)

        return csbi;
    }

    void gotoxy(int x, int y) {
        COORD coord; // coordinates
        coord.X = x;
        coord.Y = y; // X and Y coordinates
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

	void clearScreen()
	{
		util::clrscr({ 0, 0 }, { 0, 0 }, ' ');
	}

	void clearInputArea(short height) {
        clrscr({0, 1}, {0, height}, ' ');
        gotoxy(0, height);
        hr('=');
    }

    void clearMessageArea(short top) {
        clrscr({0, top}, {0, 0}, ' ');
    }

    std::string getTime() {

        time_t rawtime;
		struct tm *timeinfo = new tm;
        char buffer[80];

        time(&rawtime);
        
		localtime_s(timeinfo, &rawtime);

        strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
        std::string str(buffer);

        return str;
    }

	HANDLE createFileMapping(TCHAR * szName, size_t BUF_SIZE)
	{
		HANDLE hMapFile = CreateFileMapping(
			INVALID_HANDLE_VALUE,    // use paging file
			NULL,                    // default security
			PAGE_READWRITE,          // read/write access
			0,                       // maximum object size (high-order DWORD)
			BUF_SIZE,                // maximum object size (low-order DWORD)
			szName);                 // name of mapping object

		if (hMapFile == NULL)
		{
			_tprintf(TEXT("Could not create file mapping object (%d).\n"),
				GetLastError());
			return nullptr;
		}
		return hMapFile;
	}

	LPCTSTR getFileBuffer(HANDLE hMapFile)
	{
		App *app;
		size_t BUF_SIZE = sizeof(*app);
		LPCTSTR pBuf;
		pBuf = (LPTSTR)MapViewOfFile(hMapFile, // handle to map object
			FILE_MAP_ALL_ACCESS,  // read/write permission
			0,
			0,
			BUF_SIZE);

		if (pBuf == NULL)
		{
			_tprintf(TEXT("Could not map view of file (%d).\n"),
				GetLastError());

			CloseHandle(hMapFile);

			return NULL;
		}

		return pBuf;
	}


	HANDLE getMapFileHandle(TCHAR *szName)
	{
		HANDLE hMapFile;
		hMapFile = OpenFileMapping(
			FILE_MAP_ALL_ACCESS,   // read/write access
			FALSE,                 // do not inherit the name
			szName);               // name of mapping object

		if (hMapFile == NULL)
		{
			_tprintf(TEXT("Could not open file mapping object (%d).\n"),
				GetLastError());
			return NULL;
		}
		return hMapFile;
	}


	void readFromSharedMemory(App *app, TCHAR * szName)
    {
		HANDLE hMapFile = util::getMapFileHandle(szName);
		LPCTSTR pBuf = util::getFileBuffer(hMapFile);

		memcpy(app, (PVOID)pBuf, sizeof(*app));

		UnmapViewOfFile(pBuf);
		CloseHandle(hMapFile);
    }

	void saveToSharedMemory(App *app, TCHAR * szName)
    {
		HANDLE hMapFile = util::getMapFileHandle(szName);
		LPCTSTR pBuf = util::getFileBuffer(hMapFile);

		memcpy((PVOID)pBuf, app, sizeof(*app));

		UnmapViewOfFile(pBuf);
		CloseHandle(hMapFile);
    }

	void createApplicationFileMapping(App* app, TCHAR * szName)
    {
		size_t BUF_SIZE = sizeof(*app);

		HANDLE hMapFile = util::createFileMapping(szName, BUF_SIZE);
		LPCTSTR pBuf = util::getFileBuffer(hMapFile);

		memcpy((PVOID)pBuf, app, BUF_SIZE);

		UnmapViewOfFile(pBuf);
		CloseHandle(hMapFile);
    }

}
