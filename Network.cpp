#include "pch.h"
#include "Network.h"

Network::Network()
{
}


Network::~Network()
{
}

HRESULT Network::init()
{
	_bConnect = false;		 // ���� ���ε� ���� ����
	_listenSocket = 0;		 
	_mUserList.clear();		 // ���� ����Ʈ
	_mAccountList.clear();	 // ���� ���� ����Ʈ
	_packetSz.Clear();		 // ����ȭ �ʱ�ȭ
	_mFriendReqList.clear(); // ģ�� ��û ����Ʈ �ʱ�ȭ
	_mFriendList.clear();	 // ģ�� ����Ʈ �ʱ�ȭ

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

void Network::update()
{
	if (!_bConnect)
		return;

	// accept
	Accept();

	// user recv, send check
	if (_mUserList.empty())
		return;

	// 1���̻� ������ ����

	FD_SET fdReadSet, fdSendSet;
	FD_ZERO(&fdReadSet);
	FD_ZERO(&fdSendSet);
	
	// Session ���� ���� ���� Ŭ���̾�Ʈ�� �ִٸ� ������ �س��´�. ����� 64�̻��̸� ���� �ѹ� �� ����.
	int userCnt = 0;
	auto iter = _mUserList.begin();
	while (true)
	{
		// �� Ȯ�� �� ����
		if (iter == _mUserList.end())
		{
			iter = _mUserList.begin();
			userCnt = 0;
			break;
		}

		for (;iter != _mUserList.end(); ++iter)
		{
			// fd���� ��� ���� �ʰ� ������ �ٽ� �ѹ� ���ƾ���.
			FD_SET(iter->second->socket, &fdReadSet);
			FD_SET(iter->second->socket, &fdSendSet);
			userCnt++;
			if (userCnt > 63)
			{
				// ���� ���� ����
				break;
			}
		}

		// Read & Send
		timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		// ������ �ִ���? ��ϵǾ� �ִ� ������ ����� �����鲲 ���Դ�.
		// ����� ���� 
		int selectRet = select(NULL, &fdReadSet, &fdSendSet, NULL, &tv);
		if (selectRet > 0)
		{
			// ������ ã�Ƽ� �˻� �Ѵ�.

			for (auto userIter = _mUserList.begin(); userIter != _mUserList.end();)
			{
				// ������ �߰��� �������� ���� ���Ͱ� ���������� �װ� �����ϱ� ����
				auto copyIter = userIter;
				userIter++;

				// ��Ŷ ó��
				SOCKET sk = copyIter->second->socket;
				if (FD_ISSET(sk, &fdReadSet))
				{
					char buf[1400] = "\0";
					int recvSize = recv(sk, buf, copyIter->second->recvQ.GetFreeSize(), 0);

					if (recvSize == 0 || recvSize == SOCKET_ERROR)
					{
						printf("DISCONNECT : %d\n", sk);
						DeleteUser(sk);
						continue;
					}
					copyIter->second->recvQ.Enqueue(buf, recvSize);

					// ó���Ұ� ������ �� ó����
					while (copyIter->second->recvQ.GetUseSize() >= _headerSize)
					{
						// ��Ŷ ó��
						switch ((RECV_CHECK)PacketProc(copyIter->second->recvQ.GetFrontBufferPtr(), recvSize, sk))
						{
						case RECV_CHECK::RECV_OK:
							// ��Ŷ�� ���������� ó�� �Ͽ���. ���� ��Ŷ�� ���� �ִ� üũ
							continue;
							break;
						case RECV_CHECK::RECV_MORE:
							// ��Ŷ�� �� �޾ƾ���
							copyIter->second->recvQ.Enqueue(copyIter->second->recvQ.GetFrontBufferPtr(), recvSize);
							break;
						case RECV_CHECK::RECV_ERROR:
							// �߸��� ������ ���� ����

							// ���� �� �ִ� ���� �ΰ� ? ���� 
							if (FD_ISSET(sk, &fdSendSet))
							{
								if (copyIter->second->sendQ.GetUseSize() != 0)
									send(sk, copyIter->second->sendQ.GetFrontBufferPtr(), copyIter->second->sendQ.GetUseSize(), 0);
							}
							if (DeleteUser(sk) == false)
								printf("�������� ���� ������ �ֽ��ϴ�. %d\n", sk);

							return;
						default:
							printf("�߸��� ��Ŷ ó���� �ֽ��ϴ�. recv %d sk %d\n", recvSize, sk);
							return;
						}
					}
				}

				// ������ �ִ°�?
				if (copyIter->second->sendQ.GetUseSize() == 0)
					continue;

				// ���� �� �ִ� ���� �ΰ� ?
				if (FD_ISSET(sk, &fdSendSet))
				{
					st_PACKET_HEADER * header = (st_PACKET_HEADER*)copyIter->second->sendQ.GetFrontBufferPtr();

					int sendSize = send(sk, copyIter->second->sendQ.GetFrontBufferPtr(), _headerSize + header->wPayloadSize, 0);
					copyIter->second->sendQ.MoveFront(sendSize);
				}
			}
		}
	}
	
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

void Network::Accept()
{
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
		char buffer[128] = { 0, };
		// ��Ŷ ó��
		if (FD_ISSET(_listenSocket, &fdReadSet))
		{
			// accept�۾�
			SOCKADDR_IN sockAddr;
			int clientLen = sizeof(sockAddr);
			SOCKET client = accept(_listenSocket, (SOCKADDR*)&sockAddr, &clientLen);
			if (client == INVALID_SOCKET)
			{
				int a = 0;
			}
			// ������ ������ ������ ��´�. 
			st_USER * user = new st_USER;
			user->socket = client;
			user->sockAddr = sockAddr;
			user->isLogin = FALSE;
			_mUserList.insert(make_pair(client, user));
			printf("Accept - ip %s port %d\n", inet_ntoa(sockAddr.sin_addr), sockAddr.sin_port);
		}
	}
}

int Network::PacketProc(char * buf_, unsigned int size_, UINT sk_)
{
	// ��� ������ Ȯ��
	if (size_ < _headerSize)
		return RECV_CHECK::RECV_MORE;

	// ��� ����
	st_PACKET_HEADER header = { 0, };
	memcpy((char*)&header, buf_, _headerSize);

	// ��Ŷ�� ù ����Ʈ�� 0x89�ΰ�? ������ üũ
	if (header.byCode != 0x89)
		return RECV_CHECK::RECV_ERROR;

	// ��Ŷ�� ���̷ε� ����� �ִ��� üũ �����ϴٸ� �� �޾ƾ� �Ѵ�.
	if (header.wPayloadSize > size_)
		return RECV_CHECK::RECV_MORE;

	//// üũ�� üũ üũ�� ��,�޽��� Ÿ��, ���̷ε� ũ��, ����
	//if (CheckSum(header.byCheckSum, header.wMsgType, header.wPayloadSize, buf_ + _headerSize) == false)
	//	return RECV_CHECK::RECV_ERROR;

	// ��� ������ ������ ��Ŷ�� ó�� �ϸ� ��.
	if (PacketTypeProc(header.wMsgType, buf_ + _headerSize, sk_) == false)
		return RECV_CHECK::RECV_ERROR;

	// MovePtr
	_mUserList[sk_]->recvQ.MoveFront(header.wPayloadSize + _headerSize);

	return RECV_CHECK::RECV_OK;
}


BOOL Network::DeleteUser(UINT sk_)
{
	for (auto iter = _mUserList.begin(); iter != _mUserList.end(); ++iter)
	{
		if (iter->first == sk_)
		{
			closesocket(sk_);
			delete _mUserList[sk_];
			_mUserList.erase(sk_);
			printf("Free Done : %d\n", sk_);
			return TRUE;
		}
	}
	return FALSE;
}

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

BOOL Network::PacketTypeProc(WORD wMsgType_, char * buf_, UINT sk_)
{
	// ����ȭ �ʱ�ȭ
	_packetSz.Clear();

	BOOL bProcRet = true;
	// ��Ŷ
	switch (wMsgType_)
	{
	case df_REQ_ACCOUNT_ADD:
		bProcRet = Packet_ReqAddAccount(buf_, sk_);
		break;
	case df_REQ_LOGIN:
		bProcRet = Packet_ReqLogin(buf_, sk_);
		break;
	case df_REQ_ACCOUNT_LIST:
		bProcRet = Packet_ReqAccountList(buf_, sk_);
		break;
	case df_REQ_FRIEND_REQUEST:
		bProcRet = Packet_ReqFriendRequest(buf_, sk_);
		break;
	case df_REQ_FRIEND_REPLY_LIST:
		bProcRet = Packet_ReqFriendReplyList(buf_, sk_);
		break;
	case df_REQ_FRIEND_REQUEST_LIST:
		bProcRet = Packet_ReqFriendRequestList(buf_, sk_);
		break;
	case df_REQ_FRIEND_AGREE:
		bProcRet = Packet_ResFriendAgree(buf_, sk_);
		break;
	case df_REQ_FRIEND_DENY:
		bProcRet = Packet_ReqFriendDeny(buf_, sk_);
		break;
	case df_REQ_FRIEND_LIST:
		bProcRet = Packet_ReqFriendList(buf_, sk_);
		break;
	case df_REQ_FRIEND_REMOVE:
		bProcRet = Packet_ReqFriendRemove(buf_, sk_);
		break;

	default:
		bProcRet = false;
		break;
	}
	return bProcRet;
}

void Network::SendAll(char * buf, int size)
{
	for(auto iter = _mUserList.begin(); iter != _mUserList.end(); ++iter)
		_mUserList[iter->first]->sendQ.Enqueue(buf, size);
}

void Network::SendOther(char * buf, int size, UINT sk)
{
	for (auto iter = _mUserList.begin(); iter != _mUserList.end(); ++iter)
	{
		if (iter->first == sk)
			continue;
		
		_mUserList[iter->first]->sendQ.Enqueue(buf, size);
	}
}

//
// ȸ������
//
BOOL Network::Packet_ReqAddAccount(char * buf, UINT sk)
{
	st_ACCOUNT acut;
	memcpy(acut.nikName, buf, dfNICK_MAX_LEN * 2);
	UINT64 accountNo = ++g_uiUser;
	_mAccountList.insert(make_pair(accountNo, acut));

	// Res
	MakePacket_ResAddAcount(&_packetSz, accountNo);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());
	return TRUE;
}

