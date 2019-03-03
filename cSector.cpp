#include "pch.h"
#include "cSector.h"


void Sector_AddCharacter(st_CHARACTER * pCharacter)
{
	// 신규 추가는 기존에 들어간 적이 없는 캐릭터만 가능.
	if (pCharacter->curSector.iX != -1 || pCharacter->curSector.iY != -1)
		return;

	int iSectorX = pCharacter->iX / dfSECTOR_PIXEL_WIDTH;
	int iSectorY = pCharacter->iY / dfSECTOR_PIXEL_HEIGHT;

	if (iSectorX >= dfSECTOR_MAX_X || iSectorY >= dfSECTOR_MAX_Y)
		return;

	//_LOG(dfLOG_LEVEL_DEBUG, L"AddSector charX : %d charY : %d -> SectorX :%d SectorY : %d", pCharacter->iX, pCharacter->iY, iSectorX, iSectorY);
	g_Sector[iSectorY][iSectorX].push_back(pCharacter);

	pCharacter->oldSector.iX = pCharacter->curSector.iX = iSectorX;
	pCharacter->oldSector.iY = pCharacter->curSector.iY = iSectorY;
}

void Sector_RemoveCharacter(st_CHARACTER * pCharacter)
{
	// 케릭터에 섹터 정보가 없다면 문제 있는 상황
	if (pCharacter->curSector.iX == -1 || pCharacter->curSector.iY == -1)
		return;

	list<st_CHARACTER *> &SectorList = g_Sector[pCharacter->curSector.iY][pCharacter->curSector.iX];
	list<st_CHARACTER*>::iterator iter = SectorList.begin();

	// 섹터 리스트에서 해당 캐릭터 정보 빼기
	while (iter != SectorList.end())
	{
		if (pCharacter == *iter)
		{
			//_LOG(dfLOG_LEVEL_DEBUG, L"RemoveSector charX : %d charY : %d -> SectorX :%d SectorY : %d", pCharacter->iX, pCharacter->iY, pCharacter->curSector.iX, pCharacter->curSector.iY);
			SectorList.erase(iter);
			break;
		}
		iter++;
	}

	pCharacter->oldSector.iX = pCharacter->curSector.iX;
	pCharacter->oldSector.iY = pCharacter->curSector.iY;
	pCharacter->curSector.iX = -1;
	pCharacter->curSector.iY = -1;

}

bool Sector_UpdateCharacter(st_CHARACTER * pCharacter)
{
	int iBeforeSectorX = pCharacter->curSector.iX;
	int iBeforeSectorY = pCharacter->curSector.iY;

	int iNewSectorX = pCharacter->iX / dfSECTOR_PIXEL_WIDTH;
	int iNewSectorY = pCharacter->iY / dfSECTOR_PIXEL_HEIGHT;

	// 이전섹터랑 새로운 섹터랑 같으면 업데이트 안함
	if (iBeforeSectorX == iNewSectorX && iBeforeSectorY == iNewSectorY)
		return false;

	Sector_RemoveCharacter(pCharacter);
	Sector_AddCharacter(pCharacter);

	pCharacter->oldSector.iX = iBeforeSectorX;
	pCharacter->oldSector.iY = iBeforeSectorY;

	return true;
}

void GetSectorAround(int iSectorX, int iSectorY, st_SECTOR_AROUND * pSectorArund)
{
	int iCntX, iCntY;

	iSectorX--;
	iSectorY--;

	pSectorArund->iCount = 0;
	
	for (int iCntY = 0; iCntY < 3; iCntY++)
	{
		if (iSectorY + iCntY < 0 || iSectorY + iCntY >= dfSECTOR_MAX_Y)
			continue;

		for (iCntX = 0; iCntX < 3; iCntX++)
		{
			if (iSectorX + iCntX < 0 || iSectorX + iCntX >= dfSECTOR_MAX_X)
				continue;

			pSectorArund->around[pSectorArund->iCount].iX = iSectorX + iCntX;
			pSectorArund->around[pSectorArund->iCount].iY = iSectorY + iCntY;
			pSectorArund->iCount++;
		}
	}
}

