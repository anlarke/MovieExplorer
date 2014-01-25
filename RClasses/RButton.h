#ifndef __RBUTTON_H__
#define __RBUTTON_H__

class RButton : public RControl
{
	friend class RWindow;
	friend class RControl;

public:
	RButton()
	{
	}

	~RButton()
	{
	}

	template <class DERIVED_TYPE>
	bool Create(HWND hWndParent, DWORD style = 0, DWORD exStyle = 0, const TCHAR *lpszText = NULL, 
			int x = 0, int y = 0, int cx = 0, int cy = 0, UINT_PTR id = 0, void *pParam = NULL)
	{
		if (!RControl::Create<DERIVED_TYPE>(WC_BUTTON, hWndParent, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|
				WS_CLIPSIBLINGS|style, exStyle, lpszText, x, y, cx, cy, id, pParam))
			ASSERTRETURN(false);

		SetFont(GetDialogFont());

		return true;
	}

	int GetCheck()
	{
		ASSERT(IsWindow(m_hWnd));
		return (int)SendMessage(m_hWnd, BM_GETCHECK);
	}

	void SetCheck(int nCheck = BST_CHECKED)
	{
		ASSERT(IsWindow(m_hWnd));
		SendMessage(m_hWnd, BM_SETCHECK, nCheck);
	}
};

class RButtonFF : public RControlFF<RButton>
{
	friend class RWindow;
	friend class RControl;

public:
	RButtonFF()
	{
	}

	~RButtonFF()
	{
	}
};

inline void MoveCheckBox(HWND hWndCheckBox, int x, int y)
{
	RString strText;
	INT_PTR nLen = GetWindowTextLength(hWndCheckBox);
	if (nLen > 0)
	{
		GetWindowText(hWndCheckBox, strText.GetBuffer(nLen), (int)nLen + 1);
		strText.ReleaseBuffer(nLen);
	}

	RMemoryDC mdcTemp(0, 0);
	HFONT hPrevFont = (HFONT)SelectObject(mdcTemp, GetDialogFont());
	SIZE sz;
	GetTextExtentPoint32(mdcTemp, strText, (int)strText.GetLength(), &sz);
	SelectObject(mdcTemp, hPrevFont);

	MoveWindow(hWndCheckBox, x, y, sz.cx + SCX(20), sz.cy + SCY(2));
}

#endif // __RBUTTON_H__
