#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "framework.h"

void err_quit(char* msg);
void err_display(char* msg);

Input input; // 키입력
SOCKET sock; // 소켓

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

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

	//send(sock, (char*)&Gameover, sizeof(Gameover), 0);

	return (int)Message.wParam;
}

HINSTANCE hInst;

void gameValueInit();

int threadnum = 0;

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

void MenuClick();
void ItemMenuClick(LPARAM lParam);
void RecvObject();
void DrawItems(HDC memDC);

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hDC, memdc;               //디바이스 콘텍스트를 저장할 변수 HDC와 
	PAINTSTRUCT ps;            //출력영역(디콘)에 대한 정보를 저장할 변수 PS선언.
	static HBITMAP hBit, oldBit;

	HBRUSH hBrush, oldBrush;
	HFONT myFont, oldFont;


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
		MenuClick();
		break;
	case WM_LBUTTONUP:
		InvalidateRgn(hWnd, NULL, FALSE);
		break;
	case WM_MOUSEMOVE:
		ItemMenuClick(lParam);
		break;
	case WM_TIMER:
		send(sock, (char*)&input, sizeof(input), 0);
		cout << "send input" << endl;

		input.UP = false;
		input.DOWN = false;
		input.LEFT = false;
		input.RIGHT = false;

		RecvObject();
		InvalidateRgn(hWnd, NULL, FALSE);

		if (!player.state)
		{
			Gameover = true;
		}
		else if (!SecondPlayer.state)
		{
			Gameover2 = true;
		}
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		memdc = CreateCompatibleDC(hDC);

		hBit = CreateCompatibleBitmap(hDC, 1000, 800);
		SelectObject(memdc, hBit);

		BACKGROUND.Draw(memdc, backGround1.x, backGround1.y, backGround1.w, backGround1.h, backGround1.picX, backGround1.picY, backGround1.picW, backGround1.picH);
		BACKGROUND.Draw(memdc, backGround2.x, backGround2.y, backGround2.w, backGround2.h, backGround2.picX, backGround2.picY, backGround2.picW, backGround2.picH);

		DrawItems(memdc);

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
		if (Gameover && threadnum == 0)
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
		else if (Gameover2 && threadnum ==1)
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
	BIgFlowerindex = 0;
	BallBugIndex = 0;
	cosmosIndex = 0;
	barriorIndex = 0;

	for (auto& i : bug)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	recv(sock, (char*)&player, sizeof(player), 0);
	recv(sock, (char*)&SecondPlayer, sizeof(SecondPlayer), 0);

	recv(sock, (char*)&backGround1, sizeof(backGround1), 0);
	recv(sock, (char*)&backGround2, sizeof(backGround2), 0);

	for (auto& i : itemBigFlower)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : itemBallBug)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : cosmos)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : itemBarrior)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}
	
	recv(sock, (char*)&threadnum, sizeof(threadnum), 0);

	INTERRUPT_ITEM2.Load(TEXT("Images/방해2.png"));
	BigFlowerSheet.Load(TEXT("Images/아이템11.png"));
	BallBugSheet.Load(TEXT("Images/아이템6.png"));
	CosMosSheet.Load(TEXT("Images/아이템8.png"));
	BACKGROUND.Load(TEXT("Images/진짜배경1.png"));
	Barrior.Load(TEXT("Images/아이템10.png"));
	PLAYER.Load(TEXT("Images/레이디버그.bmp"));
	SECONDPLAYER.Load(TEXT("Images/레이디버그2.png"));
	BUG_image.Load(TEXT("Images/적.bmp"));

	imageBigFlower.Load(TEXT("Images/1.png"));
	imageBallBug.Load(TEXT("Images/6.png"));
	imageCosMos.Load(TEXT("Images/8.png"));
	imageBarrior.Load(TEXT("Images/10.png"));

	menu[0].Load(TEXT("Images/메뉴.png"));
	menu[1].Load(TEXT("Images/메뉴_아이템.png"));
	menu[2].Load(TEXT("Images/메뉴_1p.png"));
	menu[3].Load(TEXT("Images/메뉴_2p.png"));

	item_menu[0].Load(TEXT("Images/아이템 메뉴1.png"));
	item_menu[1].Load(TEXT("Images/아이템 메뉴2.png"));
	item_menu[2].Load(TEXT("Images/아이템 메뉴1-1.png"));
	item_menu[3].Load(TEXT("Images/아이템 메뉴1-2.png"));
	item_menu[4].Load(TEXT("Images/아이템 메뉴2-1.png"));
	item_menu[5].Load(TEXT("Images/아이템 메뉴2-2.png"));

	GAME_OVER[0].Load(TEXT("Images/종료 화면1.png"));
	GAME_OVER[1].Load(TEXT("Images/종료 화면2.png"));

	mx = 235;
	my = 700;
	score = 0;
}

