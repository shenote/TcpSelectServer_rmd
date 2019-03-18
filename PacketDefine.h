#pragma once
#include <winsock.h>

#ifndef __PROTOCOL_
#define __PROTOCOL__


#define dfNETWORK_PORT 20000
#define dfPACKET_CODE 0x89
#define dfPACKET_END_CODE 0x79

struct st_PACKET_HEADER
{
	BYTE	byCode;			// ��Ŷ�ڵ� 0x89 ����.
	BYTE	bySize;			// ��Ŷ ������.
	BYTE	byType;			// ��ŶŸ��.
	BYTE	byTemp;			// ������.
};

//---------------------------------------------------------------
// ��Ŷ�� ���� �տ� �� ��Ŷ�ڵ�.
//---------------------------------------------------------------
#define dfNETWORK_PACKET_CODE	((BYTE)0x89)
//---------------------------------------------------------------
// ��Ŷ�� ���� �ڿ� �� ��Ŷ�ڵ�.
//---------------------------------------------------------------
#define dfNETWORK_PACKET_END	((BYTE)0x79)
//---------------------------------------------------------------
// ��Ŷ�� �� �κп��� 1Byte �� EndCode �� ���Եȴ�.  0x79
//
//---------------------------------------------------------------


#define	dfPACKET_SC_CREATE_MY_CHARACTER			0
//---------------------------------------------------------------
// 0 - Ŭ���̾�Ʈ �ڽ��� ĳ���� �Ҵ�		Server -> Client
//
// ������ ���ӽ� ���ʷ� �ްԵǴ� ��Ŷ���� �ڽ��� �Ҵ���� ID ��
// �ڽ��� ���� ��ġ, HP �� �ް� �ȴ�. (ó���� �ѹ� �ް� ��)
// 
// �� ��Ŷ�� ������ �ڽ��� ID,X,Y,HP �� �����ϰ� ĳ���͸� �������Ѿ� �Ѵ�.
//
//	4	-	ID
//	1	-	Direction
//	2	-	X
//	2	-	Y
//	1	-	HP
//
//---------------------------------------------------------------


#pragma pack(push, 1)

struct stPACKET_SC_CREATE_MY_CHARACTER
{
	DWORD	ID;
	BYTE	Direction;
	WORD	X;
	WORD	Y;
	BYTE	HP;
};

struct st_SESSION
{
	SOCKET socket;
	DWORD dwSessionID;		// �������� ���� ���� ID
	SOCKADDR_IN sockAddr;
	RingBuffer sendQ;
	RingBuffer recvQ;
	DWORD dwHartBit;		// ��Ʈ��Ʈ �������� tcpip 3�������� ���ִ� keppAlive ������ �νð����� üũ�ؼ� �ǹ̰� ����.
};

struct st_SECTOR_POS
{
	int iX;
	int iY;
};

struct st_SECTOR_AROUND
{
	int iCount;
	st_SECTOR_POS around[9];
};

struct st_CHARACTER
{
	DWORD dwClientNo;
	BYTE byDirection;
	WORD iX;
	WORD iY;
	BYTE byHp;

	st_SESSION * pSession;
	DWORD dwAction;
	DWORD dwActionTick;

	WORD iActionX;
	WORD iActionY;

	st_SECTOR_POS curSector;
	st_SECTOR_POS oldSector;

	BYTE byArrowDirection;		// �¿� �ٷκ��� ����

	//st_SECTOR_AROUND around;

};

#pragma pack(pop, 0)

#define	dfPACKET_SC_CREATE_OTHER_CHARACTER		1
//---------------------------------------------------------------
// 1. �ٸ� Ŭ���̾�Ʈ�� ĳ���� ���� ��Ŷ		Server -> Client
//
// ó�� ������ ���ӽ� �̹� ���ӵǾ� �ִ� ĳ���͵��� ����
// �Ǵ� �����߿� ���ӵ� Ŭ���̾�Ʈ���� ���� �� ����.
//
//
//	4	-	ID
//	1	-	Direction
//	2	-	X
//	2	-	Y
//	1	-	HP
//
//---------------------------------------------------------------

#pragma pack(push, 1)

struct stPACKET_SC_CREATE_OTHER_CHARACTER
{
	DWORD	ID;
	BYTE	Direction;
	WORD	X;
	WORD	Y;
	BYTE	HP;
};

#pragma pack(pop, 0)

