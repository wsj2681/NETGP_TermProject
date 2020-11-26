#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define WINDOWWIDTH 500
#define WINDOWHEIGHT 800

// Windows 헤더 파일
#include <windows.h>
#include <WinUser.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <atlimage.h>


#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include <WinSock2.h>
#pragma comment(lib, "ws2_32")

const char* SERVERIP = "127.0.0.1";
constexpr int SERVERPORT = 9000;
constexpr int BUFSIZE = 512;

#pragma pack(push, 1)
struct Object_Data
{
    short x = 0;
    short y = 0;
    int width = 30;
    int height = 30;

    bool isCollide = false;
    bool isActive = true;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Object_Info
{
    short x = 0;
    short y = 0;
    int width = 0;
    int height = 0;

    int picX = 0;
    int picY = 0;
    int picWidth = 0;
    int picHeight = 0;

    bool isCollide = false;
    bool isActive = true;

    CImage image;

    void operator=(const Object_Data& data)
    {
        x = data.x;
        y = data.y;
        width = data.width;
        height = data.height;
        isActive = data.isActive;
        isCollide = data.isCollide;
    }
};
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