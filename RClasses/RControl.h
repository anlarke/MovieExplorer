#ifndef __RCONTROL_H__
#define __RCONTROL_H__

class RControl : public RWindow
{
	friend class RWindow;

public:
	RControl() : m_lpfnPrevWndProc(NULL)
	{
	}

	~RControl()
	{
	}

	template <class DERIVED_TYPE>
	bool Create(const TCHAR *lpszClassName, HWND hWndParent, DWORD style = WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|
				WS_CLIPSIBLINGS, DWORD exStyle = 0, const TCHAR *lpszWindowName = NULL, int x = 0, int y = 0, 
				int cx = 0, int cy = 0, UINT_PTR id = 0, void *pParam = NULL)
	{
		if (IsWindow(m_hWnd))
		{
			ASSERT(false); // calling Create on existing window, is this the intention?
			Destroy();
		}

		ASSERT(hWndParent); // hWndParent should be set for child window

		// Get pointer to original window procedure

		WNDCLASS wc;
		ZeroMemory(&wc, sizeof(WNDCLASS));

		if (!GetClassInfo(GetModuleHandle(NULL), lpszClassName, &wc))
			ASSERTRETURN(false);

		m_lpfnPrevWndProc = wc.lpfnWndProc;
		ASSERT(m_lpfnPrevWndProc);

		// Generate unique class name based on template WndProc

#ifdef _WIN64
		TCHAR szClassName[17];
		_stprintf(szClassName, _T("%.16I64X"), (UINT64)(WNDPROC)WndProc<DERIVED_TYPE>);
#else
		TCHAR szClassName[9];
		_stprintf(szClassName, _T("%.8X"), (UINT)(WNDPROC)WndProc<DERIVED_TYPE>);
#endif

		// Register new window class with our WndProc based on the original one

		WNDCLASS wc2;
		ZeroMemory(&wc2, sizeof(WNDCLASS));

		if (!GetClassInfo(GetModuleHandle(NULL), szClassName, &wc2))
		{
			wc.lpfnWndProc = (WNDPROC)WndProc<DERIVED_TYPE>;
			wc.lpszClassName = szClassName;

			if (!RegisterClass(&wc))
				ASSERTRETURN(false);
		}

		// Create the window

		WndProc<DERIVED_TYPE>((HWND)-1, -1, -1, (LPARAM)(DERIVED_TYPE*)this);

		m_hWnd = CreateWindowEx(exStyle, szClassName, lpszWindowName, style, x, y, cx, cy, 
				hWndParent, (HMENU)id, GetModuleHandle(NULL), pParam);

		ASSERTRETURN(m_hWnd != NULL);
		return true;
	}

protected:
	template <class DERIVED_TYPE>
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		// Some messages should be handled differently for a control

		DERIVED_TYPE *pWnd = (DERIVED_TYPE*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		if (pWnd)
		{
			switch (Msg)
			{
			case WM_ERASEBKGND:
				return (pWnd->OnEraseBkgnd((HDC)wParam) ? 1 : 0);
			case WM_PAINT:
				pWnd->OnPaint((HDC)wParam);
				return 0;
			}
		}

		// Just let the base class handle the rest of them

		return RWindow::WndProc<DERIVED_TYPE>(hWnd, Msg, wParam, lParam);
	}

	LRESULT WndProc(UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		return CallWindowProc(m_lpfnPrevWndProc, m_hWnd, Msg, wParam, lParam);
	}

