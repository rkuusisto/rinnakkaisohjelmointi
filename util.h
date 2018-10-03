//
// Created by kuusri on 30/09/2018.
//

#pragma once
#include <windows.h>
#include <iostream>
#include "App.h"


#ifndef UNTITLED_UTIL_H
#define UNTITLED_UTIL_H

#endif //UNTITLED_UTIL_H

namespace util{
	void clearScreen();
    void clearInputArea(short height);
    void clearMessageArea(short top);
    void gotoxy (int x, int y);
    void clrscr(COORD from, COORD to, char chr);
    void hr(char chr);
    std::string getTime();
    CONSOLE_SCREEN_BUFFER_INFO getScreenBufferInfo();
	HANDLE createFileMapping(TCHAR * szName, size_t BUF_SIZE);
	LPCTSTR getFileBuffer(HANDLE hMapFile);
	HANDLE getMapFileHandle(TCHAR *szName);
	void readFromSharedMemory(App *app, TCHAR * szName);
	void saveToSharedMemory(App *app, TCHAR * szName);
	void createApplicationFileMapping(App* app, TCHAR * szName);
}
