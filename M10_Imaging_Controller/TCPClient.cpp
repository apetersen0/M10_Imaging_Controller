#include "TCPClient.h"
#include <iostream>

using namespace std;

TCP_Client::TCP_Client(string ip, string port) {

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}
	else {
		cout << "WSAStartup Succesful" << endl;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}
	else {
		cout << "getaddrinfo Succesful" << endl;
	}
	ptr = result;

	// Create a SOCKET for connecting to server
	ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}
	else {
		cout << "Socket Created" << endl;
	}

	//connects to server
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("connection failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return;
	}
	else {
		cout << "Connection with server etablished." << endl;
	}
	//setsockopt(ClientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

	error = false;
}

//Note: readData is a blocking call
int TCP_Client::readData(char* buffer, int bufflen) {
	try {
		iResult = recv(ConnectSocket, buffer, bufflen, 0);
		if (!(iResult > 0)) {
			cout << "Data read timeout." << endl;
			Close();
		}
		else if (iResult == SOCKET_ERROR) {
			cout << "Error reading incoming data: " << WSAGetLastError() << endl;
			Close();
		}

	}
	catch (exception e) { cout << "An exception occurred reading data: " << e.what() << endl; }
	return iResult;
}

int TCP_Client::writeData(char* buffer, int numBytes) {

	try {
		iSendResult = send(ConnectSocket, buffer, numBytes, 0);
		if (iSendResult == SOCKET_ERROR) {
			cout << "Error sending data: " << WSAGetLastError() << endl;
			Close();
		}
	}
	catch (exception e) { cout << "An exception occurred writing data: " << e.what() << endl; }
	return iSendResult;
}
void TCP_Client::Close() {
	error = true;
	try {
		iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return;
		}
	}
	catch (exception e) { cout << "An exception occurred closing the socket: " << e.what() << endl; }
}

//return true if connected, false if not
bool TCP_Client::IsConnected() {
	return !error;
}

TCP_Client::~TCP_Client() {
	try {
		closesocket(ConnectSocket);
		WSACleanup();////
	}
	catch (exception e) { cout << "An exception occurred cleaning up the socket: " << e.what() << endl; };
}
void TCP_Client::SetTimeout(int milliseconds) {
	setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&milliseconds, sizeof(milliseconds));
}