#pragma once


// ȸ������
void MakePacket_ResAddAcount(cPacketSerialz * packetSz, UINT64 AccountNo);

// �α���
void MakePacket_ResLogin(cPacketSerialz * packetSz, UINT64 AccountNo, WCHAR * wNickNm);

// ȸ������Ʈ
void MakePacket_ResAccountList(cPacketSerialz * packetSz, cPacketSerialz * packetPayload);

// ģ����û
void MakePacket_ResFriendRequest(cPacketSerialz * packetSz);

// ���� ��û�� ģ�� ��� ����Ʈ
void MakePacket_ResFriendReplyList(cPacketSerialz * packetSz, UINT count);

// ���� ��û�� ģ�� ��� ����Ʈ
void MakePacket_ResFriendRequestList(cPacketSerialz * packetSz, UINT count);

// ģ�� ����
void MakePacket_ResFriendAgree(cPacketSerialz * packetSz, UINT64 friendAccountNo, BYTE btRet);

// ģ�� ���� ����
void MakePacket_ResFriendDeny(cPacketSerialz * packetSz, UINT64 friendAccountNo, BYTE btRet);

// ģ�� ��� ����Ʈ
void MakePacket_ResFriendList(cPacketSerialz * packetSz, UINT count);

// ģ�� ���� ����
void MakePacket_ResFriendRemove(cPacketSerialz * packetSz, UINT64 friendAccountNo, BYTE btRet);
//
//// �α��� ���
//void MakePacketResLogin(st_PACKET_HEADER * header, cPacketSerialz * packetSz, BYTE btRest, UINT uiUserNumber);
//
//// �� ��� ����Ʈ
//void MakePacketResRoomList(st_PACKET_HEADER * header, WORD wRoomCount, cPacketSerialz * packetSz, cPacketSerialz * subPacketSz);
//
//// �� ���� ��û ���
//void MakePacketResRoomCreate(st_PACKET_HEADER * header, cPacketSerialz* packetSz, BYTE btResult, UINT roomNo, WORD roomSize, WCHAR * wChrRoNm);
//
//// �� ���� ���
//void MakePacketResRoomEnter(st_PACKET_HEADER * header, cPacketSerialz* packetSz, cPacketSerialz * subPacketSz);
//
//// �ٸ� �������� ���� �����ߴٴ� ������ ����
//void MakePacketResUserEnter(st_PACKET_HEADER * header, cPacketSerialz* packetSz, WCHAR nickName[15], UINT userNo);
//
//// �ٸ� ä�� �۽�
//void MakePacketResChat(st_PACKET_HEADER * header, cPacketSerialz * packetSz, UINT sendUserNo, WORD wMsgSize, WCHAR * wMsg);
//
//// �� ����
//void MakePacketResRoomLeave(st_PACKET_HEADER * header, cPacketSerialz * packetSz, UINT sendUserNo);
//
//// �� ����
//void MakePacketResRoomDelete(st_PACKET_HEADER * header, cPacketSerialz * packetSz, UINT roomNo);
//