#define	dfPACKET_SC_DELETE_CHARACTER			2
//---------------------------------------------------------------
// 2. ĳ���� ���� ��Ŷ						Server -> Client
//
// ĳ������ �������� �Ǵ� ĳ���Ͱ� �׾����� ���۵�.
//
//	4	-	ID
//
//---------------------------------------------------------------



#define	dfPACKET_CS_MOVE_START					10
//---------------------------------------------------------------
// 10. ĳ���� �̵����� ��Ŷ						Client -> Server
//
// �ڽ��� ĳ���� �̵����۽� �� ��Ŷ�� ������.
// �̵� �߿��� �� ��Ŷ�� ������ ������, Ű �Է��� ����Ǿ��� ��쿡��
// ������� �Ѵ�.
//
// (���� �̵��� ���� �̵� / ���� �̵��� ���� ���� �̵�... ���)
//
//	1	-	Direction	( ���� ������ �� 8���� ��� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------

#pragma pack(push, 1)

struct stPACKET_CS_MOVE_START
{
	BYTE	Direction;
	WORD	X;
	WORD	Y;
};

#pragma pack(pop, 0)

#define dfPACKET_MOVE_DIR_LL					0
#define dfPACKET_MOVE_DIR_LU					1
#define dfPACKET_MOVE_DIR_UU					2
#define dfPACKET_MOVE_DIR_RU					3
#define dfPACKET_MOVE_DIR_RR					4
#define dfPACKET_MOVE_DIR_RD					5
#define dfPACKET_MOVE_DIR_DD					6
#define dfPACKET_MOVE_DIR_LD					7



#define	dfPACKET_SC_MOVE_START					11
//---------------------------------------------------------------
// ĳ���� �̵����� ��Ŷ						Server -> Client
//
// �ٸ� ������ ĳ���� �̵��� �� ��Ŷ�� �޴´�.
// ��Ŷ ���Ž� �ش� ĳ���͸� ã�� �̵�ó���� ���ֵ��� �Ѵ�.
// 
// ��Ŷ ���� �� �ش� Ű�� ����ؼ� ���������� �����ϰ�
// �ش� �������� ��� �̵��� �ϰ� �־�߸� �Ѵ�.
//
//	4	-	ID
//	1	-	Direction	( ���� ������ �� 8���� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------

#pragma pack(push, 1)

struct stPACKET_SC_MOVE_START
{
	DWORD	id;
	BYTE	Direction;
	WORD	X;
	WORD	Y;
};

#pragma pack(pop, 0)


#define	dfPACKET_CS_MOVE_STOP					12
//---------------------------------------------------------------
// ĳ���� �̵����� ��Ŷ						Client -> Server
//
// �̵��� Ű���� �Է��� ��� �����Ǿ��� ��, �� ��Ŷ�� ������ �����ش�.
//
//	1	-	Direction	( ���� ������ �� ��/�츸 ��� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------


#define	dfPACKET_SC_MOVE_STOP					13
//---------------------------------------------------------------
// ĳ���� �̵����� ��Ŷ						Server -> Client
//
// ID �� �ش��ϴ� ĳ���Ͱ� �̵��� ������̹Ƿ� 
// ĳ���͸� ã�Ƽ� �����, ��ǥ�� �Է����ְ� ���ߵ��� ó���Ѵ�.
//
//	4	-	ID
//	1	-	Direction	( ���� ������ ��. ��/�츸 ��� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------

#pragma pack(push, 1)

struct stPACKET_SC_MOVE_STOP
{
	DWORD	id;
	BYTE	Direction;
	WORD	X;
	WORD	Y;
};

#pragma pack(pop, 0)

#define	dfPACKET_CS_ATTACK1						20
//---------------------------------------------------------------
// ĳ���� ���� ��Ŷ							Client -> Server
//
// ���� Ű �Է½� �� ��Ŷ�� �������� ������.
// �浹 �� �������� ���� ����� �������� �˷� �� ���̴�.
//
// ���� ���� ���۽� �ѹ��� �������� ������� �Ѵ�.
//
//	1	-	Direction	( ���� ������ ��. ��/�츸 ��� )
//	2	-	X
//	2	-	Y	
//
//---------------------------------------------------------------

#define	dfPACKET_SC_ATTACK1						21
//---------------------------------------------------------------
// ĳ���� ���� ��Ŷ							Server -> Client
//
// ��Ŷ ���Ž� �ش� ĳ���͸� ã�Ƽ� ����1�� �������� �׼��� �����ش�.
// ������ �ٸ� ��쿡�� �ش� �������� �ٲ� �� ���ش�.
//
//	4	-	ID
//	1	-	Direction	( ���� ������ ��. ��/�츸 ��� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------

