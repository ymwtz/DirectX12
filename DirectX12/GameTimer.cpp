#include "GameTimer.h"

////////////////////
//QueryPerformanceFrequency�������ص�ǰ��performance counterֵ�����Կ�����ǰʱ��
//performance counter�ֱ��ʺܸ�(<1us) 
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
//����ÿ֮֡���time elapsed
//��t = t[i] - t[i-1]
//ÿ֡������㦤t
//���ҰѦ�t fed into UpdateScen������
//���scene���Ի�����һ֡�����󾭹���ʱ��������
//Call every frame.
////////////////////////////
void GameTimer::Tick()
{
	if (mStopped) {
		mDeltaTime = 0.0;
		return;
	}

	//��õ�ǰʱ��
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	//��ǰ֡��ǰһ֮֡��ļ��
	mDeltaTime = (mCurrTime - mPrevTime)*mSecondsPerCount;

	//Ϊ��һ֡��׼��
	mPrevTime = mCurrTime;

	//ǿ��ʹ�Ǹ�
	//������������ڵ�ģʽ��ý��̱����䵽��һ��������ʱ��t�п����Ǹ���
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
//���شӵ���Reset��ʼ��ʱ����
//������ͣ��ʱ�䲻������
//////////////////
float GameTimer::TotalTime() const
{
	//����ӿ�ʼ��ֹͣ��ʱ�䣬������ͣʱ��
	if (mStopped) {
		return (float)(((mStopTime - mPausedTime) - mBaseTime)*mSecondsPerCount);
	}
	//����ӿ�ʼ����ǰ��ʱ�䣬������ͣʱ��
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
	//���Ѿ�����ͣ״̬�������κ���
	if (!mStopped) {
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		//��¼��ͣ��ʱ��㣬��������ͣ��־
		mStopTime = currTime;
		mStopped = true;
	}
}