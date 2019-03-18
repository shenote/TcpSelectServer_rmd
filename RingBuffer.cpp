#include "pch.h"
#include "RingBuffer.h"

RingBuffer::RingBuffer()
{
	_pBuffer = new char[eRingBuffer::MAX_BUF_SIZE];
	_iBufferSize = eRingBuffer::MAX_BUF_SIZE;
	_iRear = 0;
	_iFront = 0;
}

RingBuffer::RingBuffer(int iBufferSize)
{
	_pBuffer = new char[iBufferSize];
	_iBufferSize = iBufferSize;
}

RingBuffer::~RingBuffer()
{
	delete[] _pBuffer;
}

unsigned int RingBuffer::GetBufferSize(void)
{
	return _iBufferSize;
}

unsigned int RingBuffer::GetUseSize(void)
{
	// ��� �ϰ� �ִ� ������
	int useSize = 0;
	if (_iRear > _iFront)
	{
		useSize = _iRear - _iFront;
	}
	else if (_iRear < _iFront)
	{
		useSize = _iBufferSize - (_iFront - _iRear);
	}

	return useSize;
}

unsigned int RingBuffer::GetFreeSize(void)
{
	int freeSize = 0;

	if (_iRear < _iFront)
	{
		// ��� ����Ʈ ���� �۴ٸ�, ��� �ѹ��� ���� �ӵ��°���
		freeSize = (_iFront - _iRear) - 1;
	}
	else if (_iRear > _iFront)
	{
		// ��ü - (���� - ����Ʈ);
		freeSize = _iBufferSize - (_iRear - _iFront) - 1;
	}
	else
	{
		// ��ġ�Ҷ�
		freeSize = _iBufferSize - 1;
	}

	return freeSize;
}

unsigned int RingBuffer::GetNotBrokenGetSize(void)
{
	// �������� �ʰ� ���� �� �� �ִ� ������
	int getSize = 0;

	if (_iRear > _iFront)
		getSize = (_iRear - _iFront);
	else if (_iRear < _iFront)
		getSize = (_iBufferSize - _iFront);

	return getSize;
}

unsigned int RingBuffer::GetNotBrokenPutSize(void)
{
	// �������� �ʰ� ���� �� �ִ� �뷮
	int putSize = 0;

	if (_iRear < _iFront)
		putSize = (_iFront - _iRear);
	else if (_iRear > _iFront)
		putSize = (_iBufferSize - _iRear);
	else
		putSize = _iBufferSize - _iRear;

	return putSize;
}

unsigned int RingBuffer::Enqueue(char * chpData, unsigned int iSize)
{
	unsigned int enqueueSize = 0;
	unsigned int getFreeSize = GetFreeSize();
	// ���� �� ���°�?
	if (getFreeSize < iSize)
		return 0;

	// �������� �ʰ� ���� �� �ִ� ������ ���ΰ�?
	unsigned int getBrokenPutSize = GetNotBrokenPutSize();

	// �������� �ʰ� ���� �� �ִ� ������ �ִ°�? 
	if (getBrokenPutSize > iSize - 1)
	{
		memcpy(_pBuffer + _iRear, chpData, iSize);
		_iRear = (_iRear + iSize) % _iBufferSize;
		return iSize;
	}
	else
	{
		// ���� �ֱ�.
		if (getBrokenPutSize != 0)
			memcpy(_pBuffer + _iRear, chpData, getBrokenPutSize);
		// ������ �ֱ� ����� 
		memcpy(_pBuffer + 0, chpData + getBrokenPutSize, iSize - getBrokenPutSize);
		_iRear = (_iRear + iSize) % _iBufferSize;

		enqueueSize = getBrokenPutSize + _iRear;
	}


	return enqueueSize;
}

unsigned int RingBuffer::Dequeue(char * chpData, unsigned int iSize)
{
	// ���� ����Ʈ ���� ������ ��ŭ ���µ�, ���ū�� �ɷ� �ִ°�? ���°�?
	unsigned int dequeueSize = 0;
	unsigned int getBrokenGetSize = GetNotBrokenGetSize();
	if (getBrokenGetSize > iSize - 1)
	{
		// �׳� �����ؼ� ������ 
		memcpy(chpData, _pBuffer + _iFront, iSize);
		_iFront = (_iFront + iSize) % _iBufferSize;
		dequeueSize = iSize;
	}
	else
	{
		// ��� ��������
		if (getBrokenGetSize != 0)
			memcpy(chpData, _pBuffer + _iFront, getBrokenGetSize);
		// ������ ��������
		memcpy(chpData + getBrokenGetSize, _pBuffer + 0, iSize - getBrokenGetSize);
		_iFront = (_iFront + iSize) % _iBufferSize;

		dequeueSize = getBrokenGetSize + _iFront;
	}

	return dequeueSize;
}

// �����ؼ� ����
unsigned int RingBuffer::Peek(char * chpData, unsigned int iSize)
{
	if (GetUseSize() < iSize - 1)
		return 0;

	unsigned int peekSize = 0;
	unsigned int getBrokenGetSize = GetNotBrokenGetSize();
	if (getBrokenGetSize > iSize - 1)
	{
		memcpy(chpData, _pBuffer + _iFront, iSize);
		peekSize = iSize;
	}
	else
	{
		// ��� ��������
		if (getBrokenGetSize != 0)
			memcpy(chpData, _pBuffer + _iFront, getBrokenGetSize);
		// ������ ��������
		memcpy(chpData + getBrokenGetSize, _pBuffer + 0, iSize - getBrokenGetSize);

		peekSize = iSize;
	}

	return peekSize;
}

void RingBuffer::MoveRear(int iSize)
{
	// �������� �Ű��ش�.
	_iRear = (_iRear + iSize) % _iBufferSize;
}

unsigned int RingBuffer::MoveFront(unsigned int iSize)
{
	// �������� �Ű��ְ�
	_iFront = (_iFront + iSize) % _iBufferSize;
	return 0;
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
