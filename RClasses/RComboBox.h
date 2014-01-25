#ifndef __RCOMBOBOX_H__
#define __RCOMBOBOX_H__

class RComboBox : public RControl
{
	friend class RWindow;
	friend class RControl;

public:
	RComboBox()
	{
	}

	~RComboBox()
	{
	}

	template <class DERIVED_TYPE>
	bool Create(HWND hWndParent, DWORD style = 0, DWORD exStyle = 0, const TCHAR *lpszText = NULL, 
			int x = 0, int y = 0, int cx = 0, int cy = 0, UINT_PTR id = 0, void *pParam = NULL)
	{
		if (!RControl::Create<DERIVED_TYPE>(WC_COMBOBOX, hWndParent, WS_CHILD|WS_VISIBLE|
				WS_CLIPCHILDREN|WS_CLIPSIBLINGS|style, exStyle, lpszText, x, y, cx, cy, id, pParam))
			ASSERTRETURN(false);

		SetFont(GetDialogFont());

		return true;
	}

	INT_PTR Add(const TCHAR *lpszText)
	{
		INT_PTR nIndex = SendMessage(m_hWnd, CB_ADDSTRING, 0, (LPARAM)lpszText);
		if (GetCount() == 1)
			SetSel(0);
		return nIndex;
	}

	bool Remove(INT_PTR nIndex)
	{
		ASSERTRETURN(SendMessage(m_hWnd, CB_DELETESTRING, nIndex, 0) != CB_ERR);
	}

	INT_PTR GetCount() const
	{
		return SendMessage(m_hWnd, CB_GETCOUNT, 0, 0);
	}

	RString GetText(INT_PTR nIndex) const
	{
		RString str;

		INT_PTR nLen = SendMessage(m_hWnd, CB_GETLBTEXTLEN, nIndex, 0);
		if (nLen == CB_ERR)
			{ASSERT(false); return str;}

		SendMessage(m_hWnd, CB_GETLBTEXT, (WPARAM)nIndex, (LPARAM)str.GetBuffer(nLen));
		str.ReleaseBuffer(nLen);
		
		return str;
	}

	//RString GetText() const
	//{
	//	return GetText(GetSel());
	//}

	bool SetText(INT_PTR nIndex, const TCHAR *lpszText)
	{
		bool bSelected = (GetSel() == nIndex);

		if (SendMessage(m_hWnd, CB_DELETESTRING, nIndex) < 0 ||
				SendMessage(m_hWnd, CB_INSERTSTRING, nIndex, (LPARAM)lpszText) < 0)
			ASSERTRETURN(false);

		if (bSelected)
			SetSel(nIndex);

		return true;
	}

	INT_PTR GetSel() const
	{
		return SendMessage(m_hWnd, CB_GETCURSEL, 0, 0);
	}

	bool SetSel(INT_PTR nIndex)
	{
		ASSERTRETURN(SendMessage(m_hWnd, CB_SETCURSEL, nIndex, 0) != CB_ERR);
	}

	bool SetSel(RString_ strText)
	{
		INT_PTR i, nCount = GetCount();
		for (i = 0; i < nCount; ++i)
			if (GetText(i) == strText)
				{SetSel(i); return true;}
		return false;
	}

	bool SetSelNoCase(RString_ strText)
	{
		INT_PTR i, nCount = GetCount();
		for (i = 0; i < nCount; ++i)
			if (_tcsicmp(GetText(i), strText) == 0)
				{SetSel(i); return true;}
		return false;
	}
};

class RComboBoxFF : public RControlFF<RComboBox>
{
	friend class RWindow;
	friend class RControl;

public:
	RComboBoxFF()
	{
	}

	~RComboBoxFF()
	{
	}
};

#endif // __RCOMBOBOX_H__
