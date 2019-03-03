#include "pch.h"
#include "timer.h"
#include <mmsystem.h>

//�����찡 �������� �ð��� �˷��ִ� API
#pragma comment(lib, "winmm.lib")

timer::timer()
{
}


timer::~timer()
{
}

HRESULT timer::init(float lockFPS)
{

	timeBeginPeriod(1);
	//���� Ÿ�̸Ӹ� �����ϴ��� ���� üũ
	//���� ������ �ȵǸ� ������ �и��������� ����(TIMEGETTIME)
	if (QueryPerformanceFrequency((LARGE_INTEGER*)&_periodFrequency))
	{
		_isHardware = true;
		QueryPerformanceCounter((LARGE_INTEGER*)&_lastTime);

		_timeScale = 1.0f / _periodFrequency;
	}
	else
	{
		//���� Ÿ�̸Ӹ� �������� ������
		_isHardware = false;

		//������ �и������� ���·� �����
		_lastTime = timeGetTime();
		_timeScale = 0.001f;
	}

	_frameRate = 0;
	_FPSFrameCount = 0;
	_FPSTimeElapsed = 0;
	_frameRenderRate = 0;
	_FPSRenderCount = 0;
	_timeElapsed = 0;
	_worldTime = 0;

	_LockFPS = 1000 / lockFPS;
	_timeMsTo = _LockFPS / 1000;

	// ���� ��ȸ�� ����� �ð�
	_dwElapsedTicks = 0;
	_dwLastTicks = timeGetTime(); //���� ���� ��ȸ�� ���� �ð�
	_dwCurTicks = 0;

	return S_OK;
}

bool timer::tick()
{
	if (_isHardware)
		QueryPerformanceCounter((LARGE_INTEGER*)&_curTime);
	else
		_curTime = timeGetTime();

	// �ð� ����
	_timeElapsed += (_curTime - _lastTime) * _timeScale;

	//_dwCurTicks = timeGetTime();
	bool bSkip = false;
	// ���� ������ ����
	//_dwElapsedTicks += (_dwCurTicks - _dwLastTicks);

	if (_timeElapsed <= _timeMsTo)
	{
		_FPSRenderCount++;
		bSkip = true;
	}
	else
	{
		_timeElapsed -= _timeMsTo;
		//_dwElapsedTicks -= _LockFPS;
	}

	//_dwLastTicks = timeGetTime();
	_lastTime = _curTime;
	return bSkip;
}

unsigned long timer::getFrameRate() const
{
	return _frameRate;
}

unsigned long timer::getRenderRate() const
{
	return _frameRenderRate;
}