void GetUpdateSectorAround(st_CHARACTER * pCharacter, st_SECTOR_AROUND * pRemoveSector, st_SECTOR_AROUND * pAddSector)
{
	int iCntOld, iCntCur;
	bool bFind;
	st_SECTOR_AROUND OldSectorAround, CurSectorAround;

	OldSectorAround.iCount = 0;
	CurSectorAround.iCount = 0;

	pRemoveSector->iCount = 0;
	pAddSector->iCount = 0;

	if(pCharacter->oldSector.iX != -1 && pCharacter->oldSector.iY != -1)
		GetSectorAround(pCharacter->oldSector.iX, pCharacter->oldSector.iY, &OldSectorAround);

	GetSectorAround(pCharacter->curSector.iX, pCharacter->curSector.iY, &CurSectorAround);
	
	// 이전 OldSector 섹터 정보중, 신규 섹터에는 없는 정보를 찾아서 RemoveSector에 넣음
	for (iCntOld = 0; iCntOld < OldSectorAround.iCount; iCntOld++)
	{
		bFind = false;
		for (iCntCur = 0; iCntCur < CurSectorAround.iCount; iCntCur++)
		{
			if (OldSectorAround.around[iCntOld].iX == CurSectorAround.around[iCntCur].iX &&
				OldSectorAround.around[iCntOld].iY == CurSectorAround.around[iCntCur].iY)
			{
				bFind = true;
				break;
			}
		}

		if (bFind == false)
		{
			pRemoveSector->around[pRemoveSector->iCount] = OldSectorAround.around[iCntOld];
			pRemoveSector->iCount++;
		}
	}

	// 현재 섹터 정보중 이전 섹터에 없는 정보를 찾아서 AddSector에 넣음
	for (iCntCur = 0; iCntCur < CurSectorAround.iCount; iCntCur++)
	{
		bFind = false;
		for (iCntOld = 0; iCntOld < OldSectorAround.iCount; iCntOld++)
		{
			if (OldSectorAround.around[iCntOld].iX == CurSectorAround.around[iCntCur].iX &&
				OldSectorAround.around[iCntOld].iY == CurSectorAround.around[iCntCur].iY)
			{
				bFind = true;
				break;
			}
		}

		if (bFind == false)
		{
			pAddSector->around[pAddSector->iCount] = CurSectorAround.around[iCntCur];
			pAddSector->iCount++;
		}
	}
}


