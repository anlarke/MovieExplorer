#ifndef __RCOMPTR_H__
#define __RCOMPTR_H__

template <class T>
class RComPtr
{
public:
	RComPtr() : p(NULL)
	{
	}

	~RComPtr()
	{
		Release();
	}

	HRESULT CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter = NULL, 
			DWORD dwClsContext = CLSCTX_INPROC_SERVER)
	{
		ASSERT(!p);
		return ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)&p);
	}

	void Release()
	{
		if (p)
		{
			p->Release();
			p = NULL;
		}
	}

	T* operator =(RComPtr<T>& t)
	{
		Release();
		p = t.p;
		t.p->AddRef();
		return p;
	}

	T* operator =(T *pT)
	{
		Release();
		p = pT;
		p->AddRef();
		return p;
	}

	T** operator &()
	{
		return &p;
	}

	T& operator *()
	{
		ASSERT(p);
		return *p;
	}

	T* operator ->()
	{
		ASSERT(p);
		return p;
	}

	operator T*()
	{
		return p;
	}

	T* p;
};

#endif // __RCOMPTR_H__
