#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "framework.h"

default_random_engine dre;

// Init Random Position 
uniform_int_distribution<>PositionX(0, 500);
uniform_int_distribution<>PositionY(0, 300);

// Update Random Position
uniform_int_distribution<>MoveX(-4, +4);
uniform_int_distribution<>MoveY(-4, +4);

// Thread ID
DWORD dwThreadID[MAXTHREAD];
static int idIndex = 0;
HANDLE hThread[MAXTHREAD];
int threadCount = 0;

CRITICAL_SECTION cs;

// ThreadFunction
DWORD WINAPI PlayerThread(LPVOID arg);

Input input;

// GameFunction
void GameValueInit();
void SendGameInit(SOCKET client_sock, int threadnum);

void UpdateFunction();
void UpdatePlayer(DWORD tID);

void SendObject(SOCKET client_sock);

// ErrorFunction
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


	InitializeCriticalSection(&cs);

	cout << "**** 서버 시작 ****" << endl;

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

	DeleteCriticalSection(&cs);

	closesocket(listen_sock);

	WSACleanup();

	return 0;
}

DWORD WINAPI PlayerThread(LPVOID arg)
{
	int retval;
	int threadIndex = idIndex;
	idIndex++;
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN client_addr;
	int client_addr_len;
	char buf[2];
	char GameReady[6];
	ZeroMemory(&buf, sizeof(buf));
	getpeername(client_sock, (SOCKADDR*)&client_addr, &client_addr_len);

	retval = recv(client_sock, (char*)GameReady, sizeof(GameReady), 0);
	if (retval == SOCKET_ERROR)
	{
		err_quit("Ready Error()");
	}

	GameValueInit();

	const char* start = "GameStart";
	retval = send(client_sock, (char*)start, sizeof(start), 0);

	SendGameInit(client_sock,threadIndex);

	DWORD id = dwThreadID[threadIndex];

	while (true)
	{
		if (!player.state && threadIndex == 0)
		{
			closesocket(client_sock);
			TerminateThread(hThread[0], 0);
		}
		if (!player2.state && threadIndex == 1)
		{
			closesocket(client_sock);
			TerminateThread(hThread[1], 0);
		}
		recv(client_sock, (char*)&input, sizeof(input), 0);
		
		
		UpdatePlayer(id);
		
		EnterCriticalSection(&cs);
		UpdateFunction();
		SendObject(client_sock);
		LeaveCriticalSection(&cs);


		if (!player.state && !player2.state)
		{
			return 0;
		}
	}

	return 0;
}

void GameValueInit()
{
	item_Drop_Timer = 0;
	item_Count = 0;
	for (int i = 0; i < 20; ++i)
	{
		item_Drop[i].state = 0;
	}

	for (int i = 0; i < 10; ++i)
		item_6_direction[i] = 0;

	item_1_count = 0;
	item_6_count = 0;
	item_8_count = 0;
	item_10_count = 0;

	for (int i = 0; i < MONSTER; i++)//버그 좌표 초기화
	{

		bug[i].x = -100;
		bug[i].y = -300;
		bug[i].x_move = 0;
		bug[i].y_move = 0;
		bug[i].state = 1;
	}

	player.x = 235;
	player.y = 650;
	player.w = 30;
	player.h = 30;

	player.picX = 0;
	player.picY = 0;
	player.picW = 76;
	player.picH = 74;

	player.state = 1;

	player2.x = 235;
	player2.y = 700;
	player2.w = 30;
	player2.h = 30;

	player2.picX = 0;
	player2.picY = 0;
	player2.picW = 76;
	player2.picH = 74;

	player2.state = 1;

	backGround1.x = 0;
	backGround1.y = 0;
	backGround1.w = 500;
	backGround1.h = 800;

	backGround1.picX = 0;
	backGround1.picY = 0;
	backGround1.picW = 500;
	backGround1.picH = 800;

	backGround2.x = 0;
	backGround2.y = -800;
	backGround2.w = 500;
	backGround2.h = 800;

	backGround2.picX = 0;
	backGround2.picY = 0;
	backGround2.picW = 500;
	backGround2.picH = 800;

	for (auto& i : item_1)
	{
		i.x = player.x - 82;
		i.y = player.y - 73;
		i.w = 230;
		i.h = 220;
		i.picX = 0;
		i.picY = 0;
		i.picW = 230;
		i.picH = 220;
	}

	for (auto& i : item_6)
	{
		i.x = player.x - 65;
		i.y = player.y - 60;
		i.w = 114;
		i.h = 122;
		 
		i.picX = 0;
		i.picY = 0;
		i.picW = 114;
		i.picH = 122;
	}

	for (auto& i : item_8)
	{
		i.x = player.x - 65;
		i.y = player.y - 60;
		i.w = 157;
		i.h = 150;

		i.picX = 0;
		i.picY = 0;
		i.picW = 157;
		i.picH = 150;
	}

	for (auto& i : item_10)
	{
		i.x = player.x;
		i.y = player.y - 60;
		i.w = 30;
		i.h = 30;
		i.picX = 0;
		i.picY = 0;
		i.picW = 76;
		i.picH = 75;
	}

}

