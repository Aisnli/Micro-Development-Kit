// Signal.h: interface for the Signal class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MDK_SINGLE_H
#define MDK_SINGLE_H

/*
	�ź���
	Ч��
	���û���߳��ڵȴ�
	NotifyAll�ᶪʧ
	Notify���ᶪʧ

	windows linux���
	���NotifyAllʱ������һ���߳����ڵ�Wait��
	���NotifyAll��windows�¿��ܻᶪʧ
	(��Ϊwindows�µ�Wait�Ὣ�ź�����Ϊ�ޣ�Ȼ���ٵȴ���
	���NotifyAll֮�󣬱���̼߳���֮ǰ��
	���ڵ���Wait���߳����ȵõ�cpuʱ��Ƭ�����ź������ˣ���Notifyall��ʧ)

	��linux���ᶪʧ
	

*/
#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <semaphore.h>
#endif


namespace mdk
{
	
class Signal  
{
public:
	Signal();
	virtual ~Signal();

	bool Wait( unsigned long nMillSecond = (unsigned long)-1 );
	bool Notify();
	bool NotifyAll();
	
private:
#ifdef WIN32
	HANDLE m_signal;
#else
	int m_waitCount;
	sem_t m_signal;			
#endif
};

}//namespace mdk

#endif // MDK_SINGLE_H
