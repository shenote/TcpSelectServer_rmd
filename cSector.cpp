#include "pch.h"
#include "cSector.h"


void Sector_AddCharacter(st_CHARACTER * pCharacter)
{
	// �ű� �߰��� ������ �� ���� ���� ĳ���͸� ����.
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
	// �ɸ��Ϳ� ���� ������ ���ٸ� ���� �ִ� ��Ȳ
	if (pCharacter->curSector.iX == -1 || pCharacter->curSector.iY == -1)
		return;

	list<st_CHARACTER *> &SectorList = g_Sector[pCharacter->curSector.iY][pCharacter->curSector.iX];
	list<st_CHARACTER*>::iterator iter = SectorList.begin();

	// ���� ����Ʈ���� �ش� ĳ���� ���� ����
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

	// �������Ͷ� ���ο� ���Ͷ� ������ ������Ʈ ����
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
	
	// ���� OldSector ���� ������, �ű� ���Ϳ��� ���� ������ ã�Ƽ� RemoveSector�� ����
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

	// ���� ���� ������ ���� ���Ϳ� ���� ������ ã�Ƽ� AddSector�� ����
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
	1. ���� ���Ϳ��� ������ �κп� - ĳ���� ���� �޽���
	2. �̵� �ϴ� ĳ���Ϳ��� ���� ���Ϳ��� ���ܵ� ������ ĳ���͸� ���� ��Ű�� �޽���
	3. ���� �߰��� ���Ϳ� - �ɸ��� ���� �޽��� & �̵� �Ž���
	4. �̵��ϴ� ĳ���Ϳ��� - ���� ������ ������ ĳ���͵� ���� �Ž���
	*/
	st_SECTOR_AROUND RemoveSector, AddSector;
	st_CHARACTER * pExistCharacter;

	list<st_CHARACTER * > *pSectorList;
	list<st_CHARACTER *>::iterator ListIter;

	static cPacketSerialz Packet;
	Packet.Clear();
	int iCnt;

	GetUpdateSectorAround(pCharacter, &RemoveSector, &AddSector);

	// 1.RemoveSector �� ĳ���� ���� ��Ŷ ������
	MakePacket_DeleteOtherCharacter(&Packet, pCharacter->dwClientNo);

	for (iCnt = 0; iCnt < RemoveSector.iCount; iCnt++)
	{
		// Ư������ �� �������� �Ž����� ���� �ϴ� �Լ�
		g_network.SendPacket_SectorOne(RemoveSector.around[iCnt].iX, RemoveSector.around[iCnt].iY, &Packet, NULL);
	}

	Packet.Clear();

	// 2. ���� �����̴� �༮����, RemoveSector�� ĳ���͵� ���� ��Ŷ ������
	for (iCnt = 0; iCnt < RemoveSector.iCount; iCnt++)
	{
		pSectorList = &g_Sector[RemoveSector.around[iCnt].iY][RemoveSector.around[iCnt].iX];

		for (ListIter = pSectorList->begin(); ListIter != pSectorList->end();)
		{
			MakePacket_DeleteOtherCharacter(&Packet, (*ListIter)->dwClientNo);

			// Ư�� Ŭ���̾�Ʈ �� �� ���Ը� �޽����� �����ϴ� �Լ�
			g_network.SendPacket_Unicast(pCharacter->pSession, &Packet);
			Packet.Clear();

			advance(ListIter, 1);
		}
	}

	Packet.Clear();

	// 3. AddSector �� �ɸ��� ���� ��Ŷ ������
	MakePacket_CreateOtherCharacter(&Packet, pCharacter->dwClientNo, pCharacter->byArrowDirection, pCharacter->iX, pCharacter->iY, pCharacter->byHp);

	for (iCnt = 0; iCnt < AddSector.iCount; iCnt++)
	{
		g_network.SendPacket_SectorOne(AddSector.around[iCnt].iX, AddSector.around[iCnt].iY, &Packet, pCharacter->pSession);
	}

	Packet.Clear();

	// 3-1. AddSector�� ������ ĳ���� �̵� ��Ŷ ������(�̵����϶���)
	if (pCharacter->dwAction == dfPACKET_SC_MOVE_START)
	{
		MakePacket_MoveOtherCharacter(&Packet, pCharacter->dwClientNo, pCharacter->byDirection, pCharacter->iX, pCharacter->iY);

		for (iCnt = 0; iCnt < AddSector.iCount; iCnt++)
		{
			g_network.SendPacket_SectorOne(AddSector.around[iCnt].iX, AddSector.around[iCnt].iY, &Packet, pCharacter->pSession);
		}
		Packet.Clear();
	}

	// 4. �̵��� �༮���� AddSector�� �ִ� ĳ���͵� ���� ��Ŷ ������
	
	for (iCnt = 0; iCnt < AddSector.iCount; iCnt++)
	{
		// ����� ���͸� ���鼭 ���� ����Ʈ ����
		pSectorList = &g_Sector[AddSector.around[iCnt].iY][AddSector.around[iCnt].iX];

		// �ش� ���͸��� ��ϵ� ĳ���͵��� �̾Ƽ� ������Ŷ ����� ����
		for (ListIter = pSectorList->begin(); ListIter != pSectorList->end();)
		{
			pExistCharacter = *ListIter;

			// ���� �ƴ� ��쿡��
			if (pExistCharacter != pCharacter)
			{
				Packet.Clear();

				MakePacket_CreateOtherCharacter(&Packet, pExistCharacter->dwClientNo, pExistCharacter->byDirection, pExistCharacter->iX, pExistCharacter->iY, pExistCharacter->byHp);
				g_network.SendPacket_Unicast(pCharacter->pSession, &Packet);

				Packet.Clear();
				// �� AddSector�� Ķ���Ͱ� �Ȱ� �־��ٸ� �̵� ��Ŷ ���� ����
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