#ifndef __RCRITICALSECTION_H__
#define __RCRITICALSECTION_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
// RCriticalSection

class RCriticalSection
{
public:
	RCriticalSection()
	{
		InitializeCriticalSection(&m_sect);
	}

	~RCriticalSection()
	{
		ASSERT(m_sect.LockCount == -1); // object should not be locked when destructed
		DeleteCriticalSection(&m_sect);
	}

	void Lock()
	{
		EnterCriticalSection(&m_sect);
	}

	void Unlock()
	{
		#ifdef _DEBUG
			if (!TryEnterCriticalSection(&m_sect)) 
			{
				ASSERT(false); // calling thread doesn't own the object
				return;
			}
			if (m_sect.RecursionCount == 1) 
			{
				LeaveCriticalSection(&m_sect);
				ASSERT(false); // critical section was never entered
				return;
			}
			LeaveCriticalSection(&m_sect); // because we called TryEnterCriticalSection
		#endif

		LeaveCriticalSection(&m_sect);
	}

	void LockRecursion(int nCount)
	{
		for (int n = 0; n < nCount; n++)
			Lock();
	}

	int UnlockRecursion()
	{
		int nCount = m_sect.RecursionCount;
		for (int n = 0; n < nCount; n++)
			Unlock();

		return nCount;
	}

protected:
	CRITICAL_SECTION m_sect;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// RLock

class RLock
{
public:
	RLock(RCriticalSection *pSect) : m_pSect(pSect)
	{
		m_pSect->Lock();
	}

	~RLock()
	{
		m_pSect->Unlock();
	}

protected:
	RCriticalSection* m_pSect;
};

///////////////////////////////////////////////////////////////////////////////
// REvent

class REvent
{
public:
	REvent(bool bManualReset = false, bool bInitialState = false, const TCHAR *lpszName = NULL, 
		   SECURITY_ATTRIBUTES *pAttributes = NULL) : m_hEvent(NULL)
	{
		Create(bManualReset, bInitialState, lpszName, pAttributes);
	}

	~REvent()
	{
		Destroy();
	}

	void Destroy()
	{
		if (m_hEvent)
		{
			CloseHandle(m_hEvent);
			m_hEvent = NULL;
		}
	}

	bool Create(bool bManualReset = false, bool bInitialState = false, const TCHAR *lpszName = NULL, 
				SECURITY_ATTRIBUTES *pAttributes = NULL)
	{
		Destroy();
		return (m_hEvent = CreateEvent(pAttributes, bManualReset, bInitialState, lpszName)) != NULL;
	}

	bool SetEvent()
	{
		ASSERT(m_hEvent);
		return ::SetEvent(m_hEvent) != FALSE;
	}

	bool ResetEvent()
	{
		ASSERT(m_hEvent);
		return ::ResetEvent(m_hEvent) != FALSE;
	}

	operator HANDLE()
	{
		return m_hEvent;
	}

protected:
	HANDLE m_hEvent;
};

#endif // __RCRITICALSECTION_H__

















/*
///////////////////////////////////////////////////////////////////////////////
// RCriticalSection

//#pragma pack(push, 4)

class RCriticalSection
{
	friend class RLock;
public:
	RCriticalSection() : m_dwOwner(0)
	{
		InitializeCriticalSection(&m_cs);
	}

	virtual ~RCriticalSection()
	{
		ASSERT(m_dwOwner == 0); // section should be unlocked at destruction
		DeleteCriticalSection(&m_cs);
	}

protected:
	bool Lock()
	{
		DWORD dwCurrent = GetCurrentThreadId();
		DWORD dwOwner = (DWORD)InterlockedCompareExchange((PVOID*)&m_dwOwner, 0, 0);
		
		if (dwOwner == dwCurrent)
			return false; // don't enter the critical section again when this thread already got ownership

		EnterCriticalSection(&m_cs);
		InterlockedExchange((LPLONG)&m_dwOwner, (LONG)dwCurrent);
		
		return true;
	}

	void Unlock()
	{
		DWORD dwCurrent = GetCurrentThreadId();
		DWORD dwOwner = (DWORD)InterlockedCompareExchange((PVOID*)&m_dwOwner, 0, 0);

		if (dwOwner == dwCurrent)
		{
			InterlockedExchange((LPLONG)&m_dwOwner, 0);
			LeaveCriticalSection(&m_cs);
			return;
		}

		ASSERT(false); // critical section was never locked, or the calling thread didn't lock it
	}

	volatile DWORD m_dwOwner;
	CRITICAL_SECTION m_cs;
};

//#pragma pack(pop)

///////////////////////////////////////////////////////////////////////////////
// RLock

class RLock
{
public:
	RLock(RCriticalSection* pSect) : m_pSect(NULL)
	{
		if (pSect->Lock())
			m_pSect = pSect;
	}

	virtual ~RLock()
	{
		if (m_pSect)
			m_pSect->Unlock();
	}

	void Lock()
	{
		if (m_pSect)
			if (!m_pSect->Lock())
				m_pSect = NULL;
	}

	void Unlock()
	{
		if (m_pSect)
			m_pSect->Unlock();
	}

protected:
	RCriticalSection* m_pSect;
};
*/