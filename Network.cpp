#include "pch.h"
#include "Network.h"

Network::Network()
{
}


Network::~Network()
{
}

BOOL Network::DomaionToIP(const WCHAR *szDomain, IN_ADDR *pAddr)
{
	ADDRINFOW * pAddrInfo;
	SOCKADDR_IN * pSockAddr;
	if (GetAddrInfo(szDomain, L"0", NULL, &pAddrInfo) != 0)
	{
		return FALSE;
	}
	pSockAddr = (SOCKADDR_IN*)pAddrInfo->ai_addr;
	*pAddr = pSockAddr->sin_addr;
	FreeAddrInfo(pAddrInfo);
	return TRUE;
}

HRESULT Network::init()
{
	_bConnect = false;		 // ���� ���ε� ���� ����
	_listenSocket = 0;		 
	_mSessionList.clear();	// ���� ���� ����Ʈ
	_packetSz.Clear();		 // ����ȭ �ʱ�ȭ
	_mCharList.clear();		 // �ɸ���

	_headerSize = sizeof(st_PACKET_HEADER);

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return FALSE;

	_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenSocket == INVALID_SOCKET)
		return FALSE;

	// �� ������ ��, ��Ƽ� ������ �ʰ�, �ٷ� �ٷ� ����. 
	int op = TRUE;
	setsockopt(_listenSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&op, sizeof(op));

	WCHAR szConnectIp[128];
	memset(szConnectIp, 0, sizeof(szConnectIp));
	wsprintf(szConnectIp, L"%S", "127.0.0.1");

	//IN_ADDR inAddr;
	//BOOL ret = DomaionToIP(szConnectIp, &inAddr);
	//if (ret == 0)
	//{
	//	printf("������ ����\n");
	//	return 0;
	//}

	SOCKADDR_IN sockaddr;
	ZeroMemory(&sockaddr, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	 //INADDR_ANY �� ������ IP�ּҸ� �ڵ����� ã�Ƽ� �������ִ� �Լ��̴�.
	//������ NIC�� 2�� �̻� ������ �ִ� ��찡 ������ ���� Ư�� NIC�� IP�ּҸ� sin_addr.s_addr�� �����ϸ� �ٸ� NIC���� ����� ��û��
	//���� �� �� ���� �ȴ�. �̶� INADDR_ANY�� �̿��ϸ� �� NIC�� ��� ���ε����ֹǷ� ��� IP�� ���� �����ص� ���񽺰� ��������.
	//IP�ּҸ� INADDR_ANY ��, ��Ʈ ��ȣ�� 9000���� �� ��� => ���� ���� ��ǻ���� 9000�� ��Ʈ�� �������� �ϴ� ��� ���� ��û�� �ش� ����
	//�������α׷����� ó���ϰڴٴ� �ǹ�.
	//sockaddr.sin_addr = inAddr;
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr.sin_port = htons(dfNETWORK_PORT);

	// ���ε� �ؾ� �ϰ�.�Ḹ ������� ����
	int bindRet = bind(_listenSocket, (SOCKADDR*)&sockaddr, sizeof(sockaddr));
	if (bindRet != 0)
	{
		WSACleanup();
		return 0;
	}

	// ����  Ŭ���̾�Ʈ�� �������� ���� 3���� ����ŷ �̶�� ���� ��. �ִ� �����ڼ��� 150�ΰ� 200�ΰ�.? ������ͼ� �������� ���� ������ٶ����� ����ϴ� ����
	// ���������� Ȯ���ؼ� �迭�� �ϳ��� �̾Ƴ��ٰ� ���� ��.
	// ��·�� �⺻���� ���� ��δ� ��� ���� �Ǿ��ٰ� ���� �ȴ�.
	listen(_listenSocket, SOMAXCONN);

	//recv
	_bConnect = true;

	return S_OK;
}

void Network::release()
{
	WSACleanup();
}

