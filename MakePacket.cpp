#include "pch.h"
#include "MakePacket.h"
//
//void MakePacketResLogin(st_PACKET_HEADER * header, cPacketSerialz * packetSz, BYTE btRest, UINT uiUserNumber)
//{
//	header->byCode = dfPACKET_CODE;
//	header->bySize = df_RES_LOGIN;
//	header->bySize = sizeof(btRest) + sizeof(uiUserNumber);
//
//	BYTE checkPaySum = 0;
//	checkPaySum += *((BYTE*)&header->bySize + 0);
//	checkPaySum += *((BYTE*)&header->bySize + 1);
//
//	checkPaySum += btRest;
//	
//	// 페이로드의 크기 만큼 바이트를 더한다.
//	for (int i = 0; i < sizeof(UINT); ++i)
//		checkPaySum += *((BYTE*)&uiUserNumber + i);
//
//	checkPaySum = checkPaySum % 256;
//	header->byCheckSum = checkPaySum;
//
//	(*packetSz).PutData((char*)header, sizeof(st_PACKET_HEADER));
//	*packetSz << btRest << uiUserNumber;
//}
//
//void MakePacketResRoomList(st_PACKET_HEADER * header, WORD wRoomCount, cPacketSerialz * packetSz, cPacketSerialz * subPacketSz)
//{
//	header->byCode = dfPACKET_CODE;
//	header->bySize = df_RES_ROOM_LIST;
//	// 전체 사이즈 + 방 넘버 + 방이름길이 + 유니코드 이름 + 참여인원
//	
//	UINT payLoadSize = subPacketSz->GetDataSize();
//	header->bySize = sizeof(wRoomCount) + payLoadSize;
//
//	BYTE checkPaySum = 0;
//
//	// MsgType
//	checkPaySum += *((BYTE*)&header->bySize + 0);
//	checkPaySum += *((BYTE*)&header->bySize + 1);
//
//	// TotalRoomCount
//	checkPaySum += *((BYTE*)&wRoomCount + 0);
//	checkPaySum += *((BYTE*)&wRoomCount + 1);
//
//	for (int i = 0; i < payLoadSize; ++i)
//		checkPaySum += *(subPacketSz->GetBufferPtr() + i);
//
//	header->byCheckSum = checkPaySum % 256;
//
//	packetSz->PutData((char*)header, sizeof(st_PACKET_HEADER));
//	*packetSz << wRoomCount;
//	packetSz->PutData(subPacketSz->GetBufferPtr(), payLoadSize);
//
//}
//void MakePacketResRoomCreate(st_PACKET_HEADER * header, cPacketSerialz* packetSz, BYTE btResult, UINT roomNo, WORD roomSize, WCHAR * wChrRoNm)
//{
//	header->byCode = dfPACKET_CODE;
//	header->bySize = df_RES_ROOM_CREATE;
//	BYTE checkPaySum = 0;
//	// 체크섬 Msg
//	checkPaySum += *((BYTE*)&header->bySize + 0);
//	checkPaySum += *((BYTE*)&header->bySize + 1);
//
//	// 서브 데이터 메모리에 로드
//	cPacketSerialz subPacketSz;
//	subPacketSz << btResult << roomNo << roomSize << wChrRoNm;
//
//	// 서브 로드된 데이터 사이즈
//	header->bySize = subPacketSz.GetDataSize();
//
//	// 체크섬
//	for (int i = 0; i < header->bySize; ++i)
//		checkPaySum += *(subPacketSz.GetBufferPtr() + i);
//
//	// 체크섬 마무리
//	header->byCheckSum = checkPaySum % 256;
//
//	packetSz->PutData((char*)header, sizeof(st_PACKET_HEADER));
//	packetSz->PutData(subPacketSz.GetBufferPtr(), header->bySize);
//
//}
//
//void MakePacketResRoomEnter(st_PACKET_HEADER * header, cPacketSerialz * packetSz, cPacketSerialz * subPacketSz)
//{
//	header->byCode = dfPACKET_CODE;
//	header->bySize = df_RES_ROOM_ENTER;
//	BYTE checkPaySum = 0;
//	// 체크섬 Msg
//	checkPaySum += *((BYTE*)&header->bySize + 0);
//	checkPaySum += *((BYTE*)&header->bySize + 1);
//
//	// 서브 로드된 데이터 사이즈
//	header->bySize = subPacketSz->GetDataSize();
//
//	// 체크섬
//	for (int i = 0; i < header->bySize; ++i)
//		checkPaySum += *(subPacketSz->GetBufferPtr() + i);
//
//	// 체크섬 마무리
//	header->byCheckSum = checkPaySum % 256;
//
//	packetSz->PutData((char*)header, sizeof(st_PACKET_HEADER));
//	packetSz->PutData(subPacketSz->GetBufferPtr(), header->bySize);
//
//
//}
//
//void MakePacketResUserEnter(st_PACKET_HEADER * header, cPacketSerialz * packetSz, WCHAR nickName[15], UINT userNo)
//{
//	header->byCode = dfPACKET_CODE;
//	header->bySize = df_RES_USER_ENTER;
//	BYTE checkPaySum = 0;
//	// 체크섬 Msg
//	checkPaySum += *((BYTE*)&header->bySize + 0);
//	checkPaySum += *((BYTE*)&header->bySize + 1);
//
//	// 서브 데이터 메모리에 로드
//	cPacketSerialz subPacketSz;
//	memcpy(subPacketSz.GetBufferPtr(), nickName, 30);
//	subPacketSz.MoveWritePos(30);
//
//	subPacketSz << userNo;
//
//	// 서브 로드된 데이터 사이즈
//	header->bySize = subPacketSz.GetDataSize();
//	
//	// 체크섬
//	for (int i = 0; i < header->bySize; ++i)
//		checkPaySum += *(subPacketSz.GetBufferPtr() + i);
//
//	// 체크섬 마무리
//	header->byCheckSum = checkPaySum % 256;
//
//	packetSz->PutData((char*)header, sizeof(st_PACKET_HEADER));
//	packetSz->PutData(subPacketSz.GetBufferPtr(), header->bySize);
//
//}
//
//void MakePacketResChat(st_PACKET_HEADER * header, cPacketSerialz * packetSz, UINT sendUserNo, WORD wMsgSize, WCHAR * wMsg)
//{
//	header->byCode = dfPACKET_CODE;
//	header->bySize = df_RES_CHAT;
//	BYTE checkPaySum = 0;
//	// 체크섬 Msg
//	checkPaySum += *((BYTE*)&header->bySize + 0);
//	checkPaySum += *((BYTE*)&header->bySize + 1);
//
//	// 서브 데이터 메모리에 로드
//	cPacketSerialz subPacketSz;
//	subPacketSz << sendUserNo;
//	subPacketSz << wMsgSize;
//	subPacketSz << wMsg;
//
//	// 서브 로드된 데이터 사이즈
//	header->bySize = subPacketSz.GetDataSize();
//
//	// 체크섬
//	for (int i = 0; i < header->bySize; ++i)
//		checkPaySum += *(subPacketSz.GetBufferPtr() + i);
//
//	// 체크섬 마무리
//	header->byCheckSum = checkPaySum % 256;
//
//	packetSz->PutData((char*)header, sizeof(st_PACKET_HEADER));
//	packetSz->PutData(subPacketSz.GetBufferPtr(), header->bySize);
//}
//
//void MakePacketResRoomLeave(st_PACKET_HEADER * header, cPacketSerialz * packetSz, UINT sendUserNo)
//{
//	header->byCode = dfPACKET_CODE;
//	header->bySize = df_RES_ROOM_LEAVE;
//	header->bySize = sizeof(sendUserNo);
//
//	BYTE checkPaySum = 0;
//	// 체크섬 Msg
//	checkPaySum += *((BYTE*)&header->bySize + 0);
//	checkPaySum += *((BYTE*)&header->bySize + 1);
//
//	for (int i = 0; i < header->bySize; ++i)
//		checkPaySum += *((BYTE*)&sendUserNo + i);
//	
//	// 체크섬 마무리
//	header->byCheckSum = checkPaySum % 256;
//
//	packetSz->PutData((char*)header, sizeof(st_PACKET_HEADER));
//	*packetSz << sendUserNo;
//}
//
//void MakePacketResRoomDelete(st_PACKET_HEADER * header, cPacketSerialz * packetSz, UINT roomNo)
//{
//	header->byCode = dfPACKET_CODE;
//	header->bySize = df_RES_ROOM_DELETE;
//	header->bySize = sizeof(roomNo);
//
//	BYTE checkPaySum = 0;
//	// 체크섬 Msg
//	checkPaySum += *((BYTE*)&header->bySize + 0);
//	checkPaySum += *((BYTE*)&header->bySize + 1);
//
//	for (int i = 0; i < header->bySize; ++i)
//		checkPaySum += *((BYTE*)&roomNo + i);
//
//	// 체크섬 마무리
//	header->byCheckSum = checkPaySum % 256;
//
//	packetSz->PutData((char*)header, sizeof(st_PACKET_HEADER));
//	*packetSz << roomNo;
//}
//
//void MakePacket_ResAddAcount(cPacketSerialz * packetSz, UINT64 AccountNo)
//{
//	st_PACKET_HEADER header;
//	header.byCode = dfPACKET_CODE;
//	header.bySize = df_RES_ACCOUNT_ADD;
//	header.bySize = sizeof(UINT64);
//	
//	*packetSz << header;
//	*packetSz << AccountNo;
//}
//
//void MakePacket_ResLogin(cPacketSerialz * packetSz, UINT64 AccountNo, WCHAR * wNickNm)
//{
//	st_PACKET_HEADER header;
//	header.byCode = dfPACKET_CODE;
//	header.bySize = df_RES_LOGIN;
//	header.bySize = sizeof(UINT64) + dfNICK_MAX_LEN * 2;
//	if (AccountNo == 0)
//		header.bySize = sizeof(UINT64);
//
//	*packetSz << header << AccountNo;
//	if(AccountNo != 0)
//		packetSz->PutData((char*)wNickNm, dfNICK_MAX_LEN * 2);
//}
//
//void MakePacket_ResAccountList(cPacketSerialz * packetSz, cPacketSerialz * packetPayload)
//{
//	st_PACKET_HEADER header;
//	header.byCode = dfPACKET_CODE;
//	header.bySize = df_RES_ACCOUNT_LIST;
//	header.bySize = packetPayload->GetDataSize();
//
//	*packetSz << header;
//	packetSz->PutData(packetPayload->GetBufferPtr(), packetPayload->GetDataSize());
//
//}
//
//void MakePacket_ResFriendRequest(cPacketSerialz * packetSz)
//{
//	st_PACKET_HEADER header;
//	header.byCode = dfPACKET_CODE;
//	header.bySize = df_RES_FRIEND_REQUEST;
//	header.bySize = packetSz->GetDataSize() - sizeof(st_PACKET_HEADER);
//
//	char * buf = packetSz->GetBufferPtr();
//	memcpy(buf, (char*)&header, sizeof(st_PACKET_HEADER));
//
//}
//
//void MakePacket_ResFriendReplyList(cPacketSerialz * packetSz, UINT count)
//{
//	st_PACKET_HEADER header;
//	header.byCode = dfPACKET_CODE;
//	header.bySize = df_RES_FRIEND_REPLY_LIST;
//	header.bySize = packetSz->GetDataSize() - sizeof(st_PACKET_HEADER);
//
//	char * buf = packetSz->GetBufferPtr();
//	memcpy(buf, (char*)&header, sizeof(st_PACKET_HEADER));
//
//	memcpy(buf + sizeof(st_PACKET_HEADER), (char*)&count, sizeof(UINT));
//}
//
//void MakePacket_ResFriendRequestList(cPacketSerialz * packetSz, UINT count)
//{
//	st_PACKET_HEADER header;
//	header.byCode = dfPACKET_CODE;
//	header.bySize = df_RES_FRIEND_REQUEST_LIST;
//	header.bySize = packetSz->GetDataSize() - sizeof(st_PACKET_HEADER);
//
//	char * buf = packetSz->GetBufferPtr();
//	memcpy(buf, (char*)&header, sizeof(st_PACKET_HEADER));
//
//	memcpy(buf + sizeof(st_PACKET_HEADER), (char*)&count, sizeof(UINT));
//}
//
//void MakePacket_ResFriendAgree(cPacketSerialz * packetSz, UINT64 friendAccountNo, BYTE btRet)
//{
//	st_PACKET_HEADER header;
//	header.byCode = dfPACKET_CODE;
//	header.bySize = df_RES_FRIEND_AGREE;
//	header.bySize = sizeof(UINT64) + sizeof(BYTE);
//
//	char * buf = packetSz->GetBufferPtr();
//	memcpy(buf, (char*)&header, sizeof(st_PACKET_HEADER));
//
//	*packetSz << friendAccountNo << btRet;
//}
//
//void MakePacket_ResFriendDeny(cPacketSerialz * packetSz, UINT64 friendAccountNo, BYTE btRet)
//{
//	st_PACKET_HEADER header;
//	header.byCode = dfPACKET_CODE;
//	header.bySize = df_RES_FRIEND_DENY;
//	header.bySize = sizeof(UINT64) + sizeof(BYTE);
//
//	char * buf = packetSz->GetBufferPtr();
//	memcpy(buf, (char*)&header, sizeof(st_PACKET_HEADER));
//
//	*packetSz << friendAccountNo << btRet;
//}
//
//void MakePacket_ResFriendList(cPacketSerialz * packetSz, UINT count)
//{
//	st_PACKET_HEADER header;
//	header.byCode = dfPACKET_CODE;
//	header.bySize = df_RES_FRIEND_LIST;
//	header.bySize = packetSz->GetDataSize() - sizeof(st_PACKET_HEADER);
//
//	char * buf = packetSz->GetBufferPtr();
//	memcpy(buf, (char*)&header, sizeof(st_PACKET_HEADER));
//
//	memcpy(buf + sizeof(st_PACKET_HEADER), (char*)&count, sizeof(UINT));
//}
//
//void MakePacket_ResFriendRemove(cPacketSerialz * packetSz, UINT64 friendAccountNo, BYTE btRet)
//{
//	st_PACKET_HEADER header;
//	header.byCode = dfPACKET_CODE;
//	header.bySize = df_RES_FRIEND_REMOVE;
//	header.bySize = sizeof(UINT64) + sizeof(BYTE);
//
//	char * buf = packetSz->GetBufferPtr();
//	memcpy(buf, (char*)&header, sizeof(st_PACKET_HEADER));
//
//	*packetSz << friendAccountNo << btRet;
//}
//
//BYTE	byCode;			// 패킷코드 0x89 고정.
//BYTE	bySize;			// 패킷 사이즈.
//BYTE	byType;			// 패킷타입.
//BYTE	byTemp;			// 사용안함.
void MakePacket_CreateMyCharacter(cPacketSerialz * packetSz, DWORD id, BYTE dir, WORD x, WORD y, BYTE hp)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_CREATE_MY_CHARACTER;
	header.bySize = sizeof(stPACKET_SC_CREATE_MY_CHARACTER);
	
	*packetSz << header;
	*packetSz << id;
	*packetSz << dir;
	*packetSz << x;
	*packetSz << y;
	*packetSz << hp;
	*packetSz << (BYTE)dfPACKET_END_CODE;
}

