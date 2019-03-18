// TcpServerProject.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>
#include <locale>
#include <time.h>
#include "timer.h"

Network g_network;
timer g_tm;
UINT g_uiUser;

BOOL g_bFlag;
BOOL g_main = TRUE;

int g_iLogLevel;
WCHAR g_szLogBuff[1024];

list<st_CHARACTER*> g_Sector[dfSECTOR_PIXEL_HEIGHT][dfSECTOR_PIXEL_WIDTH];

void Log(WCHAR *szString, int iLogLevel)
{
	time_t rawtime;
	struct tm timeinfo;

	// 로그남김
	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	WCHAR wLogContent[512];
	swprintf_s(wLogContent, L"[%02d/%02d/%02d %02d:%02d:%02d] %s\n", timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_year - 100, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, szString);

	switch (iLogLevel)
	{
		case dfLOG_LEVEL_DEBUG:
			wprintf(L"%s", wLogContent);
			break;
		case dfLOG_LEVEL_WARNING:
			break;
		case dfLOG_LEVEL_ERROR:
		{
			wprintf(L"%s", wLogContent);

			WCHAR wLogName[128];
			swprintf_s(wLogName, L"%02d-%02d-%02d Log.txt", timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_year - 100, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

			FILE * fp;
			_wfopen_s(&fp, wLogName, L"a+");
			fwprintf_s(fp, wLogContent);
			fclose(fp);
		}
			break;
	}
}

int main()
{
	srand(12345);
	rand();

	time_t rawtime;
	struct tm timeinfo;
	
	// 시간 변경 시 dfFRAME_COUNT 디파인도 같이 변경
	g_tm.init(25.0f);

	// 유니코드
	_wsetlocale(LC_ALL, L"korean");

	// 로그남김
	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	g_network.init();

	//_LOG(0, L"%d", 2);
	DWORD nowTick2 = GetTickCount();
	int frame = 0;
	int netFrame = 0;
	int checkLogCnt = 0;

	while (g_main)
	{
		g_network.netIOProcess();
		netFrame++;

		DWORD prevTick2 = GetTickCount() - nowTick2;
		// 20ms 마다 한번씩
		if (!g_tm.tick())
		{
			g_network.update();
			frame++;
		}

		if (prevTick2 >= 1000)
		{
			checkLogCnt++;

			time(&rawtime);
			localtime_s(&timeinfo, &rawtime);
			nowTick2 = GetTickCount();
			if(frame != 25)
				_LOG(dfLOG_LEVEL_ERROR, L"TPS : %d FPS : %d loop : %d ConnectUser : %d", g_network._uiTPS, frame, netFrame, g_network.GetConnectCount());

	/*		if (checkLogCnt == 60)
			{
				checkLogCnt = 0;
				_LOG(dfLOG_LEVEL_ERROR, L"TPS : %d FPS : %d loop : %d ConnectUser : %d", g_network._uiTPS, frame, netFrame, g_network.GetConnectCount());
			}*/
			g_network._uiTPS = 0;
			frame = 0;
			netFrame = 0;
		}
	}
}