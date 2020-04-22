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
#include <thread>
#include "Business.h"


#define PATH "../Debug/account.txt"
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048
#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main(int argc, char **argv) {

	SOCKET listenSock = connectWinsock(argv);

	// communicate with client

	readData(PATH);
	mainThread(listenSock, PATH);
	closesocket(listenSock);
	WSACleanup();
	return 0;
}

