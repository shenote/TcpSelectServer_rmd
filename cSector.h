#pragma once

// 케릭터의 현재 좌표 x,y로 섹터 위치를 계산하여 해당 섹터에 넣음
void Sector_AddCharacter(st_CHARACTER * pCharactr);

// 케릭터의 현재 좌표 x,y로 섹터를 계산하여 해당 섹터에서 삭제
void Sector_RemoveCharacter(st_CHARACTER * pCharacter);
	
// 위의 RemoveCharacter, AddCharacter 를 사용하여 현재 위치한 섹터에서 삭제 후 현재의 좌표로 섹터를 새롭게 계산하여 해당 섹터에 넣음
// 현재 위치한 섹터에서 삭제 후 현재의 좌표로 섹터를 새롭게 계산하여 해당 섹터에 넣음
bool Sector_UpdateCharacter(st_CHARACTER * pCharacter);
/*
캐릭터 객체의 맴버 변수에서 현재의 좌표 x,y 현재의 섹터 curSector 이전의 섹터 oldSector정보가 있따.
Sector_UpdateCharacet 함수는 픽셀좌표 x,y를 외부에서 갱신(이동처리) 한 뒤에 호출 하는 용도이다.
*/

// 특정 섹터 좌표 기준 주변 영향권 섹터 얻기.
void GetSectorAround(int iSectorX, int iSectorY, st_SECTOR_AROUND * pSectorArund);

// 섹터에서 섹터를 이동 하였을때 섹터 양향권에서 삐진 섹터, 새로 추가된 섹터의 정보 구하는 함수
void GetUpdateSectorAround(st_CHARACTER * pCharacter, st_SECTOR_AROUND * pRemoveSector, st_SECTOR_AROUND * pAddSector);

// Sector Update
void CharacterSectorUpdatePacket(st_CHARACTER * pCharacter);