int Network::DeadReckoningPos(BYTE byDir, DWORD dwActionTick, WORD wOldPosX, WORD wOldPosY, WORD & pPosX, WORD & pPosY)
{
	// �̵��� �����ߴ� ���� ���۰� ���� �ð��� ���� ���Ѵ�.
	DWORD dwIntervalTick = timeGetTime() - dwActionTick;
	int iActionFrame = dwIntervalTick / dfFRAME_COUNT;	// 40ms ������� ������ �������� ����
	int iRemoveFrame = 0;
	int iValue;

	int iRPosX = wOldPosX;
	int iRPosY = wOldPosY;
	//1. ���� ���������� X��, Y���� ��ǥ �̵����� ����.
	int iDX = iActionFrame * dfRECKONING_SPEED_PLAYER_X;
	int iDY = iActionFrame * dfRECKONING_SPEED_PLAYER_Y;

	switch (byDir)
	{
	case dfPACKET_MOVE_DIR_LL:
		iRPosX = wOldPosX - iDX;
		iRPosY = wOldPosY;
		break;
	case dfPACKET_MOVE_DIR_LU:
		iRPosX = wOldPosX - iDX;
		iRPosY = wOldPosY - iDY;
		break;
	case dfPACKET_MOVE_DIR_UU:
		iRPosX = wOldPosX;
		iRPosY = wOldPosY - iDY;
		break;
	case dfPACKET_MOVE_DIR_RU:
		iRPosX = wOldPosX + iDX;
		iRPosY = wOldPosY - iDY;
		break;
	case dfPACKET_MOVE_DIR_RR:
		iRPosX = wOldPosX + iDX;
		iRPosY = wOldPosY;
		break;
	case dfPACKET_MOVE_DIR_RD:
		iRPosX = wOldPosX + iDX;
		iRPosY = wOldPosY + iDY;
		break;
	case dfPACKET_MOVE_DIR_DD:
		iRPosX = wOldPosX;
		iRPosY = wOldPosY + iDY;
		break;
	case dfPACKET_MOVE_DIR_LD:
		iRPosX = wOldPosX - iDX;
		iRPosY = wOldPosY + iDY;
		break;
	}

	// ��������� iRPosX, iRPosY�� ���� ��ǥ�� �Ϸ� �Ǿ���.
	// �� �Ʒ� �κ��� ���� ��ǥ�� ȭ���� �̵� ������ ��� ��� �� �׼��� �߶󳻱� ���ؼ� 
	// ������ ��� ������ �������� ��� �ϴ� ����
	
	if (iRPosX <= dfRANGE_MOVE_LEFT)
	{
		iValue = abs(dfRANGE_MOVE_LEFT - abs(iRPosX)) / dfRECKONING_SPEED_PLAYER_X;
		iRemoveFrame = max(iValue, iRemoveFrame);
	}
	if (iRPosX >= dfRANGE_MOVE_RIGHT)
	{
		iValue = abs(dfRANGE_MOVE_RIGHT - iRPosX) / dfRECKONING_SPEED_PLAYER_X;
		iRemoveFrame = max(iValue, iRemoveFrame);
	}
	if (iRPosY <= dfRANGE_MOVE_TOP)
	{
		iValue = abs(dfRANGE_MOVE_TOP - abs(iRPosY)) / dfRECKONING_SPEED_PLAYER_Y;
		iRemoveFrame = max(iValue, iRemoveFrame);
	}
	if (iRPosY >= dfRANGE_MOVE_BOTTOM)
	{
		iValue = abs(dfRANGE_MOVE_BOTTOM - iRPosY) / dfRECKONING_SPEED_PLAYER_Y;
		iRemoveFrame = max(iValue, iRemoveFrame);
	}

	// ������ ���� ��� ���� �Ǿ�� �� �������� ��Ÿ���ٸ� ��ǥ�� �ٽ� �� ���
	if (iRemoveFrame > 0)
	{
		iActionFrame -= iRemoveFrame;
		// ������ ��ǥ�� �ٽ� ���
		iDX = iActionFrame * dfRECKONING_SPEED_PLAYER_X;
		iDY = iActionFrame * dfRECKONING_SPEED_PLAYER_Y;

		switch (byDir)
		{
		case dfPACKET_MOVE_DIR_LL:
			iRPosX = wOldPosX - iDX;
			iRPosY = wOldPosY;
			break;
		case dfPACKET_MOVE_DIR_LU:
			iRPosX = wOldPosX - iDX;
			iRPosY = wOldPosY - iDY;
			break;
		case dfPACKET_MOVE_DIR_UU:
			iRPosX = wOldPosX;
			iRPosY = wOldPosY - iDY;
			break;
		case dfPACKET_MOVE_DIR_RU:
			iRPosX = wOldPosX + iDX;
			iRPosY = wOldPosY - iDY;
			break;
		case dfPACKET_MOVE_DIR_RR:
			iRPosX = wOldPosX + iDX;
			iRPosY = wOldPosY;
			break;
		case dfPACKET_MOVE_DIR_RD:
			iRPosX = wOldPosX + iDX;
			iRPosY = wOldPosY + iDY;
			break;
		case dfPACKET_MOVE_DIR_DD:
			iRPosX = wOldPosX;
			iRPosY = wOldPosY + iDY;
			break;
		case dfPACKET_MOVE_DIR_LD:
			iRPosX = wOldPosX - iDX;
			iRPosY = wOldPosY + iDY;
			break;
		}
	}

	iRPosX = min(iRPosX, dfRANGE_MOVE_RIGHT);
	iRPosX = max(iRPosX, dfRANGE_MOVE_LEFT);
	iRPosY = min(iRPosY, dfRANGE_MOVE_BOTTOM);
	iRPosY = max(iRPosY, dfRANGE_MOVE_TOP);

	pPosX = iRPosX;
	pPosY = iRPosY;

	return iActionFrame;
}

void Network::DamageHitAround(st_SESSION * pSession, int iDamage)
{
	// �ֺ� 9�� ������ ���ֵ� �߿� ���� �÷��̾��� ��ǥ�� ���� ����� �༮�� ã�Ƽ� �������� ���δ�.

	list<st_CHARACTER * > *pSectorList;
	list<st_CHARACTER *>::iterator ListIter;
	st_SECTOR_AROUND CurSectorAround;
	//cPacketSerialz Packet;
	_packetSz.Clear();

	st_CHARACTER * character = FindCharacter(pSession);
	GetSectorAround(character->curSector.iX, character->curSector.iY, &CurSectorAround);

	for (int i = 0; i < CurSectorAround.iCount; ++i)
	{
		pSectorList = &g_Sector[CurSectorAround.around[i].iY][CurSectorAround.around[i].iX];

		// ���Ϳ� �ο��� ������ ����
		if (pSectorList->size() == 0)
			continue;

		for (ListIter = pSectorList->begin(); ListIter != pSectorList->end(); ListIter++)
		{
			if ((*ListIter) == character)
				continue;

			// �ɸ��Ͱ� �ٶ󺸴� ����
			if (character->byArrowDirection == dfPACKET_MOVE_DIR_LL)
			{
				// X���� �ɸ��ͺ��� �۾ƾ���
				if ((*ListIter)->iX > character->iX)
					continue;
			}
			else if(character->byArrowDirection == dfPACKET_MOVE_DIR_RR)
			{
				// X���� �ɸ��ͺ��� Ŀ����
				if ((*ListIter)->iX < character->iX)
					continue;
			}
			if(abs((*ListIter)->iX - character->iX) < dfARRANGE_DAMAGE_X && abs((*ListIter)->iY - character->iY) < dfARRANGE_DAMAGE_Y)
			{

				if ((*ListIter)->byHp - iDamage <= 0)
					(*ListIter)->byHp = 0;
				else
					(*ListIter)->byHp -= iDamage;

				MakePacket_DAMAGE(&_packetSz, character->dwClientNo, (*ListIter)->dwClientNo, (*ListIter)->byHp);
				// �´¾� �߽����� ������.
				SendPacket_Around((*ListIter)->pSession, &_packetSz, true);
				_packetSz.Clear();
				break;
			}
		}

	}
}


