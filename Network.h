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

	BOOL		DomaionToIP(const WCHAR *szDomain, IN_ADDR *pAddr);
	BOOL		isConnect() { return _bConnect; }
	
	BOOL		netProc_Accept();
	int			netProc_Recv(st_SESSION * pSession);
	int			CompleteRecvPacket(WORD wMsgType_, char * buf_, int bufSize_, st_SESSION * pSession);

	st_SESSION * CreateSession(SOCKET sk, SOCKADDR_IN sockAddr);
	void		CreateCharacter(st_SESSION * pSession);

	void		DeleteCharacter(DWORD id);
	BOOL		DisconnectSession(SOCKET sk_);

	UINT		GetConnectCount() { return _mSessionList.size(); }

	st_SESSION * FindSession(SOCKET sk);
	st_CHARACTER * FindCharacter(st_SESSION * session);

	// 데드레커닝
	int			DeadReckoningPos(BYTE byDir, DWORD dwActionTick, WORD wOldPosX, WORD wOldPosY, WORD & pPosX, WORD & pPosY);
	void		DamageHitAround(st_SESSION * pSession, int iDamage);

	//	체크섬 값, 메시지 타입, 페이로드 크기, 버퍼
	BOOL		CheckSum(int checkVal_, WORD wMsgType_, int payLoadSize_, char * payLoadBuf_);

	//			패킷 Proc 함수
	void		SendAll(char * buf, int size);
	void		SendOther(char * buf, int size, UINT sk);
	//			특정 섹터 한공간에 있는 클라들에게 메시지 전달하는 함수
	void		SendPacket_SectorOne(int iX, int iY, cPacketSerialz * packetSz, st_SESSION * pExceptSession);
	//			특정 1명의 클라이언트에게 메시지 보내기
	void		SendPacket_Unicast(st_SESSION * pSession, cPacketSerialz * packetSz);
	//			클라이언트 기준 주변 섹터에 매시지 보내기(최대 9개 영역)
	void		SendPacket_Around(st_SESSION * pSession, cPacketSerialz * packetSz, bool bSendMe = false);
	//			진정 브로드 캐스팅(시스템적인 메시지 외에는 사용하지 않음
	void		SendPacket_Broadcast(st_SESSION * pSession, cPacketSerialz * packetSz);

	// Content
	int			netPacketProc_MoveStart(cPacketSerialz * packetSz, st_SESSION * pSession);
	int			netPacketProc_MoveStop(cPacketSerialz * packetSz, st_SESSION * pSession);
	int			netPacketProc_Attack1(cPacketSerialz * packetSz, st_SESSION * pSession);
	int			netPacketProc_Attack2(cPacketSerialz * packetSz, st_SESSION * pSession);
	int			netPacketProc_Attack3(cPacketSerialz * packetSz, st_SESSION * pSession);

	UINT64						_uiPPS;
private:

	BOOL						_bConnect;
	SOCKET						_listenSocket;
	unsigned int				_headerSize;
	cPacketSerialz 				_packetSz;
	map<UINT64, st_SESSION*>	_mSessionList;
	map<UINT64, st_CHARACTER*>	_mCharList;

};