void MenuClick()
{
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
	if (Gameover && threadnum == 0)
	{
		if (180 < mx && mx < 320)
		{
			if (450 < my && my < 520)
			{
				START = false;
				menu_check = 0;

				SecondPlayer.x = 230;
				SecondPlayer.y = 700;
				player.x = 235;
				player.y = 650;
				score = 0;

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
				BIgFlowerindex = 0;
				BallBugIndex = 0;
				cosmosIndex = 0;
				barriorIndex = 0;
				for (int i = 0; i < 10; i++)
				{
					item_1_Flag[i] = 0;
					item_6_Flag[i] = 0;
					item_8_Flag[i] = 0;
					item_10_Flag[i] = 0;
				}
				interrupt_ITEM2_Flag = 0;
				key = 0;

				closesocket(sock);
				// 윈속 종료
				WSACleanup();


				PostQuitMessage(0);
			}
		}
	}
	if (Gameover2 && threadnum == 1)
	{
		if (180 < mx && mx < 320)
		{
			if (450 < my && my < 520)
			{
				START = false;
				menu_check = 0;

				SecondPlayer.x = 230;
				SecondPlayer.y = 700;
				player.x = 235;
				player.y = 650;
				score = 0;

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
				BIgFlowerindex = 0;
				BallBugIndex = 0;
				cosmosIndex = 0;
				barriorIndex = 0;
				for (int i = 0; i < 10; i++)
				{
					item_1_Flag[i] = 0;
					item_6_Flag[i] = 0;
					item_8_Flag[i] = 0;
					item_10_Flag[i] = 0;
				}
				interrupt_ITEM2_Flag = 0;
				key = 0;

				closesocket(sock);
				// 윈속 종료
				WSACleanup();
				PostQuitMessage(0);
			}
		}
	}
}

