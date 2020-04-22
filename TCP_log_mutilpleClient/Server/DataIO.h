#pragma once
#include<iostream>
#include <fstream>
#define TYPE_MESSAGE_SIZE 4
#define DIV '\n'
using namespace std;

typedef struct {
	string userName;
	string pass;
	int status;
	int falseLoginedTimes = 0;
}User;

int sendToClient(SOCKET conSock, int situation);
int read(string path, User user[]);
void updateInfor(string path, char *userName, User user[], int status, int numbersOfUser);



string intToString(int a) {
	stringstream ss;
	ss << a;
	string str = ss.str();
	return str;
}



int sendToClient(SOCKET conSock, int situation) {
	char buff[TYPE_MESSAGE_SIZE];

	string str = intToString(situation);
	int i;
	for (i = 0; i < str.length(); i++)
	{
		buff[i] = str[i];
	}
	buff[i] = 0;
	int len = send(conSock, buff, strlen(buff), 0);
	if (len == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}
	return len;
}

int read(string path, User user[])
{
	fstream f;
	f.open(path, ios::in);
	int i = 0;
	string userName, pass;
	int status;
	while (f >> userName >> pass >> status)
	{
		user[i].userName = userName;
		user[i].pass = pass;
		user[i].status = status;
		i++;
	}
	int numbersOfUser = i;
	f.close();
	return numbersOfUser;
}


void updateInfor(string path, char *userName, User user[], int status, int numbersOfUser) {

	for (int i = 0; i < numbersOfUser; i++) {
		if (user[i].userName.compare(userName) == 0)
			user[i].status = status;
	}
	fstream file;
	file.open(path, ios::out);
	for (int i = 0; i < numbersOfUser; i++)
	{
		file << user[i].userName << " " << user[i].pass << " " << user[i].status << endl;
	}

	file.close();
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
