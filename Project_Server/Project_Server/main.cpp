#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <random>
#include <vector>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32")

#include "defines.h"

using namespace std;



default_random_engine dre;

// Init Random Position 
uniform_int_distribution<>PositionX(0, 500);
uniform_int_distribution<>PositionY(0, 300);

// Update Random Position
uniform_int_distribution<>MoveX(-4, +4);
uniform_int_distribution<>MoveY(-4, +4);

// Thread ID
DWORD dwThreadID[MAXTHREAD];

// ThreadFuction
DWORD WINAPI PlayerThread(LPVOID arg);

void SendInitData(SOCKET clientSock);
void SendGameData(SOCKET clientSock);
void InitGameData();

void UpdatePlayer();
void UpdateObjects();

Object Monster[10];
Object Player[2];
Object Item[3];
Input input;

// ErrorFuction
void err_quit(const char* msg);
void err_display(const char* msg);

int main()
{
	int retval = 0;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("bind()");

	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");

	SOCKET client_sock;
	SOCKADDR_IN client_addr;
	int client_addr_len;
	char buf[BUFFERSIZE + 1];

	HANDLE hThread[MAXTHREAD];
	int threadCount = 0;

	cout << " 서버 시작 " << endl;

	while (true)
	{
		client_addr_len = sizeof(client_addr);
		client_sock = accept(listen_sock, (SOCKADDR*)&client_addr, &client_addr_len);

		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		cout << 
			"\n[TCP 서버] 클라이언트 접속: IP 주소= " << inet_ntoa(client_addr.sin_addr) << 
			", 포트 번호=" << ntohs(client_addr.sin_port) << endl;

		hThread[threadCount] = CreateThread(nullptr, 0, PlayerThread, (LPVOID)client_sock, 0, &dwThreadID[threadCount]);
	
		threadCount++;
	}

	for (auto& i : hThread)
		CloseHandle(i);

	closesocket(listen_sock);

	WSACleanup();

	return 0;
}

DWORD WINAPI PlayerThread(LPVOID arg)
{
	int retval;

	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN client_addr;
	int client_addr_len;
	char buf[2];
	char GameReady[6];
	ZeroMemory(&buf, sizeof(buf));
	getpeername(client_sock, (SOCKADDR*)&client_addr, &client_addr_len);

	Object object;

	retval = send(client_sock, (char*)&object, sizeof(object), 0);

	InitGameData();
	SendInitData(client_sock);

	retval = recv(client_sock, (char*)GameReady, sizeof(GameReady), 0);
	if (retval == SOCKET_ERROR)
		err_display("recv()");

	
	cout << "받은 데이터 [" << GameReady << "]\n";

	
	//클라이언트에서 게임을 실행하라고 보내는 신호
	retval = send(client_sock, (char*)"GameStart", sizeof("GameStart"), 0);

	int t = 0;
	while (true)
	{
		retval = recv(client_sock, (char*)&input, sizeof(input), 0);
		if (retval == SOCKET_ERROR)
			err_display("recv()");
		UpdatePlayer();
		cout << input.UP << "-" << input.DOWN << "-" << input.LEFT << "-" << input.RIGHT << endl;
		UpdateObjects();

		SendGameData(client_sock);
		t++;

		if (t >= 10)
		{
			cout << "" << endl;
		}
	}
	
	return 0;
}

void SendInitData(SOCKET clientSock)
{
	for (const auto& i : Player)
	{
		if (send(clientSock, (char*)&i, sizeof(i), 0) == -1)
		{
			cout << "Send Fail\n";
		}
	}

	for (const auto& i : Monster)
	{
		if (send(clientSock, (char*)&i, sizeof(i), 0) == -1)
		{
			cout << "Send Fail\n";
		}
	}

	for (const auto& i : Item)
	{
		if (send(clientSock, (char*)&i, sizeof(i), 0) == -1)
		{
			cout << "Send Fail\n";
		}
	}
}

void SendGameData(SOCKET clientSock)
{
	for (const auto& i : Player)
	{
		if (send(clientSock, (char*)&i, sizeof(i), 0) == -1)
		{
			cout << "Send Fail\n";
		}
	}

	for (const auto& i : Monster)
	{
		if (send(clientSock, (char*)&i, sizeof(i), 0) == -1)
		{
			cout << "Send Fail\n";
		}
	}

	for (const auto& i : Item)
	{
		if (send(clientSock, (char*)&i, sizeof(i), 0) == -1)
		{
			cout << "Send Fail\n";
		}
	}
}

void InitGameData()
{
	for (auto i = 0; i < 2; ++i)
	{
		Player[i].x = 230 + (i * 5);
		Player[i].y = 650;
	}

	for (auto& i : Monster)
	{
		i.x = PositionX(dre);
		i.y = PositionY(dre);
	}
}

void UpdatePlayer()
{
	if (input.UP)
	{
		Player[0].y -= 5;
	}
	if (input.DOWN)
	{
		Player[0].y += 5;
	}
	if (input.LEFT)
	{
		Player[0].x -= 5;
	}
	if (input.RIGHT)
	{
		Player[0].x += 5;
	}
}

void UpdateObjects()
{
	for (auto& i : Monster)
	{
		if (i.x >= 0 && i.x <= WINDOWWIDTH && i.y > 0 && i.y <= WINDOWHEIGHT)
		{
			i.x += MoveX(dre);
			i.y += MoveY(dre);
		}
		else
		{
			continue;// 창 밖으로 나갔을때 처리
		}
	}

	for (auto& i : Item)
	{
		if (i.x >= 0 && i.x <= WINDOWWIDTH && i.y > 0 && i.y <= WINDOWHEIGHT)
		{
			i.x += MoveX(dre);
			i.y += MoveY(dre);
		}
		else
		{
			continue;// 창 밖으로 나갔을때 처리
		}
	}
}

void err_quit(const char* msg)
{
}

void err_display(const char* msg)
{
}
