#pragma once

// 케릭 생성
void MakePacket_CreateMyCharacter(cPacketSerialz * packetSz, DWORD id, BYTE dir, WORD x, WORD y, BYTE hp);

// 다른 케릭터 생성
void MakePacket_CreateOtherCharacter(cPacketSerialz * packetSz, DWORD id, BYTE dir, WORD x, WORD y, BYTE hp);

// 케릭터 삭제 요청
void MakePacket_DeleteOtherCharacter(cPacketSerialz * packetSz, DWORD id);

// 케릭터 이동
void MakePacket_MoveOtherCharacter(cPacketSerialz * packetSz, DWORD id, BYTE dir, WORD x, WORD y);

// 케릭터 이동 정지
void MakePacket_MoveStopOtherCharacter(cPacketSerialz * packetSz, DWORD id, BYTE dir, WORD x, WORD y);

// 케릭터 공격 1
void MakePacket_ATTACK1(cPacketSerialz * packetSz, DWORD id, BYTE dir, WORD x, WORD y);

// 케릭터 공격 2
void MakePacket_ATTACK2(cPacketSerialz * packetSz, DWORD id, BYTE dir, WORD x, WORD y);

// 케릭터 공격 3
void MakePacket_ATTACK3(cPacketSerialz * packetSz, DWORD id, BYTE dir, WORD x, WORD y);

// HP깍기
void MakePacket_DAMAGE(cPacketSerialz * packetSz, DWORD dwAttackID, DWORD dwDamageID, BYTE byDamageHP);

// 씬 패킷
void MakePacket_Syne(cPacketSerialz * packetSz, DWORD id, WORD x, WORD y);
//
//// 회원가입
//void MakePacket_ResAddAcount(cPacketSerialz * packetSz, UINT64 AccountNo);
//
//// 로그인
//void MakePacket_ResLogin(cPacketSerialz * packetSz, UINT64 AccountNo, WCHAR * wNickNm);
//
//// 회원리스트
//void MakePacket_ResAccountList(cPacketSerialz * packetSz, cPacketSerialz * packetPayload);
//
//// 친구요청
//void MakePacket_ResFriendRequest(cPacketSerialz * packetSz);
//
//// 내게 요청한 친구 목록 리스트
//void MakePacket_ResFriendReplyList(cPacketSerialz * packetSz, UINT count);
//
//// 내가 요청한 친구 목록 리스트
//void MakePacket_ResFriendRequestList(cPacketSerialz * packetSz, UINT count);
//
//// 친구 수락
//void MakePacket_ResFriendAgree(cPacketSerialz * packetSz, UINT64 friendAccountNo, BYTE btRet);
//
//// 친구 수락 거절
//void MakePacket_ResFriendDeny(cPacketSerialz * packetSz, UINT64 friendAccountNo, BYTE btRet);
//
//// 친구 목록 리스트
//void MakePacket_ResFriendList(cPacketSerialz * packetSz, UINT count);
//
//// 친구 관계 끊기
//void MakePacket_ResFriendRemove(cPacketSerialz * packetSz, UINT64 friendAccountNo, BYTE btRet);
//
//// 로그인 결과
//void MakePacketResLogin(st_PACKET_HEADER * header, cPacketSerialz * packetSz, BYTE btRest, UINT uiUserNumber);
//
//// 방 목록 리스트
//void MakePacketResRoomList(st_PACKET_HEADER * header, WORD wRoomCount, cPacketSerialz * packetSz, cPacketSerialz * subPacketSz);
//
//// 방 생성 요청 결과
//void MakePacketResRoomCreate(st_PACKET_HEADER * header, cPacketSerialz* packetSz, BYTE btResult, UINT roomNo, WORD roomSize, WCHAR * wChrRoNm);
//
//// 방 입장 결과
//void MakePacketResRoomEnter(st_PACKET_HEADER * header, cPacketSerialz* packetSz, cPacketSerialz * subPacketSz);
//
//// 다른 유저에게 내가 입장했다는 정보를 날림
//void MakePacketResUserEnter(st_PACKET_HEADER * header, cPacketSerialz* packetSz, WCHAR nickName[15], UINT userNo);
//
//// 다른 채팅 송신
//void MakePacketResChat(st_PACKET_HEADER * header, cPacketSerialz * packetSz, UINT sendUserNo, WORD wMsgSize, WCHAR * wMsg);
//
//// 방 퇴장
//void MakePacketResRoomLeave(st_PACKET_HEADER * header, cPacketSerialz * packetSz, UINT sendUserNo);
//
//// 방 삭제
//void MakePacketResRoomDelete(st_PACKET_HEADER * header, cPacketSerialz * packetSz, UINT roomNo);
//
