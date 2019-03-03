#pragma once

// �ɸ����� ���� ��ǥ x,y�� ���� ��ġ�� ����Ͽ� �ش� ���Ϳ� ����
void Sector_AddCharacter(st_CHARACTER * pCharactr);

// �ɸ����� ���� ��ǥ x,y�� ���͸� ����Ͽ� �ش� ���Ϳ��� ����
void Sector_RemoveCharacter(st_CHARACTER * pCharacter);
	
// ���� RemoveCharacter, AddCharacter �� ����Ͽ� ���� ��ġ�� ���Ϳ��� ���� �� ������ ��ǥ�� ���͸� ���Ӱ� ����Ͽ� �ش� ���Ϳ� ����
// ���� ��ġ�� ���Ϳ��� ���� �� ������ ��ǥ�� ���͸� ���Ӱ� ����Ͽ� �ش� ���Ϳ� ����
bool Sector_UpdateCharacter(st_CHARACTER * pCharacter);
/*
ĳ���� ��ü�� �ɹ� �������� ������ ��ǥ x,y ������ ���� curSector ������ ���� oldSector������ �ֵ�.
Sector_UpdateCharacet �Լ��� �ȼ���ǥ x,y�� �ܺο��� ����(�̵�ó��) �� �ڿ� ȣ�� �ϴ� �뵵�̴�.
*/

// Ư�� ���� ��ǥ ���� �ֺ� ����� ���� ���.
void GetSectorAround(int iSectorX, int iSectorY, st_SECTOR_AROUND * pSectorArund);

// ���Ϳ��� ���͸� �̵� �Ͽ����� ���� ����ǿ��� ���� ����, ���� �߰��� ������ ���� ���ϴ� �Լ�
void GetUpdateSectorAround(st_CHARACTER * pCharacter, st_SECTOR_AROUND * pRemoveSector, st_SECTOR_AROUND * pAddSector);

// Sector Update
void CharacterSectorUpdatePacket(st_CHARACTER * pCharacter);