void Network::update()
{
	for (auto iter = _mCharList.begin(); iter != _mCharList.end();)
	{
		auto backUpIter = iter;
		advance(iter, 1);
		//iter++;
		//if (backUpIter->second->byHp == 0)
		//{
		//	DisconnectSession(backUpIter->first);
		//	continue;
		//}

		switch (backUpIter->second->dwAction)
		{
		case dfPACKET_SC_MOVE_START:
			switch (backUpIter->second->byDirection)
			{
			case dfPACKET_MOVE_DIR_LL:
				backUpIter->second->byArrowDirection = dfPACKET_MOVE_DIR_LL;
				backUpIter->second->iX -= dfRECKONING_SPEED_PLAYER_X;
				if (backUpIter->second->iX < 10)
					backUpIter->second->iX = 11;
				break;
			case dfPACKET_MOVE_DIR_LU:
				backUpIter->second->byArrowDirection = dfPACKET_MOVE_DIR_LL;
				if (backUpIter->second->iX > dfRECKONING_SPEED_PLAYER_X && backUpIter->second->iY > dfRECKONING_SPEED_PLAYER_Y)
				{
					backUpIter->second->iX -= dfRECKONING_SPEED_PLAYER_X;
					backUpIter->second->iY -= dfRECKONING_SPEED_PLAYER_Y;
				}
				break;
			case dfPACKET_MOVE_DIR_UU:
				backUpIter->second->iY -= dfRECKONING_SPEED_PLAYER_Y;
				if (backUpIter->second->iY < dfRECKONING_SPEED_PLAYER_Y)
					backUpIter->second->iY = dfRECKONING_SPEED_PLAYER_Y;
				break;
			case dfPACKET_MOVE_DIR_RU:
				backUpIter->second->byArrowDirection = dfPACKET_MOVE_DIR_RR;
				if (backUpIter->second->iX < 6397 && backUpIter->second->iY > dfRECKONING_SPEED_PLAYER_Y)
				{
					backUpIter->second->iX += dfRECKONING_SPEED_PLAYER_X;
					backUpIter->second->iY -= dfRECKONING_SPEED_PLAYER_Y;
				}
				break;
			case dfPACKET_MOVE_DIR_RR:
				backUpIter->second->byArrowDirection = dfPACKET_MOVE_DIR_RR;
				backUpIter->second->iX += dfRECKONING_SPEED_PLAYER_X;
				if (backUpIter->second->iX > 6397)
					backUpIter->second->iX = 6397;
				break;
			case dfPACKET_MOVE_DIR_RD:
				backUpIter->second->byArrowDirection = dfPACKET_MOVE_DIR_RR;
				if (backUpIter->second->iX < 6397 && backUpIter->second->iY < 6397)
				{
					backUpIter->second->iX += dfRECKONING_SPEED_PLAYER_X;
					backUpIter->second->iY += dfRECKONING_SPEED_PLAYER_Y;
				}
				break;
			case dfPACKET_MOVE_DIR_DD:
				backUpIter->second->iY += dfRECKONING_SPEED_PLAYER_Y;
				if (backUpIter->second->iY > 6397)
					backUpIter->second->iY = 6397;
				break;
			case dfPACKET_MOVE_DIR_LD:
				backUpIter->second->byArrowDirection = dfPACKET_MOVE_DIR_LL;
				if (backUpIter->second->iX > dfRECKONING_SPEED_PLAYER_X && backUpIter->second->iY < 6397)
				{
					backUpIter->second->iX -= dfRECKONING_SPEED_PLAYER_X;
					backUpIter->second->iY += dfRECKONING_SPEED_PLAYER_Y;
				}
				break;
			}
			//_LOG(dfLOG_LEVEL_DEBUG, L"# gamerun # SessionID:%d / Dir:%d / X:%d / Y:%d", iter->second->dwClientNo, iter->second->byDirection, iter->second->iX, iter->second->iY);
			if (backUpIter->second->byDirection >= dfPACKET_MOVE_DIR_LL && backUpIter->second->byDirection <= dfPACKET_MOVE_DIR_LD)
			{
				if (Sector_UpdateCharacter(backUpIter->second))
				{
					CharacterSectorUpdatePacket(backUpIter->second);
				}
			}
			break;
		case dfPACKET_SC_MOVE_STOP:
			break;
		
		default:
			break;
		}
	}
	
}

