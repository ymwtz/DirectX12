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
	double mSecondsPerCount;	//ÿ��������Ӧ��ʱ��
	double mDeltaTime;			//��ǰ֡��ǰһ֡��ʱ����
								//�����ʱ��ָperformance counterֵ
	__int64 mBaseTime;			//����ʼ����ʱ��
	__int64 mPausedTime;		//��ͣ�����й���ʱ��
	__int64 mStopTime;			//����ֹͣʱ��
	__int64 mPrevTime;			//ǰһ֡��ʱ��
	__int64 mCurrTime;			//��ǰ֡��ʱ��

	bool mStopped;
};

#endif // GAMETIMER_H