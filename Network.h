#pragma once

#include <map>

enum RECV_CHECK
{
	RECV_OK = 0,
	RECV_MORE,
	RECV_ERROR
};



class Network
{
public:
	Network();
	~Network();

	HRESULT		init();
	void		release();
	void		update();

	void		netIOProcess();
	void		netSelectSocket(SOCKET * pTableSocket, FD_SET * pReadSet, FD_SET * pWriteSET);
	int			CompleteRecvPacket(WORD wMsgType_, char * buf_, int bufSize_, st_SESSION * pSession);
	
	BOOL		netProc_Accept();
	int			netProc_Recv(st_SESSION * pSession);

	BOOL		DomaionToIP(const WCHAR *szDomain, IN_ADDR *pAddr);
	BOOL		isConnect() { return _bConnect; }

	st_SESSION * CreateSession(SOCKET sk, SOCKADDR_IN sockAddr);
	void		CreateCharacter(st_SESSION * pSession);

	BOOL		DisconnectSession(SOCKET sk_);

	// 접속 유저수 
	UINT		GetConnectCount() { return _mSessionList.size(); }

	st_SESSION * FindSession(SOCKET sk);
	st_CHARACTER * FindCharacter(st_SESSION * session);

	// 데드레커닝
	int			DeadReckoningPos(BYTE byDir, DWORD dwActionTick, WORD wOldPosX, WORD wOldPosY, WORD & pPosX, WORD & pPosY);
	void		DamageHitAround(st_SESSION * pSession, int iDamage);

	//	체크섬 값, 메시지 타입, 페이로드 크기, 버퍼
	BOOL		CheckSum(int checkVal_, WORD wMsgType_, int payLoadSize_, char * payLoadBuf_);

	void		SendPacket_SectorOne(int iX, int iY, cPacketSerialz * packetSz, st_SESSION * pExceptSession);
	void		SendPacket_Unicast(st_SESSION * pSession, cPacketSerialz * packetSz);
	void		SendPacket_Around(st_SESSION * pSession, cPacketSerialz * packetSz, bool bSendMe = false);
	void		SendPacket_Broadcast(st_SESSION * pSession, cPacketSerialz * packetSz);

	// Content
	int			netPacketProc_MoveStart(cPacketSerialz * packetSz, st_SESSION * pSession);
	int			netPacketProc_MoveStop(cPacketSerialz * packetSz, st_SESSION * pSession);
	int			netPacketProc_Attack1(cPacketSerialz * packetSz, st_SESSION * pSession);
	int			netPacketProc_Attack2(cPacketSerialz * packetSz, st_SESSION * pSession);
	int			netPacketProc_Attack3(cPacketSerialz * packetSz, st_SESSION * pSession);

	UINT64						_uiTPS;
private:

	BOOL						_bConnect;
	SOCKET						_listenSocket;
	unsigned int				_headerSize;
	cPacketSerialz 				_packetSz;
	map<UINT64, st_SESSION*>	_mSessionList;
	map<UINT64, st_CHARACTER*>	_mCharList;

};