void Network::netIOProcess()
{
	if (!_bConnect)
		return;

	netProc_Accept();

	if (_mSessionList.empty())
		return;

	FD_SET fdReadSet, fdSendSet;

	FD_ZERO(&fdReadSet);
	FD_ZERO(&fdSendSet);

	SOCKET skUserTable[FD_SETSIZE] = { INVALID_SOCKET,}; // �ʱ�ȭ�� ���� ����.
	memset(skUserTable, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);

	int iTotalUser = 0;
	for (auto sessionIter = _mSessionList.begin(); sessionIter != _mSessionList.end();)
	{
		// ���� �� ��������ŭ �������̺� �ְ�, �������̺��� 64�� ��� ������.
		skUserTable[iTotalUser] = sessionIter->first;
		FD_SET(sessionIter->second->socket, &fdReadSet);
		FD_SET(sessionIter->second->socket, &fdSendSet);

		// Disconnect �Ҷ� Map������ ������� ���� �׷��� ���͸� ���� �����ͷ� ���� ����
		//sessionIter++;
		// ��ȯ�� ���� �ε����� �Է��ϸ� �ش� �ε����� ���� ��ȯ
		advance(sessionIter, 1);
		++iTotalUser;

		if (iTotalUser >= FD_SETSIZE)
		{
			// ��Ŷ ���� ó��
			netSelectSocket(skUserTable, &fdReadSet, &fdSendSet);

			FD_ZERO(&fdReadSet);
			FD_ZERO(&fdSendSet);

			memset(skUserTable, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);

			iTotalUser = 0;
		}
	}

	// ó�� ���� �ʴ� ��Ŷ ó��
	if(iTotalUser != 0)
		netSelectSocket(skUserTable, &fdReadSet, &fdSendSet);
}

void Network::netSelectSocket(SOCKET * pTableSocket, FD_SET * pReadSet, FD_SET * pWriteSet)
{
	TIMEVAL tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	// ������ �ִ���? ��ϵǾ� �ִ� ������ ����� �����鲲 ���Դ�.
	// ����� ���� 
	int selectRet = select(NULL, pReadSet, pWriteSet, NULL, &tv);
	if (selectRet > 0)
	{
		// ���� ���̺� ���ε� Ű������ ó��
		for (int i = 0; i < FD_SETSIZE; ++i)
		{
			if (pTableSocket[i] == INVALID_SOCKET)
				continue;

			st_SESSION * pSession = FindSession(pTableSocket[i]);
			if (pSession == nullptr)
			{
				continue;
			}

			// ��Ŷ ó��
			if (FD_ISSET(pSession->socket, pReadSet))
			{
				// Ŭ�󿡼��� �ϳ��ְ� �ϳ� �ް�, ���� ������ ����ѵ�, ���ū ������ �� �޾Ҵٸ�.. ������ �ٽ� ���� �� ���� �ʳ�?
				int recvSize = recv(pSession->socket, pSession->recvQ.GetRearBufferPtr(), pSession->recvQ.GetNotBrokenPutSize(), 0);

				pSession->recvQ.MoveRear(recvSize);

				if (recvSize == 0 || recvSize == SOCKET_ERROR)
				{
					DisconnectSession(pSession->socket);
					continue;
				}

				//continue;
				// ó���Ұ� ������ �� ó����
				while (pSession->recvQ.GetUseSize() >= _headerSize)
				{
					// ��Ŷ ó��
					switch ((RECV_CHECK)netProc_Recv(pSession))
					{
					case RECV_CHECK::RECV_OK:
						break;
					case RECV_CHECK::RECV_MORE:
						break;
					case RECV_CHECK::RECV_ERROR:
						if (DisconnectSession(pSession->socket) == false)
							_LOG(dfLOG_LEVEL_DEBUG, L"�������� ���� ������ �ֽ��ϴ�. %d", pSession->socket);
						continue;
						break;
					default:
						_LOG(dfLOG_LEVEL_DEBUG, L"�߸��� netProc_Recv return ����. recv %d sk %d", recvSize, pSession->socket);
						continue;
						break;
					}
				}
				int bb = 0;
			}

			// ������ �ִ°�?
			if (pSession->sendQ.GetUseSize() < _headerSize)
				continue;

			// ��� ����
			st_PACKET_HEADER header = { 0, };
			pSession->sendQ.Peek((char*)&header, _headerSize);

			// ��Ŷ�� ���̷ε� ����� �ִ��� üũ �����ϴٸ� �� �޾ƾ� �Ѵ�.
			if (pSession->sendQ.GetUseSize() < header.bySize + _headerSize)
				continue;

			// ���� �� �ִ� ���� �ΰ� ?
			if (FD_ISSET(pSession->socket, pWriteSet))
			{
				int sendSize = send(pSession->socket, pSession->sendQ.GetFrontBufferPtr(), pSession->sendQ.GetNotBrokenGetSize(), 0);
				pSession->sendQ.MoveFront(sendSize);
			}
		}
	}
	
}

