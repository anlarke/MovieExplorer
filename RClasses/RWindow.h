#ifndef __RWINDOW_H__
#define __RWINDOW_H__

#define LOSHORT(x)	((short)(((UINT_PTR)(x)) & 0xFFFF))
#define HISHORT(x)	((short)((((UINT_PTR)(x)) >> 16) & 0xFFFF))

class RWindow
{
public:
	RWindow() : m_hWnd(NULL)
	{
	}

	~RWindow()
	{
		Destroy();
	}

	template <class DERIVED_TYPE>
	bool Create(HWND hWndParent, DWORD style = WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 
				DWORD exStyle = 0, const TCHAR *lpszName = NULL, int x = 0, int y = 0, int cx = 0, 
				int cy = 0, HMENU hMenuOrChildID = NULL, void *pParam = NULL)
	{
		if (IsWindow(m_hWnd))
		{
			ASSERT(false); // calling Create on existing window, is this the intention?
			Destroy();
		}

		ASSERT(!(style & WS_CHILD) || hWndParent); // hWndParent should be set for child window

		// Generate unique class name based on template WndProc

#ifdef _WIN64
		TCHAR szClassName[17];
		_stprintf(szClassName, _T("%.16I64X"), (UINT64)(WNDPROC)WndProc<DERIVED_TYPE>);
#else
		TCHAR szClassName[9];
		_stprintf(szClassName, _T("%.8X"), (UINT)(WNDPROC)WndProc<DERIVED_TYPE>);
#endif

		// Register window class if it does not exist

		WNDCLASS wc;
		ZeroMemory(&wc, sizeof(WNDCLASS));

		if (!GetClassInfo(GetModuleHandle(NULL), szClassName, &wc))
		{
			ZeroMemory(&wc, sizeof(WNDCLASS));
			wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hInstance = GetModuleHandle(NULL);
			wc.lpfnWndProc = (WNDPROC)WndProc<DERIVED_TYPE>;
			wc.lpszClassName = szClassName;
			wc.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
			
			if (!RegisterClass(&wc))
				ASSERTRETURN(false);
		}

		// Create the window

		WndProc<DERIVED_TYPE>((HWND)-1, -1 , -1, (LPARAM)(DERIVED_TYPE*)this);

		m_hWnd = CreateWindowEx(exStyle, szClassName, lpszName, style, x, y, cx, cy, hWndParent,
				hMenuOrChildID, GetModuleHandle(NULL), pParam);

		ASSERTRETURN(m_hWnd != NULL);
	}

	void Destroy()
	{
		//if (m_hWnd)
		//{
		//	TCHAR szClassName[17] = {0};
		//	VERIFY(GetClassName(m_hWnd, szClassName, 16));
		//	DestroyWindow(m_hWnd);
		//	m_hWnd = NULL;
		//	UnregisterClass(szClassName, GetModuleHandle(NULL));
		//}

		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}

	void SetFont(HFONT hFont, bool bRedraw = true)
	{
		ASSERT(IsWindow(m_hWnd));
		SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hFont, (LPARAM)(bRedraw ? TRUE : FALSE));
	}

	RString GetText() const
	{
		ASSERT(IsWindow(m_hWnd));
		RString str;
		int nLen = GetWindowTextLength(m_hWnd);
		if (nLen > 0)
		{
			GetWindowText(m_hWnd, str.GetBuffer(nLen), nLen + 1);
			str.ReleaseBuffer(nLen);
		}
		return str;
	}
	
	bool SetText(const TCHAR *lpszText)
	{
		ASSERT(IsWindow(m_hWnd));
		return SetWindowText(m_hWnd, lpszText) != FALSE;
	}

	void Enable()
	{
		EnableWindow(m_hWnd, TRUE);
	}

	void Disable()
	{
		EnableWindow(m_hWnd, FALSE);
	}

	int GetWidth() const
	{
		RECT rc;
		GetClientRect(m_hWnd, &rc);
		return rc.right - rc.left;
	}

	int GetHeight() const
	{
		RECT rc;
		GetClientRect(m_hWnd, &rc);
		return rc.bottom - rc.top;
	}

	operator HWND() const
	{
		return m_hWnd;
	}

