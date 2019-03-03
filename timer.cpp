#include "pch.h"
#include "timer.h"
#include <mmsystem.h>

//윈도우가 실행중인 시간을 알려주는 API
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
	//고성능 타이머를 지원하는지 여부 체크
	//만약 지원이 안되면 강제로 밀리세컨으로 맞춤(TIMEGETTIME)
	if (QueryPerformanceFrequency((LARGE_INTEGER*)&_periodFrequency))
	{
		_isHardware = true;
		QueryPerformanceCounter((LARGE_INTEGER*)&_lastTime);

		_timeScale = 1.0f / _periodFrequency;
	}
	else
	{
		//고성능 타이머를 지원하지 않을땐
		_isHardware = false;

		//강제로 밀리세컨드 형태로 맞춘다
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

	// 루프 순회간 경과된 시간
	_dwElapsedTicks = 0;
	_dwLastTicks = timeGetTime(); //이전 루프 순회가 끝난 시간
	_dwCurTicks = 0;

	return S_OK;
}

bool timer::tick()
{
	if (_isHardware)
		QueryPerformanceCounter((LARGE_INTEGER*)&_curTime);
	else
		_curTime = timeGetTime();

	// 시간 측정
	_timeElapsed += (_curTime - _lastTime) * _timeScale;

	//_dwCurTicks = timeGetTime();
	bool bSkip = false;
	// 게임 프레임 측정
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