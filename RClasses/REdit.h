#ifndef __REDIT_H__
#define __REDIT_H__

#define EN_RETURN		0x0700

class REdit : public RControl
{
	friend class RWindow;
	friend class RControl;
	
public:
	REdit()
	{
	}

	~REdit()
	{
	}

	template <class DERIVED_TYPE>
	bool Create(HWND hWndParent, DWORD style = 0, DWORD exStyle = 0, const TCHAR *lpszText = NULL, 
			int x = 0, int y = 0, int cx = 0, int cy = 0, UINT_PTR id = 0, void *pParam = NULL)
	{
		if (!RControl::Create<DERIVED_TYPE>(WC_EDIT, hWndParent, WS_CHILD|WS_VISIBLE|
				WS_CLIPCHILDREN|WS_CLIPSIBLINGS|style, exStyle, lpszText, 
				x, y, cx, cy, id, pParam))
			ASSERTRETURN(false);

		SetFont(GetDialogFont());

		return true;
	}

	void GetRect(RECT *pRect)
	{
		ASSERT(IsWindow(m_hWnd));
		SendMessage(m_hWnd, EM_GETRECT, 0, (LPARAM)pRect);
	}

	void SetRect(RECT *pRect)
	{
		ASSERT(IsWindow(m_hWnd));
		SendMessage(m_hWnd, EM_SETRECT, 0, (LPARAM)pRect);
	}

	void LimitText(int nMaxCharacters)
	{
		ASSERT(IsWindow(m_hWnd));
		SendMessage(m_hWnd, EM_LIMITTEXT, nMaxCharacters, 0);
	}

	void SetSel(int nStart = 0, int nEnd = -1)
	{
		ASSERT(IsWindow(m_hWnd));
		SendMessage(m_hWnd, EM_SETSEL, nStart, nEnd);
	}

	void SetMargins(int nLeft = -1, int nRight = -1)
	{
		ASSERT(IsWindow(m_hWnd));
		SendMessage(m_hWnd, EM_SETMARGINS, (nLeft > -1 ? EC_LEFTMARGIN : 0)|(nRight > -1 ? EC_RIGHTMARGIN : 0), 
				MAKELPARAM(nLeft, nRight));
	}

	void AppendText(const TCHAR* lpszText)
	{
		ASSERT(IsWindow(m_hWnd));
		int nLen = GetWindowTextLength(m_hWnd);
		SendMessage(m_hWnd, EM_SETSEL, nLen, nLen);
		SendMessage(m_hWnd, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)lpszText);
	}

protected:
	void OnKeyDown(UINT virtKey, WORD repCount, UINT flags)
	{
		RControl::OnKeyDown(virtKey, repCount, flags);
		if (virtKey == VK_RETURN)
			PostMessage(GetParent(m_hWnd), WM_COMMAND, MAKEWPARAM((WORD)GetMenu(m_hWnd), 
					EN_RETURN), (LPARAM)m_hWnd);
		else if (virtKey == _T('A') && GetKeyState(VK_CONTROL) < 0)
			SetSel(0);
	}

	void OnMouseWheel(WORD keys, short delta, short x, short y)
	{
		UNREFERENCED_PARAMETER(keys);
		UNREFERENCED_PARAMETER(x);
		UNREFERENCED_PARAMETER(y);

		UINT nScrollLines = 3;
		SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &nScrollLines, 0);
		int nPos = GetScrollPos(m_hWnd, SB_VERT);
		nPos -= (delta * (int)nScrollLines) / WHEEL_DELTA;
		if (nPos < 0)
			nPos = 0;
		PostMessage(m_hWnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, nPos));		
	}
};

class REditFF : public RControlFF<REdit>
{
	friend class RWindow;
	friend class RControl;

public:
	REditFF()
	{
	}

	~REditFF()
	{
	}

protected:
	LRESULT WndProc(UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (Msg == WM_LBUTTONDOWN || Msg == WM_RBUTTONDOWN)
		{
			InvalidateRect(m_hWnd, NULL, FALSE);
			PostMessage(m_hWnd, WM_PAINT, 0, 0);
		}
		return RControlFF<REdit>::WndProc(Msg, wParam, lParam);
	}
};

#endif // __REDIT_H__















/*
// Edit.h : Defines the REdit class.
//

#pragma once

class REdit
{
public:
	REdit() : m_hWnd(NULL) //, m_lpfnPrevWndProc(NULL)
	{
	}

	~REdit()
	{
	}

	bool Create(HWND hWndParent, const TCHAR* lpszText = NULL, DWORD dwStyle = 0, DWORD dwExStyle = 0)
	{
		m_hWnd = CreateWindowEx(dwExStyle, WC_EDIT, lpszText, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|dwStyle, 
							    0, 0, 0, 0, hWndParent, NULL, GetModuleHandle(NULL), NULL);
		
		if (!m_hWnd)
			ASSERTRETURN(false);

		SetFont(GetDialogFont());

		// Subclass window
		
		//m_lpfnPrevWndProc = (WNDPROC)SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)(WNDPROC)WndProc);
		//SetProp(m_hWnd, _T("pControl"), (HANDLE)this);
		
		return true;
	}

	RString GetText()
	{
		RString str;
		int cch = GetWindowTextLength(m_hWnd);
		GetWindowText(m_hWnd, str.GetBuffer(cch), cch + 1);
		return str;
	}

	bool SetText(const TCHAR *lpszText)
	{
		ASSERT(IsWindow(m_hWnd));
		return (bool)SetWindowText(m_hWnd, lpszText);
	}

	void GetRect(RECT *pRect)
	{
		ASSERT(IsWindow(m_hWnd));
		SendMessage(m_hWnd, EM_GETRECT, 0, (LPARAM)pRect);
	}

	void SetRect(RECT *pRect)
	{
		ASSERT(IsWindow(m_hWnd));
		SendMessage(m_hWnd, EM_SETRECT, 0, (LPARAM)pRect);
	}

	void LimitText(int nMaxCharacters)
	{
		ASSERT(IsWindow(m_hWnd));
		SendMessage(m_hWnd, EM_LIMITTEXT, nMaxCharacters, 0);
	}

	void SetFont(HFONT hFont)
	{
		ASSERT(IsWindow(m_hWnd));
		SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
	}

	operator HWND() const
	{
		return m_hWnd;
	}

protected:
	//static LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	//{
	//	REdit *pControl = (REdit*)GetProp(hWnd, _T("pControl"));
	//	if (!pControl)
	//		{ASSERT(false); return 0;}
	//
	//	return pControl->WndProc(Msg, wParam, lParam);
	//}
	//
	//virtual LRESULT WndProc(UINT Msg, WPARAM wParam, LPARAM lParam)
	//{
	//	switch (Msg)
	//	{
	//	case WM_NCDESTROY:
	//		SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)m_lpfnPrevWndProc);
	//		RemoveProp(m_hWnd, _T("pControl"));
	//		break;
	//	}
	//
	//	return CallWindowProc(m_lpfnPrevWndProc, m_hWnd, Msg, wParam, lParam);
	//}

	HWND m_hWnd;
	//WNDPROC m_lpfnPrevWndProc;
};
*/
