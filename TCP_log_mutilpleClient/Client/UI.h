#pragma once



#include "Business.h" 
#define ACCOUNT_SIZE 256

string userName;
string password;

void menuUI();
int enterChoice();
int clean_stdin();
void enterFromKeyboardLogin();
void enterFromKeyboardLogout();
bool handleUserChoice(int choice, SOCKET conSock);
void showAlert(int situation);
bool isOverSize(string str);
bool hasSpace(string str);

void menuUI() {
	cout << "====================" << endl;
	cout << "1. Login" << endl;
	cout << "2. Logout" << endl;
	cout << "3. Exit" << endl;
	cout << "====================" << endl;
}

int clean_stdin() {
	while (getchar() != '\n') {
	}
	return 1;
}
int enterChoice() {

	int choice;
	char c;
	do {
		printf("Enter your choice: ");
	} while (((scanf("%d%c", &choice, &c) != 2 || c != '\n') && clean_stdin()));
	return choice;
}

void enterFromKeyboardLogin() {
	bool check = false;
	do {
		cout << "user Name: ";
		getline(cin, userName);
		cout << "password: ";
		getline(cin, password);
		if (hasSpace(userName) || hasSpace(password) || isOverSize(userName) || isOverSize(password)) {
			cout << "userName and password mustn't have space and be over " << ACCOUNT_SIZE << " words!" << endl;
			cout << "enter again!" << endl;
		}
		else
			check = true;

	} while (!check);


}

void enterFromKeyboardLogout() {
	bool check = false;
	do {
		cout << "user Name: ";
		getline(cin, userName);
		if (hasSpace(userName) || isOverSize(userName)) {
			cout << "userName mustn't have space and be over " << ACCOUNT_SIZE << " words!" << endl;
			cout << "enter again!" << endl;
			check = false;
		}
		else
			check = true;

	} while (!check);
}


bool handleUserChoice(int choice, SOCKET conSock) {
	int situation;
	switch (choice)
	{
	case 1:
		enterFromKeyboardLogin();
		login(conSock, userName, password);
		situation = receiveFromServer(conSock);
		showAlert(situation);
		return false;

	case 2:
		enterFromKeyboardLogout();
		logout(conSock, userName);
		situation = receiveFromServer(conSock);
		showAlert(situation);
		return false;
	case 3:
		return true;
	default:
		return false;
	}
}

void showAlert(int situation) {
	switch (situation) {

	case SUCCESS_LOGIN:
		cout << "\nSUCCESS: Login successfully!!\n" << endl;
		haveActiveAcount = true;
		break;
	case LOCKED:
		cout << "\nERROR: this Account has been locked!!\n" << endl;
		break;
	case PASS_FALSE:
		cout << "\nERROR: password wrong.\n" << endl;
		break;
	case SUCCESS_LOGOUT:
		cout << "\nSUCCESS: Logout successfully!!\n" << endl;
		haveActiveAcount = false;
		break;
	case ERROR_LOGOUT:
		cout << "\nERORR: this Account hasn't logined or not valid\n" << endl;
		break;
	case HASBEENLOCKING:
		cout << "\nERROR: this Account has been locking because you have entered uncorrectly password more " << MAX_TIMES_FALSE << " times\n" << endl;
		break;
	case NOT_FOUND_ACCOUNT:
		cout << "\nERROR: this userName doesn't available... Have you registered yet?\n" << endl;
		break;
	}
}

bool isOverSize(string str) {
	if (str.size() >= ACCOUNT_SIZE)
		return true;
	return false;
}

bool hasSpace(string str) {
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] == ' ')
			return true;
	}
	return false;
}