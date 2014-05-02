#pragma once

#define CTBBSTATE_NORMAL		0
#define CTBBSTATE_HIGHLIGHT		1
#define CTBBSTATE_PRESSED		2
#define CTBBSTATE_CHECKED		3
#define CTBBSTATE_SELECTED		CTBBSTATE_CHECKED

class RCustomToolBarButton : public RWindow
{
	friend class RWindow;
	
public:
	template <class DERIVED_TYPE>
	bool Create(HWND hWndParent, WORD idCommand, RMemoryDC *pIconOrigMDC, 
			const TCHAR *lpszText = NULL, bool bCommandOnDown = false, HMENU hMenu = NULL)
	{
		if (!pIconOrigMDC && !lpszText)
			ASSERTRETURN(false);

		if (!RWindow::Create<DERIVED_TYPE>(hWndParent, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			0, NULL, 0, 0, 0, 0, hMenu, NULL))
			ASSERTRETURN(false);

		m_idCommand = idCommand;
		m_pIconOrigMDC = pIconOrigMDC;
		m_bCommandOnDown = bCommandOnDown;
		m_state = CTBBSTATE_NORMAL;
		m_clrNormalText = 0x000000;
		m_clrHighlightText = 0x000000;
		m_clrPressedText = 0x000000;
		m_hWndPrevFocus = NULL;

		return true;
	}

	void SetCheck(bool bCheck = true)
	{
		RECT rc;
		GetClientRect(m_hWnd, &rc);

		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(m_hWnd, &pt);
		
		m_state = (bCheck ? CTBBSTATE_CHECKED : (PtInRect(&rc, pt) ? 
				CTBBSTATE_HIGHLIGHT : CTBBSTATE_NORMAL));
		Draw();
	}

	bool GetCheck()
	{
		return (m_state == CTBBSTATE_CHECKED);
	}

protected:
	LRESULT WndProc(UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (Msg == WM_CAPTUREM)
		{
			OnMouseMove(0, (short)wParam, (short)lParam);
			return 0;
		}
		return RWindow::WndProc(Msg, wParam, lParam);
	}

	void OnSize(DWORD type, WORD cx, WORD cy)
	{
		UNREFERENCED_PARAMETER(type);

		m_mdc.Create(cx, cy);

		if (cx == 0 || cy == 0)
			return;

		PrepareDraw();
		Draw();
	}

	void OnPaint(HDC hDC)
	{
		m_mdc.Draw(hDC);
	}

	void OnMouseMove(DWORD keys, short x, short y)
	{
		UNREFERENCED_PARAMETER(keys);

		RECT rc;
		GetClientRect(m_hWnd, &rc);

		if (PtInRect(&rc, x, y))
		{
			if (m_state == CTBBSTATE_NORMAL)
			{
				m_state = CTBBSTATE_HIGHLIGHT;
				Draw();
			}
			SetCapture(m_hWnd);
		}
		else
		{
			if (m_state == CTBBSTATE_HIGHLIGHT)
			{
				m_state = CTBBSTATE_NORMAL;
				Draw();
				ReleaseCapture();
			}
			else if (m_state == CTBBSTATE_CHECKED)
				ReleaseCapture();
		}
	}

	void OnLButtonDown(DWORD keys, short x, short y)
	{
		UNREFERENCED_PARAMETER(keys);
		UNREFERENCED_PARAMETER(x);
		UNREFERENCED_PARAMETER(y);

		if (m_state != CTBBSTATE_CHECKED)
			m_state = CTBBSTATE_PRESSED;

		Draw();

		if (m_bCommandOnDown)
			PostMessage(GetParent(m_hWnd), WM_COMMAND, m_idCommand, (LPARAM)m_hWnd);

		m_hWndPrevFocus = SetFocus(m_hWnd);
	}

	void OnLButtonUp(DWORD keys, short x, short y)
	{
		UNREFERENCED_PARAMETER(keys);

		RECT rc;
		GetClientRect(m_hWnd, &rc);

		if (PtInRect(&rc, x, y))
		{
			if (m_state == CTBBSTATE_PRESSED || m_state == CTBBSTATE_CHECKED)
			{
				if (!m_bCommandOnDown)
					PostMessage(GetParent(m_hWnd), WM_COMMAND, m_idCommand, (LPARAM)m_hWnd);
				if (m_state == CTBBSTATE_PRESSED)
					m_state = CTBBSTATE_HIGHLIGHT; // don't uncheck, should be done by caller
			}
		}
		else
		{
			m_state = CTBBSTATE_NORMAL;
			ReleaseCapture();
		}

		Draw();
		SetFocus(m_hWndPrevFocus);
		m_hWndPrevFocus = NULL;
	}

	void OnPrefChanged()
	{
		PrepareDraw();
		Draw();
	}