//
// �α��� ��û
//
BOOL Network::Packet_ReqLogin(char * buf, UINT sk)
{
	bool bRet = false;
	WCHAR * wNick = NULL;
	UINT64 accountNo = (UINT64)*buf;
	if (accountNo == 0)
		return FALSE;

	for (auto iter = _mAccountList.begin(); iter != _mAccountList.end(); ++iter)
	{
		// ȸ�� ��ȣ�� ������ �ִ��� 
		if (iter->first == accountNo)
		{
			wNick = iter->second.nikName;
			bRet = true;
			_mUserList[sk]->accountNo = accountNo;
			_mUserList[sk]->isLogin = TRUE;
			break;
		}
	}

	if (!bRet)
		accountNo = 0;

	MakePacket_ResLogin(&_packetSz, accountNo, wNick);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());
	return TRUE;
}

BOOL Network::Packet_ReqAccountList(char * buf, UINT sk)
{
	cPacketSerialz packetPayLoad;

	packetPayLoad << _mAccountList.size();
	for (auto iter = _mAccountList.begin(); iter != _mAccountList.end(); ++iter)
	{
		packetPayLoad << iter->first;
		packetPayLoad.PutData((char*)iter->second.nikName, dfNICK_MAX_LEN * 2);
	}

	MakePacket_ResAccountList(&_packetSz, &packetPayLoad);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());

	return TRUE;
}

