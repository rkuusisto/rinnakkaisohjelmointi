//
// Created by kuusri on 30/09/2018.
//

#include "./stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <cstring>
#include "model/Message.h"
#include "App.h"
#include "util.h"
#include <string>
#include "mcast_windows.h"


#pragma comment(lib, "ws2_32")

#define PORT 12345
#define GROUP "10.100.0.202"                      // Multicast -osoite, jota käytetään

#define USER_IDENT "<NAME>: "                    // Käyttäjän tunniste


bool setReuseAddress(SOCKET RecvSocket)
{
	u_int yes = 1;
	if (
		setsockopt(
			RecvSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)
		) < 0
	) {
		perror("Reusing ADDR failed");
		return false;
	}
	return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DWORD WINAPI multicastListener(LPVOID data)          // Kuuntelee multicastia
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

{

	App *app = (App*)data;

    int iResult = 0;

    WSADATA wsaData;

    SOCKET RecvSocket;
    sockaddr_in RecvAddr;

    const int BufLen = 1024;
    char RecvBuf[BufLen];


    sockaddr_in SenderAddr;
    int SenderAddrSize = sizeof(SenderAddr);

	if (!initializeWinsock()) return 1;

    //-----------------------------------------------
    // Create a receiver socket to receive datagrams
	if (!createSocket(RecvSocket)) return 1;

	
	if (setReuseAddress(RecvSocket)) return 1;


	//-----------------------------------------------
    // Bind the socket to any address and the specified port.
    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(PORT);
	RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    iResult = bind(RecvSocket, (SOCKADDR *) &RecvAddr, sizeof(RecvAddr));
    if (iResult != 0) {
        wprintf(L"bind failed with error %d\n", WSAGetLastError());
        return 1;
    } else
    {
		//printf("Bound to %d", RecvAddr.sin_addr.S_un.S_addr);
    }
    //-----------------------------------------------
    // Call the recvfrom function to receive datagrams
    // on the bound socket.
    //wprintf(L"Receiving datagrams...waadsfasdf\n");

	

    while (app->running) {
		
        char msg[2048];                               // Lukupuskuri
        int addrlen = sizeof(SenderAddr);                   // Osoitekentän pituus kutsua varten



        iResult = recvfrom(RecvSocket,
                           RecvBuf, BufLen, 0, (SOCKADDR *) &SenderAddr, &addrlen);
        if (iResult == SOCKET_ERROR) {
            wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
            return 1;
        }

		COORD cursor = util::getScreenBufferInfo().dwCursorPosition;

		
		Message *m = Message::deserialize(std::string(RecvBuf));
		app->submitMessage(m);

		cursor = util::getScreenBufferInfo().dwCursorPosition;
		if (app->getLock())
		{

			app->clearMessageArea();
		
			app->renderMessages();
			util::gotoxy(cursor.X, cursor.Y);
			app->releaseLock();
		}

	
    }



    //-----------------------------------------------
    // Close the socket when finished receiving datagrams
    wprintf(L"Finished receiving. Closing socket.\n");
    iResult = closesocket(RecvSocket);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"closesocket failed with error %d\n", WSAGetLastError());
        return 1;
    }

    return 0;
}

bool initializeWinsock()
{

	int iResult;
	WSADATA wsaData;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"WSAStartup failed with error: %d\n", iResult);
		return false;
	}
	return true;
}

bool createSocket(SOCKET& sendSocket)
{
	sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sendSocket == INVALID_SOCKET) {
		wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return false;
	}
	return true;
}

void setupRecieverArgs(sockaddr_in& recieveAddress, std::string address)
{
	recieveAddress.sin_family = AF_INET;
	recieveAddress.sin_port = htons(PORT);
	recieveAddress.sin_addr.s_addr = inet_addr(address.c_str());
}

int closeSocket(SOCKET sendSocket)
{
	int iResult = closesocket(sendSocket);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"closesocket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	return 0;
}

bool sendMessage(SOCKET sendSocket, sockaddr_in recieveAddress, std::string msg)
{
	return sendto(sendSocket, msg.c_str(), strlen(msg.c_str()) + 1, 0, (struct sockaddr *) &recieveAddress, sizeof(recieveAddress)) >= 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int multicastSender(Message &message, std::string address)   // Lähettää multicastia
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

{

    SOCKET sendSocket = INVALID_SOCKET;
    sockaddr_in recieveAddress;

    if (!initializeWinsock()) return 1;

	if (!createSocket(sendSocket)) return 1;

    setupRecieverArgs(recieveAddress, address);
	
    std::string msg = message.serialize();



    if (!sendMessage(sendSocket, recieveAddress, msg))
    {
		wprintf(L"sendto failed with error: %ld\n", WSAGetLastError());
    };

    return closeSocket(sendSocket);
}