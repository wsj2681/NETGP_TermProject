#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "framework.h"

void err_quit(char* msg);
void err_display(char* msg);

Input input; // 키입력
SOCKET sock; // 소켓

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
void err_quit(const char* msg)
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
void err_display(const char* msg)
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

//디바이스 컨테스트 얻기
HINSTANCE g_hInst;
LPCTSTR lpszClass = "Window Class Name";


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;

	g_hInst = hInstance;
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);

	int retval = 0;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	SOCKADDR_IN server_sock_addr;
	ZeroMemory(&server_sock_addr, sizeof(server_sock_addr));
	server_sock_addr.sin_family = AF_INET;
	server_sock_addr.sin_addr.S_un.S_addr = inet_addr(SERVERIP);
	server_sock_addr.sin_port = ntohs(SERVERPORT);

	retval = connect(sock, (sockaddr*)&server_sock_addr, sizeof(server_sock_addr));
	if (retval == SOCKET_ERROR)
		err_quit("connect()");

	//준비 완료 상태
	retval = send(sock, (char*)"Ready", sizeof("Ready"), 0); 

	// 서버에서 게임 시작 신호 받기
	char GameStart[10];
	recv(sock, (char*)GameStart, sizeof(GameStart), 0);

	hWnd = CreateWindow(lpszClass, "레이디 버그", WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_THICKFRAME, 0, 0, 500, 800, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

HINSTANCE hInst;

void gameValueInit();
float bug_x_move(BUG bug, Move player);

static int item_1_Flag[10] = { 0, };
static int item_1_frame[10] = { 0, };
static int item_1_carry[10] = { 0, };

void ITEM1(Move& item_1, int i)
{

	item_1.picX = 0 + (item_1_frame[i] * 230);
	item_1.picY = item_1_carry[i] * 226;
	item_1_frame[i]++;

	if (item_1_frame[i] >= 8)
	{
		item_1_frame[i] = 0;
		item_1_carry[i]++;
		//item_1_Flag = 0;
	}
	if (item_1_carry[i] > 5)
	{
		item_1_frame[i] = 0;
		item_1_carry[i] = 0;
		item_1_Flag[i] = 0;
	}
}

//아이템 6
static int item_6_Flag[10] = { 0, };
static int item_6_frame[10] = { 0, };
static int item_6_direction[10] = { 0, };
static int item_6_bounce[10] = { 0, };
void ITEM6_DIRECTION(Move& item_6, int i)
{
	if (item_6_direction[i] == 0)
	{
		if (item_6.y <= 0)
		{
			item_6_direction[i] = 4;
			item_6_bounce[i]++;
		}
	}
	if (item_6_direction[i] == 4)
	{
		if (item_6.x <= 0)
		{
			item_6_direction[i] = 3;
			item_6_bounce[i]++;
		}
		else if (item_6.y + item_6.h >= 800)
		{
			item_6_direction[i] = 1;
			item_6_bounce[i]++;
		}
	}
	else if (item_6_direction[i] == 3)
	{
		if (item_6.x + item_6.w >= 500)
		{
			item_6_direction[i] = 4;
			item_6_bounce[i]++;
		}
		else if (item_6.y + item_6.h >= 800)
		{
			item_6_direction[i] = 2;
			item_6_bounce[i]++;
		}
	}
	else if (item_6_direction[i] == 2)
	{
		if (item_6.x + item_6.w >= 500)
		{
			item_6_direction[i] = 1;
			item_6_bounce[i]++;
		}
		else if (item_6.y <= 0)
		{
			item_6_direction[i] = 3;
			item_6_bounce[i]++;
		}
	}
	else if (item_6_direction[i] == 1)
	{
		if (item_6.x <= 0)
		{
			item_6_direction[i] = 2;
			item_6_bounce[i]++;
		}
		else if (item_6.y <= 0)
		{
			item_6_direction[i] = 4;
			item_6_bounce[i]++;
		}
	}

}
void ITEM6_MOVE(Move& item_6, int i)
{
	if (item_6_direction[i] == 0)
	{
		item_6.y -= 13;
		ITEM6_DIRECTION(item_6, i);
	}
	else if (item_6_direction[i] == 1)
	{
		item_6.x -= 13;
		item_6.y -= 13;
		ITEM6_DIRECTION(item_6, i);
	}
	else if (item_6_direction[i] == 2)
	{
		item_6.x += 13;
		item_6.y -= 13;
		ITEM6_DIRECTION(item_6, i);
	}
	else if (item_6_direction[i] == 3)
	{
		item_6.x += 13;
		item_6.y += 13;
		ITEM6_DIRECTION(item_6, i);
	}
	else if (item_6_direction[i] == 4)
	{
		item_6.x -= 13;
		item_6.y += 13;
		ITEM6_DIRECTION(item_6, i);
	}
}
void ITEM6(Move& item_6, int i)
{
	item_6.picX = 0 + (item_6_frame[i] * 114);
	item_6_frame[i]++;
	ITEM6_MOVE(item_6, i);
	if (item_6_frame[i] >= 5)
	{
		item_6_frame[i] = 0;
	}
	if (item_6_bounce[i] >= 6)
	{
		item_6_bounce[i] = 0;
		item_6_Flag[i] = 0;
		item_6_frame[i] = 0;
		item_6_direction[i] = 0;
	}
}

//아이템 8
static int item_8_Flag[10] = { 0, };
static int item_8_frame[10] = { 0, };
void ITEM8(Move& player, Move& item_8, int i)
{
	item_8.picX = 0 + (item_8_frame[i] * 157);
	item_8_frame[i]++;

	if (item_8_frame[i] >= 7)
	{
		item_8_frame[i] = 0;
		if (item_8_Flag[i] != 2)
		{
			item_8_Flag[i] = 2;//앞으로 돌진 플래그 on
			item_8.x = player.x - 65;
			item_8.y = player.y - 60;
		}
	}
	if (item_8_Flag[i] == 2)
	{
		item_8.y -= 7;
		if (item_8.y <= -300)
		{
			item_8_Flag[i] = 0;
		}
	}
}

//아이템 10
static int item_10_Flag[10] = { 0, };
void ITEM10(Move& player, Move& item_10, int i)
{

	item_10.y -= 5;

	if (item_10.y + item_10.h <= 0)
	{
		item_10_Flag[i] = 0;
	}
}

//방해요소 2 플래그
static int interrupt_ITEM2_Flag = 0;
static int interrupt_ITEM2_timer = 0;
static float interrupt_ITEM2_frame = 0;
void interrupted_ITEM_2()
{
	if (interrupt_ITEM2_timer < 60)
	{
		interrupt_ITEM2_timer++;
		interrupt_ITEM2_frame += 10;
	}
	else if (interrupt_ITEM2_timer >= 60)
	{
		interrupt_ITEM2_frame -= 10;

		if (interrupt_ITEM2_frame <= 0)
		{
			interrupt_ITEM2_Flag = 0;
			interrupt_ITEM2_timer = 0;
		}
	}
}
//무적 키
static int key = 0;


int check_menu(int mx, int my)
{
	if (350 < mx && mx < 500)
	{
		if (500 < my && my < 550)
		{
			return 1;
		}
		else if (570 < my && my < 620)
		{
			return 2;
		}
		else if (640 < my && my < 690)
		{
			return 3;
		}

	}
	return 0;
}

void RecvObject();

void DrawItems(HDC memDC);

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hDC, memdc;               //디바이스 콘텍스트를 저장할 변수 HDC와 
	PAINTSTRUCT ps;            //출력영역(디콘)에 대한 정보를 저장할 변수 PS선언.
	static HBITMAP hBit, oldBit;

	HBRUSH hBrush, oldBrush;
	HFONT myFont, oldFont;

	input.UP = false;
	input.DOWN = false;
	input.LEFT = false;
	input.RIGHT = false;

	switch (iMessage)
	{
	case WM_CREATE:

		srand((unsigned)time(NULL));
		SetTimer(hWnd, 1, 17, NULL);

		gameValueInit();

		break;
	case WM_CHAR:
		switch (wParam)
		{
		case 'p':
		case 'P':
			if (START)
			{
				if (!pause)
				{
					KillTimer(hWnd, 1);
					pause = true;
				}
				else
				{
					SetTimer(hWnd, 1, 17, NULL);
					pause = false;
				}
			}
			break;
		}
		InvalidateRgn(hWnd, NULL, FALSE);
		break;
	case WM_KEYDOWN:
		if (GetKeyState(VK_LEFT) & 0x8000)
			input.LEFT = true;
		if (GetKeyState(VK_RIGHT) & 0x8000)
			input.RIGHT = true;
		if (GetKeyState(VK_UP) & 0x8000)
			input.UP = true;
		if (GetKeyState(VK_DOWN) & 0x8000)
			input.DOWN = true;


		InvalidateRgn(hWnd, NULL, FALSE);
		break;
	case WM_LBUTTONDOWN:
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		if (!START)
		{
			if (itme_menu_check == 6)
			{

				if (350 < mx && mx < 500)
				{
					if ((500 < my && my < 550) && menu_check == 1)
					{
						itme_menu_check = 0;
					}
					else if ((570 < my && my < 620) && (menu_check == 2))
					{
						START = true;
					}
					else if ((640 < my && my < 690) && (menu_check == 3))
					{
						START = true;
						mode_2p = true;
					}

				}
			}
			else
			{
				if (20 < mx && mx < 120)
				{
					if (20 < my && my < 70)
					{
						if (itme_menu_check == 2)
						{
							itme_menu_check = 6;
						}
						else if (itme_menu_check == 4)
						{
							itme_menu_check = 6;
						}
					}
				}
				if (380 < mx && mx < 480)
				{
					if (20 < my && my < 70)
					{
						if (itme_menu_check == 3)
						{
							itme_menu_check = 1;
						}
						else if (itme_menu_check == 5)
						{
							itme_menu_check = 0;
						}
					}
				}
			}
		}
		if (Gameover)
		{
			if (180 < mx && mx < 320)
			{
				if (450 < my && my < 520)
				{
					START = false;
					Gameover = false;
					menu_check = 0;

					SecondPlayer.x = 230;
					SecondPlayer.y = 700;
					player.x = 235;
					player.y = 650;
					score = 0;
					bug_num = 0;
					player.state = 1;
					SecondPlayer.state = 1;
					mode_2p = false;
					for (int i = 0; i < 500; i++)
					{
						bug[i].x = -100;
						bug[i].y = -100;
						bug[i].state = 0;
					}
					item_Drop_Timer = 0;
					item_Count = 0;
					for (int i = 0; i < 20; ++i)
					{
						item_Drop[i].state = 0;
					}
					item_1_count = 0;
					item_6_count = 0;
					item_8_count = 0;
					item_10_count = 0;
					for (int i = 0; i < 10; i++)
					{
						item_1_Flag[i] = 0;
						item_6_Flag[i] = 0;
						item_8_Flag[i] = 0;
						item_10_Flag[i] = 0;
					}
					interrupt_ITEM2_Flag = 0;
					key = 0;

				}
			}
		}
		break;
	case WM_LBUTTONUP:
		InvalidateRgn(hWnd, NULL, FALSE);
		break;
	case WM_MOUSEMOVE:
		if (!START)
		{
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
			menu_check = check_menu(mx, my);
		}
		if (itme_menu_check < 6)
		{
			if (itme_menu_check == 0 || itme_menu_check == 2 || itme_menu_check == 3)
			{
				if (20 < my && my < 70)
				{
					if (20 < mx && mx < 120)
					{
						itme_menu_check = 2;
					}
					else if (380 < mx && mx < 480)
					{
						itme_menu_check = 3;
					}
					else
					{
						itme_menu_check = 0;
					}
				}
				else
				{
					itme_menu_check = 0;
				}
			}
			if (itme_menu_check == 1 || itme_menu_check == 4 || itme_menu_check == 5)
			{
				if (20 < my && my < 70)
				{
					if (20 < mx && mx < 120)
					{
						itme_menu_check = 4;
					}
					else if (380 < mx && mx < 480)
					{
						itme_menu_check = 5;
					}
					else
					{
						itme_menu_check = 1;
					}
				}
				else
				{
					itme_menu_check = 1;
				}
			}
		}

		if (Gameover)
		{
			mx = LOWORD(lParam);
			my = HIWORD(lParam);

			if (180 < mx && mx < 320)
			{
				if (450 < my && my < 520)
				{
					over_check = 1;
				}
				else
				{
					over_check = 0;
				}
			}
		}
		break;
	case WM_TIMER:
		send(sock, (char*)&input, sizeof(input), 0);
		cout << "send input" << endl;
		RecvObject();
		InvalidateRgn(hWnd, NULL, FALSE);
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		memdc = CreateCompatibleDC(hDC);

		hBit = CreateCompatibleBitmap(hDC, 1000, 800);
		SelectObject(memdc, hBit);

		BACKGROUND.Draw(memdc, backGround1.x, backGround1.y, backGround1.w, backGround1.h, backGround1.picX, backGround1.picY, backGround1.picW, backGround1.picH);
		BACKGROUND.Draw(memdc, backGround2.x, backGround2.y, backGround2.w, backGround2.h, backGround2.picX, backGround2.picY, backGround2.picW, backGround2.picH);

		//아이템 드롭
		for (int i = 0; i < 20; ++i)
		{
			if (item_Drop[i].state == 1)
				image1.TransparentBlt(memdc, item_Drop[i].x, item_Drop[i].y, item_Drop[i].w, item_Drop[i].h, item_Drop[i].picX, item_Drop[i].picY, item_Drop[i].picW, item_Drop[i].picH, RGB(0, 255, 255));
			if (item_Drop[i].state == 2)
				INTERRUPT_ITEM2.Draw(memdc, item_Drop[i].x, item_Drop[i].y, item_Drop[i].w, item_Drop[i].h, item_Drop[i].picX, item_Drop[i].picY, item_Drop[i].picW, item_Drop[i].picH);
			if (item_Drop[i].state == 3)
				image6.TransparentBlt(memdc, item_Drop[i].x, item_Drop[i].y, item_Drop[i].w, item_Drop[i].h, item_Drop[i].picX, item_Drop[i].picY, item_Drop[i].picW, item_Drop[i].picH, RGB(0, 255, 255));
			if (item_Drop[i].state == 4)
				image8.TransparentBlt(memdc, item_Drop[i].x, item_Drop[i].y, item_Drop[i].w, item_Drop[i].h, item_Drop[i].picX, item_Drop[i].picY, item_Drop[i].picW, item_Drop[i].picH, RGB(0, 255, 255));
			if (item_Drop[i].state == 5)
				image10.TransparentBlt(memdc, item_Drop[i].x, item_Drop[i].y, item_Drop[i].w, item_Drop[i].h, item_Drop[i].picX, item_Drop[i].picY, item_Drop[i].picW, item_Drop[i].picH, RGB(0, 255, 255));
		}

		DrawItems(memdc);

		//플레이어
		if (player.state == 1)
		{
			PLAYER.TransparentBlt(memdc, player.x, player.y, player.w, player.h, player.picX, player.picY, player.picW, player.picH, RGB(255, 255, 255));//플레이어
		}
		if (mode_2p)
		{
			if (SecondPlayer.state == 1)
			{
				SECONDPLAYER.Draw(memdc, SecondPlayer.x, SecondPlayer.y, SecondPlayer.w, SecondPlayer.h, SecondPlayer.picX, SecondPlayer.picY, SecondPlayer.picW, SecondPlayer.picH);//플레이어
			}
		}

		//버그
		for (int i = 0; i < MONSTER; i++)
		{

			if (bug[i].state == 1)
			{
				BUG_image.TransparentBlt(memdc, bug[i].x, bug[i].y, player.w, player.h, player.picX, player.picY, 72, 73, RGB(255, 255, 255));
			}
			if (bug[i].impact_num != 9)
			{

				impact[bug[i].impact_num].Draw(memdc, bug[i].x - 15, bug[i].y - 15, player.w + 15, player.h + 15, bug[i].impact_time * 172, 0, 172, 160);
				bug[i].impact_time++;
				if (bug[i].impact_time == 4)
				{
					bug[i].impact_time = 0;
					bug[i].impact_num = 9;
				}
			}
		}
		//방해요소 2
		if (interrupt_ITEM2_Flag == 1)
		{
			hBrush = CreateSolidBrush(RGB(0, 0, 0));
			oldBrush = ((HBRUSH)SelectObject(memdc, hBrush));
			Rectangle(memdc, 0, 0, 500, interrupt_ITEM2_frame);
			SelectObject(memdc, oldBrush);
			DeleteObject(hBrush);
		}


		//점수
		if (Gameover)
		{
			GAME_OVER[over_check].Draw(memdc, 0, 0, 500, 800, 0, 0, 606, 1080);
			SetBkMode(memdc, TRANSPARENT);
			myFont = CreateFont(40, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("고딕"));
			oldFont = (HFONT)SelectObject(memdc, myFont);
			wsprintf(str, "%d  ", score);
			TextOut(memdc, 200, 385, str, lstrlen(str));
			SelectObject(memdc, oldFont);
			DeleteObject(myFont);
		}
		else
		{
			SetTextColor(memdc, RGB(255, 228, 0));
			SetBkMode(memdc, TRANSPARENT);
			myFont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("고딕"));
			oldFont = (HFONT)SelectObject(memdc, myFont);
			wsprintf(str, "SCORE : %d  ", score);
			TextOut(memdc, 10, 10, str, lstrlen(str));
			if (key == 1)
			{
				wsprintf(str, "무적 ");
				TextOut(memdc, 400, 10, str, lstrlen(str));
			}
			SelectObject(memdc, oldFont);
			DeleteObject(myFont);
		}

		if (pause)
		{
			SetTextColor(memdc, RGB(255, 255, 255));
			SetBkMode(memdc, TRANSPARENT);
			myFont = CreateFont(80, 00, 0, 0, 500, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("고딕"));
			oldFont = (HFONT)SelectObject(memdc, myFont);
			wsprintf(str, "PAUSE  ", score);
			TextOut(memdc, 110, 200, str, lstrlen(str));
			SelectObject(memdc, oldFont);
			DeleteObject(myFont);
		}

		if (!START)
		{
			//cout << mx << my << endl;
			menu[menu_check].Draw(memdc, 0, 0, 500, 800, 0, 0, 596, 996);
		}
		if (itme_menu_check < 6)
		{
			item_menu[itme_menu_check].Draw(memdc, 0, 0, 500, 800, 0, 0, 500, 900);

		}

		BitBlt(hDC, 0, 0, 500, 800, memdc, 0, 0, SRCCOPY);

		DeleteObject(hBit);
		DeleteDC(memdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void gameValueInit()
{
	item_1_count = 0;
	item_6_count = 0;
	item_8_count = 0;
	item_10_count = 0;

	for (auto& i : bug)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	recv(sock, (char*)&player, sizeof(player), 0);

	recv(sock, (char*)&backGround1, sizeof(backGround1), 0);
	recv(sock, (char*)&backGround2, sizeof(backGround2), 0);

	for (auto& i : item_1)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : item_6)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : item_8)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : item_10)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	BACKGROUND.Load(TEXT("진짜배경1.png"));
	ITEM_1.Load(TEXT("아이템11.png"));
	ITEM_6.Load(TEXT("아이템6.png"));
	ITEM_8.Load(TEXT("아이템8.png"));
	ITEM_10.Load(TEXT("아이템10.png"));
	INTERRUPT_ITEM2.Load(TEXT("방해2.png"));
	PLAYER.Load(TEXT("레이디버그.bmp"));
	SECONDPLAYER.Load(TEXT("레이디버그2.png"));
	BUG_image.Load(TEXT("적.bmp"));

	image1.Load(TEXT("1.png"));
	image2.Load(TEXT("2.png"));
	image6.Load(TEXT("6.png"));
	image8.Load(TEXT("8.png"));
	image10.Load(TEXT("10.png"));

	impact[0].Load(TEXT("빨간 빤짝.png"));
	impact[1].Load(TEXT("주황 빤짝.png"));
	impact[2].Load(TEXT("노란 빤짝.png"));
	impact[3].Load(TEXT("연두 빤짝.png"));
	impact[4].Load(TEXT("초록 빤짝.png"));
	impact[5].Load(TEXT("하늘 빤짝.png"));
	impact[6].Load(TEXT("파란 빤짝.png"));
	impact[7].Load(TEXT("보라 빤짝.png"));

	menu[0].Load(TEXT("메뉴.png"));
	menu[1].Load(TEXT("메뉴_아이템.png"));
	menu[2].Load(TEXT("메뉴_1p.png"));
	menu[3].Load(TEXT("메뉴_2p.png"));

	item_menu[0].Load(TEXT("아이템 메뉴1.png"));
	item_menu[1].Load(TEXT("아이템 메뉴2.png"));
	item_menu[2].Load(TEXT("아이템 메뉴1-1.png"));
	item_menu[3].Load(TEXT("아이템 메뉴1-2.png"));
	item_menu[4].Load(TEXT("아이템 메뉴2-1.png"));
	item_menu[5].Load(TEXT("아이템 메뉴2-2.png"));

	GAME_OVER[0].Load(TEXT("종료 화면1.png"));
	GAME_OVER[1].Load(TEXT("종료 화면2.png"));

	mx = 235;
	my = 700;
	score = 0;

	
}

