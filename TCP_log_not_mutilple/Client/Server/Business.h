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


#define DIV '\n'
#define ERROR_CONNECT -1
#define BUFF_SIZE 2048
#define ACCOUNT_SIZE 256
#define MAX_USER 100
#define MAX_LOGIN 1000
#define MAX_FALSE_LOGINED_TIMES 3
typedef enum { ACTIVE = 0, INACTIVE = 1 }Status;

User users[MAX_USER];
int numbersOfUser;
typedef struct {
	string userName;
	int atClient;
}LOGINED;

vector<LOGINED> managerLogin;

int receiveFromClient(SOCKET conSock, string path, int atClient);
void getAccountInputFromClient(char *buff, char* userName, char* pass);
int responseClient(SOCKET conSock, int situation);
int checkAccount(char* userName, char *pass); 
void readData(string path);
void addToListLogined(char *userName, int atClient);
int handleLogout(char *userName, int atClient);
int handleLogin(char *userName, int situation, string path, int atClient);
int hasLoginedYet(char* userName, int atClient);
void lockAccount(char *userName, string path);
int getTypeFromBuff(char *buff);

void readData(string path) {
	numbersOfUser = read(path, users);
}

int responseClient(SOCKET conSock, int situation) {
	return sendToClient(conSock, situation);
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

int getTypeFromBuff(char *buff) {
	int i;
	string str;
	for (i = 0; i < TYPE_MESSAGE_SIZE - 1; i++)
	{
		str.push_back(buff[i]);
	}
	return stoi(str);
}

int receiveFromClient(SOCKET conSock, string path, int atClient) {
	char buff[BUFF_SIZE];
	int len = recv(conSock, buff, BUFF_SIZE, 0);
	buff[len] = 0;
	int typeMessage = getTypeFromBuff(buff);
	if (len == SOCKET_ERROR) {
		cout << "Error: " << WSAGetLastError() << endl;
		return ERROR_CONNECT;
	}
	else {
		char userName[ACCOUNT_SIZE];
		char pass[ACCOUNT_SIZE];
		getAccountInputFromClient(buff, userName, pass);
		if (typeMessage == LOGIN) {
			int check = checkAccount(userName, pass);
			return handleLogin(userName, check, path, atClient);
		}
		else 
			return handleLogout(userName, atClient);
		
	}
}

int handleLogout(char *userName, int atClient) {
	int positionAccountLogined = hasLoginedYet(userName, atClient);
	if (positionAccountLogined == -1) {
		return ERROR_LOGOUT;
	}
	else {
		managerLogin.erase(managerLogin.begin() + positionAccountLogined);		// delete account is loginning out of session
		return SUCCESS_LOGOUT;
	}
}

int hasLoginedYet(char* userName, int atClient) {
	for (int i = 0; i < managerLogin.size(); i++) {
		if (managerLogin[i].userName.compare(userName) == 0 &&
			managerLogin[i].atClient == atClient) {
			return i;
		}
	}
	return -1;
}

void addToListLogined(char *userName, int atClient) {
	int check = hasLoginedYet(userName, atClient);
	if (check == -1) {
		LOGINED accountLogined = {};
		accountLogined.userName.append(userName);
		accountLogined.atClient = atClient;
		managerLogin.push_back(accountLogined);
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

void getAccountInputFromClient(char *buff, char* userName, char* pass) {
	int i;
	for (i = TYPE_MESSAGE_SIZE; i < strlen(buff); i++)
	{
		if (buff[i] == DIV)
			break;
		else
			userName[i - TYPE_MESSAGE_SIZE] = buff[i];
	}
	userName[i - TYPE_MESSAGE_SIZE] = '\0';
	int j;
	for (j = i + 1; j < strlen(buff); j++) {
		pass[j - i - 1] = buff[j];
	}
	pass[j - i - 1] = '\0';
}


