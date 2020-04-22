#pragma once
#include "DataIO.h"



enum {
	LOGOUT = 100,
	LOGIN = 101,
	PASS_FALSE = 102,
	NOT_FOUND_ACCOUNT = 103,
	LOCKED = 104,
	LIVE = 105,
	SUCCESS_LOGIN = 106,
	SUCCESS_LOGOUT = 107,
	ERROR_LOGOUT = 108,
	HASBEENLOCKING = 109
};


#define ERROR_CONNECT -1
#define BUFF_SIZE 2048
#define ACCOUNT_SIZE 256
#define MAX_USER 100
#define MAX_LOGIN 1000
#define MAX_FALSE_LOGINED_TIMES 3
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048
#define NOT_LOGIN -1
typedef enum { ACTIVE = 0, INACTIVE = 1 }Status;
User users[MAX_USER];
int numbersOfUser;
typedef struct {
	string userName;
	int atClient;
}LOGINED;

vector<LOGINED> managerLogin;

void readData(string path);
void mainThread(SOCKET listenSock, string path);
void acceptClient(SOCKET listenSock, string path);
void communicate(SOCKET connSock, string path, int atClient);
int receiveFromClient(SOCKET conSock, string path, int atClient);
int responseClient(SOCKET conSock, int situation);
int checkAccount(char* userName, char *pass);
void addToListLogined(char *userName, int atClient);
int handleLogout(char *userName, int atClient);
int handleLogin(char *userName, int situation, string path, int atClient);
void lockAccount(char *userName, string path);
int getTypeFromBuff(char *buff);
int connectWinsock(char **argv);

void readData(string path) {
	numbersOfUser = read(path, users);
}


void mainThread(SOCKET listenSock, string path) {
	thread mainTh = thread(acceptClient, listenSock, path);
	mainTh.join();
}

void acceptClient(SOCKET listenSock, string path) {
	sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);
	while (1) {
		SOCKET connSock;
		connSock = accept(listenSock, (sockaddr*)&clientAddr, &clientAddrLen);
		int atClient = ntohs(clientAddr.sin_port);
		cout << "client " << atClient << " connected" << endl;
		thread myThread = thread(communicate, connSock, path, atClient);	
		if (myThread.std::thread::joinable()) {
			myThread.std::thread::detach();		
		}
	}	// end of while
}


void communicate(SOCKET connSock, string path, int atClient) {
	while (1) {
		int situation = receiveFromClient(connSock, path, atClient);
		if (situation == SOCKET_ERROR) {
			cout << "client: " << atClient <<" disconnected" << endl;
			break;
		}
		int len = responseClient(connSock, situation);
		if (len == SOCKET_ERROR) {
			cout << "Error! Can not send message" << endl;
			break;
		}
	}
}

int responseClient(SOCKET conSock, int situation) {
	return sendToClient(conSock, situation);
}



int receiveFromClient(SOCKET conSock, string path, int atClient) {
	char buff[BUFF_SIZE];
	int len = recv(conSock, buff, BUFF_SIZE, 0);
	if (len == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}
	else {
		buff[len] = 0;
		int typeMessage = getTypeFromBuff(buff);
		char userName[ACCOUNT_SIZE];
		char pass[ACCOUNT_SIZE];
		getAccountInputFromClient(buff, userName, pass);
		if (typeMessage == LOGIN) {
			int check = checkAccount(userName, pass);
			return handleLogin(userName, check, path, atClient);
		}
		else {
			return handleLogout(userName, atClient);
		}

	}
}

int getTypeFromBuff(char *buff) {
	int i;
	string str;
	for (i = 0; i < TYPE_MESSAGE_SIZE - 1; i++)
	{
		str.push_back(buff[i]);
	}
	return stoi(str);
}

int handleLogin(char *userName, int situation, string path, int atClient) {

	for (int i = 0; i < numbersOfUser; i++) {
		if (users[i].userName.compare(userName) == 0) {
			if (situation == LIVE) {
				users[i].falseLoginedTimes = 0;
				addToListLogined(userName, atClient);
				return SUCCESS_LOGIN;
			}
			if (situation == PASS_FALSE &&
				users[i].falseLoginedTimes < MAX_FALSE_LOGINED_TIMES) {
				users[i].falseLoginedTimes++;
				return PASS_FALSE;
			}
			if (situation == PASS_FALSE &&
				users[i].falseLoginedTimes == MAX_FALSE_LOGINED_TIMES) {
				lockAccount(userName, path);
				return HASBEENLOCKING;
			}
		}
	}
	return situation;
}

int hasLoginedYet(char *userName, int atClient) {
	for (int i = 0; i < managerLogin.size(); i++) {
		if (managerLogin[i].userName.compare(userName) == 0 &&
			managerLogin[i].atClient == atClient)
			return i;
	}
	return NOT_LOGIN;
}
int handleLogout(char *userName, int atClient) {
	int position = hasLoginedYet(userName, atClient);
	if (position == NOT_LOGIN) {
		return ERROR_LOGOUT;
	}
	else {
		managerLogin.erase(managerLogin.begin() + position);		// delete account is loginning out of session
		return SUCCESS_LOGOUT;
	}
}



void addToListLogined(char *userName, int atClient) {
	int position = hasLoginedYet(userName, atClient);
	if (position == NOT_LOGIN) {
		LOGINED logined;
		logined.atClient = atClient;
		logined.userName.append(userName);
		managerLogin.push_back(logined);
	}
}

void lockAccount(char *userName, string path) {
	updateInfor(path, userName, users, INACTIVE, numbersOfUser);
}

int checkAccount(char* userName, char *pass) {
	for (int i = 0; i < numbersOfUser; i++)
	{
		if (users[i].userName.compare(userName) == 0) {
			if (users[i].pass.compare(pass) == 0) {
				if (users[i].status == ACTIVE)
					return LIVE;

				else
					return LOCKED;
			}
			else
				return PASS_FALSE;
		}
	}
	return NOT_FOUND_ACCOUNT;
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

	if (::bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr))) {
		cout << "Error! Can not bind this address. " << endl;
		_getch();
		return 0;
	}

	listen(listenSock, 10);

	cout << "Server started" << endl;
	return listenSock;
}






