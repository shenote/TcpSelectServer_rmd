#include "pch.h"
#include "MakePacket.h"
//
//void MakePacketResLogin(st_PACKET_HEADER * header, cPacketSerialz * packetSz, BYTE btRest, UINT uiUserNumber)
//{
//	header->byCode = dfPACKET_CODE;
//	header->wMsgType = df_RES_LOGIN;
//	header->wPayloadSize = sizeof(btRest) + sizeof(uiUserNumber);
//
//	BYTE checkPaySum = 0;
//	checkPaySum += *((BYTE*)&header->wMsgType + 0);
//	checkPaySum += *((BYTE*)&header->wMsgType + 1);
//
//	checkPaySum += btRest;
//	
//	// ���̷ε��� ũ�� ��ŭ ����Ʈ�� ���Ѵ�.
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
//	header->wMsgType = df_RES_ROOM_LIST;
//	// ��ü ������ + �� �ѹ� + ���̸����� + �����ڵ� �̸� + �����ο�
//	
//	UINT payLoadSize = subPacketSz->GetDataSize();
//	header->wPayloadSize = sizeof(wRoomCount) + payLoadSize;
//
//	BYTE checkPaySum = 0;
//
//	// MsgType
//	checkPaySum += *((BYTE*)&header->wMsgType + 0);
//	checkPaySum += *((BYTE*)&header->wMsgType + 1);
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
//	header->wMsgType = df_RES_ROOM_CREATE;
//	BYTE checkPaySum = 0;
//	// üũ�� Msg
//	checkPaySum += *((BYTE*)&header->wMsgType + 0);
//	checkPaySum += *((BYTE*)&header->wMsgType + 1);
//
//	// ���� ������ �޸𸮿� �ε�
//	cPacketSerialz subPacketSz;
//	subPacketSz << btResult << roomNo << roomSize << wChrRoNm;
//
//	// ���� �ε�� ������ ������
//	header->wPayloadSize = subPacketSz.GetDataSize();
//
//	// üũ��
//	for (int i = 0; i < header->wPayloadSize; ++i)
//		checkPaySum += *(subPacketSz.GetBufferPtr() + i);
//
//	// üũ�� ������
//	header->byCheckSum = checkPaySum % 256;
//
//	packetSz->PutData((char*)header, sizeof(st_PACKET_HEADER));
//	packetSz->PutData(subPacketSz.GetBufferPtr(), header->wPayloadSize);
//
//}
//
//void MakePacketResRoomEnter(st_PACKET_HEADER * header, cPacketSerialz * packetSz, cPacketSerialz * subPacketSz)
//{
//	header->byCode = dfPACKET_CODE;
//	header->wMsgType = df_RES_ROOM_ENTER;
//	BYTE checkPaySum = 0;
//	// üũ�� Msg
//	checkPaySum += *((BYTE*)&header->wMsgType + 0);
//	checkPaySum += *((BYTE*)&header->wMsgType + 1);
//
//	// ���� �ε�� ������ ������
//	header->wPayloadSize = subPacketSz->GetDataSize();
//
//	// üũ��
//	for (int i = 0; i < header->wPayloadSize; ++i)
//		checkPaySum += *(subPacketSz->GetBufferPtr() + i);
//
//	// üũ�� ������
//	header->byCheckSum = checkPaySum % 256;
//
//	packetSz->PutData((char*)header, sizeof(st_PACKET_HEADER));
//	packetSz->PutData(subPacketSz->GetBufferPtr(), header->wPayloadSize);
//
//
//}
//
//void MakePacketResUserEnter(st_PACKET_HEADER * header, cPacketSerialz * packetSz, WCHAR nickName[15], UINT userNo)
//{
//	header->byCode = dfPACKET_CODE;
//	header->wMsgType = df_RES_USER_ENTER;
//	BYTE checkPaySum = 0;
//	// üũ�� Msg
//	checkPaySum += *((BYTE*)&header->wMsgType + 0);
//	checkPaySum += *((BYTE*)&header->wMsgType + 1);
//
//	// ���� ������ �޸𸮿� �ε�
//	cPacketSerialz subPacketSz;
//	memcpy(subPacketSz.GetBufferPtr(), nickName, 30);
//	subPacketSz.MoveWritePos(30);
//
//	subPacketSz << userNo;
//
//	// ���� �ε�� ������ ������
//	header->wPayloadSize = subPacketSz.GetDataSize();
//	
//	// üũ��
//	for (int i = 0; i < header->wPayloadSize; ++i)
//		checkPaySum += *(subPacketSz.GetBufferPtr() + i);
//
//	// üũ�� ������
//	header->byCheckSum = checkPaySum % 256;
//
//	packetSz->PutData((char*)header, sizeof(st_PACKET_HEADER));
//	packetSz->PutData(subPacketSz.GetBufferPtr(), header->wPayloadSize);
//
//}
//
//void MakePacketResChat(st_PACKET_HEADER * header, cPacketSerialz * packetSz, UINT sendUserNo, WORD wMsgSize, WCHAR * wMsg)
//{
//	header->byCode = dfPACKET_CODE;
//	header->wMsgType = df_RES_CHAT;
//	BYTE checkPaySum = 0;
//	// üũ�� Msg
//	checkPaySum += *((BYTE*)&header->wMsgType + 0);
//	checkPaySum += *((BYTE*)&header->wMsgType + 1);
//
//	// ���� ������ �޸𸮿� �ε�
//	cPacketSerialz subPacketSz;
//	subPacketSz << sendUserNo;
//	subPacketSz << wMsgSize;
//	subPacketSz << wMsg;
//
//	// ���� �ε�� ������ ������
//	header->wPayloadSize = subPacketSz.GetDataSize();
//
//	// üũ��
//	for (int i = 0; i < header->wPayloadSize; ++i)
//		checkPaySum += *(subPacketSz.GetBufferPtr() + i);
//
//	// üũ�� ������
//	header->byCheckSum = checkPaySum % 256;
//
//	packetSz->PutData((char*)header, sizeof(st_PACKET_HEADER));
//	packetSz->PutData(subPacketSz.GetBufferPtr(), header->wPayloadSize);
//}
//
//void MakePacketResRoomLeave(st_PACKET_HEADER * header, cPacketSerialz * packetSz, UINT sendUserNo)
//{
//	header->byCode = dfPACKET_CODE;
//	header->wMsgType = df_RES_ROOM_LEAVE;
//	header->wPayloadSize = sizeof(sendUserNo);
//
//	BYTE checkPaySum = 0;
//	// üũ�� Msg
//	checkPaySum += *((BYTE*)&header->wMsgType + 0);
//	checkPaySum += *((BYTE*)&header->wMsgType + 1);
//
//	for (int i = 0; i < header->wPayloadSize; ++i)
//		checkPaySum += *((BYTE*)&sendUserNo + i);
//	
//	// üũ�� ������
//	header->byCheckSum = checkPaySum % 256;
//
//	packetSz->PutData((char*)header, sizeof(st_PACKET_HEADER));
//	*packetSz << sendUserNo;
//}
//
//void MakePacketResRoomDelete(st_PACKET_HEADER * header, cPacketSerialz * packetSz, UINT roomNo)
//{
//	header->byCode = dfPACKET_CODE;
//	header->wMsgType = df_RES_ROOM_DELETE;
//	header->wPayloadSize = sizeof(roomNo);
//
//	BYTE checkPaySum = 0;
//	// üũ�� Msg
//	checkPaySum += *((BYTE*)&header->wMsgType + 0);
//	checkPaySum += *((BYTE*)&header->wMsgType + 1);
//
//	for (int i = 0; i < header->wPayloadSize; ++i)
//		checkPaySum += *((BYTE*)&roomNo + i);
//
//	// üũ�� ������
//	header->byCheckSum = checkPaySum % 256;
//
//	packetSz->PutData((char*)header, sizeof(st_PACKET_HEADER));
//	*packetSz << roomNo;
//}

