// Client.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"

#include<iostream>
#include<string>
#include<string.h>
#include<stdio.h>
#include<conio.h>
#include<winsock2.h>
#include<ws2tcpip.h>
#include<stdlib.h>
#include<sstream>
#include "UI.h"
#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32.lib")

using namespace std;


char serverIpAddress[100];
int serverPort = 0;
int connectWinsockAndServer(char **argv);

int main(int argc, char **argv)
{
	SOCKET client = connectWinsockAndServer( argv);
	bool checkLoop = false;
	do {
 again:	menuUI();
		int choice = enterChoice();
		if (haveActiveAcount && choice == 1) {
			cout << "There is currently an active account!! you cannot login!!!" << endl;
			goto again;
		}
		if (!haveActiveAcount && choice == 0) {
			cout << "There is NO currently an active account!! you cannot logout!!!" << endl;
			goto again;
		}
		checkLoop = handleUserChoice(choice, client);
	} while (!checkLoop);
	closesocket(client);
	WSACleanup();
	return 0;
}


int connectWinsockAndServer(char **argv) {
	WSADATA wsaData;
	SOCKET client;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData))
		cout << "Version is not supported" << endl;
	// step 2: construct socket
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// enter addressIp and port Server
	strcpy(serverIpAddress, argv[1]);
	serverPort = atoi(argv[2]);
	int tv = 10000;
	setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(int));

	// step 3: Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);
	serverAddr.sin_addr.s_addr = inet_addr(serverIpAddress);

	// Resquest Connect to Server
	if (connect(client, (sockaddr*)&serverAddr, sizeof(serverAddr))) {
		cout << "Error! Cannot connect server" << endl;
		return 0;
	}
	cout << "connected with Server!" << endl;
	return client;
}