void MakePacket_CreateOtherCharacter(cPacketSerialz * packetSz, DWORD id, BYTE dir, WORD x, WORD y, BYTE hp)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;
	header.bySize = sizeof(stPACKET_SC_CREATE_MY_CHARACTER);

	*packetSz << header;
	*packetSz << id;
	*packetSz << dir;
	*packetSz << x;
	*packetSz << y;
	*packetSz << hp;
	*packetSz << (BYTE)dfPACKET_END_CODE;
}

void MakePacket_DeleteOtherCharacter(cPacketSerialz * packetSz, DWORD id)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_DELETE_CHARACTER;
	header.bySize = sizeof(DWORD);

	*packetSz << header;
	*packetSz << id;
	*packetSz << (BYTE)dfPACKET_END_CODE;
}

void MakePacket_MoveOtherCharacter(cPacketSerialz * packetSz, DWORD id, BYTE dir, WORD x, WORD y)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_MOVE_START;
	header.bySize = sizeof(stPACKET_SC_MOVE_START);

	*packetSz << header;
	*packetSz << id;
	*packetSz << dir;
	*packetSz << x;
	*packetSz << y;
	*packetSz << (BYTE)dfPACKET_END_CODE;
}

void MakePacket_MoveStopOtherCharacter(cPacketSerialz * packetSz, DWORD id, BYTE dir, WORD x, WORD y)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_MOVE_STOP;
	header.bySize = sizeof(stPACKET_SC_MOVE_STOP);

	*packetSz << header;
	*packetSz << id;
	*packetSz << dir;
	*packetSz << x;
	*packetSz << y;
	*packetSz << (BYTE)dfPACKET_END_CODE;
}

