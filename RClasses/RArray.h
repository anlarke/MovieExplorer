#ifndef __RARRAY_H__
#define __RARRAY_H__

inline void memswap(BYTE *p1, BYTE *p2, size_t size)
{
	BYTE temp;
	for (size_t i = 0; i < size; ++i, ++p1, ++p2)
	{
		temp = *p1;
		*p1 = *p2;
		*p2 = temp;
	}
}

inline void memswap(void *p1, void *p2, size_t size)
	{memswap((BYTE*)p1, (BYTE*)p2, size);}

///////////////////////////////////////////////////////////////////////////////////////////////////
// RArray

template <class T>
class RArray
{
public:
	RArray() : m_pTs(NULL), m_nSize(0)
	{
	}

	RArray(INT_PTR nSize) : m_pTs(NULL), m_nSize(0)
	{
		SetSize(nSize);
	}

	RArray(INT_PTR nSize, const T& initVal) : m_pTs(NULL), m_nSize(0)
	{
		SetSize(nSize, initVal);
	}

	RArray(const RArray<T>& arr) : m_pTs(NULL), m_nSize(0)
	{
		operator=(arr);
	}

#if _MSC_VER >= 1600
	RArray(RArray<T>&& arr) : m_pTs(arr.m_pTs), m_nSize(arr.m_nSize)
	{
		arr.m_pTs = NULL;
		arr.m_nSize = 0;
	}
#endif

	~RArray()
	{
		free(m_pTs);
	}

	void SetSize(INT_PTR nSize)
	{
		ASSERT(nSize >= 0);

		if (nSize != m_nSize)
		{
			if (nSize > 0)
			{
				m_pTs = (T*)realloc(m_pTs, nSize * sizeof(T));
				m_nSize = nSize;
			}
			else
			{
				free(m_pTs);
				m_pTs = NULL;
				m_nSize = 0;
			}
		}
	}

	void SetSize(INT_PTR nSize, const T& initVal)
	{
		SetSize(nSize);
		for (INT_PTR i = 0; i < nSize; ++i)
			m_pTs[i] = initVal;
	}

	RArray<T>& operator=(const RArray<T>& arr)
	{
		SetSize(arr.m_nSize);
		memcpy(m_pTs, arr.m_pTs, m_nSize * sizeof(T));
		return *this;
	}

#if _MSC_VER >= 1600
	RArray<T>& operator =(RArray<T>&& arr)
	{
		if (&arr != this)
		{
			free(m_pTs);
			m_pTs = arr.m_pTs;
			m_nSize = arr.m_nSize;
			arr.m_pTs = NULL;
			arr.m_nSize = 0;
		}
		return *this;
	}
#endif

	INT_PTR Add(const RArray<T>& arr)
	{
		INT_PTR nIndex = m_nSize;
		SetSize(m_nSize + arr.GetSize());
		memcpy(m_pTs + nIndex, arr.m_pTs, arr.GetSize() * sizeof(T));
		return nIndex;
	}

	bool InsertAt(INT_PTR nIndex, const T& element)
	{
		if (nIndex < 0 || nIndex > m_nSize)
			ASSERTRETURN(false);

		if (nIndex == m_nSize)
		{
			Add(element);
			return true;
		}

		//SetSize(m_nSize + 1);
		//memmove(m_pTs + nIndex + 1, m_pTs + nIndex, (m_nSize - nIndex - 1) * sizeof(T));
		
		ASSERT(m_pTs);
		T *pTs = (T*)malloc((m_nSize + 1) * sizeof(T));
		ASSERT(pTs);
		memcpy(pTs, m_pTs, nIndex * sizeof(T));
		memcpy(pTs + nIndex + 1, m_pTs + nIndex, (m_nSize - nIndex) * sizeof(T));
		free(m_pTs);
		m_pTs = pTs;
		++m_nSize;

		m_pTs[nIndex] = element;
		
		return true;
	}

	bool RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1)
	{
		if (nIndex < 0 || nCount < 1)
			ASSERTRETURN(false);

		if (nIndex + nCount > m_nSize)
			ASSERTRETURN(false);

		memmove(m_pTs + nIndex, m_pTs + nIndex + nCount, (m_nSize - nIndex - nCount) * sizeof(T));
		SetSize(m_nSize - nCount);

		return true;
	}

	INT_PTR Remove(const T& element)
	{
		INT_PTR nCount = 0;
		for (INT_PTR i = 0; i < m_nSize; ++i)
			if (m_pTs[i] == element)
				{RemoveAt(i); ++nCount; --i;}
		return nCount;
	}

	#include "RArray_common.h"

protected:
	T *m_pTs;
	INT_PTR m_nSize;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// RObArray

template <class T, class INT_PTR = INT_PTR>
class RObArray
{
public:
	RObArray() : m_pTs(NULL), m_nSize(0)
	{
	}

	RObArray(INT_PTR nSize) : m_pTs(NULL), m_nSize(0)
	{
		SetSize(nSize);
	}

	RObArray(INT_PTR nSize, const T& initVal) : m_pTs(NULL), m_nSize(0)
	{
		SetSize(nSize, initVal);
	}