void RecvObject()
{
	for (auto& i : bug)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	recv(sock, (char*)&player, sizeof(player), 0);

	recv(sock, (char*)&backGround1, sizeof(backGround1), 0);
	recv(sock, (char*)&backGround2, sizeof(backGround2), 0);

	for (auto& i : item_1)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : item_6)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : item_8)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : item_10)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	for (int i = 0; i < 10; ++i)
	{
		recv(sock, (char*)&item_1_Flag[i], sizeof(item_1_Flag[i]), 0);
		recv(sock, (char*)&item_1_frame[i], sizeof(item_1_frame[i]), 0);
		recv(sock, (char*)&item_1_carry[i], sizeof(item_1_carry[i]), 0);
	}

	for (int i = 0; i < 10; ++i)
	{
		recv(sock, (char*)&item_6_Flag[i], sizeof(item_6_Flag[i]), 0);
		recv(sock, (char*)&item_6_frame[i], sizeof(item_6_frame[i]), 0);
		recv(sock, (char*)&item_6_direction[i], sizeof(item_6_direction[i]), 0);
		recv(sock, (char*)&item_6_bounce[i], sizeof(item_6_bounce[i]), 0);
	}

	for (int i = 0; i < 10; ++i)
	{
		recv(sock, (char*)&item_8_Flag[i], sizeof(item_8_Flag[i]), 0);
		recv(sock, (char*)&item_8_frame[i], sizeof(item_8_frame[i]), 0);
	}

	for (int i = 0; i < 10; ++i)
	{
		recv(sock, (char*)&item_10_Flag[i], sizeof(item_8_Flag[i]), 0);
	}

	recv(sock, (char*)&interrupt_ITEM2_Flag, sizeof(interrupt_ITEM2_Flag), 0);
	recv(sock, (char*)&interrupt_ITEM2_timer, sizeof(interrupt_ITEM2_timer), 0);
	recv(sock, (char*)&interrupt_ITEM2_frame, sizeof(interrupt_ITEM2_frame), 0);

	for (auto& i : item_Drop)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

}

