#ifndef _NE_SINGLETON_H_
#define _NE_SINGLETON_H_

#include "neuncopyable.h"
#include "nelockmutex.h"

template <class T>
class NESingleTon:public Uncopyable
{
public:

	static T & instance() 
	{
		if (!m_pInstance)
		{
			Locker lock(&m_mtx);
			if (!m_pInstance)
			{
				m_pInstance = new T();
			}
		}
		
		return *m_pInstance;
	};

protected:
	NESingleTon() {};
	~NESingleTon() {};

private:
	static T* volatile m_pInstance;
	static L_Mutex m_mtx;
};

template<class T>
T* volatile NESingleTon<T>::m_pInstance = nullptr;

template<class T>
L_Mutex NESingleTon<T>::m_mtx;

#endif