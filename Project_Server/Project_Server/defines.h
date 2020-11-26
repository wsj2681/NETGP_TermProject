#pragma once


#define BUFFERSIZE 512
#define SERVERPORT 9000
#define WINDOWWIDTH 500
#define WINDOWHEIGHT 800

#define MAXTHREAD 10

#pragma pack(push, 1)
struct Object
{
	short x = 0;
	short y = 0;
	int width = 30;
	int height = 30;

	bool isCollide = false;
	bool isActive = true;
};
#pragma pack(pop)