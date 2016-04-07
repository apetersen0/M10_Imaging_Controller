#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define SOCKET_READ_TIMEOUT_SEC 5

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment (lib, "Ws2_32.lib")


#define DEFAULT_PORT 27015
using namespace std;

class TCP_Client {
	private:
		WSADATA wsaData;
		int iResult;
		DWORD timeout = SOCKET_READ_TIMEOUT_SEC * 1000;
		bool error = true;

		SOCKET ConnectSocket = INVALID_SOCKET;

		struct addrinfo *result = NULL;
		struct addrinfo hints;
		struct addrinfo *ptr = NULL;

		int iSendResult;
	public:
		//type = "server", "client"
		TCP_Client(string ip, string port);
		~TCP_Client();

		//return the # of bytes read
		int readData(char* buffer, int bufflen);
		int writeData(char* buffer, int numBytes);
		void Close();
		bool IsConnected();
		void SetTimeout(int milliseconds);
};