void ItemMenuClick(LPARAM lParam)
{
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
}
void RecvObject()
{

	for (auto& i : bug)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	recv(sock, (char*)&player, sizeof(player), 0);
	recv(sock, (char*)&SecondPlayer, sizeof(SecondPlayer), 0);


	for (auto& i : itemBigFlower)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : itemBallBug)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : cosmos)
	{
		recv(sock, (char*)&i, sizeof(i), 0);
	}

	for (auto& i : itemBarrior)
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
	for (int i = 0; i < 20; ++i)
	{
		if (item_Drop[i].state == 1)
			imageBigFlower.TransparentBlt(memdc, item_Drop[i].x, item_Drop[i].y, item_Drop[i].w, item_Drop[i].h, item_Drop[i].picX, item_Drop[i].picY, item_Drop[i].picW, item_Drop[i].picH, RGB(0, 255, 255));
		if (item_Drop[i].state == 2)
			INTERRUPT_ITEM2.Draw(memdc, item_Drop[i].x, item_Drop[i].y, item_Drop[i].w, item_Drop[i].h, item_Drop[i].picX, item_Drop[i].picY, item_Drop[i].picW, item_Drop[i].picH);
		if (item_Drop[i].state == 3)
			imageBallBug.TransparentBlt(memdc, item_Drop[i].x, item_Drop[i].y, item_Drop[i].w, item_Drop[i].h, item_Drop[i].picX, item_Drop[i].picY, item_Drop[i].picW, item_Drop[i].picH, RGB(0, 255, 255));
		if (item_Drop[i].state == 4)
			imageCosMos.TransparentBlt(memdc, item_Drop[i].x, item_Drop[i].y, item_Drop[i].w, item_Drop[i].h, item_Drop[i].picX, item_Drop[i].picY, item_Drop[i].picW, item_Drop[i].picH, RGB(0, 255, 255));
		if (item_Drop[i].state == 5)
			imageBarrior.TransparentBlt(memdc, item_Drop[i].x, item_Drop[i].y, item_Drop[i].w, item_Drop[i].h, item_Drop[i].picX, item_Drop[i].picY, item_Drop[i].picW, item_Drop[i].picH, RGB(0, 255, 255));
	}

	//아이템 그리기
	for (int i = 0; i < 10; ++i)
	{
		//아이템 1
		if (item_1_Flag[i] == 1)
		{
			BigFlowerSheet.Draw(memdc, itemBigFlower[i].x, itemBigFlower[i].y, itemBigFlower[i].w, itemBigFlower[i].h, itemBigFlower[i].picX, itemBigFlower[i].picY, itemBigFlower[i].picW, itemBigFlower[i].picH);
		}

		//아이템 6
		if (item_6_Flag[i] == 1)
		{
			BallBugSheet.Draw(memdc, itemBallBug[i].x, itemBallBug[i].y, itemBallBug[i].w, itemBallBug[i].h, itemBallBug[i].picX, itemBallBug[i].picY, itemBallBug[i].picW, itemBallBug[i].picH);
		}

		//아이템 8
		if (item_8_Flag[i] != 0)
		{
			if (cosmos[i].collisionWithWho == 1)
			{
				//주변 맴돌 때
				if (item_8_Flag[i] == 1)
					CosMosSheet.Draw(memdc, player.x - 65, player.y - 60, cosmos[i].w, cosmos[i].h, cosmos[i].picX, cosmos[i].picY, cosmos[i].picW, cosmos[i].picH);
				//앞으로 돌진할 때
				if (item_8_Flag[i] == 2)
					CosMosSheet.Draw(memdc, cosmos[i].x, cosmos[i].y, cosmos[i].w, cosmos[i].h, cosmos[i].picX, cosmos[i].picY, cosmos[i].picW, cosmos[i].picH);
			}
			else
			{
				//주변 맴돌 때
				//if (item_8_Flag[i] == 1)
					//CosMosSheet.Draw(memdc, SecondPlayer.x - 65, SecondPlayer.y - 60, item_8[i].w, item_8[i].h, item_8[i].picX, item_8[i].picY, item_8[i].picW, item_8[i].picH);
				//앞으로 돌진할 때
				if (item_8_Flag[i] == 2)
					CosMosSheet.Draw(memdc, cosmos[i].x, cosmos[i].y, cosmos[i].w, cosmos[i].h, cosmos[i].picX, cosmos[i].picY, cosmos[i].picW, cosmos[i].picH);

			}
		}

		//아이템 10
		if (item_10_Flag[i] != 0)
		{
			Barrior.Draw(memdc, itemBarrior[i].x, itemBarrior[i].y, itemBarrior[i].w, itemBarrior[i].h, itemBarrior[i].picX, itemBarrior[i].picY, itemBarrior[i].picW, itemBarrior[i].picH);
		}
	}

	//플레이어
	if (player.state == 1)
	{
		PLAYER.TransparentBlt(memdc, player.x, player.y, player.w, player.h, player.picX, player.picY, player.picW, player.picH, RGB(255, 255, 255));//플레이어
	}

	if (SecondPlayer.state == 1)
	{
		SECONDPLAYER.TransparentBlt(memdc, SecondPlayer.x, SecondPlayer.y, SecondPlayer.w, player.h, SecondPlayer.picX, SecondPlayer.picY, SecondPlayer.picW, SecondPlayer.picH, RGB(255, 255, 255));//플레이어2
	}

	//버그
	for (int i = 0; i < MONSTER; i++)
	{
		if (bug[i].state == 1)
		{
			BUG_image.TransparentBlt(memdc, bug[i].x, bug[i].y, player.w, player.h, player.picX, player.picY, 72, 73, RGB(255, 255, 255));
		}
	}
}
