//***************************************************************************************
// GameTimer.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#ifndef GAMETIMER_H
#define GAMETIMER_H

class GameTimer
{
public:
	GameTimer();

	float TotalTime()const; // in seconds
	float DeltaTime()const; // in seconds

	void Reset(); // Call before message loop.
	void Start(); // Call when unpaused.
	void Stop();  // Call when paused.
	void Tick();  // Call every frame.

private:
	double mSecondsPerCount;	//每个计数对应的时长
	double mDeltaTime;			//当前帧到前一帧的时间间隔
								//下面的时间指performance counter值
	__int64 mBaseTime;			//程序开始运行时间
	__int64 mPausedTime;		//暂停过程中过的时间
	__int64 mStopTime;			//程序停止时间
	__int64 mPrevTime;			//前一帧的时间
	__int64 mCurrTime;			//当前帧的时间

	bool mStopped;
};

#endif // GAMETIMER_H