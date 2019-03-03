#pragma once

class timer
{
private:
	bool _isHardware;
	float _timeScale;
	float _timeElapsed;
	__int64 _curTime;
	__int64 _lastTime;
	__int64 _periodFrequency;
	float _LockFPS;
	float _timeMsTo;

	float _dwElapsedTicks;
	DWORD _dwLastTicks;
	DWORD _dwCurTicks;

	unsigned long _frameRate;
	unsigned long _FPSFrameCount;
	unsigned long _frameRenderRate;
	unsigned long _FPSRenderCount;
	float _FPSTimeElapsed;
	float _worldTime;

public:
	HRESULT init(float lockFPS);
	bool tick();
	unsigned long getFrameRate() const;
	inline float getElapsedTime(void) const { return _timeElapsed; }
	inline float getWorldTime(void) const { return _worldTime; }
	unsigned long getRenderRate() const;

	timer();
	~timer();
};

