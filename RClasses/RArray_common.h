	INT_PTR GetSize() const
	{
		return m_nSize;
	}

//#if _MSC_VER >= 1600
//	template <class ARG_TYPE>
//	INT_PTR Add(ARG_TYPE&& element) // interferes with INT_PTR Add(const RArray<T>& arr)
//	{
//		INT_PTR nIndex = m_nSize;
//		SetSize(m_nSize + 1);
//		m_pTs[nIndex] = forw<ARG_TYPE>(element);
//		return nIndex;
//	}
//#else
	INT_PTR Add(const T& element)
	{
		INT_PTR nIndex = m_nSize;
		SetSize(m_nSize + 1);
		m_pTs[nIndex] = element;
		return nIndex;
	}
//#endif

	T* AddNew()
	{
		SetSize(m_nSize + 1);
		return (m_pTs + m_nSize - 1);
	}
	
	operator INT_PTR() const
	{
		return m_nSize;
	}

	T& operator [](INT_PTR nIndex)
	{
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		ASSERT(m_pTs);
		return m_pTs[nIndex];
	}

	const T& operator [](INT_PTR nIndex) const
	{
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		ASSERT(m_pTs);
		return m_pTs[nIndex];
	}

	T& GetAt(INT_PTR nIndex)
	{
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		ASSERT(m_pTs);
		return m_pTs[nIndex];
	}

	const T& GetAt(INT_PTR nIndex) const
	{
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		ASSERT(m_pTs);
		return m_pTs[nIndex];
	}

	T& GetFirst()
	{
		ASSERT(m_nSize > 0);
		return m_pTs[0];
	}

	const T& GetFirst() const
	{
		ASSERT(m_nSize > 0);
		return m_pTs[0];
	}

	T& GetLast()
	{
		ASSERT(m_nSize > 0);
		return m_pTs[m_nSize - 1];
	}

	const T& GetLast() const
	{
		ASSERT(m_nSize > 0);
		return m_pTs[m_nSize - 1];
	}

#if _MSC_VER >= 1600
	template <class ARG_TYPE>
	INT_PTR Push(ARG_TYPE&& element)
	{
		INT_PTR nIndex = m_nSize;
		SetSize(m_nSize + 1);
		m_pTs[nIndex] = forw<ARG_TYPE>(element);
		return nIndex;
	}
#else
	INT_PTR Push(const T& element)
	{
		INT_PTR nIndex = m_nSize;
		SetSize(m_nSize + 1);
		m_pTs[nIndex] = element;
		return nIndex;
	}
#endif

#if _MSC_VER >= 1600
	T Pop()
	{
		ASSERT(m_nSize > 0);
		T element = move(m_pTs[m_nSize - 1]);
		SetSize(m_nSize - 1);
		return element;
	}
#else
	T Pop()
	{
		ASSERT(m_nSize > 0);
		T element = m_pTs[m_nSize - 1];
		SetSize(m_nSize - 1);
		return element;
	}
#endif

	T* GetData()
	{
		return m_pTs;
	}

	const T* GetData() const
	{
		return m_pTs;
	}

	INT_PTR IndexOf(const T& element) const
	{
		if (m_pTs)
		{
			for (INT_PTR i = 0; i < m_nSize; ++i)
				if (m_pTs[i] == element)
					return i;
		}
		return -1;
	}

	void Swap(INT_PTR nIndex1, INT_PTR nIndex2)
	{
		ASSERT(nIndex1 < m_nSize);
		ASSERT(nIndex2 < m_nSize);
		memswap(m_pTs + nIndex1, m_pTs + nIndex2, sizeof(T));
	}
