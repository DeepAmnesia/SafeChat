#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream> 
#include <cstdio> 
#include <cstring> 
#include <winsock2.h> 
#pragma comment(lib, "WS2_32.lib")
using namespace std;

#include "xorstr.h"
#include "base64.h"

DWORD WINAPI clientReceive(LPVOID lpParam) 
{ //Получение данных от сервера
	char buffer[1024] = { 0 };
	SOCKET server = *(SOCKET*)lpParam;
	while (true) 
	{
		if (recv(server, buffer, sizeof(buffer), 0) == SOCKET_ERROR) 
		{
			cout << "recv function failed with error: " << WSAGetLastError() << endl;
			return -1;
		}
		if (strcmp(buffer, "exit\n") == 0) 
		{
			cout << "Server disconnected." << endl;
			return 1;
		}

		Base64 base64;

		std::string decoded_data;
		std::string got_message = buffer;
		base64.decode(got_message, &decoded_data);

		cout << "Admin: " << decoded_data << endl;
		memset(decoded_data.data(), 0, sizeof(decoded_data.data()));
	}
	return 1;
}

DWORD WINAPI clientSend(LPVOID lpParam) 
{ //Отправка данных на сервер
	char buffer[1024] = { 0 };
	SOCKET server = *(SOCKET*)lpParam;
	while (true) 
	{
		fgets(buffer, 1024, stdin);

		std::string encoded_data;

		Base64 base64;
		base64.encode(buffer, &encoded_data);

		if (send(server, encoded_data.c_str(), sizeof(encoded_data), 0) == SOCKET_ERROR)
		{
			cout << "send failed with error: " << WSAGetLastError() << endl;
			return -1;
		}
		if (strcmp(buffer, "exit") == 0) 
		{
			cout << "Thank you for using the application" << endl;
			break;
		}
	}
	return 1;
}

int main() 
{
	WSADATA WSAData;
	SOCKET server;
	SOCKADDR_IN addr;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	if ((server = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) 
	{
		cout << "Socket creation failed with error: " << WSAGetLastError() << endl;
		return -1;
	}

	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //коннект к серверу
	addr.sin_family = AF_INET;
	addr.sin_port = htons(5555); //порт
	if (connect(server, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		cout << "Server connection failed with error: " << WSAGetLastError() << endl;
		return -1;
	}

	cout << "Connected to server!" << endl;
	cout << "Now you can use our live chat application. " << " Enter \"exit\" to disconnect" << endl;

	DWORD tid;
	HANDLE t1 = CreateThread(NULL, 0, clientReceive, &server, 0, &tid);
	if (t1 == NULL) cout << "Thread creation error: " << GetLastError();
	HANDLE t2 = CreateThread(NULL, 0, clientSend, &server, 0, &tid);
	if (t2 == NULL) cout << "Thread creation error: " << GetLastError();

	WaitForSingleObject(t1, INFINITE);
	WaitForSingleObject(t2, INFINITE);
	closesocket(server);
	WSACleanup();
}