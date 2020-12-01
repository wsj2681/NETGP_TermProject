#pragma once

#define MONSTER 10

#define BUFFERSIZE 512
#define SERVERPORT 9000
#define WINDOWWIDTH 500
#define WINDOWHEIGHT 800

#define MAXTHREAD 10

#include <iostream>
#include <random>
#include <vector>
using namespace std;

#include <WinSock2.h>
#pragma comment(lib, "ws2_32")

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


static Move backGround1;
static Move backGround2;

static Move item_1[10];
static int item_1_count;
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

static Move item_6[10];
static int item_6_count;
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

static Move item_8[10];
static int item_8_count;
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

static Move item_10[10];
static int item_10_count;
static int item_10_Flag[10] = { 0, };
void ITEM10(Move& player, Move& item_10, int i)
{

	item_10.y -= 5;

	if (item_10.y + item_10.h <= 0)
	{
		item_10_Flag[i] = 0;
	}
}

static Move player;
static Move player2;

static int item_Drop_Timer;
static Move item_Drop[20];
static int item_Count;

static int interrupt_ITEM2_Flag = 0;
static int interrupt_ITEM2_timer = 0;
static int interrupt_ITEM2_frame = 0;

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

static int mx, my;
static int score;
static int bug_num;

static BOOL Gameover = false;
static BOOL pause = false;
static BOOL START = false;

static int menu_check = 0;
static int over_check = 0;
static int itme_menu_check = 6;

float bug_x_move(BUG bug, Move player)
{
	float bug_lean = ((float)(player.y - bug.y) / (float)(player.x - bug.x));
	bug.x_move = bug.y_move / bug_lean;
	return bug.x_move;
}