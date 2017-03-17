#include "GameTimer.h"

////////////////////
//QueryPerformanceFrequency函数返回当前的performance counter值，可以看作当前时间
//performance counter分辨率很高(<1us) 
////////////////////
GameTimer::GameTimer()
	: mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0),
	mPausedTime(0), mPrevTime(0), mCurrTime(0), mStopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}

/////////////////////////////
//计算每帧之间的time elapsed
//Δt = t[i] - t[i-1]
//每帧都会计算Δt
//并且把Δt fed into UpdateScen函数里
//如此scene可以基于上一帧动画后经过的时间来更新
//Call every frame.
////////////////////////////
void GameTimer::Tick()
{
	if (mStopped) {
		mDeltaTime = 0.0;
		return;
	}

	//获得当前时间
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	//当前帧和前一帧之间的间隔
	mDeltaTime = (mCurrTime - mPrevTime)*mSecondsPerCount;

	//为下一帧做准备
	mPrevTime = mCurrTime;

	//强制使非负
	//当处理器进入节电模式或该进程被分配到另一个处理器时Δt有可能是负的
	if (mDeltaTime < 0.0) {
		mDeltaTime = 0.0;
	}
}

////////////////
float GameTimer::DeltaTime() const
{
	return (float)mDeltaTime;
}

///////////
//Call before message loop.
//////////
void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

//////////////////
//返回从调用Reset开始的时间间隔
//其中暂停的时间不算在内
//////////////////
float GameTimer::TotalTime() const
{
	//计算从开始到停止的时间，不算暂停时间
	if (mStopped) {
		return (float)(((mStopTime - mPausedTime) - mBaseTime)*mSecondsPerCount);
	}
	//计算从开始到当前的时间，不算暂停时间
	else {
		return (float)(((mCurrTime - mPausedTime) - mBaseTime)*mSecondsPerCount);
	}

	return 0.0f;
}

//////////////////
//Call when unpaused.
//////////////////
void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)startTime);

	//Accumulate the time elapsed between stop and start pairs.
	if (mStopped) {
		mPausedTime += (startTime - mStopTime);

		//
		mPrevTime = startTime;

		//
		mStopTime = 0;
		mStopped = false;
	}
}

//////////////////
//Call when paused.
//////////////////
void GameTimer::Stop()
{
	//若已经是暂停状态，不做任何事
	if (!mStopped) {
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		//记录暂停的时间点，并设置暂停标志
		mStopTime = currTime;
		mStopped = true;
	}
}