BOOL Network::Packet_ReqFriendRequest(char * buf, UINT sk)
{
	if (_mUserList[sk]->isLogin == FALSE)
	{
		printf("Not Logged in\n");
		return FALSE;
	}
	// ģ�� ��û
	UINT64 accountNo = (UINT64)*buf;
	BYTE btResult = 0;

	//accountNo�� ��ȣ�� ȸ�� ��Ͽ� �ִ��� �˻�
	auto iter = _mAccountList.find(accountNo);

	// �α����� �Ǿ� �־�� �ϰ�, �α����� �Ǿ� ������ accountNo�� �ִ�. �ڱ� �ڽ��� ģ�� �Ϸ����Ҷ�
	if (_mUserList[sk]->accountNo == 0 || _mUserList[sk]->accountNo == accountNo || iter == _mAccountList.end())
	{
		btResult = df_RESULT_FRIEND_REQUEST_NOTFOUND;
		printf("�߸��� no : %d\n",accountNo);
	}
	else
	{
		//_mFriendReqLists�� ģ����û ����� ��� �� ����.
		BOOL bRet = FALSE;
		//�� '��û�� ģ�� ��Ͽ�' ���� ���� �ϴ��� �˻�
		auto reqFriend = _mFriendReqList.equal_range(accountNo);
		for (auto iter = reqFriend.first; iter != reqFriend.second; ++iter)
		{
			if (iter->second == _mUserList[sk]->accountNo)
			{
				// �ҹ� ��Ȳ
				bRet = TRUE;
				btResult = df_RESULT_FRIEND_REQUEST_NOTFOUND;
				printf("��û�� ģ�� ��Ͽ� ���� ����\n");
				break;
			}
		}

		if (bRet == FALSE)
		{
			// ���� ��û ����Ʈ�� �̹� �ִ��� �˻�
			auto reqFriend = _mFriendReqList.equal_range(_mUserList[sk]->accountNo);
			for (auto iter = reqFriend.first; iter != reqFriend.second; ++iter)
			{
				if (iter->second == accountNo)
				{
					// �ҹ� ��Ȳ
					bRet = TRUE;
					btResult = df_RESULT_FRIEND_REQUEST_NOTFOUND;
					printf("���� ��û�� ģ�� ��Ͽ� ��밡 �̹� ����\n");
					break;
				}
			}
		}

		if (bRet == FALSE)
		{
			// �� ģ�� ��Ͽ��� �ߺ� ���� �ʾ����� ���� ��� �ϱ� ���� �� ���� ģ�� ���� Ȯ��
			// _mFriendList ���⿡�� A->B B->A �� �̷��� �Ǿ� ����. 
			//_mFriendList ���� AŰ���� ã�Ƽ� A�� ������ B�� �ִ��� �˻� ������ �׶� ���
			auto range = _mFriendList.equal_range(_mUserList[sk]->accountNo);
			for (auto iter = range.first; iter != range.second; ++iter)
			{
				// ���� ģ����� �߿� ����Ϸ��� �ְ� �ִٸ�
				if (iter->second == accountNo)
				{
					// �ҹ� ��Ȳ
					bRet = TRUE;
					btResult = df_RESULT_FRIEND_REQUEST_NOTFOUND;
					printf("���� ģ�� ����\n");
				}
			}
		}

		if (btResult == FALSE)
		{
			// ���
			btResult = df_RESULT_FRIEND_REQUEST_OK;
			// �α����� ��ī��Ʈ�� ��û�Ϸ��� ģ���� ��ī��Ʈ�� ���
			// Req
			_mFriendReqList.insert(make_pair(_mUserList[sk]->accountNo, accountNo));
			// Res
			_mFriendResList.insert(make_pair(accountNo, _mUserList[sk]->accountNo));
			printf("���\n");
		}

	}


	st_PACKET_HEADER header;
	_packetSz.PutData((char*)&header, _headerSize);
	_packetSz << _mUserList[sk]->accountNo << btResult;
	MakePacket_ResFriendRequest(&_packetSz);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());

	return TRUE;
}

