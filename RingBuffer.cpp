#include "pch.h"
#include "RingBuffer.h"

RingBuffer::RingBuffer()
{
	_pBuffer = new char[eRingBuffer::MAX_BUF_SIZE];
	_iBufferSize = eRingBuffer::MAX_BUF_SIZE;
	_iRear = 0;
	_iFront = 0;
	_initCiritical = false;
}

RingBuffer::RingBuffer(int iBufferSize)
{
	_pBuffer = new char[iBufferSize];
	_iBufferSize = iBufferSize;
}

RingBuffer::~RingBuffer()
{
	delete[] _pBuffer;
	if (_initCiritical == true)
	{
		DeleteCriticalSection(&_cs);
	}
}

int RingBuffer::GetBufferSize(void)
{
	return _iBufferSize;
}

int RingBuffer::GetUseSize(void)
{
	// 사용 하고 있는 사이즈
	int useSize = 0;

	// 멀티 스레드 세이프 코드
	int rear = _iRear;
	int front = _iFront;

	if (rear > front)
	{
		useSize = rear - front;
	}
	else if (rear < front)
	{
		useSize = _iBufferSize - (front - rear);
	}

	return useSize;
}

int RingBuffer::GetFreeSize(void)
{
	int freeSize = 0;

	// 멀티 스레드 세이프 코드
	int rear = _iRear;
	int front = _iFront;

	if (rear < front)
		freeSize = (front - rear) - 1;
	else if (rear > front)
		freeSize = _iBufferSize - (rear - front) - 1;
	else
		freeSize = _iBufferSize - 1;

	return freeSize;
}

int RingBuffer::GetNotBrokenGetSize(void)
{
	// 끊어지지 않고 가져 올 수 있는 사이즈
	int getSize = 0;

	// 멀티 스레드 세이프 코드
	int rear = _iRear;
	int front = _iFront;

	if (rear > front)
		getSize = (rear - front);
	else if (rear < front)
		getSize = (_iBufferSize - front);

	return getSize;
}

int RingBuffer::GetNotBrokenPutSize(void)
{
	// 끊어지지 않고 넣을 수 있는 용량
	int putSize = 0;

	// 멀티 스레드 세이프 코드
	int rear = _iRear;
	int front = _iFront;

	if (rear < front)
		putSize = (front - rear);
	else if (rear > front)
		putSize = (_iBufferSize - rear);
	else
		putSize = _iBufferSize - rear;

	return putSize;
}

int RingBuffer::Enqueue(char * chpData, int iSize)
{
	int enqueueSize = 0;
	int getFreeSize = GetFreeSize();
	// 넣을 수 없는가?
	if (getFreeSize < iSize)
		return 0;

	// 끊어지지 않고 넣을 수 있는 공간은 얼마인가?
	int getBrokenPutSize = GetNotBrokenPutSize();

	if (getBrokenPutSize < 0)
		return 0;

	// 끊어지지 않고 넣을 수 있는 공간이 있는가? 
	if (getBrokenPutSize > iSize - 1)
	{
		memcpy(_pBuffer + _iRear, chpData, iSize);
		_iRear = (_iRear + iSize) % _iBufferSize;
		return iSize;
	}
	else
	{
		// 끊어 넣기.
		if (getBrokenPutSize != 0)
			memcpy(_pBuffer + _iRear, chpData, getBrokenPutSize);
		// 나머지 넣기 리어는 
		memcpy(_pBuffer + 0, chpData + getBrokenPutSize, iSize - getBrokenPutSize);
		_iRear = (_iRear + iSize) % _iBufferSize;

		enqueueSize = getBrokenPutSize + _iRear;
	}


	return enqueueSize;
}

int RingBuffer::Dequeue(char * chpData, int iSize)
{
	int getUseSize = GetUseSize();
	if (getUseSize < iSize)
		return 0;

	// 현재 프론트 에서 사이즈 만큼 빼는데, 브로큰에 걸려 있는가? 없는가?
	int dequeueSize = 0;
	int getBrokenGetSize = GetNotBrokenGetSize();

	if (getBrokenGetSize < 0)
		return 0;

	if (getBrokenGetSize > iSize - 1)
	{
		// 그냥 복사해서 가져옴 
		memcpy(chpData, _pBuffer + _iFront, iSize);
		_iFront = (_iFront + iSize) % _iBufferSize;
		dequeueSize = iSize;
	}
	else
	{
		// 끊어서 가져오기
		if (getBrokenGetSize != 0)
			memcpy(chpData, _pBuffer + _iFront, getBrokenGetSize);
		// 나머지 가져오기
		memcpy(chpData + getBrokenGetSize, _pBuffer + 0, iSize - getBrokenGetSize);
		_iFront = (_iFront + iSize) % _iBufferSize;

		dequeueSize = getBrokenGetSize + _iFront;
	}

	return dequeueSize;
}

// 복사해서 빼기
int RingBuffer::Peek(char * chpData, int iSize)
{
	if (GetUseSize() < iSize - 1)
		return 0;

	int peekSize = 0;
	int getBrokenGetSize = GetNotBrokenGetSize();

	if (getBrokenGetSize < 0)
		return 0;

	if (getBrokenGetSize > iSize - 1)
	{
		memcpy(chpData, _pBuffer + _iFront, iSize);
		peekSize = iSize;
	}
	else
	{
		// 끊어서 가져오기
		if (getBrokenGetSize != 0)
			memcpy(chpData, _pBuffer + _iFront, getBrokenGetSize);
		// 나머지 가져오기
		memcpy(chpData + getBrokenGetSize, _pBuffer + 0, iSize - getBrokenGetSize);

		peekSize = iSize;
	}

	return peekSize;
}

void RingBuffer::MoveRear(int iSize)
{
	// 수동으로 옮겨준다.
	_iRear = (_iRear + iSize) % _iBufferSize;
}

void RingBuffer::MoveFront(int iSize)
{
	// 수동으로 옮겨주고
	_iFront = (_iFront + iSize) % _iBufferSize;
}

void RingBuffer::ClearBuffer(void)
{
	_iRear = 0;
	_iFront = 0;
}

char * RingBuffer::GetFrontBufferPtr(void)
{
	return _pBuffer + _iFront;
}

char * RingBuffer::GetRearBufferPtr(void)
{
	return _pBuffer + _iRear;
}

void RingBuffer::Lock(void)
{
	if (_initCiritical == false)
	{
		InitializeCriticalSection(&_cs);
		_initCiritical = true;
	}

	EnterCriticalSection(&_cs);
}

void RingBuffer::Unlock(void)
{
	LeaveCriticalSection(&_cs);
}


void RingBuffer::ClearBuffer(void)
{
	_iRear = 0;
	_iFront = 0;
}

char * RingBuffer::GetFrontBufferPtr(void)
{
	return _pBuffer + _iFront;
}

char * RingBuffer::GetRearBufferPtr(void)
{
	return _pBuffer + _iRear;
}
