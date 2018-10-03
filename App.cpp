#include <utility>

//
// Created by kuusri on 30/09/2018.
//

#include "stdafx.h"
#include "App.h"
#include "util.h"
#include <string>

using namespace std;

void App::renderMessages()
{
	COORD cursor = util::getScreenBufferInfo().dwCursorPosition;
	util::gotoxy(1, 5);
	for (int i = messages->size() - 1; i >= 0; i--)
	{
		Message m = messages->get(i);
		printf("%s wrote at %s\n\n", m.getSender().c_str(), m.getTimestamp().c_str());
		printf("%s\n", m.getBody().c_str());
		util::hr('-');
	}

	util::gotoxy(cursor.X, cursor.Y);
}


void App::submitMessage(string str)
{
	Message* m = new Message();
	m->setSender(username);
	m->setBody(str);
	m->setTimestamp(util::getTime());

	messages->push(*m);
}

void App::init()
{
	ghMutex = CreateMutex(
		NULL, // default security attributes
		FALSE, // initially not owned
		NULL); // unnamed mutex
	if (ghMutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
	}

	util::clrscr({0, 0}, {0, 0}, ' ');

	printf("Set username: ");
	getline(cin, username);

	printf("Set remote address: ");
	getline(cin, remoteAddress);

	util::clrscr({ 0, 0 }, { 0, 0 }, ' ');

	clearInputArea();
	clearMessageArea();
	messages = new Stack<Message>(512);
}

void App::clearMessageArea()
{
	if (getLock())
	{
		util::clearMessageArea(inputHeight + 1);
	}
	else
	{
		Sleep(100);
		clearInputArea();
	}
	releaseLock();
}

void App::clearInputArea()
{
	if (getLock())
	{
		util::clearInputArea(inputHeight);
	}
	else
	{
		Sleep(100);
		clearInputArea();
	}
	releaseLock();
}

bool App::getLock()
{
	DWORD dwCount = 0, dwWaitResult;

	// Request ownership of mutex.

	while (dwCount < 20)
	{
		dwWaitResult = WaitForSingleObject(
			ghMutex, // handle to mutex
			INFINITE); // no time-out interval

		switch (dwWaitResult)
		{
		case WAIT_OBJECT_0:
			return true;

		case WAIT_ABANDONED:
			return false;
		default:
			Sleep(1);
			dwCount++;
		}
	}
	return false;
}

void App::releaseLock()
{
	ReleaseMutex(ghMutex);
}

void App::threadSafeGotoXY(int x, int y)
{
	if (getLock())
	{
		util::gotoxy(x, y);
	}
	else
	{
		Sleep(100);
		threadSafeGotoXY(x, y);
	}
	releaseLock();
}

std::string App::getUsername()
{
	return this->username;
}

void App::submitMessage(Message* message)
{
	messages->push(*message);
}

std::string App::getRemoteAddress()
{
	return this->remoteAddress;
}
