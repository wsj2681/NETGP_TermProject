// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
#include <WinUser.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <stdio.h>
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

struct InputFlag
{
	unsigned char UP : 1;	// 0x01
	unsigned char DOWN : 1;	// 0x02
	unsigned char RIGHT : 1;// 0x04
	unsigned char LEFT : 1;	// 0x08

	void Init()
	{
		UP = 0;
		DOWN = 0;
		RIGHT = 0;
		LEFT = 0;
	}
	
	friend ostream& operator<<(ostream& os, const InputFlag& input);
	
};

ostream& operator<<(ostream& os, const InputFlag& input)
{
	os << input.UP << " " << input.DOWN << " " << input.LEFT << " " << input.RIGHT;
	return os; 
}