BOOL Network::Packet_ReqFriendReplyList(char * buf, UINT sk)
{
	if (_mUserList[sk]->isLogin == FALSE)
	{
		printf("Not Logged in\n");
		return FALSE;
	}
	// ģ�� ��û�� ��� ����Ʈ
	// �� ģ�� ����� ã�Ƽ� �ش� ����� �����ش�.
	st_PACKET_HEADER header = { 0, };
	_packetSz << header;

	auto range = _mFriendResList.equal_range(_mUserList[sk]->accountNo);
	UINT count = 0;
	_packetSz << count;

	for (auto iter = range.first; iter != range.second; ++iter) 
	{
		// ��û ���� ģ�� ��ȣ
		_packetSz << iter->second;
		_packetSz.PutData((char*)_mAccountList[iter->second].nikName, dfNICK_MAX_LEN * 2);
		count++;
	}

	MakePacket_ResFriendReplyList(&_packetSz, count);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());
	return TRUE;
}

BOOL Network::Packet_ReqFriendRequestList(char * buf, UINT sk)
{
	if (_mUserList[sk]->isLogin == FALSE)
	{
		printf("Not Logged in\n");
		return FALSE;
	}
	st_PACKET_HEADER header = { 0, };
	_packetSz << header;

	auto range = _mFriendReqList.equal_range(_mUserList[sk]->accountNo);
	UINT count = 0;
	_packetSz << count;

	for (auto iter = range.first; iter != range.second; ++iter)
	{
		// ��û ���� ģ�� ��ȣ
		_packetSz << iter->second;
		_packetSz.PutData((char*)_mAccountList[iter->second].nikName, dfNICK_MAX_LEN * 2);
		count++;
	}

	MakePacket_ResFriendRequestList(&_packetSz, count);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());
	return TRUE;
}

