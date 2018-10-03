#pragma once

#include <windows.h>
#include "model/Message.h"


DWORD WINAPI multicastListener(LPVOID data);
int multicastSender(Message &message, std::string address);
bool initializeWinsock();
bool createSocket(SOCKET& sendSocket);
void setupRecieverArgs(sockaddr_in& recieveAddress, std::string address);
int closeSocket(SOCKET sendSocket);
bool sendMessage(SOCKET sendSocket, sockaddr_in recieveAddress, std::string msg);