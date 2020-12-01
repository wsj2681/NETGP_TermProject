// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <atlimage.h>

#include <time.h>
#include <math.h>
#include <iostream>

using namespace std;

#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#define MONSTER 10
#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

#define ITEM_TYPE 8

#pragma pack(push, 1)
struct Move {

	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;

	int picX = 0;
	int picY = 0;
	int picW = 0;
	int picH = 0;

	int y_move = 0;
	int x_move = 0;

	int state = 0;

	int collisionWithWho = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
static struct BUG
{
	int x = 0;
	int y = 0;
	int y_move = 0;
	int x_move = 0;
	int w = 0;
	int h = 0;

	int impact_num = 9;
	int impact_time = 0;

	int state = 0;

}bug[MONSTER];
#pragma pack(pop)

#pragma pack(push, 1)
struct Input
{
	bool UP = false;
	bool DOWN = false;
	bool LEFT = false;
	bool RIGHT = false;
};
#pragma pack(pop)


static CImage BACKGROUND;
static Move backGround1;
static Move backGround2;

static CImage BigFlowerSheet;
static Move itemBigFlower[10];
static int BIgFlowerindex;  
							
static CImage BallBugSheet;
static Move itemBallBug[10];
static int BallBugIndex;  

static CImage CosMosSheet;
static Move cosmos[10];
static int cosmosIndex;  
						  
static CImage Barrior;
static Move itemBarrior[10];
static int barriorIndex; 

static CImage PLAYER;
static Move player;

static CImage SECONDPLAYER;
static Move SecondPlayer;

static CImage BUG_image;

static CImage INTERRUPT_ITEM2;

static TCHAR str[100];

static CImage imageBigFlower;
static CImage imageBallBug;
static CImage imageCosMos;
static CImage imageBarrior;

static int item_Drop_Timer;
static Move item_Drop[20];
static int item_Count;

static CImage impact[8];

static CImage GAME_OVER[2];
static CImage menu[4];
static CImage item_menu[6];

static BOOL Gameover = false;
static BOOL Gameover2 = false;

static int mx, my;
static int score;
static int bug_num;

static int item_1_Flag[10] = { 0, };
static int item_1_frame[10] = { 0, };
static int item_1_carry[10] = { 0, };

//아이템 6
static int item_6_Flag[10] = { 0, };
static int item_6_frame[10] = { 0, };
static int item_6_direction[10] = { 0, };
static int item_6_bounce[10] = { 0, };

//아이템 8
static int item_8_Flag[10] = { 0, };
static int item_8_frame[10] = { 0, };

//아이템 10
static int item_10_Flag[10] = { 0, };

//방해요소 2 플래그
static int interrupt_ITEM2_Flag = 0;
static int interrupt_ITEM2_timer = 0;
static int interrupt_ITEM2_frame = 0;


static BOOL pause = false;
static BOOL START = false;
static BOOL mode_2p = false;

static int menu_check = 0;
static int over_check = 0;
static int itme_menu_check = 6;


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