BOOL Network::Packet_ResFriendAgree(char * buf, UINT sk)
{
	if (_mUserList[sk]->isLogin == FALSE)
	{
		printf("Not Logged in\n");
		return FALSE;
	}
	// ģ����û ���� ���
	// ���� B�� �ε� A���� ģ�� ��û�� �޾Ҵµ� ������ �ϰڴ� 
	
	UINT64 accountNo = (UINT64)*buf;
	BYTE bRet = 0;
	auto range = _mFriendResList.equal_range(_mUserList[sk]->accountNo);

	// ģ�� ��û ���� ����Ʈ���� nufno�� ã��
	for (auto iter = range.first; iter != range.second; ++iter)
	{
		if (iter->second == accountNo)
		{
			bRet = df_RESULT_FRIEND_AGREE_OK;
			_mFriendResList.erase(iter);
			break;
		}
	}
	
	// ģ�� ��û�� ����Ʈ���� ã��
	auto rangeReq = _mFriendReqList.equal_range(accountNo);
	for (auto iter = rangeReq.first; iter != rangeReq.second; ++iter)
	{
		if (iter->second == _mUserList[sk]->accountNo)
		{
			_mFriendReqList.erase(iter);
			break;
		}
	}
	
	if (bRet == 0)
	{
		bRet = df_RESULT_FRIEND_AGREE_FAIL;
	}
	else
	{
		// �ִٸ� ģ�� ��� ����Ʈ�� �Ѵ� �ְ�, ��û ���� ����Ʈ���� ����
		// A->B
		_mFriendList.insert(make_pair(_mUserList[sk]->accountNo, accountNo));
		// B->A
		_mFriendList.insert(make_pair(accountNo, _mUserList[sk]->accountNo));
	}
	st_PACKET_HEADER header = { 0, };
	_packetSz << header;

	MakePacket_ResFriendAgree(&_packetSz, accountNo, bRet);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());

	return TRUE;
}

