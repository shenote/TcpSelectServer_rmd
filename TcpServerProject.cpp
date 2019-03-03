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
	switch (iLogLevel)
	{
		case dfLOG_LEVEL_DEBUG:
			wprintf(L"%s\n", szString);
			break;
		case dfLOG_LEVEL_WARNING:
			break;
		case dfLOG_LEVEL_ERROR:
		{
			//time_t rawtime;
			//struct tm timeinfo;

			//// 로그남김
			//time(&rawtime);
			//localtime_s(&timeinfo, &rawtime);
			//FILE * fp;
			//fopen_s(&fp, "log.txt", "a+");
			//fwrite(szString, wcslen(szString) * 2, 1, fp);
			//fclose(fp);
		}
			break;
	}
}


char * timeToString(struct tm * t)
{
	static char s[20];

	sprintf_s(s, "%04d-%02d-%02d %02d:%02d:%02d",
		t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
		t->tm_hour, t->tm_min, t->tm_sec
	);

	return s;
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

	g_network.init();

	//_LOG(0, L"%d", 2);
	DWORD nowTick2 = GetTickCount();
	int frame = 0;
	int netFrame = 0;
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
			time(&rawtime);
			localtime_s(&timeinfo, &rawtime);
			nowTick2 = GetTickCount();
			//if(frame != 25)
			printf("TIME : %s TPS : %llu FPS : %d loop : %d ConnectUser : %d\n", timeToString(&timeinfo), g_network._uiTPS, frame, netFrame,g_network.GetConnectCount());
			g_network._uiTPS = 0;
			frame = 0;
			netFrame = 0;

			//for (int i = 0; i < dfSECTOR_PIXEL_HEIGHT; ++i)
			//{
			//	for (int j = 0; j < dfSECTOR_PIXEL_WIDTH; ++j)
			//	{
			//		if (g_Sector[j][i].size() == 0)
			//			continue;
			//		auto iter = g_Sector[j][i].begin();
			//		printf("Sector[%d][%d] Cnt : %d [", j, i, g_Sector[j][i].size());
			//		for (;iter != g_Sector[j][i].end(); ++iter)
			//		{
			//			printf("%d,", (*iter)->dwClientNo);
			//		}
			//		printf("\n");
			//	}
			//}
		}
		
	}

}