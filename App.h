//
// Created by kuusri on 30/09/2018.
//

#ifndef UNTITLED_APP_H
#define UNTITLED_APP_H


#include "model/Message.h"
#include "model/Stack.h"
#include <windows.h>


class App {
	Stack<Message> * messages = nullptr;
	HANDLE ghMutex;
	std::string username;
	std::string remoteAddress;

public:
	short inputHeight = 4;
	bool running;

    void init();
    void renderMessages();
    void submitMessage(std::string str);
    void submitMessage(Message *message);

    void clearMessageArea();

    void clearInputArea();

	bool getLock();
	void releaseLock();

	void threadSafeGotoXY(int x, int y);

	std::string getUsername();

	std::string getRemoteAddress();
    
};


#endif //UNTITLED_APP_H
