
#include <iostream>
#include <cstdio>
#include "main.h"
#include <sstream>
#include "util.h"
#include "App.h"
#include "mcast_windows.h"
#include <mutex>         
#include <condition_variable> 
#include <tchar.h>

#define MAX_SEM_COUNT 10
#define STATUS_GUARD_PAGE_VIOLATION      ((DWORD   )0x80000001L)



using namespace std;


bool readInput(App *app) {
    app->clearInputArea();
	
	if(app->getLock())
	{
		util::gotoxy(1, 1);
		printf("Write message: ");
		app->releaseLock();
	} else
	{
		readInput(app);
	}

    

    string str;

    getline(cin, str);

    if (str.empty()) {
        return false;
    }

	TCHAR szName[] = TEXT("MyFileMappingObject");
	HANDLE tsem = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, _T("AppSem"));
	util::readFromSharedMemory(app, szName);
	CloseHandle(tsem);

	


    Message *m = Message::create(app->getUsername(), str);
	app->submitMessage(m);


	if(app->getLock())
	{
		app->clearMessageArea();
		util::gotoxy(16, 1);
		app->renderMessages();
		app->releaseLock();
	}
	
	
    multicastSender(*m, app->getRemoteAddress());

	

    return true;
}

DWORD WINAPI inputFunc(LPVOID data){

	TCHAR szName[] = TEXT("MyFileMappingObject");
	App *app = new App();

	HANDLE tsem = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, _T("AppSem"));
	util::readFromSharedMemory(app, szName);
	CloseHandle(tsem);

    while (true) {
				

        bool cont = readInput(app);
        if(!cont){
            break;
        }
    }
    return 0;
}

DWORD WINAPI receiveFunc(LPVOID data){

	//Init phase
	HANDLE tsem = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, _T("AppSem"));

	TCHAR szName[] = TEXT("MyFileMappingObject");

    util::gotoxy(0, 0);
    printf("Waiting for messages...");
    //COORD coord = util::getScreenBufferInfo().dwCursorPosition;
    //util::gotoxy(coord.X, coord.Y);

	App *app = new App();

	

	util::readFromSharedMemory(app, szName);

    HANDLE rxThread = CreateThread(NULL, 0, &multicastListener, app, 0, NULL);
	
	CloseHandle(tsem);

	//Init complete

    while (app->running){
        Sleep(100);
    }

    app->clearInputArea();

    util::gotoxy(0, 0);

    printf("Quit signal received...\n\n");
    Sleep(1000);

    app->clearMessageArea();


    WaitForSingleObject(rxThread, INFINITE);

    printf("Quiting now");
    Sleep(3000);
    util::clrscr({0,0}, {0,0}, ' ');

    return 0;
}

int main() {



	TCHAR szName[] = TEXT("MyFileMappingObject");
	

    App *app = new App();
    app->inputHeight = 3;
    app->running = true;

    app->init();

	HANDLE ghSemaphore = CreateSemaphore(
		NULL,           // default security attributes
		MAX_SEM_COUNT,  // initial count
		MAX_SEM_COUNT,  // maximum count
		_T("AppSem"));          // unnamed semaphore

	if (ghSemaphore == NULL)
	{
		printf("CreateSemaphore error: %d\n", GetLastError());
		return 1;
	}
	
	//util::createApplicationFileMapping(app, szName);


	size_t BUF_SIZE = sizeof(*app);

	HANDLE hMapFile = util::createFileMapping(szName, BUF_SIZE);
	LPCTSTR pBuf = util::getFileBuffer(hMapFile);

	memcpy((PVOID)pBuf, app, BUF_SIZE);




	CloseHandle(ghSemaphore);
    

	HANDLE receiveThread = CreateThread(nullptr, 0, &receiveFunc, NULL, 0, nullptr);
    HANDLE inputThread = CreateThread(nullptr, 0, &inputFunc, NULL, 0, nullptr);

	

    WaitForSingleObject(inputThread, INFINITE);

	util::readFromSharedMemory(app, szName);

	app->running = false;
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	CloseHandle(receiveThread);
    //WaitForSingleObject(receiveThread, INFINITE);

    return 0;
}