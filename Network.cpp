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
	_bConnect = false;		 // 서버 바인딩 성공 여부
	_listenSocket = 0;		 
	_mSessionList.clear();	// 유저 세션 리스트
	_packetSz.Clear();		 // 직렬화 초기화
	_mCharList.clear();		 // 케릭터

	_headerSize = sizeof(st_PACKET_HEADER);

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return FALSE;

	_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenSocket == INVALID_SOCKET)
		return FALSE;

	// 노 딜레이 켬, 모아서 보내지 않고, 바로 바로 보냄. 
	int op = TRUE;
	setsockopt(_listenSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&op, sizeof(op));

	WCHAR szConnectIp[128];
	memset(szConnectIp, 0, sizeof(szConnectIp));
	wsprintf(szConnectIp, L"%S", "127.0.0.1");

	//IN_ADDR inAddr;
	//BOOL ret = DomaionToIP(szConnectIp, &inAddr);
	//if (ret == 0)
	//{
	//	printf("도메인 에러\n");
	//	return 0;
	//}

	SOCKADDR_IN sockaddr;
	ZeroMemory(&sockaddr, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	 //INADDR_ANY 는 서버의 IP주소를 자동으로 찾아서 대입해주는 함수이다.
	//서버는 NIC을 2개 이상 가지고 있는 경우가 많은데 만일 특정 NIC의 IP주소를 sin_addr.s_addr에 지정하면 다른 NIC에서 연결된 요청은
	//서비스 할 수 없게 된다. 이때 INADDR_ANY를 이용하면 두 NIC를 모두 바인딩해주므로 어느 IP를 통해 접속해도 서비스가 가능해짐.
	//IP주소를 INADDR_ANY 로, 포트 번호를 9000으로 할 경우 => 현재 서버 컴퓨터의 9000번 포트를 목적지로 하는 모든 연결 요청을 해당 서버
	//응용프로그램에서 처리하겠다는 의미.
	//sockaddr.sin_addr = inAddr;
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr.sin_port = htons(dfNETWORK_PORT);

	// 바인드 해야 하고.잠만 별찍기좀 보자
	int bindRet = bind(_listenSocket, (SOCKADDR*)&sockaddr, sizeof(sockaddr));
	if (bindRet != 0)
	{
		WSACleanup();
		return 0;
	}

	// 리슨  클라이언트가 이쪽으로 들어옴 3핸즈 쉐이킹 이라고 보면 됨. 최대 접속자수가 150인가 200인가.? 연결들어와서 서버에서 소켓 만들어줄때까지 대기하는 공간
	// 리슨소켓을 확인해서 배열의 하나씩 뽑아낸다고 보면 됨.
	// 어쨌든 기본적인 연결 통로는 모두 마련 되었다고 보면 된다.
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
	// 이동을 시작했던 시작 시작과 현재 시간의 차를 구한다.
	DWORD dwIntervalTick = timeGetTime() - dwActionTick;
	int iActionFrame = dwIntervalTick / dfFRAME_COUNT;	// 40ms 세컨드로 나누면 프레임이 나옴
	int iRemoveFrame = 0;
	int iValue;

	int iRPosX = wOldPosX;
	int iRPosY = wOldPosY;
	//1. 계산된 프레임으로 X축, Y축의 좌표 이동값을 구함.
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

	// 여기까지가 iRPosX, iRPosY에 계산된 좌표가 완료 되었음.
	// 이 아래 부분은 계산된 좌표가 화면의 이동 영역을 벗어난 경우 그 액션을 잘라내기 위해서 
	// 영역을 벗어난 이후의 프레임을 계산 하는 과정
	
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

	// 위에서 계산된 결과 삭제 되어야 할 프레임이 나타났다면 좌표를 다시 재 계산
	if (iRemoveFrame > 0)
	{
		iActionFrame -= iRemoveFrame;
		// 보정된 좌표로 다시 계산
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
	// 주변 9개 섹터의 유닛들 중에 현재 플레이어의 좌표롸 가장 가까운 녀석을 찾아서 데미지를 먹인다.

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

		// 섹터에 인원이 없으면 리턴
		if (pSectorList->size() == 0)
			continue;

		for (ListIter = pSectorList->begin(); ListIter != pSectorList->end(); ListIter++)
		{
			if ((*ListIter) == character)
				continue;

			// 케릭터가 바라보는 방향
			if (character->byArrowDirection == dfPACKET_MOVE_DIR_LL)
			{
				// X축이 케릭터보다 작아야함
				if ((*ListIter)->iX > character->iX)
					continue;
			}
			else if(character->byArrowDirection == dfPACKET_MOVE_DIR_RR)
			{
				// X축이 케릭터보다 커야함
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
				// 맞는애 중심으로 보낸다.
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

	SOCKET skUserTable[FD_SETSIZE] = { INVALID_SOCKET,}; // 초기화가 되지 않음.
	memset(skUserTable, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);

	int iTotalUser = 0;
	for (auto sessionIter = _mSessionList.begin(); sessionIter != _mSessionList.end();)
	{
		// 여기 맵 유저수만큼 유저테이블에 넣고, 유저테이블을 64로 끊어서 돌린다.
		skUserTable[iTotalUser] = sessionIter->first;
		FD_SET(sessionIter->second->socket, &fdReadSet);
		FD_SET(sessionIter->second->socket, &fdSendSet);

		// Disconnect 할때 Map내용이 사라지면 터짐 그래서 이터를 다음 포인터로 가게 만듬
		//sessionIter++;
		// 반환이 없고 인덱스를 입력하면 해당 인덱스의 값을 반환
		advance(sessionIter, 1);
		++iTotalUser;

		if (iTotalUser >= FD_SETSIZE)
		{
			// 패킷 로직 처리
			netSelectSocket(skUserTable, &fdReadSet, &fdSendSet);

			FD_ZERO(&fdReadSet);
			FD_ZERO(&fdSendSet);

			memset(skUserTable, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);

			iTotalUser = 0;
		}
	}

	// 처리 되지 않는 패킷 처리
	if(iTotalUser != 0)
		netSelectSocket(skUserTable, &fdReadSet, &fdSendSet);
}

void Network::netSelectSocket(SOCKET * pTableSocket, FD_SET * pReadSet, FD_SET * pWriteSet)
{
	TIMEVAL tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	// 읽을게 있는지? 등록되어 있는 유저중 몇명의 유저들께 들어왔다.
	// 샌드는 따로 
	int selectRet = select(NULL, pReadSet, pWriteSet, NULL, &tv);
	if (selectRet > 0)
	{
		// 유저 테이블에 매핑된 키값으로 처리
		for (int i = 0; i < FD_SETSIZE; ++i)
		{
			if (pTableSocket[i] == INVALID_SOCKET)
				continue;

			st_SESSION * pSession = FindSession(pTableSocket[i]);
			if (pSession == nullptr)
			{
				continue;
			}

			// 패킷 처리
			if (FD_ISSET(pSession->socket, pReadSet))
			{
				// 클라에서는 하나주고 하나 받고, 받을 공간이 충분한데, 브로큰 때문에 못 받았다면.. 다음에 다시 받을 수 있지 않나?
				int recvSize = recv(pSession->socket, pSession->recvQ.GetRearBufferPtr(), pSession->recvQ.GetNotBrokenPutSize(), 0);

				pSession->recvQ.MoveRear(recvSize);

				if (recvSize == 0 || recvSize == SOCKET_ERROR)
				{
					DisconnectSession(pSession->socket);
					continue;
				}

				//continue;
				// 처리할게 있으면 다 처리함
				while (pSession->recvQ.GetUseSize() >= _headerSize)
				{
					// 패킷 처리
					switch ((RECV_CHECK)netProc_Recv(pSession))
					{
					case RECV_CHECK::RECV_OK:
						break;
					case RECV_CHECK::RECV_MORE:
						break;
					case RECV_CHECK::RECV_ERROR:
						if (DisconnectSession(pSession->socket) == false)
							_LOG(dfLOG_LEVEL_DEBUG, L"지워지지 않은 유저가 있습니다. %d", pSession->socket);
						continue;
						break;
					default:
						_LOG(dfLOG_LEVEL_DEBUG, L"잘못된 netProc_Recv return 정의. recv %d sk %d", recvSize, pSession->socket);
						continue;
						break;
					}
				}
				int bb = 0;
			}

			// 보낼께 있는가?
			if (pSession->sendQ.GetUseSize() < _headerSize)
				continue;

			// 헤더 복사
			st_PACKET_HEADER header = { 0, };
			pSession->sendQ.Peek((char*)&header, _headerSize);

			// 패킷의 페이로드 사이즈가 있는지 체크 부족하다면 더 받아야 한다.
			if (pSession->sendQ.GetUseSize() < header.bySize + _headerSize)
				continue;

			// 보낼 수 있는 상태 인가 ?
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
	// 헤더 복사
	st_PACKET_HEADER header = { 0 };

	pSession->recvQ.Peek((char*)&header, _headerSize);

	// 패킷의 첫 바이트가 0x89인가? 위변조 체크
	if (header.byCode != 0x89)
		return RECV_CHECK::RECV_ERROR;

	// 패킷의 페이로드 사이즈가 있는지 체크 부족하다면 더 받아야 한다.
	if (header.bySize + _headerSize > pSession->recvQ.GetUseSize())
		return RECV_CHECK::RECV_MORE;

	// 데이타그램 복사
	char userDataGram[18000];
	pSession->recvQ.Peek(userDataGram, header.bySize + _headerSize);

	//// 체크썸 체크 체크섬 값,메시지 타입, 페이로드 크기, 버퍼
	//if (CheckSum(header.byCheckSum, header.bySize, header.bySize, buf_ + _headerSize) == false)
	//	return RECV_CHECK::RECV_ERROR;

	// 모든 조건이 만족함 패킷을 처리 하면 됨.
	if (CompleteRecvPacket(header.byType, userDataGram, header.bySize + _headerSize, pSession) == 0)
		return RECV_CHECK::RECV_ERROR;

	// MovePtr + 패킷 엔드 코드
	pSession->recvQ.MoveFront(header.bySize + _headerSize + 1);

	++_uiPPS;

	return RECV_CHECK::RECV_OK;
}


int Network::CompleteRecvPacket(WORD wMsgType_, char * buf_, int bufSize_, st_SESSION * pSession)
{
	// 직렬화 초기화
	_packetSz.Clear();

	//cPacketSerialz packetSz;
	//packetSz.PutData(buf_ + _headerSize, bufSize_ - _headerSize);
	_packetSz.PutData(buf_ + _headerSize, bufSize_ - _headerSize);
	
	int iProcRet = 1;
	// 패킷
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
// 세션 생성
//
st_SESSION * Network::CreateSession(SOCKET sk, SOCKADDR_IN sockAddr)
{
	// 서버에 유저의 정보를 담는다. 
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
// Accept 접속 클라이언트 
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
		// 패킷 처리
		if (FD_ISSET(_listenSocket, &fdReadSet))
		{
			// accept작업
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

// 소켓 번호로 세션에 매핑된 케릭터를 찾음
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
// 캐릭터 생성 및 기타 
//----------------------------------------------------------------
//
void Network::CreateCharacter(st_SESSION * pSession)
{
	_packetSz.Clear();

	// 케릭터 생성 후 맵에 저장
	// 메이크패킷으로 다시 돌려줌 
	// 상대방에서도 새로들어온 녀석의 정보를 생성하라는 메이크패킷 생성 브로드 캐스팅
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

	// 내 케릭터 생성
	MakePacket_CreateMyCharacter(&_packetSz, character->dwClientNo, character->byDirection, character->iX, character->iY, character->byHp);
	pSession->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());

	_packetSz.Clear();

	// 내케릭터 정보는 지금 저장
	_mCharList.insert(make_pair(pSession->socket, character));

	// 섹터 추가
	if (Sector_UpdateCharacter(character))
	{
		CharacterSectorUpdatePacket(character);
	}
}

//
//----------------------------------------------------------------
// 캐릭터 삭제
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
// 클라이언트 접속 해제
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
			// 1.RemoveSector 에 캐릭터 삭제 패킷 보내기
			MakePacket_DeleteOtherCharacter(&packetDelete, _mCharList[sk_]->dwClientNo);

			// 주변에게 제거 패킷
			SendPacket_Around(_mSessionList[sk_], &packetDelete);
			
			// 섹터 제거
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
// 체크섬 사용하지 않음
//----------------------------------------------------------------
//

BOOL Network::CheckSum(int checkVal_, WORD wMsgType_, int payLoadSize_, char * payLoadBuf_)
{
	BYTE checkPaySum = 0;
	// 먼저 메시지 타입 더하고 
	checkPaySum += (*(&wMsgType_ + 0));
	checkPaySum += (*(&wMsgType_ + 1));

	// 페이로드의 크기 만큼 바이트를 더한다.
	for (int i = 0; i < payLoadSize_; ++i)
		checkPaySum += (BYTE)payLoadBuf_[i];

	// 체크섬이 같은가?
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

	// 해당 섹터마다 등록된 캐릭터들을 뽑아서 생선패킷 만들어 보냄
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

		// 섹터에 인원이 없으면 리턴
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


// 어떤 케릭터가 어떤 방향으로 이동을 시작함
int Network::netPacketProc_MoveStart(cPacketSerialz * packetSz, st_SESSION * pSession)
{
	st_CHARACTER * pCharacter = FindCharacter(pSession);
	if (pCharacter == NULL)
	{
		_LOG(dfLOG_LEVEL_ERROR, L"MoveStart Error Character Not Found! Critical Error SessionID : %d", pSession->dwSessionID);
		return 0;
	}

	// 방향과, X,Y만 넘어옴.
	BYTE byDir;
	WORD x;
	WORD y;

	*packetSz >> byDir >> x >> y;
	packetSz->Clear();
	//_LOG(dfLOG_LEVEL_DEBUG, L"# MOVESTART # SessionID:%d / Direction:%d / X:%d / Y:%d", pSession->dwSessionID, byDir, x, y);

	// 서버의 위치와 받은 패킷의 위치값이 너무 큰차이가 나며 데드 레커닝으로 재위치 확인. MMORPG에서는 무조건 서버 좌표가 맞다.
	// 좌표가 다르면 싱크 패킷을 보내어 좌표 보정.

	if (abs(x - pCharacter->iX) > dfERROR_RANG || abs(y - pCharacter->iY) > dfERROR_RANG)
	{
		WORD wdrX, wdrY;
		int iDeadFrame = DeadReckoningPos(	pCharacter->byDirection,
											pCharacter->dwActionTick,
											pCharacter->iActionX,
											pCharacter->iActionY,
											wdrX, wdrY);

		// 서버측 수정된 좌표로 보정
		// 클라의 봐표랑 데드레커닝한 좌표랑 너무 틀리면 데드 레커닝 좌표로 클라를 조절 씬 패킷 날린다. 
		if (abs(wdrX - x) > dfERROR_RANG || abs(wdrY - y) > dfERROR_RANG)
		{
			// 씬 패킷을 수정된 x,y 로 자신에게 날림
			MakePacket_Syne(packetSz, pCharacter->dwClientNo, wdrX, wdrY);
			pSession->sendQ.Enqueue(packetSz->GetBufferPtr(), packetSz->GetDataSize());
			_LOG(dfLOG_LEVEL_DEBUG, L"# SYNE # SessionID:%d / Direction:%d / X:%d / Y:%d", pSession->dwSessionID, byDir, wdrX, wdrY);
		}

		// 실제 좌표
		x = wdrX;
		y = wdrY;
	}

	// Sector
	if (Sector_UpdateCharacter(pCharacter))
	{
		// 섹터가 변경된 경우는 클라이언트 관련 정보를 쏜다. 
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

	// 다른 케릭터한테도 걔가 이동했다고 알려줘야 한다.
	MakePacket_MoveOtherCharacter(packetSz, pCharacter->dwClientNo, byDir, x, y);
	SendPacket_Around(pSession, packetSz);

	return 1;
}

int Network::netPacketProc_MoveStop(cPacketSerialz * packetSz, st_SESSION * pSession)
{
	st_CHARACTER * pCharacter = FindCharacter(pSession);
	if (pCharacter == NULL)
		return 0;

	// 방향과, X,Y만 넘어옴.
	BYTE byDir;
	WORD x;
	WORD y;

	*packetSz >> byDir >> x >> y;
	packetSz->Clear();
	//_LOG(dfLOG_LEVEL_DEBUG, L"# MOVESTOP # SessionID:%d / Direction:%d / X:%d / Y:%d", pSession->dwSessionID, byDir, x, y);

	// 서버의 위치와 받은 패킷의 위치값이 너무 큰차이가 나며 데드 레커닝으로 재위치 확인. MMORPG에서는 무조건 서버 좌표가 맞다.
	// 좌표가 다르면 싱크 패킷을 보내어 좌표 보정.
	if (abs(x - pCharacter->iX) > dfERROR_RANG || abs(y - pCharacter->iY) > dfERROR_RANG)
	{
		WORD wdrX, wdrY;
		int iDeadFrame = DeadReckoningPos(pCharacter->byDirection,
			pCharacter->dwActionTick,
			pCharacter->iActionX,
			pCharacter->iActionY,
			wdrX, wdrY);

		// 서버측 수정된 좌표로 보정
		// 클라의 봐표랑 데드레커닝한 좌표랑 너무 틀리면 데드 레커닝 좌표로 클라를 조절 씬 패킷 날린다. 
		if (abs(wdrX - x) > dfERROR_RANG || abs(wdrY - y) > dfERROR_RANG)
		{
			// 씬 패킷을 수정된 x,y 로 자신에게 날림
			MakePacket_Syne(packetSz, pCharacter->dwClientNo, wdrX, wdrY);
			pSession->sendQ.Enqueue(packetSz->GetBufferPtr(), packetSz->GetDataSize());
			_LOG(dfLOG_LEVEL_DEBUG, L"# SYNE # SessionID:%d / Direction:%d / X:%d / Y:%d", pSession->dwSessionID, byDir, wdrX, wdrY);
		}

		// 실제 좌표
		x = wdrX;
		y = wdrY;
	}

	// Sector
	if (Sector_UpdateCharacter(pCharacter))
	{
		// 섹터가 변경된 경우는 클라이언트 관련 정보를 쏜다. 
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

	// 다른 케릭터한테도 걔가 이동했다고 알려줘야 한다.
	MakePacket_MoveStopOtherCharacter(packetSz, pCharacter->dwClientNo, byDir, x, y);
	SendPacket_Around(pSession, packetSz);

	return 1;
}

int Network::netPacketProc_Attack1(cPacketSerialz * packetSz, st_SESSION * pSession)
{
	// 어느 방향으로 어디 위치에서 공격이 날아옴.
	st_CHARACTER * pCharacter = FindCharacter(pSession);
	if (pCharacter == NULL)
		return 0;

	// 방향과, X,Y만 넘어옴.
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

	// 공격 검사
	DamageHitAround(pSession, 1);

	return 1;
}

int Network::netPacketProc_Attack2(cPacketSerialz * packetSz, st_SESSION * pSession)
{
	st_CHARACTER * pCharacter = FindCharacter(pSession);
	if (pCharacter == NULL)
		return 0;

	// 방향과, X,Y만 넘어옴.
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

	// 공격 검사
	DamageHitAround(pSession, 3);

	return 1;
}

int Network::netPacketProc_Attack3(cPacketSerialz * packetSz, st_SESSION * pSession)
{
	st_CHARACTER * pCharacter = FindCharacter(pSession);
	if (pCharacter == NULL)
		return 0;

	// 방향과, X,Y만 넘어옴.
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

	// 공격 검사
	DamageHitAround(pSession, 5);

	return 1;
}