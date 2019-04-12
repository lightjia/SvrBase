#ifndef __SINGLETON__H_
#define __SINGLETON__H_
#include "UvMutex.h"

#define SINGLE_CLASS_INITIAL(TypeName)	private:\
	TypeName();\
    TypeName(const TypeName&);\
    TypeName& operator=(const TypeName&);\
	friend class CSingleton<TypeName>

#define   DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);                \
	TypeName& operator=(const TypeName&)

template <typename T>
class CSingleton
{
public:
	static T* Instance();
	virtual ~CSingleton(){};

protected:
	CSingleton(){};

private:
	class CGarbo   //它的唯一工作就是在析构函数中删除CSingleton的实例  
	{
	public:
		~CGarbo()
		{
			if (NULL != CSingleton::mpInstance)
			{
				DODELETE(CSingleton::mpInstance);
			}
				
		}
	};

private:
	DISALLOW_COPY_AND_ASSIGN(CSingleton);
	static T* mpInstance;
	static CGarbo mcGarbo;  //定义一个静态成员变量，程序结束时，系统会自动调用它的析构函数  
	static CUvMutex mcStaticMutex;
};

template <typename T>
T* CSingleton<T>::mpInstance = NULL;

template <typename T>
CUvMutex CSingleton<T>::mcStaticMutex;

//template <typename T>
//CSingleton<typename>::CGarbo CSingleton<T>::mcGarbo;

template <typename T>
T* CSingleton<T>::Instance()
{
	if (NULL == mpInstance)
	{
		mcStaticMutex.Lock();
		while (NULL == mpInstance)
		{
			mpInstance = new T();
		}
		mcStaticMutex.UnLock();
	}

	return mpInstance;
}
#endif