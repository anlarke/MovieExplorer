#ifndef __RDIALOG_H__
#define __RDIALOG_H__

class RDialog : public RWindow
{
	friend class RWindow;

public:
	RDialog()
	{
	}

	~RDialog()
	{
	}

	template <class DERIVED_TYPE>
	bool Create(HWND hWndParent, const TCHAR *lpszName, int x, int y, int cx, int cy, 
			bool bResizable = false, bool bContextHelp = false)
	{
		m_bResizable = bResizable;
		m_bSizing = false;
		memset(&m_ptSizing, 0, sizeof(POINT));
		memset(&m_rcSizing, 0, sizeof(RECT));
		memset(&m_rcSizeBox, 0, sizeof(RECT));

		if (!RWindow::Create<DERIVED_TYPE>(hWndParent, WS_POPUP|WS_CAPTION|WS_SYSMENU|
				WS_CLIPCHILDREN|(bResizable ? WS_THICKFRAME : 0), WS_EX_DLGMODALFRAME|
				(bContextHelp ? WS_EX_CONTEXTHELP : 0), lpszName, x, y, cx, cy))
			ASSERTRETURN(false);

		m_hbrBackgr = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
		SetClassLongPtr(m_hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)m_hbrBackgr);

		return true;
	}

	template <class DERIVED_TYPE>
	bool ShowModal(bool bCenterToParentWnd = false)
	{
		if (!IsWindow(m_hWnd))
			ASSERTRETURN(false);

		HWND hWndParent = GetParent(m_hWnd);

		// Center dialog relatively to parent window

		if (bCenterToParentWnd)
		{
			RRect rc, rcParent;
			GetWindowRect(m_hWnd, &rc);
			GetWindowRect(hWndParent, &rcParent);
			MoveWindow(m_hWnd, rcParent.x + (rcParent.cx - rc.cx) / 2,
					rcParent.y + (rcParent.cy - rc.cy) / 2, rc.cx, rc.cy, TRUE);
		}

		// Show the dialog and disable the parent window

		ShowWindow(m_hWnd, SW_SHOW);
		EnableWindow(hWndParent, FALSE);

		// Substitute message loop 

		MSG msg;

		while (true)
		{
			if (GetMessage(&msg, NULL, 0, 0) > 0)
			{
				if (msg.message == WM_SYSKEYDOWN)
					continue; // otherwise controls disappear when ALT key is pressed

				if (!((DERIVED_TYPE*)this)->PreTranslateMessage(&msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				if (msg.message == WM_ENDMODAL)
					break;
				else if (msg.message == WM_SYSCOMMAND && msg.wParam == SC_CLOSE)
				{
					PostMessage(msg.hwnd, WM_CLOSE, 0, 0);
					break;
				}
			}
			else
			{
				PostQuitMessage(0); // post to main message loop
				break;
			}
		}

		// Hide the dialog and enable the parent window again

		EnableWindow(hWndParent, TRUE);
		ShowWindow(m_hWnd, SW_HIDE);

		return true;
	}

protected:
	bool PreTranslateMessage(MSG *pMsg)
	{
		if (IsDialogMessage(m_hWnd, pMsg)) // handles keyboard input like TAB etc
			return true;

		return false;
	}

	void OnClose()
	{
		PostMessage(m_hWnd, WM_ENDMODAL, 0, 0);
	}

	HBRUSH OnCtlColorStatic(HDC hDC, HWND hWnd)
	{
		RECT rc;
		GetClientRect(hWnd, &rc);
		COLORREF clrBackgr = GetSysColor(COLOR_BTNFACE);
		SetBkColor(hDC, clrBackgr);
		FillSolidRect(hDC, &rc, clrBackgr);
		return m_hbrBackgr;
	}

	HBRUSH OnCtlColorBtn(HDC hDC, HWND hWnd)
	{
		return OnCtlColorStatic(hDC, hWnd);
	}

	void OnLButtonDown(DWORD keys, short x, short y)
	{
		if (m_bResizable)
		{
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(m_hWnd, &pt);
			if (PtInRect(&m_rcSizeBox, pt))
			{
				m_bSizing = true;
				GetWindowRect(m_hWnd, &m_rcSizing);
				m_ptSizing.x = x;
				m_ptSizing.y = y;
				SetCapture(m_hWnd);
			}
		}
	}

	void OnLButtonUp(DWORD keys, short x, short y)
	{
		if (m_bSizing)
		{
			m_bSizing = false;
			memset(&m_rcSizing, 0, sizeof(RECT));
			memset(&m_ptSizing, 0, sizeof(POINT));
			ReleaseCapture();
		}
	}

	void OnMouseMove(DWORD keys, short x, short y)
	{
		if (m_bSizing)
		{
			RECT rc = m_rcSizing;
			rc.right += x - m_ptSizing.x;
			rc.bottom += y - m_ptSizing.y;
			SendMessage(m_hWnd, WM_SIZING, WMSZ_BOTTOMRIGHT, (LPARAM)&rc);//OnSizing(WMSZ_BOTTOMRIGHT, &rc);
			MoveWindow(m_hWnd, &rc);
		}
	}

	void OnPaint(HDC hDC)
	{
		// Fill background

		RECT rc;
		GetClientRect(m_hWnd, &rc);
		FillRect(hDC, &rc, (HBRUSH)GetClassLongPtr(m_hWnd, GCLP_HBRBACKGROUND));

		// Draw gripper when resizable

		if (m_bResizable)
		{
#ifdef _UXTHEME_H_
			if (IsAppThemed())
			{
				HTHEME hTheme = OpenThemeData(m_hWnd, L"SCROLLBAR");
				ASSERT(hTheme);
				DrawThemeBackground(hTheme, hDC, SBP_SIZEBOX, SZB_HALFBOTTOMRIGHTALIGN, 
						&m_rcSizeBox, NULL);
				CloseThemeData(hTheme);
			}
			else
#endif
				DrawFrameControl(hDC, &m_rcSizeBox, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
		}
	}

	bool OnSetCursor(HWND hWnd, WORD hitTest, WORD mouseMsg)
	{
		if (hWnd == m_hWnd)
		{
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(m_hWnd, &pt);
			if (PtInRect(&m_rcSizeBox, pt))
			{
				SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
				return true;
			}
		}
		return RWindow::OnSetCursor(hWnd, hitTest, mouseMsg);
	}

	void OnSize(DWORD type, WORD cx, WORD cy)
	{
		if (m_bResizable)
		{
			m_rcSizeBox.left = cx - GetSystemMetrics(SM_CXHTHUMB) + 1;
			m_rcSizeBox.top = cy - GetSystemMetrics(SM_CYVTHUMB);
			m_rcSizeBox.right = cx;
			m_rcSizeBox.bottom = cy;
		}
	}

	bool m_bResizable, m_bSizing;
	POINT m_ptSizing;
	RECT m_rcSizing, m_rcSizeBox;
	HBRUSH m_hbrBackgr;
};

#endif // __RDIALOG_H__
