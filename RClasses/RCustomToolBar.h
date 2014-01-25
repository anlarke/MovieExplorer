#ifndef __RCUSTOMTOOLBAR_H__
#define __RCUSTOMTOOLBAR_H__

class RCustomToolBar : public RWindow
{
	friend class RWindow;

public:
	RCustomToolBar() : m_nIconSize(20)
	{
	}

	~RCustomToolBar()
	{
	}

	INT_PTR AddButton(WORD idCommand, RMemoryDC *pIconMDC)
	{
		RCustomToolBarButton *pButton = new RCustomToolBarButton;
		m_buttons.Add(pButton);
		if (!pButton->Create<RCustomToolBarButton>(m_hWnd, idCommand, pIconMDC))
			{ASSERT(false); m_buttons.SetSize(m_buttons-1); return -1;}
		return m_buttons-1;
	}

protected:
	void OnCommand(WORD id, WORD notifyCode, HWND hWndControl)
	{
		PostMessage(GetParent(m_hWnd), WM_COMMAND, MAKEWPARAM(id, notifyCode), (LPARAM)m_hWnd);
	}

	void OnDestroy()
	{
		foreach (m_buttons, pButton)
			delete pButton;
		m_buttons.SetSize(0);
	}

	void OnPaint(HDC hDC)
	{
		m_mdc.Draw(hDC);
	}

	void OnSize(DWORD type, WORD cx, WORD cy)
	{
		m_mdc.Create(cx, cy);

		RECT rc;
		GetClientRectRelative(m_hWnd, GetParent(m_hWnd), &rc);
		SetWindowOrgEx(m_mdc, rc.left, rc.top, NULL);
		SendMessage(GetParent(m_hWnd), WM_PAINT, (WPARAM)(HDC)m_mdc, 0);
		SetWindowOrgEx(m_mdc, 0, 0, NULL);

		int xButton = SCX(1);
		int yButton = SCX(1);
		int cxButton = SCX(m_nIconSize);
		int cyButton = SCX(m_nIconSize);

		foreach (m_buttons, pButton)
		{
			MoveWindow(*pButton, xButton, yButton, cxButton, cyButton);
			xButton += cxButton + SCX(2);
		}

		PostChildren(m_hWnd, WM_PAINT);
	}

	RMemoryDC m_mdc;
	int m_nIconSize;
	RArray<RCustomToolBarButton*> m_buttons; // pointer is saved as window data, can't move
};