int Network::netProc_Recv(st_SESSION * pSession)
{
	// ��� ����
	st_PACKET_HEADER header = { 0 };

	pSession->recvQ.Peek((char*)&header, _headerSize);

	// ��Ŷ�� ù ����Ʈ�� 0x89�ΰ�? ������ üũ
	if (header.byCode != 0x89)
		return RECV_CHECK::RECV_ERROR;

	// ��Ŷ�� ���̷ε� ����� �ִ��� üũ �����ϴٸ� �� �޾ƾ� �Ѵ�.
	if (header.bySize + _headerSize > pSession->recvQ.GetUseSize())
		return RECV_CHECK::RECV_MORE;

	// ����Ÿ�׷� ����
	char userDataGram[18000];
	pSession->recvQ.Peek(userDataGram, header.bySize + _headerSize);

	//// üũ�� üũ üũ�� ��,�޽��� Ÿ��, ���̷ε� ũ��, ����
	//if (CheckSum(header.byCheckSum, header.bySize, header.bySize, buf_ + _headerSize) == false)
	//	return RECV_CHECK::RECV_ERROR;

	// ��� ������ ������ ��Ŷ�� ó�� �ϸ� ��.
	if (CompleteRecvPacket(header.byType, userDataGram, header.bySize + _headerSize, pSession) == 0)
		return RECV_CHECK::RECV_ERROR;

	// MovePtr + ��Ŷ ���� �ڵ�
	pSession->recvQ.MoveFront(header.bySize + _headerSize + 1);

	++_uiPPS;

	return RECV_CHECK::RECV_OK;
}


int Network::CompleteRecvPacket(WORD wMsgType_, char * buf_, int bufSize_, st_SESSION * pSession)
{
	// ����ȭ �ʱ�ȭ
	_packetSz.Clear();

	//cPacketSerialz packetSz;
	//packetSz.PutData(buf_ + _headerSize, bufSize_ - _headerSize);
	_packetSz.PutData(buf_ + _headerSize, bufSize_ - _headerSize);
	
	int iProcRet = 1;
	// ��Ŷ
	switch (wMsgType_)
	{
	case dfPACKET_CS_MOVE_START:
		iProcRet = netPacketProc_MoveStart(&_packetSz, pSession);
		break;
	case dfPACKET_CS_MOVE_STOP:
		iProcRet = netPacketProc_MoveStop(&_packetSz, pSession);
		break;
	case dfPACKET_CS_ATTACK1:
		iProcRet = netPacketProc_Attack1(&_packetSz, pSession);
		break;
	case dfPACKET_CS_ATTACK2:
		iProcRet = netPacketProc_Attack2(&_packetSz, pSession);
		break;
	case dfPACKET_CS_ATTACK3:
		iProcRet = netPacketProc_Attack3(&_packetSz, pSession);
		break;
	case 252:
		_LOG(dfLOG_LEVEL_DEBUG, L"252Packet");
		iProcRet = true;
		break;
	default:
		iProcRet = false;
		break;
	}
	return iProcRet;
}

//
// ���� ����
//
st_SESSION * Network::CreateSession(SOCKET sk, SOCKADDR_IN sockAddr)
{
	// ������ ������ ������ ��´�. 
	st_SESSION * user = new st_SESSION;
	user->socket = sk;
	user->sockAddr = sockAddr;
	user->dwSessionID = ++g_uiUser;
	_LOG(dfLOG_LEVEL_DEBUG, L"Accept - ip %S port %d", inet_ntoa(sockAddr.sin_addr), sockAddr.sin_port);
	_mSessionList.insert(make_pair(sk, user));
	return user;
}

//
//----------------------------------------------------------------
// Accept ���� Ŭ���̾�Ʈ 
//----------------------------------------------------------------
//
BOOL Network::netProc_Accept()
{
	BOOL bRet = FALSE;
	FD_SET fdReadSet;
	FD_ZERO(&fdReadSet);

	FD_SET(_listenSocket, &fdReadSet);

	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	// accept
	int selectRet = select(NULL, &fdReadSet, NULL, NULL, &tv);
	if (selectRet > 0)
	{
		// ��Ŷ ó��
		if (FD_ISSET(_listenSocket, &fdReadSet))
		{
			// accept�۾�
			SOCKADDR_IN sockAddr;
			int clientLen = sizeof(sockAddr);
			SOCKET client = accept(_listenSocket, (SOCKADDR*)&sockAddr, &clientLen);
			if (client == INVALID_SOCKET)
				_LOG(dfLOG_LEVEL_DEBUG, L"INVALID_SOCKET");

			bRet = TRUE;

			st_SESSION * pSession = CreateSession(client, sockAddr);
			if (pSession != nullptr)
			{
				CreateCharacter(pSession);
			}
			else
			{
				_LOG(dfLOG_LEVEL_DEBUG, L"Session Create Error");
			}

		}
	}
	return bRet;
}

st_SESSION * Network::FindSession(SOCKET sk)
{
	auto iter = _mSessionList.find(sk);
	if (iter != _mSessionList.end())
	{
		return iter->second;
	}
	return nullptr;
}

// ���� ��ȣ�� ���ǿ� ���ε� �ɸ��͸� ã��
st_CHARACTER * Network::FindCharacter(st_SESSION * session)
{
	auto iter = _mCharList.find(session->socket);
	if (iter != _mCharList.end())
	{
		return iter->second;
	}
	return nullptr;
}