void SendGameInit(SOCKET client_sock,int threadnum)
{
	for (auto& i : bug)
	{
		send(client_sock, (char*)&i, sizeof(i), 0);
	}

	send(client_sock, (char*)&player, sizeof(player), 0);
	send(client_sock, (char*)&player2, sizeof(player2), 0);

	send(client_sock, (char*)&backGround1, sizeof(backGround1), 0);
	send(client_sock, (char*)&backGround2, sizeof(backGround2), 0);

	for (auto& i : item_1)
	{
		send(client_sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : item_6)
	{
		send(client_sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : item_8)
	{
		send(client_sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : item_10)
	{
		send(client_sock, (char*)&i, sizeof(i), 0);
	}

	//스레드 넘버 보내기
	send(client_sock, (char*)&threadnum, sizeof(threadnum), 0);

	cout << "Send OK" << endl;
}

void SendObject(SOCKET client_sock)
{
	for (auto& i : bug)
	{
		send(client_sock, (char*)&i, sizeof(i), 0);
	}

	send(client_sock, (char*)&player, sizeof(player), 0);
	send(client_sock, (char*)&player2, sizeof(player2), 0);

	for (auto& i : item_1)
	{
		send(client_sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : item_6)
	{
		send(client_sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : item_8)
	{
		send(client_sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : item_10)
	{
		send(client_sock, (char*)&i, sizeof(i), 0);
	}

	for (int i = 0; i < 10; ++i)
	{
		send(client_sock, (char*)&item_1_Flag[i], sizeof(item_1_Flag[i]), 0);
		send(client_sock, (char*)&item_1_frame[i], sizeof(item_1_frame[i]), 0);
		send(client_sock, (char*)&item_1_carry[i], sizeof(item_1_carry[i]), 0);
	}

	for (int i = 0; i < 10; ++i)
	{
		send(client_sock, (char*)&item_6_Flag[i], sizeof(item_6_Flag[i]), 0);
		send(client_sock, (char*)&item_6_frame[i], sizeof(item_6_frame[i]), 0);
		send(client_sock, (char*)&item_6_direction[i], sizeof(item_6_direction[i]), 0);
		send(client_sock, (char*)&item_6_bounce[i], sizeof(item_6_bounce[i]), 0);
	}

	for (int i = 0; i < 10; ++i)
	{
		send(client_sock, (char*)&item_8_Flag[i], sizeof(item_8_Flag[i]), 0);
		send(client_sock, (char*)&item_8_frame[i], sizeof(item_8_frame[i]), 0);
	}

	for (int i = 0; i < 10; ++i)
	{
		send(client_sock, (char*)&item_10_Flag[i], sizeof(item_8_Flag[i]), 0);		
	}

	send(client_sock, (char*)&interrupt_ITEM2_Flag, sizeof(interrupt_ITEM2_Flag), 0);
	send(client_sock, (char*)&interrupt_ITEM2_timer, sizeof(interrupt_ITEM2_timer), 0);
	send(client_sock, (char*)&interrupt_ITEM2_frame, sizeof(interrupt_ITEM2_frame), 0);

	for (auto& i : item_Drop)
	{
		send(client_sock, (char*)&i, sizeof(i), 0);
	}
}

static int key = 0;
void playerCollisionCheck(Move& player)
{
	//플레이어 + 버그 충돌
	for (int i = 0; i < MONSTER; ++i)
	{
		if (bug[i].state != 0)
		{
			//플레이어 top에서 충돌
			if ((bug[i].x <= player.x && player.x <= bug[i].x + 30) &&
				(0 <= bug[i].y + 30 - player.y && bug[i].y + 30 - player.y <= 20))
			{
				player.state = 0;
			}
			else if ((bug[i].x <= player.x + 30 && player.x + 30 <= bug[i].x + 30) &&
				(0 <= player.y - bug[i].y + 30 && player.y - bug[i].y + 30 <= 20))
			{
				player.state = 0;
			}

			//플레이어 left에서 충돌
			else if ((bug[i].y <= player.y && player.y <= bug[i].y + 30) &&
				(0 <= player.x - bug[i].x + 30 && player.x - bug[i].x + 30 <= 20))
			{
				player.state = 0;
			}
			else if ((bug[i].y <= player.y + 30 && player.y + 30 <= bug[i].y + 30) &&
				(0 <= player.x - bug[i].x + 30 && player.x - bug[i].x + 30 <= 20))
			{
				player.state = 0;
			}

			//플레이어 right에서 충돌
			else if ((bug[i].y <= player.y && player.y <= bug[i].y + 30) &&
				(0 <= bug[i].x - player.x + 30 && bug[i].x - player.x + 30 <= 20))
			{
				player.state = 0;
			}
			else if ((bug[i].y <= player.y + 30 && player.y + 30 <= bug[i].y + 30) &&
				(0 <= bug[i].x - player.x + 30 && bug[i].x - player.x + 30 <= 20))
			{
				player.state = 0;
			}

			//플레이어 bottom에서 충돌
			else if ((bug[i].x <= player.x && player.x <= bug[i].x + 30) &&
				(0 <= bug[i].y - player.y + 30 && bug[i].y - player.y + 30 <= 20))
			{
				player.state = 0;
			}
			else if ((bug[i].x <= player.x + 30 && player.x + 30 <= bug[i].x + 30) &&
				(0 <= bug[i].y - player.y + 30 && bug[i].y - player.y + 30 <= 20))
			{
				player.state = 0;
			}
		}
	}

}

//아이템 + 버그 충돌
void collisionCheck(Move& item)
{

	for (int i = 0; i < MONSTER; ++i)
	{
		if (bug[i].state != 0)
		{
			//아이템 top에서 충돌
			if ((item.x <= bug[i].x + 15 && bug[i].x + 15 <= item.x + item.w) && (0 <= item.y - (bug[i].y + 15) && item.y - (bug[i].y + 15) <= 20))
			{
				bug[i].impact_num = (rand() % 8);
				bug[i].state = 0;
			}
			else if ((item.x <= bug[i].x && bug[i].x <= item.x + item.w) && (0 <= item.y - (bug[i].y + 15) && item.y - (bug[i].y + 15) <= 20))
			{
				bug[i].impact_num = (rand() % 8);
				bug[i].state = 0;
			}

			//아이템 left에서 충돌
			else if ((item.y <= bug[i].y && bug[i].y <= item.y + item.h) && (0 <= item.x - (bug[i].x + 15) && item.x - (bug[i].x + 15) <= 20))
			{
				bug[i].impact_num = (rand() % 8);
				bug[i].state = 0;
			}
			else if ((item.y <= bug[i].y + 15 && bug[i].y + 15 <= item.y + item.h) && (0 <= item.x - (bug[i].x + 15) && item.x - (bug[i].x + 15) <= 20))
			{
				bug[i].impact_num = (rand() % 8);
				bug[i].state = 0;
			}

			//아이템 right에서 충돌
			else if ((item.y <= bug[i].y && bug[i].y <= item.y + item.h) && (0 <= bug[i].x - (item.x + item.w) && bug[i].x - (item.x + item.w) <= 20))
			{
				bug[i].impact_num = (rand() % 8);
				bug[i].state = 0;
			}
			else if ((item.y <= bug[i].y + 15 && bug[i].y + 15 <= item.y + item.h) && (0 <= bug[i].x - (item.x + item.w) && bug[i].x - (item.x + item.w) <= 20))
			{
				bug[i].impact_num = (rand() % 8);
				bug[i].state = 0;
			}

			//아이템 bottom에서 충돌
			else if ((item.x <= bug[i].x + 15 && bug[i].x + 15 <= item.x + item.w) && (0 <= bug[i].y - (item.y + item.h) && bug[i].y - (item.y + item.h) <= 20))
			{
				bug[i].impact_num = (rand() % 8);
				bug[i].state = 0;
			}
			else if ((item.x <= bug[i].x && bug[i].x <= item.x + item.w) && (0 <= bug[i].y - (item.y + item.h) && bug[i].y - (item.y + item.h) <= 20))
			{
				bug[i].impact_num = (rand() % 8);
				bug[i].state = 0;
			}

			//아이템 내부에서 충돌
			else if ((item.x <= bug[i].x + 15 && bug[i].x + 15 <= item.x + item.w) && (item.x <= bug[i].x && bug[i].x <= item.x + item.w) &&
				(item.y <= bug[i].y && bug[i].y <= item.y + item.h) && (item.y <= bug[i].y + 15 && bug[i].y + 15 <= item.y + item.h))
			{
				bug[i].impact_num = (rand() % 8);
				bug[i].state = 0;
			}
		}

	}
}

//아이템드롭 + 플레이어 충돌
int IsItemCollisionCheck(Move& item, Move& player)
{
	//아이템 top에서 충돌
	if (((item.x <= player.x + 30 && player.x + 30 <= item.x + item.w) && (0 <= item.y - (player.y + 30) && item.y - (player.y + 30) <= 10)) ||
		((item.x <= player.x && player.x <= item.x + item.w) && (0 <= item.y - (player.y + 30) && item.y - (player.y + 30) <= 10)))
	{
		return 1;
	}

	//아이템 left에서 충돌
	else if (((item.y <= player.y && player.y <= item.y + item.h) && (0 <= item.x - (player.x + 30) && item.x - (player.x + 30) <= 10)) ||
		((item.y <= player.y + 30 && player.y + 30 <= item.y + item.h) && (0 <= item.x - (player.x + 30) && item.x - (player.x + 30) <= 10)))
	{
		return 1;
	}

	//아이템 right에서 충돌
	else if (((item.y <= player.y && player.y <= item.y + item.h) && (0 <= player.x - (item.x + item.w) && player.x - (item.x + item.w) <= 10)) ||
		((item.y <= player.y + 30 && player.y + 30 <= item.y + item.h) && (0 <= player.x - (item.x + item.w) && player.x - (item.x + item.w) <= 10)))
	{
		return 1;
	}

	//아이템 bottom에서 충돌
	else if (((item.x <= player.x + 30 && player.x + 30 <= item.x + item.w) && (0 <= player.y - (item.y + item.h) && player.y - (item.y + item.h) <= 10)) ||
		((item.x <= player.x && player.x <= item.x + item.w) && (0 <= player.y - (item.y + item.h) && player.y - (item.y + item.h) <= 10)))
	{
		return 1;
	}

	//아이템 내부에서 충돌
	else if ((item.x <= player.x + 30 && player.x + 30 <= item.x + item.w) && (item.x <= player.x && player.x <= item.x + item.w) &&
		(item.y <= player.y && player.y <= item.y + item.h) && (item.y <= player.y + 30 && player.y + 30 <= item.y + item.h))
	{
		return 1;
	}
	return 0;
}

void UpdateFunction()
{
	backGround1.y += 5;
	backGround2.y += 5;

	if (backGround1.y >= 800)
	{
		backGround1.y = -800;
	}
	if (backGround2.y >= 800)
	{
		backGround2.y = -800;
	}

	playerCollisionCheck(player);
	playerCollisionCheck(player2);

	score++;
	//아이템 드롭 = 종류 설정
	item_Drop_Timer++;
	if (item_Drop_Timer % 50 == 0)
	{
		if (item_Drop[item_Count].state == 0)
		{
			item_Drop[item_Count].state = rand() % 5 + 1;
			item_Drop[item_Count].x = rand() % 450;
			item_Drop[item_Count].y = -30;
			item_Drop[item_Count].w = 40;
			item_Drop[item_Count].h = 40;
			item_Drop[item_Count].picX = 0;
			item_Drop[item_Count].picY = 0;
			item_Drop[item_Count].picW = 40;
			item_Drop[item_Count].picH = 40;
		}
		//cout << item_Drop[item_Count].state << endl;
		item_Count++;
		if (item_Count >= 20)
			item_Count = 0;
	}

	//아이템 드롭 => 이동 + 아이템 드롭 + 플레이어 충돌
	for (int i = 0; i < 20; ++i)
	{

		if (item_Drop[i].state != 0)
		{
			item_Drop[i].y += 2;

			if (item_Drop[i].y + item_Drop[i].h >= 800)
				item_Drop[i].state = 0;

			//드롭 아이템과 충돌체크
			//1p일때 
			if (IsItemCollisionCheck(item_Drop[i], player)|| IsItemCollisionCheck(item_Drop[i], player2))
			{
				if (item_Drop[i].state == 1)
				{
					if (item_1_count == 10)
					{
						item_1_count = 0;
					}
					item_1[item_1_count].x = item_Drop[i].x - 100;
					item_1[item_1_count].y = item_Drop[i].y - 100;

					item_1_Flag[item_1_count] = 1;
					ITEM1(item_1[item_1_count], item_1_count);

					item_1_count++;
					item_Drop[i].state = 0;
				}
				if (item_Drop[i].state == 2)
				{
					interrupt_ITEM2_Flag = 1;
					item_Drop[i].state = 0;
				}

				if (item_Drop[i].state == 3)
				{
					if (item_6_count == 10)
					{
						item_6_count = 0;
					}
					item_6[item_6_count].x = item_Drop[i].x - 50;
					item_6[item_6_count].y = item_Drop[i].y - 50;

					item_6_Flag[item_6_count] = 1;
					ITEM6(item_6[item_6_count], item_6_count);
					item_6_count++;
					item_Drop[i].state = 0;
				}

				if (item_Drop[i].state == 4)
				{
					if (item_8_count == 10)
					{
						item_8_count = 0;
					}

					item_8[item_8_count].x = player.x - 65;
					item_8[item_8_count].y = player.y - 60;


					item_8_Flag[item_8_count] = 1;
					ITEM8(player, item_8[item_8_count], item_8_count);

					item_8[item_8_count].collisionWithWho = 1;
					item_8_count++;
					item_Drop[i].state = 0;
				}

				if (item_Drop[i].state == 5)
				{
					if (item_10_count == 10)
					{
						item_10_count = 0;
					}
					item_10[item_10_count].x = item_Drop[i].x;
					item_10[item_10_count].y = item_Drop[i].y - 50;

					item_10_Flag[item_10_count] = 1;
					ITEM10(player, item_10[item_10_count], item_10_count);

					item_10[item_10_count].collisionWithWho = 1;

					item_Drop[i].state = 0;
					item_10_count++;
				}
			}
		}
		//아이템 플래그 확인
		for (int i = 0; i < 10; ++i)
		{
			if (score % 2 == 0)
			{
				//아이템 1 플래그
				if (item_1_Flag[i] == 1)
				{
					ITEM1(item_1[i], i);
				}

				//아이템 6 플래그
				if (item_6_Flag[i] != 0)
				{
					ITEM6(item_6[i], i);
				}

				//아이템 8 플래그
				if (item_8_Flag[i] != 0)
				{
					if (item_8[i].collisionWithWho == 1)
						ITEM8(player, item_8[i], i);
				}

				//아이템 10 플래그
				if (item_10_Flag[i] != 0)
				{
					if (item_10[i].collisionWithWho == 1)
						ITEM10(player, item_10[i], i);
				}
			}
		}
		//방해요소 2 플래그 on 
		if (interrupt_ITEM2_Flag == 1)
		{
			interrupted_ITEM_2();
		}
	}

	//죽었는지
	if (player.state == 0)
	{
		Gameover = true;
	}
	else if (player2.state == 0)
	{

	}
	bug[bug_num].x = (rand() % 500);//버그 x좌표
	bug[bug_num].y = -1;//버그 y좌표
	bug[bug_num].y_move = (rand() % 3) + 2;//버그 y좌표 이동값

	for (int i = 0; i < 10; ++i)
	{
		if (item_10_Flag[i] == 0)
		{
			bug[bug_num].x_move = bug_x_move(bug[bug_num], player);// 버그의 x이동값 정하기

		}

		if (item_10_Flag[i] == 1)
		{
			bug[bug_num].x_move = bug_x_move(bug[bug_num], item_10[i]);// 버그의 x이동값 정하기
		}
	}

	bug[bug_num].state = 1;
	bug[bug_num].w = 30;
	bug[bug_num].h = 30;

	if (bug_num == MONSTER - 1)// 버그 돌려 막기
	{
		bug_num = 0;
	}
	else
	{
		if (rand() % 1 == 0)
		{
			bug_num++;//버그 수 증가
		}
	}
	for (int i = 0; i < MONSTER; i++)// 버그 움직이기
	{
		bug[i].x += bug[i].x_move;
		bug[i].y += bug[i].y_move;

		if (bug[i].y + 30 >= 800)
		{
			bug[i].state = 1;
		}
	}

	//버그랑 아이템 충돌체크
	for (int i = 0; i < 10; ++i)
	{
		if (item_1_Flag[i] != 0)
			collisionCheck(item_1[i]);
		if (item_6_Flag[i] != 0)
			collisionCheck(item_6[i]);
		if (item_8_Flag[i] != 0)
			collisionCheck(item_8[i]);
		if (item_10_Flag[i] != 0)
			collisionCheck(item_10[i]);
	}
}

void UpdatePlayer(DWORD tID)
{
	if (tID == dwThreadID[0])
	{
		if (input.LEFT)player.x -= 5;
		if (input.RIGHT)player.x += 5;
		if (input.UP)player.y -= 5;
		if (input.DOWN)player.y += 5;
	}
	else if (tID == dwThreadID[1])
	{
		if (input.LEFT)player2.x -= 5;
		if (input.RIGHT)player2.x += 5;
		if (input.UP)player2.y -= 5;
		if (input.DOWN)player2.y += 5;
	}
}



void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
