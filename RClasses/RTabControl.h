#ifndef __RTABCONTROL_H__
#define __RTABCONTROL_H__

class RTabControl : public RControl
{
	friend class RWindow;
	friend class RControl;

public:
	RTabControl()
	{
	}

	~RTabControl()
	{
	}

	template <class DERIVED_TYPE>
	bool Create(HWND hWndParent, DWORD style = 0, DWORD exStyle = 0, int x = 0, int y = 0, 
			int cx = 0, int cy = 0, UINT_PTR id = 0, void *pParam = NULL)
	{
		if (!RControl::Create<DERIVED_TYPE>(WC_TABCONTROL, hWndParent, WS_CHILD|WS_VISIBLE|
				WS_CLIPCHILDREN|WS_CLIPSIBLINGS|style, exStyle, NULL, x, y, cx, cy, id, pParam))
			ASSERTRETURN(false);

		SetFont(GetDialogFont());

		return true;
	}

	INT_PTR GetItemCount()
	{
		ASSERT(IsWindow(m_hWnd));
		return SendMessage(m_hWnd, TCM_GETITEMCOUNT, 0, 0);
	}

	INT_PTR AddItem(const TCHAR *lpszText)
	{
		ASSERT(IsWindow(m_hWnd));
		TCITEM tci;
		ZeroMemory(&tci, sizeof(TCITEM));
		tci.mask = TCIF_TEXT;
		tci.pszText = (TCHAR*)lpszText;
		return SendMessage(m_hWnd, TCM_INSERTITEM, GetItemCount(), (LPARAM)&tci);
	}

	INT_PTR GetCurSel()
	{
		ASSERT(IsWindow(m_hWnd));
		return SendMessage(m_hWnd, TCM_GETCURSEL, 0, 0);
	}

	void AdjustRect(bool bLarger, RECT *pRect)
	{
		ASSERT(IsWindow(m_hWnd));
		SendMessage(m_hWnd, TCM_ADJUSTRECT, (WPARAM)(BOOL)bLarger, (LPARAM)pRect);
	}

	bool SetItemText(INT_PTR nItem, const TCHAR *lpszText)
	{
		ASSERT(IsWindow(m_hWnd));
		TCITEM tci;
		ZeroMemory(&tci, sizeof(TCITEM));
		tci.mask = TCIF_TEXT;
		tci.pszText = (TCHAR*)lpszText;
		return (bool)SendMessage(m_hWnd, TCM_SETITEM, nItem, (LPARAM)&tci);
	}
};

class RTabControlFF : public RControlFF<RTabControl>
{
	friend class RWindow;
	friend class RControl;

public:
	RTabControlFF()
	{
	}

	~RTabControlFF()
	{
	}
};

#endif // __RTABCONTROL_H__