void CharacterSectorUpdatePacket(st_CHARACTER * pCharacter)
{
	/*
	1. 이전 섹터에서 없어진 부분에 - 캐릭터 삭제 메시지
	2. 이동 하는 캐릭터에게 이전 섹터에서 제외된 섹터의 캐릭터를 삭제 시키는 메시지
	3. 새로 추가된 섹터에 - 케릭터 생성 메시지 & 이동 매시지
	4. 이동하는 캐릭터에게 - 새로 진입한 센터의 캐릭터들 생성 매시지
	*/
	st_SECTOR_AROUND RemoveSector, AddSector;
	st_CHARACTER * pExistCharacter;

	list<st_CHARACTER * > *pSectorList;
	list<st_CHARACTER *>::iterator ListIter;

	static cPacketSerialz Packet;
	Packet.Clear();
	int iCnt;

	GetUpdateSectorAround(pCharacter, &RemoveSector, &AddSector);

	// 1.RemoveSector 에 캐릭터 삭제 패킷 보내기
	MakePacket_DeleteOtherCharacter(&Packet, pCharacter->dwClientNo);

	for (iCnt = 0; iCnt < RemoveSector.iCount; iCnt++)
	{
		// 특정섹터 한 공간에만 매시지를 전달 하는 함수
		g_network.SendPacket_SectorOne(RemoveSector.around[iCnt].iX, RemoveSector.around[iCnt].iY, &Packet, NULL);
	}

	Packet.Clear();

	// 2. 지금 움직이는 녀석에게, RemoveSector의 캐릭터들 삭제 패킷 보내기
	for (iCnt = 0; iCnt < RemoveSector.iCount; iCnt++)
	{
		pSectorList = &g_Sector[RemoveSector.around[iCnt].iY][RemoveSector.around[iCnt].iX];

		for (ListIter = pSectorList->begin(); ListIter != pSectorList->end();)
		{
			MakePacket_DeleteOtherCharacter(&Packet, (*ListIter)->dwClientNo);

			// 특정 클라이언트 한 명 에게만 메시지를 전달하는 함수
			g_network.SendPacket_Unicast(pCharacter->pSession, &Packet);
			Packet.Clear();

			advance(ListIter, 1);
		}
	}

	Packet.Clear();

	// 3. AddSector 에 케릭터 생성 패킷 보내기
	MakePacket_CreateOtherCharacter(&Packet, pCharacter->dwClientNo, pCharacter->byArrowDirection, pCharacter->iX, pCharacter->iY, pCharacter->byHp);

	for (iCnt = 0; iCnt < AddSector.iCount; iCnt++)
	{
		g_network.SendPacket_SectorOne(AddSector.around[iCnt].iX, AddSector.around[iCnt].iY, &Packet, pCharacter->pSession);
	}

	Packet.Clear();

	// 3-1. AddSector에 생성된 캐릭터 이동 패킷 보내기(이동중일때만)
	if (pCharacter->dwAction == dfPACKET_SC_MOVE_START)
	{
		MakePacket_MoveOtherCharacter(&Packet, pCharacter->dwClientNo, pCharacter->byDirection, pCharacter->iX, pCharacter->iY);

		for (iCnt = 0; iCnt < AddSector.iCount; iCnt++)
		{
			g_network.SendPacket_SectorOne(AddSector.around[iCnt].iX, AddSector.around[iCnt].iY, &Packet, pCharacter->pSession);
		}
		Packet.Clear();
	}

	// 4. 이동한 녀석에게 AddSector에 있는 캐릭터들 생성 패킷 보내기
	
	for (iCnt = 0; iCnt < AddSector.iCount; iCnt++)
	{
		// 얻어진 섹터를 돌면서 섹터 리스트 접근
		pSectorList = &g_Sector[AddSector.around[iCnt].iY][AddSector.around[iCnt].iX];

		// 해당 섹터마다 등록된 캐릭터들을 뽑아서 생선패킷 만들어 보냄
		for (ListIter = pSectorList->begin(); ListIter != pSectorList->end();)
		{
			pExistCharacter = *ListIter;

			// 내가 아닌 경우에만
			if (pExistCharacter != pCharacter)
			{
				Packet.Clear();

				MakePacket_CreateOtherCharacter(&Packet, pExistCharacter->dwClientNo, pExistCharacter->byDirection, pExistCharacter->iX, pExistCharacter->iY, pExistCharacter->byHp);
				g_network.SendPacket_Unicast(pCharacter->pSession, &Packet);

				Packet.Clear();
				// 새 AddSector의 캘기터가 걷고 있었다면 이동 패킷 만들어서 보냄
				switch (pExistCharacter->dwAction)
				{
				case dfPACKET_SC_MOVE_START:
					MakePacket_MoveOtherCharacter(&Packet, pExistCharacter->dwClientNo, pExistCharacter->byDirection, pExistCharacter->iX, pExistCharacter->iY);
					g_network.SendPacket_Unicast(pCharacter->pSession, &Packet);
					break;
				case dfPACKET_SC_ATTACK1:
					MakePacket_ATTACK1(&Packet, pExistCharacter->dwClientNo, pExistCharacter->byDirection, pExistCharacter->iX, pExistCharacter->iY);
					g_network.SendPacket_Unicast(pCharacter->pSession, &Packet);
					break;
				case dfPACKET_SC_ATTACK2:
					MakePacket_ATTACK2(&Packet, pExistCharacter->dwClientNo, pExistCharacter->byDirection, pExistCharacter->iX, pExistCharacter->iY);
					g_network.SendPacket_Unicast(pCharacter->pSession, &Packet);
					break;
				case dfPACKET_SC_ATTACK3:
					MakePacket_ATTACK3(&Packet, pExistCharacter->dwClientNo, pExistCharacter->byDirection, pExistCharacter->iX, pExistCharacter->iY);
					g_network.SendPacket_Unicast(pCharacter->pSession, &Packet);
					break;
				}
			}

			advance(ListIter, 1);
		}
	}

	int aa = 0;
}