void MakePacket_ATTACK1(cPacketSerialz * packetSz, DWORD id, BYTE dir, WORD x, WORD y)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_ATTACK1;
	header.bySize = sizeof(stPACKET_SC_ATTACK1);

	*packetSz << header;
	*packetSz << id;
	*packetSz << dir;
	*packetSz << x;
	*packetSz << y;
	*packetSz << (BYTE)dfPACKET_END_CODE;
}

void MakePacket_ATTACK2(cPacketSerialz * packetSz, DWORD id, BYTE dir, WORD x, WORD y)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_ATTACK2;
	header.bySize = sizeof(stPACKET_SC_ATTACK2);

	*packetSz << header;
	*packetSz << id;
	*packetSz << dir;
	*packetSz << x;
	*packetSz << y;
	*packetSz << (BYTE)dfPACKET_END_CODE;
}

void MakePacket_ATTACK3(cPacketSerialz * packetSz, DWORD id, BYTE dir, WORD x, WORD y)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_ATTACK3;
	header.bySize = sizeof(stPACKET_SC_ATTACK3);

	*packetSz << header;
	*packetSz << id;
	*packetSz << dir;
	*packetSz << x;
	*packetSz << y;
	*packetSz << (BYTE)dfPACKET_END_CODE;
}

void MakePacket_DAMAGE(cPacketSerialz * packetSz, DWORD dwAttackID, DWORD dwDamageID, BYTE byDamageHP)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_DAMAGE;
	header.bySize = sizeof(stPACKET_SC_DAMAGE);

	*packetSz << header;
	*packetSz << dwAttackID;
	*packetSz << dwDamageID;
	*packetSz << byDamageHP;
	*packetSz << (BYTE)dfPACKET_END_CODE;
}

void MakePacket_Syne(cPacketSerialz * packetSz, DWORD id, WORD x, WORD y)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_SYNC;
	header.bySize = sizeof(stPACKET_SC_SYNC);

	*packetSz << header;
	*packetSz << id;
	*packetSz << x;
	*packetSz << y;
	*packetSz << (BYTE)dfPACKET_END_CODE;
}

void MakePacket_EHCO(cPacketSerialz * packetSz, DWORD dwTime)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.byType = dfPACKET_SC_ECHO;
	header.bySize = sizeof(DWORD);

	*packetSz << header;
	*packetSz << dwTime;
	*packetSz << (BYTE)dfPACKET_END_CODE;
}