void DrawItems(HDC memdc)
{
	//아이템 그리기
	for (int i = 0; i < 10; ++i)
	{
		//아이템 1
		if (item_1_Flag[i] == 1)
		{
			ITEM_1.Draw(memdc, item_1[i].x, item_1[i].y, item_1[i].w, item_1[i].h, item_1[i].picX, item_1[i].picY, item_1[i].picW, item_1[i].picH);
		}

		//아이템 6
		if (item_6_Flag[i] == 1)
		{
			ITEM_6.Draw(memdc, item_6[i].x, item_6[i].y, item_6[i].w, item_6[i].h, item_6[i].picX, item_6[i].picY, item_6[i].picW, item_6[i].picH);
		}

		//아이템 8
		if (item_8_Flag[i] != 0)
		{
			if (item_8[i].collisionWithWho == 1)
			{
				//주변 맴돌 때
				if (item_8_Flag[i] == 1)
					ITEM_8.Draw(memdc, player.x - 65, player.y - 60, item_8[i].w, item_8[i].h, item_8[i].picX, item_8[i].picY, item_8[i].picW, item_8[i].picH);
				//앞으로 돌진할 때
				if (item_8_Flag[i] == 2)
					ITEM_8.Draw(memdc, item_8[i].x, item_8[i].y, item_8[i].w, item_8[i].h, item_8[i].picX, item_8[i].picY, item_8[i].picW, item_8[i].picH);
			}
			else
			{
				//주변 맴돌 때
				if (item_8_Flag[i] == 1)
					ITEM_8.Draw(memdc, SecondPlayer.x - 65, SecondPlayer.y - 60, item_8[i].w, item_8[i].h, item_8[i].picX, item_8[i].picY, item_8[i].picW, item_8[i].picH);
				//앞으로 돌진할 때
				if (item_8_Flag[i] == 2)
					ITEM_8.Draw(memdc, item_8[i].x, item_8[i].y, item_8[i].w, item_8[i].h, item_8[i].picX, item_8[i].picY, item_8[i].picW, item_8[i].picH);

			}
		}

		//아이템 10
		if (item_10_Flag[i] != 0)
		{
			ITEM_10.Draw(memdc, item_10[i].x, item_10[i].y, item_10[i].w, item_10[i].h, item_10[i].picX, item_10[i].picY, item_10[i].picW, item_10[i].picH);
		}
	}
}
