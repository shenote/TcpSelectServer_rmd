// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.

#ifndef PCH_H
#define PCH_H

// TODO: 여기에 미리 컴파일하려는 헤더 추가
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <Windows.h>
#include <list>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

using namespace std;

// lib
#include "RingBuffer.h"
#include "PacketDefine.h"
#include "cPacketSerialz.h"

#include "MakePacket.h"
#include "cSector.h"

// engine
#include "Network.h"

// define
#define dfLOG_LEVEL_DEBUG	0
#define dfLOG_LEVEL_WARNING	1
#define dfLOG_LEVEL_ERROR	2

#define dfFRAME_COUNT 40

#define dfERROR_RANG 50
#define dfRECKONING_SPEED_PLAYER_X 6
#define dfRECKONING_SPEED_PLAYER_Y 4

#define dfRANGE_MOVE_TOP	0
#define dfRANGE_MOVE_LEFT	0
#define dfRANGE_MOVE_RIGHT	6400
#define dfRANGE_MOVE_BOTTOM	6400

#define dfSECTOR_MAX_X 50
#define dfSECTOR_MAX_Y 50
#define dfSECTOR_PIXEL_WIDTH (dfRANGE_MOVE_RIGHT / dfSECTOR_MAX_X)
#define dfSECTOR_PIXEL_HEIGHT (dfRANGE_MOVE_BOTTOM / dfSECTOR_MAX_Y)

#define dfARRANGE_DAMAGE_X 60
#define dfARRANGE_DAMAGE_Y 20

void Log(WCHAR *szString, int iLogLevel);

#define _LOG(LogLevel, fmt, ...)					\
do {												\
	if (g_iLogLevel <= LogLevel)					\
	{												\
		wsprintf(g_szLogBuff, fmt, ##__VA_ARGS__);	\
		Log(g_szLogBuff, LogLevel);					\
	}												\
}while(0)											\

extern int g_iLogLevel;
extern WCHAR g_szLogBuff[1024];
extern UINT g_uiUser;
extern list<st_CHARACTER*> g_Sector[dfSECTOR_PIXEL_WIDTH][dfSECTOR_PIXEL_HEIGHT];
extern Network g_network;

#endif //PCH_H