//
//----------------------------------------------------------------
// ĳ���� ���� �� ��Ÿ 
//----------------------------------------------------------------
//
void Network::CreateCharacter(st_SESSION * pSession)
{
	_packetSz.Clear();

	// �ɸ��� ���� �� �ʿ� ����
	// ����ũ��Ŷ���� �ٽ� ������ 
	// ���濡���� ���ε��� �༮�� ������ �����϶�� ����ũ��Ŷ ���� ��ε� ĳ����
	st_CHARACTER * character = new st_CHARACTER;
	ZeroMemory(character, 0, sizeof(character));
	character->dwClientNo = pSession->dwSessionID;
	character->byDirection = 0;
	//character->iX = 50 + pSession->dwSessionID * 10;
	//character->iY = 50 + pSession->dwSessionID * 10;
	character->iX = rand() % dfRANGE_MOVE_RIGHT + 1;
	character->iY = rand() % dfRANGE_MOVE_BOTTOM + 1;
	character->byHp = 100;
	character->dwAction = dfPACKET_SC_MOVE_STOP;
	character->curSector.iX = -1;
	character->curSector.iY = -1;
	character->oldSector.iX = -1;
	character->oldSector.iY = -1;
	character->pSession = pSession;
	character->byArrowDirection = 0;

	// �� �ɸ��� ����
	MakePacket_CreateMyCharacter(&_packetSz, character->dwClientNo, character->byDirection, character->iX, character->iY, character->byHp);
	pSession->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());

	_packetSz.Clear();

	// ���ɸ��� ������ ���� ����
	_mCharList.insert(make_pair(pSession->socket, character));

	// ���� �߰�
	if (Sector_UpdateCharacter(character))
	{
		CharacterSectorUpdatePacket(character);
	}
}

//
//----------------------------------------------------------------
// ĳ���� ����
//----------------------------------------------------------------
//

void Network::DeleteCharacter(DWORD id)
{
	_packetSz.Clear();
	MakePacket_DeleteOtherCharacter(&_packetSz, id);
	SendOther(_packetSz.GetBufferPtr(), _packetSz.GetDataSize(), id);
}

//
//----------------------------------------------------------------
// Ŭ���̾�Ʈ ���� ����
//----------------------------------------------------------------
//

BOOL Network::DisconnectSession(SOCKET sk_)
{
	for (auto iter = _mSessionList.begin(); iter != _mSessionList.end(); ++iter)
	{
		if (iter->first == sk_)
		{
			_LOG(dfLOG_LEVEL_DEBUG, L"Disconnected - ip %S port %d", inet_ntoa(_mSessionList[sk_]->sockAddr.sin_addr), _mSessionList[sk_]->sockAddr.sin_port);

			cPacketSerialz packetDelete;
			// 1.RemoveSector �� ĳ���� ���� ��Ŷ ������
			MakePacket_DeleteOtherCharacter(&packetDelete, _mCharList[sk_]->dwClientNo);

			// �ֺ����� ���� ��Ŷ
			SendPacket_Around(_mSessionList[sk_], &packetDelete);
			
			// ���� ����
			Sector_RemoveCharacter(_mCharList[sk_]);

			closesocket(sk_);
			delete _mSessionList[sk_];
			_mSessionList.erase(sk_);
			
			delete _mCharList[sk_];
			_mCharList.erase(sk_);

			return TRUE;
		}
	}
	return FALSE;
}

//
//----------------------------------------------------------------
// üũ�� ������� ����
//----------------------------------------------------------------
//

BOOL Network::CheckSum(int checkVal_, WORD wMsgType_, int payLoadSize_, char * payLoadBuf_)
{
	BYTE checkPaySum = 0;
	// ���� �޽��� Ÿ�� ���ϰ� 
	checkPaySum += (*(&wMsgType_ + 0));
	checkPaySum += (*(&wMsgType_ + 1));

	// ���̷ε��� ũ�� ��ŭ ����Ʈ�� ���Ѵ�.
	for (int i = 0; i < payLoadSize_; ++i)
		checkPaySum += (BYTE)payLoadBuf_[i];

	// üũ���� ������?
	if ((checkPaySum % 256) == checkVal_)
		return TRUE;

	return FALSE;
}

//------------------------------------------------------------------------
//
// SendPacket ------------------------------------------------------
//
//------------------------------------------------------------------------
//

void Network::SendAll(char * buf, int size)
{
	return;
	for(auto iter = _mSessionList.begin(); iter != _mSessionList.end(); ++iter)
		_mSessionList[iter->first]->sendQ.Enqueue(buf, size);
}

void Network::SendOther(char * buf, int size, UINT sk)
{
	return;
	for (auto iter = _mSessionList.begin(); iter != _mSessionList.end(); ++iter)
	{
		if (iter->first == sk)
			continue;
		
		_mSessionList[iter->first]->sendQ.Enqueue(buf, size);
	}
}

void Network::SendPacket_SectorOne(int iX, int iY, cPacketSerialz * packetSz, st_SESSION * pExceptSession)
{
	list<st_CHARACTER * > *pSectorList;
	list<st_CHARACTER *>::iterator ListIter;
	pSectorList = &g_Sector[iY][iX];

	// �ش� ���͸��� ��ϵ� ĳ���͵��� �̾Ƽ� ������Ŷ ����� ����
	for (ListIter = pSectorList->begin(); ListIter != pSectorList->end(); ListIter++)
	{
		if ((*ListIter)->pSession == pExceptSession)
			continue;
		(*ListIter)->pSession->sendQ.Enqueue(packetSz->GetBufferPtr(), packetSz->GetDataSize());
	}
}

void Network::SendPacket_Unicast(st_SESSION * pSession, cPacketSerialz * packetSz)
{
	pSession->sendQ.Enqueue(packetSz->GetBufferPtr(), packetSz->GetDataSize());
}