void MakePacket_ResAddAcount(cPacketSerialz * packetSz, UINT64 AccountNo)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.wMsgType = df_RES_ACCOUNT_ADD;
	header.wPayloadSize = sizeof(UINT64);
	
	*packetSz << header;
	*packetSz << AccountNo;
}

void MakePacket_ResLogin(cPacketSerialz * packetSz, UINT64 AccountNo, WCHAR * wNickNm)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.wMsgType = df_RES_LOGIN;
	header.wPayloadSize = sizeof(UINT64) + dfNICK_MAX_LEN * 2;
	if (AccountNo == 0)
		header.wPayloadSize = sizeof(UINT64);

	*packetSz << header << AccountNo;
	if(AccountNo != 0)
		packetSz->PutData((char*)wNickNm, dfNICK_MAX_LEN * 2);
}

void MakePacket_ResAccountList(cPacketSerialz * packetSz, cPacketSerialz * packetPayload)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.wMsgType = df_RES_ACCOUNT_LIST;
	header.wPayloadSize = packetPayload->GetDataSize();

	*packetSz << header;
	packetSz->PutData(packetPayload->GetBufferPtr(), packetPayload->GetDataSize());

}

void MakePacket_ResFriendRequest(cPacketSerialz * packetSz)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.wMsgType = df_RES_FRIEND_REQUEST;
	header.wPayloadSize = packetSz->GetDataSize() - sizeof(st_PACKET_HEADER);

	char * buf = packetSz->GetBufferPtr();
	memcpy(buf, (char*)&header, sizeof(st_PACKET_HEADER));

}