	RObArray(const RObArray<T>& arr) : m_pTs(NULL), m_nSize(0)
	{
		operator=(arr);
	}

#if _MSC_VER >= 1600
	RObArray(RObArray<T>&& arr) : m_pTs(arr.m_pTs), m_nSize(arr.m_nSize)
	{
		arr.m_pTs = NULL;
		arr.m_nSize = 0;
	}
#endif

	~RObArray()
	{
		SetSize(0);
	}

	void SetSize(INT_PTR nSize)
	{
		ASSERT(nSize >= 0);

		if (nSize != m_nSize)
		{
			if (nSize >= 0 && nSize < m_nSize)
				for (INT_PTR i = nSize; i < m_nSize; ++i)
					m_pTs[i].~T();

			if (nSize > 0)
			{
				m_pTs = (T*)realloc(m_pTs, nSize * sizeof(T));
				ASSERT(m_pTs);
			
				#pragma push_macro("new")
				#undef new
				if (m_nSize < nSize)
					for (INT_PTR i = m_nSize; i < nSize; ++i)
						new(m_pTs + i) T;
				#pragma pop_macro("new")

				m_nSize = nSize;
			}
			else
			{
				free(m_pTs);
				m_pTs = NULL;
				m_nSize = 0;
			}
		}
	}

	void SetSize(INT_PTR nSize, const T& initVal)
	{
		SetSize(nSize);
		for (INT_PTR i = 0; i < nSize; ++i)
			m_pTs[i] = initVal;
	}

	RObArray<T>& operator=(const RObArray<T>& arr)
	{
		SetSize(arr.m_nSize);
		for (INT_PTR i = 0; i < m_nSize; ++i)
			m_pTs[i] = arr.m_pTs[i];
		return *this;
	}

#if _MSC_VER >= 1600
	RObArray<T>& operator =(RObArray<T>&& arr)
	{
		if (&arr != this)
		{
			free(m_pTs);
			m_pTs = arr.m_pTs;
			m_nSize = arr.m_nSize;
			arr.m_pTs = NULL;
			arr.m_nSize = 0;
		}
		return *this;
	}
#endif

	bool RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1)
	{
		if (nIndex < 0 || nCount < 1)
			ASSERTRETURN(false);

		if (nIndex + nCount > m_nSize)
			ASSERTRETURN(false);

		for (INT_PTR i = nIndex; i < (nIndex + nCount); ++i)
			m_pTs[i].~T();

		memmove(m_pTs + nIndex, m_pTs + nIndex + nCount, (m_nSize - nIndex - nCount) * sizeof(T));

		m_nSize -= nCount;
		if (m_nSize > 0)
			m_pTs = (T*)realloc(m_pTs, m_nSize * sizeof(T));
		else
		{
			free(m_pTs);
			m_pTs = NULL;
			m_nSize = 0;
		}

		return true;
	}

	INT_PTR Add(const RObArray<T>& arr)
	{
		ASSERT(false); // Not tested this code yet!
		INT_PTR nIndex = m_nSize;
		SetSize(m_nSize + arr.m_nSize);
		for (INT_PTR i = nIndex, j = 0; i < m_nSize; ++i, ++j)
			m_pTs[i] = arr.m_pTs[j];
		return nIndex;
	}

	/*
#if _MSC_VER >= 1600
	template <class A>
	T* AddNew(A&& a)
	{
		INT_PTR nIndex = m_nSize++;
		m_pTs = (T*)realloc(m_pTs, m_nSize * sizeof(T));
		#pragma push_macro("new")
		#undef new
		new(m_pTs + nIndex) T(forw<A>(a));
		#pragma pop_macro("new")
		return m_pTs + nIndex;
	}

	template <class A1, class A2>
	T* AddNew(A1&& a1, A2&& a2)
	{
		INT_PTR nIndex = m_nSize++;
		m_pTs = (T*)realloc(m_pTs, m_nSize * sizeof(T));
		#pragma push_macro("new")
		#undef new
		new(m_pTs + nIndex) T(forw<A1>(a1), forw<A2>(a2));
		#pragma pop_macro("new")
		return m_pTs + nIndex;
	}
#endif
	*/

	T* InsertNewAt(INT_PTR nIndex)
	{
		if (nIndex == m_nSize)
			{ASSERT(false); return AddNew();} // should not be used for this
		if (nIndex < 0 || nIndex > m_nSize)
			{ASSERT(false); return NULL;} // index out of range

		m_pTs = (T*)realloc(m_pTs, (m_nSize + 1) * sizeof(T));
		ASSERT(m_pTs);
		memmove(m_pTs + nIndex + 1, m_pTs + nIndex, (m_nSize - nIndex) * sizeof(T));
		++m_nSize;

		#pragma push_macro("new")
		#undef new
		new(m_pTs + nIndex) T;
		#pragma pop_macro("new")

		return (m_pTs + nIndex);
	}

	#include "RArray_common.h"

protected:
	T *m_pTs;
	INT_PTR m_nSize;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Global size implementation

template <typename T>
inline INT_PTR size(const RArray<T>& arr)
	{return arr.GetSize();}

template <class T>
inline INT_PTR size(const RObArray<T>& arr)
	{return arr.GetSize();}

#endif // __RARRAY_H__