void Network::SendPacket_Around(st_SESSION * pSession, cPacketSerialz * packetSz, bool bSendMe)
{
	list<st_CHARACTER * > *pSectorList;
	list<st_CHARACTER *>::iterator ListIter;
	st_SECTOR_AROUND CurSectorAround;

	st_CHARACTER * character = _mCharList[pSession->socket];
	GetSectorAround(character->curSector.iX, character->curSector.iY, &CurSectorAround);

	for (int i = 0; i < CurSectorAround.iCount; ++i)
	{
		pSectorList = &g_Sector[CurSectorAround.around[i].iY][CurSectorAround.around[i].iX];

		// ���Ϳ� �ο��� ������ ����
		if (pSectorList->size() == 0)
			continue;

		for (ListIter = pSectorList->begin(); ListIter != pSectorList->end(); ListIter++)
		{
			if (bSendMe == false)
			{
				if ((*ListIter)->pSession == pSession)
					continue;
			}

			(*ListIter)->pSession->sendQ.Enqueue(packetSz->GetBufferPtr(), packetSz->GetDataSize());
		}
		
	}
}

void Network::SendPacket_Broadcast(st_SESSION * pSession, cPacketSerialz * packetSz)
{
}


//------------------------------------------------------------------------
//
// Content ------------------------------------------------------
//
//------------------------------------------------------------------------
//


// � �ɸ��Ͱ� � �������� �̵��� ������
int Network::netPacketProc_MoveStart(cPacketSerialz * packetSz, st_SESSION * pSession)
{
	st_CHARACTER * pCharacter = FindCharacter(pSession);
	if (pCharacter == NULL)
	{
		_LOG(dfLOG_LEVEL_ERROR, L"MoveStart Error Character Not Found! Critical Error SessionID : %d", pSession->dwSessionID);
		return 0;
	}

	// �����, X,Y�� �Ѿ��.
	BYTE byDir;
	WORD x;
	WORD y;

	*packetSz >> byDir >> x >> y;
	packetSz->Clear();
	//_LOG(dfLOG_LEVEL_DEBUG, L"# MOVESTART # SessionID:%d / Direction:%d / X:%d / Y:%d", pSession->dwSessionID, byDir, x, y);

	// ������ ��ġ�� ���� ��Ŷ�� ��ġ���� �ʹ� ū���̰� ���� ���� ��Ŀ������ ����ġ Ȯ��. MMORPG������ ������ ���� ��ǥ�� �´�.
	// ��ǥ�� �ٸ��� ��ũ ��Ŷ�� ������ ��ǥ ����.

	if (abs(x - pCharacter->iX) > dfERROR_RANG || abs(y - pCharacter->iY) > dfERROR_RANG)
	{
		WORD wdrX, wdrY;
		int iDeadFrame = DeadReckoningPos(	pCharacter->byDirection,
											pCharacter->dwActionTick,
											pCharacter->iActionX,
											pCharacter->iActionY,
											wdrX, wdrY);

		// ������ ������ ��ǥ�� ����
		// Ŭ���� ��ǥ�� ���巹Ŀ���� ��ǥ�� �ʹ� Ʋ���� ���� ��Ŀ�� ��ǥ�� Ŭ�� ���� �� ��Ŷ ������. 
		if (abs(wdrX - x) > dfERROR_RANG || abs(wdrY - y) > dfERROR_RANG)
		{
			// �� ��Ŷ�� ������ x,y �� �ڽſ��� ����
			MakePacket_Syne(packetSz, pCharacter->dwClientNo, wdrX, wdrY);
			pSession->sendQ.Enqueue(packetSz->GetBufferPtr(), packetSz->GetDataSize());
			_LOG(dfLOG_LEVEL_DEBUG, L"# SYNE # SessionID:%d / Direction:%d / X:%d / Y:%d", pSession->dwSessionID, byDir, wdrX, wdrY);
		}

		// ���� ��ǥ
		x = wdrX;
		y = wdrY;
	}

	// Sector
	if (Sector_UpdateCharacter(pCharacter))
	{
		// ���Ͱ� ����� ���� Ŭ���̾�Ʈ ���� ������ ���. 
		CharacterSectorUpdatePacket(pCharacter);
	}

	pCharacter->dwActionTick = GetTickCount();
	pCharacter->byDirection = byDir;
	pCharacter->dwAction = dfPACKET_SC_MOVE_START;
	pCharacter->iX = x;
	pCharacter->iY = y;
	pCharacter->iActionX = x;
	pCharacter->iActionY = y;

	packetSz->Clear();

	// �ٸ� �ɸ������׵� �°� �̵��ߴٰ� �˷���� �Ѵ�.
	MakePacket_MoveOtherCharacter(packetSz, pCharacter->dwClientNo, byDir, x, y);
	SendPacket_Around(pSession, packetSz);

	return 1;
}