	virtual void PrepareDraw()
	{
		// TODO: On WinXP the DrawThemeBackground has no alpha channel, so first copy background
		// of toolbar into the sprite, then draw these things.

		int cx, cy;
		m_mdc.GetDimensions(&cx, &cy);

		if (cy < 2 * SCX(4) + 1)
			return;

		// Normal sprite

		m_sprNormal.Create(cy, cy, SCX(4), 0, SCX(4), 0); // may stretch horizontally

		if (IsAppThemed())
		{
			RRect rc = (IsWin7() ? RRect(0, -1, cy, cy + 1) : RRect(0, 0, cy, cy));
			HTHEME hTheme = OpenThemeData(m_hWnd, _T("TOOLBAR"));
			DrawThemeBackground(hTheme, m_sprNormal, TP_BUTTON, TS_NORMAL, &rc, NULL);
			CloseThemeData(hTheme);
		}

		// Highlight sprite

		m_sprHighlight.Create(cy, cy, SCX(4), 0, SCX(4), 0); // may stretch horizontally

		if (IsAppThemed())
		{
			RRect rc = (IsWin7() ? RRect(0, -1, cy, cy + 1) : RRect(0, 0, cy, cy));
			HTHEME hTheme = OpenThemeData(m_hWnd, _T("TOOLBAR"));
			DrawThemeBackground(hTheme, m_sprHighlight, TP_BUTTON, TS_HOT, &rc, NULL);
			CloseThemeData(hTheme);
		}
		else
		{
			RECT rc = {0, 0, cy, cy};
			DrawEdge(m_sprHighlight, &rc, BDR_RAISEDINNER, BF_RECT);
			RGBQUAD *pPixels = m_sprHighlight.GetPixels();
			for (INT_PTR i = 0; i < cy * cy; ++i)
				if (*(DWORD*)(pPixels + i) > 0)
					pPixels[i].rgbReserved = 0xFF;
		}

		// Pressed sprite

		m_sprPressed.Create(cy, cy, SCX(4), 0, SCX(4), 0); // may stretch horizontally

		if (IsAppThemed())
		{
			RRect rc = (IsWin7() ? RRect(0, -1, cy, cy + 1) : RRect(0, 0, cy, cy));
			HTHEME hTheme = OpenThemeData(m_hWnd, _T("TOOLBAR"));
			DrawThemeBackground(hTheme, m_sprPressed, TP_BUTTON, TS_PRESSED, &rc, NULL);
			CloseThemeData(hTheme);
		}
		else
		{
			RECT rc = {0, 0, cy, cy};
			DrawEdge(m_sprPressed, &rc, BDR_SUNKENOUTER, BF_RECT);
			RGBQUAD *pPixels = m_sprPressed.GetPixels();
			for (INT_PTR i = 0; i < cy * cy; ++i)
				if (*(DWORD*)(pPixels + i) > 0)
					pPixels[i].rgbReserved = 0xFF;
		}

		// Resize icon

		if (m_pIconOrigMDC)
		{
			m_mdcIcon.Create(cx, cy);
			Resize(&m_mdcIcon, m_pIconOrigMDC, true);
		}
	}

	void Draw()
	{
		int cx, cy;
		m_mdc.GetDimensions(&cx, &cy);

		// Copy background from parent window

		RECT rc;
		GetClientRectRelative(m_hWnd, GetParent(m_hWnd), &rc);
		SetWindowOrgEx(m_mdc, rc.left, rc.top, NULL);
		SendMessage(GetParent(m_hWnd), WM_PAINT, (WPARAM)(HDC)m_mdc);
		SetWindowOrgEx(m_mdc, 0, 0, NULL);
		
		// Draw button in correct state

		GetClientRect(m_hWnd, &rc);

		if (m_state == CTBBSTATE_NORMAL)
			m_sprNormal.Draw(m_mdc, &rc, true);
		else if (m_state == CTBBSTATE_HIGHLIGHT)
			m_sprHighlight.Draw(m_mdc, &rc, true);
		else if (m_state == CTBBSTATE_PRESSED || m_state == CTBBSTATE_CHECKED)
			m_sprPressed.Draw(m_mdc, &rc, true);

		// Draw icon

		m_mdcIcon.Draw(m_mdc, true);

		// Draw text

		// ...

		// Copy to screen

		Invalidate(m_hWnd);
		PostMessage(m_hWnd, WM_PAINT);
	}

	RMemoryDC m_mdc, *m_pIconOrigMDC, m_mdcIcon;
	RSprite m_sprNormal, m_sprHighlight, m_sprPressed;
	UINT_PTR m_state;
	WORD m_idCommand;
	bool m_bCommandOnDown;
	COLORREF m_clrNormalText, m_clrHighlightText, m_clrPressedText;
	HWND m_hWndPrevFocus;
};