	void OnActivate(WORD state, bool bMinimized, HWND hWndOther)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_ACTIVATE, MAKEWPARAM(state, bMinimized),
				(LPARAM)hWndOther);
	}

	void OnCaptureChanged(HWND hWndNew)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_CAPTURECHANGED, 0, (LPARAM)hWndNew);
	}

	void OnChar(UINT charCode, WORD repCount, UINT flags)
	{
		UNREFERENCED_PARAMETER(repCount);
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_CHAR, (WPARAM)charCode, (LPARAM)flags);
	}

	void OnClose()
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_CLOSE, 0, 0);
	}

	void OnCommand(WORD id, WORD notifyCode, HWND hWndControl)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_COMMAND, MAKEWPARAM(id, notifyCode), (LPARAM)hWndControl);
	}

	bool OnCreate(CREATESTRUCT *pCS)
	{
		return CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_CREATE, 0, (LPARAM)pCS) >= 0;
	}

	HBRUSH OnCtlColorBtn(HDC hDC, HWND hWnd)
	{
		return (HBRUSH)CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_CTLCOLORBTN, (WPARAM)hDC, (LPARAM)hWnd);
	}

	HBRUSH OnCtlColorEdit(HDC hDC, HWND hWnd)
	{
		return (HBRUSH)CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_CTLCOLOREDIT, (WPARAM)hDC, (LPARAM)hWnd);
	}

	HBRUSH OnCtlColorStatic(HDC hDC, HWND hWnd)
	{
		return (HBRUSH)CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_CTLCOLORSTATIC, (WPARAM)hDC, (LPARAM)hWnd);
	}

	void OnDestroy()
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_DESTROY, 0, 0);
	}

	void OnDrawItem(UINT_PTR id, DRAWITEMSTRUCT *pDIS)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_DRAWITEM, id, (LPARAM)pDIS);
	}

	LRESULT OnGetDlgCode(UINT virtKey, MSG *pMsg)
	{
		return CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_GETDLGCODE, virtKey, (LPARAM)pMsg);
	}

	void OnHScroll(WORD scrollCode, WORD pos, HWND hWndScrollBar)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_HSCROLL, MAKEWPARAM(scrollCode, pos), (LPARAM)hWndScrollBar);
	}

	void OnKeyDown(UINT virtKey, WORD repCount, UINT flags)
	{
		UNREFERENCED_PARAMETER(repCount);
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_KEYDOWN, (WPARAM)virtKey, (LPARAM)flags);
	}

	bool OnEraseBkgnd(HDC hDC)
	{
		return CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_ERASEBKGND, (WPARAM)hDC, 0) != 0;
	}

	void OnKillFocus(HWND hWndGetFocus)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_KILLFOCUS, (WPARAM)hWndGetFocus, 0);
	}

	void OnLButtonDblClk(DWORD keys, short x, short y)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_LBUTTONDBLCLK, keys, MAKELPARAM(x, y));
	}

	void OnLButtonDown(DWORD keys, short x, short y)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_LBUTTONDOWN, keys, MAKELPARAM(x, y));
	}

	void OnLButtonUp(DWORD keys, short x, short y)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_LBUTTONUP, keys, MAKELPARAM(x, y));
	}

	void OnMeasureItem(UINT_PTR id, MEASUREITEMSTRUCT *pMIS)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_MEASUREITEM, id, (LPARAM)pMIS);
	}

	void OnMouseLeave()
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_MOUSELEAVE, 0, 0);
	}

	void OnMouseMove(DWORD keys, short x, short y)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_MOUSEMOVE, keys, MAKELPARAM(x, y));
	}

	void OnMove(short x, short y)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_MOVE, 0, MAKELPARAM(x, y));
	}

	LRESULT OnNotify(UINT_PTR id, NMHDR *pNMH)
	{
		return CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_NOTIFY, id, (LPARAM)pNMH);
	}

	void OnPaint(HDC hDC)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_PAINT, (WPARAM)hDC, 0);
	}

	void OnPrefChanged() // not a system message
	{
	}

	void OnScaleChanged() // not a system message
	{
	}

	bool OnSetCursor(HWND hWnd, WORD hitTest, WORD mouseMsg)
	{
		return CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_SETCURSOR, (WPARAM)hWnd, MAKELPARAM(hitTest, mouseMsg)) != FALSE;
	}

	void OnSetFocus(HWND hWndLoseFocus)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_SETFOCUS, (WPARAM)hWndLoseFocus, 0);
	}

	void OnSetFont(HFONT hFont, bool bRedraw)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_SETFONT, (WPARAM)hFont, (LPARAM)(BOOL)bRedraw);
	}

	LRESULT OnSetText(const TCHAR *lpszText)
	{
		return CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_SETTEXT, 0, (LPARAM)lpszText);
	}

	void OnSize(DWORD type, WORD cx, WORD cy)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_SIZE, type, MAKELPARAM(cx, cy));
	}

	void OnSizing(DWORD side, RECT *pRect)
	{
		LRESULT res = CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_SIZING, side, (LPARAM)pRect);
		ASSERT(res == 1);
	}

	void OnTimer(UINT_PTR nIDEvent)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_TIMER, nIDEvent, 0);
	}

	void OnVScroll(WORD scrollCode, WORD pos, HWND hWndScrollBar)
	{
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_VSCROLL, MAKEWPARAM(scrollCode, pos), (LPARAM)hWndScrollBar);
	}

	WNDPROC m_lpfnPrevWndProc;
};