int Network::netPacketProc_MoveStop(cPacketSerialz * packetSz, st_SESSION * pSession)
{
	st_CHARACTER * pCharacter = FindCharacter(pSession);
	if (pCharacter == NULL)
		return 0;

	// �����, X,Y�� �Ѿ��.
	BYTE byDir;
	WORD x;
	WORD y;

	*packetSz >> byDir >> x >> y;
	packetSz->Clear();
	//_LOG(dfLOG_LEVEL_DEBUG, L"# MOVESTOP # SessionID:%d / Direction:%d / X:%d / Y:%d", pSession->dwSessionID, byDir, x, y);

	// ������ ��ġ�� ���� ��Ŷ�� ��ġ���� �ʹ� ū���̰� ���� ���� ��Ŀ������ ����ġ Ȯ��. MMORPG������ ������ ���� ��ǥ�� �´�.
	// ��ǥ�� �ٸ��� ��ũ ��Ŷ�� ������ ��ǥ ����.
	if (abs(x - pCharacter->iX) > dfERROR_RANG || abs(y - pCharacter->iY) > dfERROR_RANG)
	{
		WORD wdrX, wdrY;
		int iDeadFrame = DeadReckoningPos(pCharacter->byDirection,
			pCharacter->dwActionTick,
			pCharacter->iActionX,
			pCharacter->iActionY,
			wdrX, wdrY);

		// ������ ������ ��ǥ�� ����
		// Ŭ���� ��ǥ�� ���巹Ŀ���� ��ǥ�� �ʹ� Ʋ���� ���� ��Ŀ�� ��ǥ�� Ŭ�� ���� �� ��Ŷ ������. 
		if (abs(wdrX - x) > dfERROR_RANG || abs(wdrY - y) > dfERROR_RANG)
		{
			// �� ��Ŷ�� ������ x,y �� �ڽſ��� ����
			MakePacket_Syne(packetSz, pCharacter->dwClientNo, wdrX, wdrY);
			pSession->sendQ.Enqueue(packetSz->GetBufferPtr(), packetSz->GetDataSize());
			_LOG(dfLOG_LEVEL_DEBUG, L"# SYNE # SessionID:%d / Direction:%d / X:%d / Y:%d", pSession->dwSessionID, byDir, wdrX, wdrY);
		}

		// ���� ��ǥ
		x = wdrX;
		y = wdrY;
	}

	// Sector
	if (Sector_UpdateCharacter(pCharacter))
	{
		// ���Ͱ� ����� ���� Ŭ���̾�Ʈ ���� ������ ���. 
		CharacterSectorUpdatePacket(pCharacter);
	}
	//_LOG(dfLOG_LEVEL_DEBUG, L"# MOVESTOP # SessionID:%d / Direction:%d / X:%d / Y:%d", pSession->dwSessionID, byDir, x, y);
	pCharacter->dwActionTick = GetTickCount();
	pCharacter->byDirection = byDir;
	pCharacter->dwAction = dfPACKET_SC_MOVE_STOP;
	pCharacter->iX = x;
	pCharacter->iY = y;
	pCharacter->iActionX = x;
	pCharacter->iActionY = y;

	packetSz->Clear();

	// �ٸ� �ɸ������׵� �°� �̵��ߴٰ� �˷���� �Ѵ�.
	MakePacket_MoveStopOtherCharacter(packetSz, pCharacter->dwClientNo, byDir, x, y);
	SendPacket_Around(pSession, packetSz);

	return 1;
}

int Network::netPacketProc_Attack1(cPacketSerialz * packetSz, st_SESSION * pSession)
{
	// ��� �������� ��� ��ġ���� ������ ���ƿ�.
	st_CHARACTER * pCharacter = FindCharacter(pSession);
	if (pCharacter == NULL)
		return 0;

	// �����, X,Y�� �Ѿ��.
	BYTE btDir = 0;
	WORD x = 0;
	WORD y = 0;
	*packetSz >> btDir >> x >> y;

	pCharacter->dwActionTick = GetTickCount();
	pCharacter->dwAction = dfPACKET_SC_ATTACK1;
	pCharacter->iX = x;
	pCharacter->iY = y;
	pCharacter->iActionX = x;
	pCharacter->iActionY = y;

	packetSz->Clear();

	MakePacket_ATTACK1(packetSz, pCharacter->dwClientNo, btDir, x, y);
	SendPacket_Around(pSession, packetSz);

	// ���� �˻�
	DamageHitAround(pSession, 1);

	return 1;
}

int Network::netPacketProc_Attack2(cPacketSerialz * packetSz, st_SESSION * pSession)
{
	st_CHARACTER * pCharacter = FindCharacter(pSession);
	if (pCharacter == NULL)
		return 0;

	// �����, X,Y�� �Ѿ��.
	BYTE btDir = 0;
	WORD x = 0;
	WORD y = 0;
	*packetSz >> btDir >> x >> y;

	pCharacter->dwActionTick = GetTickCount();
	pCharacter->dwAction = dfPACKET_SC_ATTACK2;
	pCharacter->iX = x;
	pCharacter->iY = y;
	pCharacter->iActionX = x;
	pCharacter->iActionY = y;

	packetSz->Clear();

	MakePacket_ATTACK2(packetSz, pCharacter->dwClientNo, btDir, x, y);
	SendPacket_Around(pSession, packetSz);

	// ���� �˻�
	DamageHitAround(pSession, 3);

	return 1;
}

int Network::netPacketProc_Attack3(cPacketSerialz * packetSz, st_SESSION * pSession)
{
	st_CHARACTER * pCharacter = FindCharacter(pSession);
	if (pCharacter == NULL)
		return 0;

	// �����, X,Y�� �Ѿ��.
	BYTE btDir = 0;
	WORD x = 0;
	WORD y = 0;
	*packetSz >> btDir >> x >> y;

	pCharacter->dwActionTick = GetTickCount();
	pCharacter->dwAction = dfPACKET_SC_ATTACK3;
	pCharacter->iX = x;
	pCharacter->iY = y;
	pCharacter->iActionX = x;
	pCharacter->iActionY = y;

	packetSz->Clear();

	MakePacket_ATTACK3(packetSz, pCharacter->dwClientNo, btDir, x, y);
	SendPacket_Around(pSession, packetSz);

	// ���� �˻�
	DamageHitAround(pSession, 5);

	return 1;
}