BOOL Network::Packet_ReqFriendDeny(char * buf, UINT sk)
{
	if (_mUserList[sk]->isLogin == FALSE)
	{
		printf("Not Logged in\n");
		return FALSE;
	}
	UINT64 accountNo = (UINT64)*buf;
	BYTE bRet = 0;
	auto range = _mFriendResList.equal_range(_mUserList[sk]->accountNo);

	// ģ�� ��û ���� ����Ʈ���� nufno�� ã��
	for (auto iter = range.first; iter != range.second; ++iter)
	{
		if (iter->second == accountNo)
		{
			bRet = df_RESULT_FRIEND_DENY_OK;
			_mFriendResList.erase(iter);
			break;
		}
	}

	// ģ�� ��û�� ����Ʈ���� ã��
	auto rangeReq = _mFriendReqList.equal_range(accountNo);
	for (auto iter = rangeReq.first; iter != rangeReq.second; ++iter)
	{
		if (iter->second == _mUserList[sk]->accountNo)
		{
			_mFriendReqList.erase(iter);
			break;
		}
	}

	if (bRet == 0)
	{
		bRet = df_RESULT_FRIEND_DENY_FAIL;
	}

	st_PACKET_HEADER header = { 0, };
	_packetSz << header;

	MakePacket_ResFriendDeny(&_packetSz, accountNo, bRet);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());

	return TRUE;
}

BOOL Network::Packet_ReqFriendList(char * buf, UINT sk)
{
	if (_mUserList[sk]->isLogin == FALSE)
	{
		printf("Not Logged in\n");
		return FALSE;
	}
	st_PACKET_HEADER header = { 0, };
	_packetSz << header;

	auto range = _mFriendList.equal_range(_mUserList[sk]->accountNo);
	UINT count = 0;
	_packetSz << count;

	for (auto iter = range.first; iter != range.second; ++iter)
	{
		// �� ģ�� ���
		_packetSz << iter->second;
		_packetSz.PutData((char*)_mAccountList[iter->second].nikName, dfNICK_MAX_LEN * 2);
		count++;
	}

	MakePacket_ResFriendList(&_packetSz, count);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());

	return TRUE;
}

BOOL Network::Packet_ReqFriendRemove(char * buf, UINT sk)
{
	if (_mUserList[sk]->isLogin == FALSE)
	{
		printf("Not Logged in\n");
		return FALSE;
	}
	// ģ�� ���� ����

	UINT64 accountNo = (UINT64)*buf;
	BYTE bRet = 0;

	// ģ����Ͽ��� accountNo ����
	auto range = _mFriendList.equal_range(_mUserList[sk]->accountNo);
	for (auto iter = range.first; iter != range.second; ++iter)
	{
		if (iter->second == accountNo)
		{
			bRet = df_RESULT_FRIEND_REMOVE_OK;
			_mFriendList.erase(iter);
			break;
		}
	}
	
	if (bRet == 0)
	{
		bRet = df_RESULT_FRIEND_REMOVE_OK;
		printf("Friend Remove Fail Not FriendNo : %d\n", accountNo);
	}
	else
	{
		bRet = 0;
		// ģ����Ͽ��� ���� ����
		auto rangeMe = _mFriendList.equal_range(accountNo);

		for (auto iter = rangeMe.first; iter != rangeMe.second; ++iter)
		{
			if (iter->second == _mUserList[sk]->accountNo)
			{
				bRet = df_RESULT_FRIEND_REMOVE_OK;
				_mFriendList.erase(iter);
				break;
			}
		}
	}

	if (bRet == 0)
	{
		bRet = df_RESULT_FRIEND_REMOVE_OK;
		printf("Friend Remove Fail Not meNo : %d\n", _mUserList[sk]->accountNo);
	}

	st_PACKET_HEADER header = { 0, };
	_packetSz << header;

	MakePacket_ResFriendRemove(&_packetSz, accountNo, bRet);
	_mUserList[sk]->sendQ.Enqueue(_packetSz.GetBufferPtr(), _packetSz.GetDataSize());

	return TRUE;
}