/*
struct CUSTOMTOOLBARBUTTON
{
	WORD idCommand;
	RMemoryDC *pImageDC;
	RRect rc;
	RString strText;
	bool bEnabled, bChecked;
};

class RCustomToolBar : public RWindow
{
	friend class RWindow;

public:
	RCustomToolBar() : m_nIconSize(16), m_cx(0), m_cy(0), m_nHighlight(-1), m_nPressed(-1), m_clrText(0x171717)
	{
	}

	~RCustomToolBar()
	{
	}

	template <class DERIVED_TYPE>
	bool Create(HWND hWndParent, int nIconSize = 16)
	{
		if (m_nIconSize <= 0)
			ASSERTRETURN(false);

		if (!RWindow::Create<DERIVED_TYPE>(hWndParent))
			ASSERTRETURN(false);

		m_nIconSize = nIconSize;
		SetFont(GetDialogFont());

		return true;
	}

	bool AddButton(WORD idCommand, RMemoryDC *pImageDC, const TCHAR* lpszText = NULL)
	{
		if (!IsWindow(m_hWnd))
			ASSERTRETURN(false);

		if (!pImageDC && !lpszText)
			ASSERTRETURN(false); // at least supply image or text

		CUSTOMTOOLBARBUTTON *pButton = m_buttons.AddNew();
		pButton->idCommand = idCommand;
		pButton->pImageDC = pImageDC;
		pButton->strText = lpszText;
		pButton->bEnabled = true;
		pButton->bChecked = false;

		CalculateRects();

		Draw();
		Invalidate(m_hWnd);

		return true;
	}

	bool GetDimensions(int *pnWidth, int *pnHeight)
	{
		if (!IsWindow(m_hWnd))
			ASSERTRETURN(false);
		
		*pnWidth = m_cx;
		*pnHeight = m_cy;

		return true;
	}

protected:
	void OnPaint(HDC hDC)
	{
		m_mdc.Draw(hDC);
		//FillSolidRect(hDC, &rc, 0x00FF00);
	}

	void OnScaleChanged()
	{
		CalculateRects();

		CreateSprHighlight();
		CreateSprPressed();

		Draw();
		Invalidate(m_hWnd);
	}

	void OnSize(DWORD type, WORD cx, WORD cy)
	{
		VERIFY(m_mdc.Create(cx, cy));
		Draw();
	}

	void CalculateRects()
	{
		if (m_buttons == 0)
			return;

		SIZE sz;
		RMemoryDC mdcTemp(0, 0);
		HFONT hOldFont = (HFONT)SelectObject(mdcTemp, GetDialogFont());
		GetTextExtentPoint32(mdcTemp, _T("Ag"), 2, &sz);
		int nButtonHeight = ((sz.cy + SCY(3)) / 2) * 2; // want an even value

		INT_PTR i;
		for (i = 0; i < m_buttons.GetSize(); ++i)
			if (m_buttons[i].pImageDC)
				{nButtonHeight = SCX(m_nIconSize) + SCY(6); break;}
		
		int offsetLeft = m_buttons[0].pImageDC ? 0 : 0;
		int offsetRight = 0;
		int offsetTop = 0;
		int offsetBottom = 0;

		foreach (m_buttons, button)
		{
			button.rc.left = button.rc.right = offsetLeft;
			button.rc.top = offsetTop;
			button.rc.bottom = offsetTop + nButtonHeight;

			if (button.idCommand == 0xFFFFFFFF) // separator
			{
				button.rc.right += 7;
				button.rc.bottom = offsetTop;
			}

			if (!button.strText.IsEmpty()) // has text
			{
				GetTextExtentPoint32(mdcTemp, button.strText, &sz);
				button.rc.right += sz.cx + SCX(12);
			}

			if (button.pImageDC) // has image
			{
				button.rc.right += SCX(m_nIconSize) + SCX(6);
				if (!button.strText.IsEmpty())
					button.rc.right -= SCX(6);
			}
			
			offsetLeft = button.rc.right + SCX(2);
		}

		SelectObject(mdcTemp, hOldFont);

		m_cx = m_buttons[m_buttons-1].rc.right + offsetRight;
		m_cy = m_buttons[m_buttons-1].rc.bottom + offsetBottom;
	}

	virtual void CreateSprHighlight()
	{
		int r = SCX(3);
		int cxSpr = m_cy;
		int cySpr = m_cy;

		m_sprHighlight.Create(cxSpr, cySpr, 10, 0, 10, 0);
			
		//DrawRoundedRect(m_sprHighlight, 0, 0, cxSpr, cySpr, r, 0, false, 71);
		//DrawRoundedRect(m_sprHighlight, 1, 1, cxSpr - 2, cySpr - 2, r - 1, 0xFFFFFF, false, 92);
		//DrawRoundedRectGradientV(m_sprHighlight, 1, 1, cxSpr - 2, cySpr / 2 - 1, r - 1, 0xFFFFFF, 0xFFFFFF, true, 3, 26, DRR_TL|DRR_TR);
		//DrawRoundedRectGradientV(m_sprHighlight, 1, cySpr / 2, cxSpr - 2, cySpr / 2 - 1, r - 1, 0, 0, true, 10, 0, DRR_BL|DRR_BR);
			
		if (IsAppThemed())
		{
			RECT rc = {0, -1, cxSpr, cySpr+1};
			HTHEME hTheme = OpenThemeData(m_hWnd, _T("TOOLBAR"));
			DrawThemeBackground(hTheme, m_sprHighlight, TP_BUTTON, TS_HOT, &rc, NULL);
			CloseThemeData(hTheme);
		}
		else
		{
			RECT rc = {1, 1, cxSpr - 1, cySpr - 1};
			DrawEdge(m_sprHighlight, &rc, BDR_RAISEDINNER, BF_RECT);
			RGBQUAD *pPixels = m_sprHighlight.GetPixels();
			for (int i = 0; i < cxSpr * cySpr; i++)
				if (*(DWORD*)(pPixels + i) > 0)
					pPixels[i].rgbReserved = 0xFF;
		}
	}

	virtual void CreateSprPressed()
	{
		int r = SCX(3);
		int cxSpr = m_cy;
		int cySpr = m_cy;

		m_sprPressed.Create(cxSpr, cySpr, 10, 0, 10, 0);
			
		//DrawRoundedRect(m_sprPressed, 0, 0, cxSpr, cySpr, r, 0, false, 150);
		//DrawRoundedRect(m_sprPressed, 1, 1, cxSpr - 2, cySpr - 2, r - 1, 0, false, 10);
		//DrawRoundedRectGradientV(m_sprPressed, 1, 1, cxSpr - 2, cySpr - 2, r - 1, 0, 0, true, 26, 6);
		//DrawRect(m_sprPressed, 2, 1, cxSpr - 4, 1, 0, true, 36);
		//DrawRect(m_sprPressed, 1, 2, cxSpr - 2, 1, 0, true, 10);
		//DrawRect(m_sprPressed, 1, 2, 1, cySpr - 4, 0, true, 30);
		//DrawRect(m_sprPressed, 2, 1, 1, cySpr - 2, 0, true, 8);
			
		if (IsAppThemed())
		{
			RECT rc = {0, -1, cxSpr, cySpr+1};
			HTHEME hTheme = OpenThemeData(m_hWnd, _T("TOOLBAR"));
			DrawThemeBackground(hTheme, m_sprPressed, TP_BUTTON, TS_PRESSED, &rc, NULL);
			CloseThemeData(hTheme);
		}
		else
		{
			RECT rc = {1, 1, cxSpr - 1, cySpr - 1};
			DrawEdge(m_sprPressed, &rc, BDR_SUNKENOUTER, BF_RECT);
			RGBQUAD *pPixels = m_sprPressed.GetPixels();
			for (int i = 0; i < cxSpr * cySpr; i++)
				if (*(DWORD*)(pPixels + i) > 0)
					pPixels[i].rgbReserved = 0xFF;
		}
	}

	virtual void CreateSprSeparator()
	{
		// TODO
	}

	void Draw()
	{
		// Copy background from parent window

		RECT rc;
		GetClientRectRelative(m_hWnd, GetParent(m_hWnd), &rc);
		SetWindowOrgEx(m_mdc, rc.left, rc.top, NULL);
		SendMessage(GetParent(m_hWnd), WM_PAINT, (WPARAM)(HDC)m_mdc, 0);
		SetWindowOrgEx(m_mdc, 0, 0, NULL);

		// Draw buttons in correct state

		for (INT_PTR i = 0; i < m_buttons; i++)
		{
			CUSTOMTOOLBARBUTTON &button = m_buttons[i];

			// separator

			if (button.idCommand == 0xFFFFFFFF)
			{
				DrawRect(m_mdc, button.rc.left + 3, 1, 1, m_cy - 2, 0x000000, 20);
				//DrawRect(m_mdc, button.rc.left + 4, 1, 1, m_cy - 2, 0xFFFFFF, 14);
				continue;
			}

			// pressed or highlighted

			if (i == m_nPressed || button.bChecked)
				m_sprPressed.Draw(m_mdc, &button.rc, true);
			else if (i == m_nHighlight)
				m_sprHighlight.Draw(m_mdc, &button.rc, true);

			// draw image

			if (button.pImageDC)
				AlphaBlt(m_mdc, button.rc.left + SCX(3), button.rc.top + SCY(3), button.rc.Width(),
						button.rc.Height(), *button.pImageDC);
			
			// draw text

			SetTextColor(m_mdc, m_clrText);
			
			if (button.pImageDC)
				button.rc.left += m_nIconSize + SCX(2); // temporary offset

			HFONT hOldFont = (HFONT)SelectObject(m_mdc, GetDialogFont());
			DrawText(m_mdc, button.strText, &button.rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			SelectObject(m_mdc, hOldFont);

			if (button.pImageDC)
				button.rc.left -= m_nIconSize + SCX(2);
		}
	}

	int m_nIconSize, m_cx, m_cy;
	INT_PTR m_nHighlight, m_nPressed;
	RObArrayNRC<CUSTOMTOOLBARBUTTON> m_buttons;
	RMemoryDC m_mdc;
	RSprite m_sprHighlight, m_sprPressed, m_sprSeparator;
	COLORREF m_clrText;
};
*/

#endif // __RCUSTOMTOOLBAR_H__