void MakePacket_ResFriendReplyList(cPacketSerialz * packetSz, UINT count)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.wMsgType = df_RES_FRIEND_REPLY_LIST;
	header.wPayloadSize = packetSz->GetDataSize() - sizeof(st_PACKET_HEADER);

	char * buf = packetSz->GetBufferPtr();
	memcpy(buf, (char*)&header, sizeof(st_PACKET_HEADER));

	memcpy(buf + sizeof(st_PACKET_HEADER), (char*)&count, sizeof(UINT));
}

void MakePacket_ResFriendRequestList(cPacketSerialz * packetSz, UINT count)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.wMsgType = df_RES_FRIEND_REQUEST_LIST;
	header.wPayloadSize = packetSz->GetDataSize() - sizeof(st_PACKET_HEADER);

	char * buf = packetSz->GetBufferPtr();
	memcpy(buf, (char*)&header, sizeof(st_PACKET_HEADER));

	memcpy(buf + sizeof(st_PACKET_HEADER), (char*)&count, sizeof(UINT));
}

void MakePacket_ResFriendAgree(cPacketSerialz * packetSz, UINT64 friendAccountNo, BYTE btRet)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.wMsgType = df_RES_FRIEND_AGREE;
	header.wPayloadSize = sizeof(st_PACKET_HEADER) + sizeof(UINT64) + sizeof(BYTE);

	char * buf = packetSz->GetBufferPtr();
	memcpy(buf, (char*)&header, sizeof(st_PACKET_HEADER));

	*packetSz << friendAccountNo << btRet;
}

void MakePacket_ResFriendDeny(cPacketSerialz * packetSz, UINT64 friendAccountNo, BYTE btRet)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.wMsgType = df_RES_FRIEND_DENY;
	header.wPayloadSize = sizeof(st_PACKET_HEADER) + sizeof(UINT64) + sizeof(BYTE);

	char * buf = packetSz->GetBufferPtr();
	memcpy(buf, (char*)&header, sizeof(st_PACKET_HEADER));

	*packetSz << friendAccountNo << btRet;
}

void MakePacket_ResFriendList(cPacketSerialz * packetSz, UINT count)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.wMsgType = df_RES_FRIEND_LIST;
	header.wPayloadSize = packetSz->GetDataSize() - sizeof(st_PACKET_HEADER);

	char * buf = packetSz->GetBufferPtr();
	memcpy(buf, (char*)&header, sizeof(st_PACKET_HEADER));

	memcpy(buf + sizeof(st_PACKET_HEADER), (char*)&count, sizeof(UINT));
}

void MakePacket_ResFriendRemove(cPacketSerialz * packetSz, UINT64 friendAccountNo, BYTE btRet)
{
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.wMsgType = df_RES_FRIEND_REMOVE;
	header.wPayloadSize = sizeof(st_PACKET_HEADER) + sizeof(UINT64) + sizeof(BYTE);

	char * buf = packetSz->GetBufferPtr();
	memcpy(buf, (char*)&header, sizeof(st_PACKET_HEADER));

	*packetSz << friendAccountNo << btRet;
}
