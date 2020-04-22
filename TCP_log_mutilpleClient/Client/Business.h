#pragma once




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


#define TYPE_MESSAGE_SIZE 4
#define DIV '\n'
#define ERROR_CONNECT -1
#define BUFF_SIZE 2048
#define ACCOUNT_SIZE 256
#define MAX_USER 100
#define MAX_LOGIN 1000
#define MAX_TIMES_FALSE 3
using namespace std;

int login(SOCKET conSock, string userName, string pass);
int logout(SOCKET conSock, string userName);
int receiveFromServer(SOCKET conSock);
int sentToServer(SOCKET conSock, char *buff);
char* package(string userName, string pass, int typeMessage);
string intToString(int a);

bool haveActiveAcount = false;

int login(SOCKET conSock, string userName, string pass) {
	char* pack = package(userName, pass, LOGIN);
	return sentToServer(conSock, pack);
}


int logout(SOCKET conSock, string userName) {

	char* pack = package(userName, "", LOGOUT);
	return sentToServer(conSock, pack);
}


int sentToServer(SOCKET conSock, char *buff) {
	int len = send(conSock, buff, strlen(buff), 0);
	if (len = SOCKET_ERROR)
		return ERROR_CONNECT;
	return len;
}

void copy(char *buff, string temp) {
	int i;
	for (i = 0; i < temp.size(); i++)
		buff[i] = temp[i];

	buff[i] = '\0';

}


char* package(string userName, string pass, int typeMessage) {
	char* buff = new char(BUFF_SIZE);
	string temp;
	string type = intToString(typeMessage);
	temp.append(type);
	temp.push_back(DIV);
	temp.append(userName);
	if (pass.empty()) {
		copy(buff, temp);
	}
	else {
		temp.push_back(DIV);
		temp.append(pass);
		copy(buff, temp);
	}
	return buff;
}


int receiveFromServer(SOCKET conSock) {
	char buff[BUFF_SIZE];
	int len = recv(conSock, buff, BUFF_SIZE, 0);
	if (len == SOCKET_ERROR) {
		return ERROR_CONNECT;
	}
	else {
		buff[len] = 0;
		char situation[TYPE_MESSAGE_SIZE + 1];
		int i = 0;
		for (i = 0; i < len; i++)
		{
			situation[i] = buff[i];
		}
		situation[++i] = 0;
		int res = stoi(situation);
		return res;
	}

}

string intToString(int a) {
	stringstream ss;
	ss << a;
	string str = ss.str();
	return str;
}