protected:
	template <class DERIVED_TYPE>
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		// Store pointer to new window (this method is not thread safe!)

		static DERIVED_TYPE *pNewWnd = NULL;
		if ((INT_PTR)hWnd == -1 && (int)Msg == -1 && (INT_PTR)wParam == -1)
		{
			pNewWnd = (DERIVED_TYPE*)lParam;
			return 0;
		}

		// Get pointer to derived window

		DERIVED_TYPE *pWnd = (DERIVED_TYPE*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (!pWnd)
		{
			ASSERT(pNewWnd);
			pWnd = pNewWnd;
			pWnd->m_hWnd = hWnd;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pWnd);
			pNewWnd = NULL;
		}

		// Check to be sure

		if (!pWnd)
			{ASSERT(false); return DefWindowProc(hWnd, Msg, wParam, lParam);}

		// Call member function belonging to window message

		switch (Msg)
		{
		case WM_ACTIVATEAPP:
			pWnd->OnActivateApp((bool)wParam,(DWORD)lParam);
			return 0;
		case WM_ACTIVATE:
			pWnd->OnActivate(LOWORD(wParam), (bool)HIWORD(wParam), (HWND)lParam);
			return 0;
		case WM_CAPTURECHANGED:
			pWnd->OnCaptureChanged((HWND)lParam);
			return 0;
		case WM_CHAR:
			pWnd->OnChar((UINT)wParam, LOWORD(lParam), (UINT)lParam);
			return 0;
		case WM_CLOSE:
			pWnd->OnClose();
			return 0;
		case WM_COMMAND:
			pWnd->OnCommand(LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
			return 0;
		case WM_CREATE:
			return (pWnd->OnCreate((CREATESTRUCT*)lParam) ? 0 : -1);
		case WM_CTLCOLORBTN:
			return (LRESULT)pWnd->OnCtlColorBtn((HDC)wParam, (HWND)lParam);
		case WM_CTLCOLOREDIT:
			return (LRESULT)pWnd->OnCtlColorEdit((HDC)wParam, (HWND)lParam);
		case WM_CTLCOLORSTATIC:
			return (LRESULT)pWnd->OnCtlColorStatic((HDC)wParam, (HWND)lParam);
		case WM_DESTROY:
			pWnd->OnDestroy();
			return 0;
		case WM_DRAWITEM:
			pWnd->OnDrawItem(wParam, (DRAWITEMSTRUCT*)lParam);
			return TRUE;
		case WM_GETDLGCODE:
			return pWnd->OnGetDlgCode((UINT)wParam, (MSG*)lParam);
		case WM_HSCROLL:
			pWnd->OnHScroll(LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
			return 0;
		case WM_ERASEBKGND:
			//RECT rc;
			//GetClientRect(pWnd->m_hWnd, &rc);
			//FillSolidRect((HDC)wParam, &rc, 0x00FF00);
			return TRUE; // eliminate flicker, we erase the window on WM_PAINT
		case WM_KEYDOWN:
			pWnd->OnKeyDown((UINT)wParam, LOWORD(lParam), (UINT)lParam);
			return 0;
		case WM_KILLFOCUS:
			pWnd->OnKillFocus((HWND)wParam);
			return 0;
		case WM_LBUTTONDBLCLK:
			pWnd->OnLButtonDblClk((DWORD)wParam, LOSHORT(lParam), HISHORT(lParam));
			return 0;
		case WM_LBUTTONDOWN:
			pWnd->OnLButtonDown((DWORD)wParam, LOSHORT(lParam), HISHORT(lParam));
			return 0;
		case WM_LBUTTONUP:
			pWnd->OnLButtonUp((DWORD)wParam, LOSHORT(lParam), HISHORT(lParam));
			return 0;
		case WM_MEASUREITEM:
			pWnd->OnMeasureItem(wParam, (MEASUREITEMSTRUCT*)lParam);
			return TRUE;
		case WM_MOUSELEAVE:
			pWnd->OnMouseLeave();
			return 0;
		case WM_MOUSEMOVE:
			pWnd->OnMouseMove((DWORD)wParam, LOSHORT(lParam), HISHORT(lParam));
			return 0;
		case WM_MOUSEWHEEL:
			pWnd->OnMouseWheel(LOWORD(wParam), HISHORT(wParam), LOSHORT(lParam), HISHORT(lParam));
			return 0;
		case WM_MOVE:
			pWnd->OnMove(LOSHORT(lParam), HISHORT(lParam));
			return 0;
		case WM_NOTIFY:
			return pWnd->OnNotify(wParam, (NMHDR*)lParam);
		case WM_PAINT:
			if (wParam)
				pWnd->OnPaint((HDC)wParam);
			else
			{
				if (!GetUpdateRect(hWnd, NULL, FALSE))
					return 0; // no update region, BeginPaint should not be called
				PAINTSTRUCT ps;
				if (!BeginPaint(hWnd, &ps))
					{ASSERT(false); return 0;}
				pWnd->OnPaint(ps.hdc);
				EndPaint(hWnd, &ps);
			}
			return 0;
		case WM_PREFCHANGED:
			pWnd->OnPrefChanged();
			return 0;
		case WM_PRINTCLIENT:
			pWnd->OnPaint((HDC)wParam);
			return 0;
		case WM_SCALECHANGED:
			pWnd->OnScaleChanged();
			return 0;
		case WM_SETCURSOR:
			return (LRESULT)pWnd->OnSetCursor((HWND)wParam, LOWORD(lParam), HIWORD(lParam));
		case WM_SETFOCUS:
			pWnd->OnSetFocus((HWND)wParam);
			return 0;
		case WM_SETFONT:
			pWnd->OnSetFont((HFONT)wParam, lParam != FALSE);
			return 0;
		case WM_SETTEXT:
			return pWnd->OnSetText((const TCHAR*)lParam);
		case WM_SIZE:
			pWnd->OnSize((DWORD)wParam, LOWORD(lParam), HIWORD(lParam));
			return 0;
		case WM_SIZING:
			pWnd->OnSizing((DWORD)wParam, (RECT*)lParam);
			return TRUE;
		case WM_TIMER:
			pWnd->OnTimer(wParam);
			ASSERT(lParam == 0); // not very well documented
			return 0;
		case WM_TOUCH:
			pWnd->OnTouch(LOWORD(wParam), (HTOUCHINPUT)lParam);
			CloseTouchInputHandle((HTOUCHINPUT)lParam);
			return 0;
		case WM_VSCROLL:
			pWnd->OnVScroll(LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
			return 0;
		}

		return pWnd->WndProc(Msg, wParam, lParam);
	}

	LRESULT WndProc(UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(m_hWnd, Msg, wParam, lParam);
	}

	void OnActivate(WORD state, bool bMinimized, HWND hWndOther)
	{
		UNREFERENCED_PARAMETER(state);
		UNREFERENCED_PARAMETER(bMinimized);
		UNREFERENCED_PARAMETER(hWndOther);
	}

	void OnActivateApp(bool bActive, DWORD dThreadID)
	{
		UNREFERENCED_PARAMETER(bActive);
		UNREFERENCED_PARAMETER(dThreadID);
	}

	void OnCaptureChanged(HWND hWndNew)
	{
		UNREFERENCED_PARAMETER(hWndNew);
	}

	void OnChar(UINT charCode, WORD repCount, UINT flags)
	{
		UNREFERENCED_PARAMETER(charCode);
		UNREFERENCED_PARAMETER(repCount);
		UNREFERENCED_PARAMETER(flags);
	}

	void OnClose()
	{
	}

	void OnCommand(WORD id, WORD notifyCode, HWND hWndControl)
	{
		UNREFERENCED_PARAMETER(id);
		UNREFERENCED_PARAMETER(notifyCode);
		UNREFERENCED_PARAMETER(hWndControl);
	}

	bool OnCreate(CREATESTRUCT *pCS)
	{
		UNREFERENCED_PARAMETER(pCS);
		return true;
	}

	HBRUSH OnCtlColorBtn(HDC hDC, HWND hWnd)
	{
		return (HBRUSH)DefWindowProc(m_hWnd, WM_CTLCOLORBTN, (WPARAM)hDC, (LPARAM)hWnd);
	}

	HBRUSH OnCtlColorEdit(HDC hDC, HWND hWnd)
	{
		return (HBRUSH)DefWindowProc(m_hWnd, WM_CTLCOLOREDIT, (WPARAM)hDC, (LPARAM)hWnd);
	}

	HBRUSH OnCtlColorStatic(HDC hDC, HWND hWnd)
	{
		return (HBRUSH)DefWindowProc(m_hWnd, WM_CTLCOLORSTATIC, (WPARAM)hDC, (LPARAM)hWnd);
	}

	void OnDestroy()
	{
	}

	void OnDrawItem(UINT_PTR id, DRAWITEMSTRUCT *pDIS)
	{
		UNREFERENCED_PARAMETER(id);
		UNREFERENCED_PARAMETER(pDIS);
	}

	LRESULT OnGetDlgCode(UINT virtKey, MSG *pMsg)
	{
		return DefWindowProc(m_hWnd, WM_GETDLGCODE, virtKey, (LPARAM)pMsg);
	}

	void OnHScroll(WORD scrollCode, WORD pos, HWND hWndScrollBar)
	{
		UNREFERENCED_PARAMETER(scrollCode);
		UNREFERENCED_PARAMETER(pos);
		UNREFERENCED_PARAMETER(hWndScrollBar);
	}

	void OnKeyDown(UINT virtKey, WORD repCount, UINT flags)
	{
		UNREFERENCED_PARAMETER(virtKey);
		UNREFERENCED_PARAMETER(repCount);
		UNREFERENCED_PARAMETER(flags);
	}

	void OnKillFocus(HWND hWndGetFocus)
	{
		UNREFERENCED_PARAMETER(hWndGetFocus);
	}

	void OnLButtonDblClk(DWORD keys, short x, short y)
	{
		UNREFERENCED_PARAMETER(keys);
		UNREFERENCED_PARAMETER(x);
		UNREFERENCED_PARAMETER(y);
	}

	void OnLButtonDown(DWORD keys, short x, short y)
	{
		UNREFERENCED_PARAMETER(keys);
		UNREFERENCED_PARAMETER(x);
		UNREFERENCED_PARAMETER(y);
	}

	void OnLButtonUp(DWORD keys, short x, short y)
	{
		UNREFERENCED_PARAMETER(keys);
		UNREFERENCED_PARAMETER(x);
		UNREFERENCED_PARAMETER(y);
	}

	void OnMeasureItem(UINT_PTR id, MEASUREITEMSTRUCT *pMIS)
	{
		UNREFERENCED_PARAMETER(id);
		UNREFERENCED_PARAMETER(pMIS);
	}

	void OnMouseLeave()
	{
	}

	void OnMouseMove(DWORD keys, short x, short y)
	{
		UNREFERENCED_PARAMETER(keys);
		UNREFERENCED_PARAMETER(x);
		UNREFERENCED_PARAMETER(y);
	}

	void OnMouseWheel(WORD keys, short delta, short x, short y)
	{
		UNREFERENCED_PARAMETER(keys);
		UNREFERENCED_PARAMETER(delta);
		UNREFERENCED_PARAMETER(x);
		UNREFERENCED_PARAMETER(y);
	}

	void OnMove(short x, short y)
	{
		UNREFERENCED_PARAMETER(x);
		UNREFERENCED_PARAMETER(y);
	}

	LRESULT OnNotify(UINT_PTR id, NMHDR *pNMH)
	{
		return DefWindowProc(m_hWnd, WM_NOTIFY, id, (LPARAM)pNMH);
	}

	void OnPaint(HDC hDC)
	{
		UNREFERENCED_PARAMETER(hDC);
	}

	void OnPrefChanged()
	{
	}

	void OnScaleChanged()
	{
	}

	bool OnSetCursor(HWND hWnd, WORD hitTest, WORD mouseMsg)
	{
		return DefWindowProc(m_hWnd, WM_SETCURSOR, (WPARAM)hWnd, MAKELPARAM(hitTest, mouseMsg)) != FALSE;
	}

	void OnSetFocus(HWND hWndLoseFocus)
	{
		UNREFERENCED_PARAMETER(hWndLoseFocus);
	}

	void OnSetFont(HFONT hFont, bool bRedraw)
	{
		UNREFERENCED_PARAMETER(hFont);
		UNREFERENCED_PARAMETER(bRedraw);
	}

	LRESULT OnSetText(const TCHAR *lpszText)
	{
		return DefWindowProc(m_hWnd, WM_SETTEXT, 0, (LPARAM)lpszText);
	}

	void OnSize(DWORD type, WORD cx, WORD cy)
	{
		UNREFERENCED_PARAMETER(type);
		UNREFERENCED_PARAMETER(cx);
		UNREFERENCED_PARAMETER(cy);
	}

	void OnSizing(DWORD side, RECT *pRect)
	{
		UNREFERENCED_PARAMETER(side);
		UNREFERENCED_PARAMETER(pRect);
	}

	void OnTimer(UINT_PTR nIDEvent)
	{
		UNREFERENCED_PARAMETER(nIDEvent);
	}

	void OnTouch(WORD nInputs, HTOUCHINPUT hTouchInput)
	{
		UNREFERENCED_PARAMETER(nInputs);
		UNREFERENCED_PARAMETER(hTouchInput);
	}

	void OnVScroll(WORD scrollCode, WORD pos, HWND hWndScrollBar)
	{
		UNREFERENCED_PARAMETER(scrollCode);
		UNREFERENCED_PARAMETER(pos);
		UNREFERENCED_PARAMETER(hWndScrollBar);
	}

	HWND m_hWnd;
};

#endif // __RWINDOW_H__
