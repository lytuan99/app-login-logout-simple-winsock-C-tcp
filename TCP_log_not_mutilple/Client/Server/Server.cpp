// Server.cpp : Defines the entry point for the console application.
   //

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<string>
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<winsock2.h>
#include<ws2tcpip.h>
#include<sstream>
#include<vector>
#include "Business.h"


#define PATH "../Debug/account.txt"
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048
#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32.lib")

using namespace std;

int connectWinsock(char **argv);

int main(int argc, char **argv) {

	SOCKET listenSock = connectWinsock(argv);

	// communicate with client
	sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);

	readData(PATH);
	while (1) {
		SOCKET connSock;
		connSock = accept(listenSock, (sockaddr*)&clientAddr, &clientAddrLen);
		while (1) {

			int atClient = ntohs(clientAddr.sin_port);
			int situation = receiveFromClient(connSock, PATH, atClient);
			int len = responseClient(connSock, situation);
			if (len == SOCKET_ERROR) {
				cout << "Error! Can not send message" << endl;
			}
		}
	}
	closesocket(listenSock);
	WSACleanup();
	return 0;
}

int connectWinsock(char **argv) {
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData))
		cout << "Version is not supported" << endl;

	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in serverAddr;

	int port = atoi(argv[1]);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	if (bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr))) {
		cout << "Error! Can not bind this address. " << endl;
		_getch();
		return 0;
	}

	listen(listenSock, 10);

	cout << "Server started" << endl;
	return listenSock;
}
