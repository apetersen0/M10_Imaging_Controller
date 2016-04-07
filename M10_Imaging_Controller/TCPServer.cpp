#include "TCPServer.h"
#include <iostream>

using namespace std;

TCP_Server::TCP_Server(string port) {

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
	iResult = getaddrinfo(NULL, port.c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}
	else {
		cout << "getaddrinfo Succesful" << endl;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("Socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}
	else {
		cout << "Socket Created" << endl;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	else {
		cout << "Socket Listening..." << endl;
	}
	freeaddrinfo(result);

	//Note: listen is a blocking call
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	else {
		cout << "Client Accepted" << endl;
	}
	//setsockopt(ClientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

	error = false;
}

//Note: readData is a blocking call
int TCP_Server::readData(char* buffer, int bufflen) {
	try {
		iResult = recv(ClientSocket, buffer, bufflen, 0);
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

int TCP_Server::writeData(char* buffer, int numBytes) {

	try {
		iSendResult = send(ClientSocket, buffer, numBytes, 0);
		if (iSendResult == SOCKET_ERROR) {
			cout << "Error sending data: " << WSAGetLastError() << endl;
			Close();
		}
	}
	catch (exception e) { cout << "An exception occurred writing data: " << e.what() << endl; }
	return iSendResult;
}
void TCP_Server::Close() {
	error = true;
	try {
		iResult = shutdown(ClientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return;
		}
	}
	catch (exception e) { cout << "An exception occurred closing the socket: " << e.what() << endl; }
}

//return true if connected, false if not
bool TCP_Server::IsConnected() {
	return !error;
}

TCP_Server::~TCP_Server() {
	try {
		closesocket(ClientSocket);
		WSACleanup();////
	}
	catch (exception e) { cout << "An exception occurred cleaning up the socket: " << e.what() << endl; };
}
void TCP_Server::SetTimeout(int milliseconds) {
	setsockopt(ClientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&milliseconds, sizeof(milliseconds));
}