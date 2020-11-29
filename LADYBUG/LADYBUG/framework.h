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
#define MONSTER 500
#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

#define ITEM_TYPE 8

#pragma pack(push, 1)
typedef struct Move {

	float x;
	float y;
	int w;
	int h;

	int picX;
	int picY;
	int picW;
	int picH;

	int y_move;
	int x_move;

	int state = 0;

	int collisionWithWho = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
static struct BUG
{
	float x;
	float y;
	float y_move;
	float x_move;
	int w;
	int h;

	int impact_num = 9;
	int impact_time = 0;

	int state = 0;

}bug[MONSTER];
#pragma pack(pop)

#pragma pack(push, 1)
static struct Input
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

static CImage ITEM_1;
static Move item_1[10];
static int item_1_count;    //배열몇번짼지 관리
							//
static CImage ITEM_6;
static Move item_6[10];
static int item_6_count;   //배열몇번짼지 관리

static CImage ITEM_8;
static Move item_8[10];
static int item_8_count;   //배열몇번짼지 관리
						   //
static CImage ITEM_10;
static Move item_10[10];
static int item_10_count;   //배열몇번짼지 관리

static CImage PLAYER;
static Move player;
static CImage SECONDPLAYER;
static Move SecondPlayer;

static CImage BUG_image;

static CImage INTERRUPT_ITEM2;

static TCHAR str[100];

static CImage image1;
static CImage image2;
static CImage image6;
static CImage image8;
static CImage image10;

static int item_Drop_Timer;
static Move item_Drop[20];
static int item_Count;

static CImage impact[8];

static CImage GAME_OVER[2];
static CImage menu[4];
static CImage item_menu[6];

static BOOL Gameover = false;
static float mx, my;
static int score;
static int bug_num;



static BOOL pause = false;
static BOOL START = false;
static BOOL mode_2p = false;

static int menu_check = 0;
static int over_check = 0;
static int itme_menu_check = 6;