#pragma pack(push, 1)

struct stPACKET_SC_ATTACK1
{
	DWORD	id;
	BYTE	Direction;
	WORD	X;
	WORD	Y;
};

#pragma pack(pop, 0)


#define	dfPACKET_CS_ATTACK2						22
//---------------------------------------------------------------
// ĳ���� ���� ��Ŷ							Client -> Server
//
// ���� Ű �Է½� �� ��Ŷ�� �������� ������.
// �浹 �� �������� ���� ����� �������� �˷� �� ���̴�.
//
// ���� ���� ���۽� �ѹ��� �������� ������� �Ѵ�.
//
//	1	-	Direction	( ���� ������ ��. ��/�츸 ��� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------


#define	dfPACKET_SC_ATTACK2						23
//---------------------------------------------------------------
// ĳ���� ���� ��Ŷ							Server -> Client
//
// ��Ŷ ���Ž� �ش� ĳ���͸� ã�Ƽ� ����2�� �������� �׼��� �����ش�.
// ������ �ٸ� ��쿡�� �ش� �������� �ٲ� �� ���ش�.
//
//	4	-	ID
//	1	-	Direction	( ���� ������ ��. ��/�츸 ��� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------

#pragma pack(push, 1)

struct stPACKET_SC_ATTACK2
{
	DWORD	id;
	BYTE	Direction;
	WORD	X;
	WORD	Y;
};

#pragma pack(pop, 0)

#define	dfPACKET_CS_ATTACK3						24
//---------------------------------------------------------------
// ĳ���� ���� ��Ŷ							Client -> Server
//
// ���� Ű �Է½� �� ��Ŷ�� �������� ������.
// �浹 �� �������� ���� ����� �������� �˷� �� ���̴�.
//
// ���� ���� ���۽� �ѹ��� �������� ������� �Ѵ�.
//
//	1	-	Direction	( ���� ������ ��. ��/�츸 ��� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------

#define	dfPACKET_SC_ATTACK3						25
//---------------------------------------------------------------
// ĳ���� ���� ��Ŷ							Server -> Client
//
// ��Ŷ ���Ž� �ش� ĳ���͸� ã�Ƽ� ����3�� �������� �׼��� �����ش�.
// ������ �ٸ� ��쿡�� �ش� �������� �ٲ� �� ���ش�.
//
//	4	-	ID
//	1	-	Direction	( ���� ������ ��. ��/�츸 ��� )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------

#pragma pack(push, 1)

struct stPACKET_SC_ATTACK3
{
	DWORD	id;
	BYTE	Direction;
	WORD	X;
	WORD	Y;
};

#pragma pack(pop, 0)

#define	dfPACKET_SC_DAMAGE						30
//---------------------------------------------------------------
// ĳ���� ������ ��Ŷ							Server -> Client
//
// ���ݿ� ���� ĳ������ ������ ����.
//
//	4	-	AttackID	( ������ ID )
//	4	-	DamageID	( ������ ID )
//	1	-	DamageHP	( ������ HP )
//
//---------------------------------------------------------------

#pragma pack(push, 1)

struct stPACKET_SC_DAMAGE
{
	DWORD	AttackID;
	DWORD	DamageID;
	BYTE	DamageHP;
};

#pragma pack(pop, 0)



#define	dfPACKET_SC_SYNC						251
//---------------------------------------------------------------
// ����ȭ�� ���� ��Ŷ					Server -> Client
//
// �����κ��� ����ȭ ��Ŷ�� ������ �ش� ĳ���͸� ã�Ƽ�
// ĳ���� ��ǥ�� �������ش�.
//
//	4	-	ID
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------

#pragma pack(push, 1)

struct stPACKET_SC_SYNC
{
	DWORD	id;
	WORD	X;
	WORD	Y;
};

#pragma pack(pop, 0)


#define	dfPACKET_CS_ECHO						252
//---------------------------------------------------------------
// Echo �� ��Ŷ					Client -> Server
//
//	4	-	Time
//
//---------------------------------------------------------------

#define	dfPACKET_SC_ECHO						253
//---------------------------------------------------------------
// Echo ���� ��Ŷ				Server -> Client
//
//	4	-	Time
//
//---------------------------------------------------------------

#endif
