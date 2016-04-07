#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define SOCKET_READ_TIMEOUT_SEC 5

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

class TCP_Server {
	private:
		WSADATA wsaData;
		int iResult;
		DWORD timeout = SOCKET_READ_TIMEOUT_SEC * 1000;
		bool error = true;

		SOCKET ListenSocket = INVALID_SOCKET;
		SOCKET ClientSocket = INVALID_SOCKET;

		struct addrinfo *result = NULL;
		struct addrinfo hints;

		int iSendResult;
	public:
		//type = "server", "client"
		TCP_Server(string port);
		~TCP_Server();

		//return the # of bytes read
		int readData(char* buffer, int bufflen);
		int writeData(char* buffer, int numBytes);
		void Close();
		bool IsConnected();
		void SetTimeout(int milliseconds);
};