// The RControlFF class will enable flicker free drawing for common controls, but will disable 
// control animations.

// The main issue with the standard window controls is that not all painting is done within the
// appropriate WM_ERASEBKGND and WM_PAINT messages. Due to this behavior the following class does 
// not always give the expected result.

template <class T>
class RControlFF : public T
{
	friend class RWindow;
	friend class RControl;

public:
	RControlFF()
	{
	}

	~RControlFF()
	{
	}

protected:
	void OnSize(DWORD type, WORD cx, WORD cy)
	{
		m_mdc.Create(cx, cy);
		T::OnSize(type, cx, cy);
	}

	bool OnEraseBkgnd(HDC hDC)
	{
		UNREFERENCED_PARAMETER(hDC);
		return true;
	}

	void OnPaint(HDC hDC)
	{
		if (!m_mdc)
		{
			ASSERT(false);
			T::OnEraseBkgnd(hDC);
			T::OnPaint(hDC);
			return;
		}

		int cx, cy;
		m_mdc.GetDimensions(cx, cy);

		/*
		// Let the parent draw the background into memory DC
		
		HWND hWndParent = GetParent(m_hWnd);
		RECT rc;
		GetClientRectRelative(m_hWnd, hWndParent, &rc);
		POINT pt;
		SetWindowOrgEx(m_mdc, rc.left, rc.top, &pt);
		SendMessage(hWndParent, WM_PRINTCLIENT, (WPARAM)(HDC)m_mdc, PRF_CLIENT);
		SetWindowOrgEx(m_mdc, pt.x, pt.y, NULL);
		*/

		// Let control draw into memory DC

		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_ERASEBKGND, (WPARAM)(HDC)m_mdc, 0);
		CallWindowProc(m_lpfnPrevWndProc, m_hWnd, WM_PRINTCLIENT, (WPARAM)(HDC)m_mdc, PRF_CLIENT);

		if (hDC)
			BitBlt(hDC, 0, 0, cx, cy, m_mdc, 0, 0, SRCCOPY);
		else
		{
			if (GetUpdateRect(m_hWnd, NULL, FALSE))
			{
				PAINTSTRUCT ps;
				BeginPaint(m_hWnd, &ps);
				BitBlt(ps.hdc, 0, 0, cx, cy, m_mdc, 0, 0, SRCCOPY);
				EndPaint(m_hWnd, &ps);
			}
			//else
			//{
			//	hDC = GetDC(m_hWnd);
			//	BitBlt(hDC, 0, 0, cx, cy, m_mdc, 0, 0, SRCCOPY);
			//	ReleaseDC(m_hWnd, hDC);
			//}
		}
	}

	RMemoryDC m_mdc;
};

#endif // __RCONTROL_H__
