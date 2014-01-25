#ifndef __RPROPERTYSHEET_H__
#define __RPROPERTYSHEET_H__

class RPropertySheet;

////////////////////////////////////////////////////////////////////////////////////////////////////
// RPropertyPage

class RPropertyPage : public RWindow
{
	friend class RWindow;
	friend class RPropertySheet;

public:
	RPropertyPage()
	{
	}

	~RPropertyPage()
	{
		DestroyWindow(m_hWnd);
	}

	template <class DERIVED_TYPE>
	bool Create(HWND hPropertySheet, HWND hTabControl)
	{
		if (!hPropertySheet || !hTabControl)
			ASSERTRETURN(false);

		m_hTabControl = hTabControl;
		m_bChanged = false;

		if (!RWindow::Create<DERIVED_TYPE>(hPropertySheet, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|
				WS_CLIPSIBLINGS, WS_EX_CONTROLPARENT))
			{m_hTabControl = NULL; ASSERTRETURN(false);}

		m_hbrBackgr = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
		SetClassLongPtr(m_hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)m_hbrBackgr);

		return true;
	}

	virtual void SetChanged()
	{
		m_bChanged = true;
		PostMessage(GetParent(m_hWnd), PSM_CHANGED, 0, 0);
	}

	virtual void ApplyChanges()
	{
		m_bChanged = false;
	}

protected:
	void OnPaint(HDC hDC)
	{
		if (IsAppThemed())
		{
			RECT rc;
			GetClientRectRelative(m_hWnd, m_hTabControl, &rc);
			POINT pt;
			SetWindowOrgEx(hDC, rc.left, rc.top, &pt);
			SendMessage(m_hTabControl, WM_PRINTCLIENT, (WPARAM)hDC, PRF_CLIENT);
			SetWindowOrgEx(hDC, pt.x, pt.y, NULL);
		}
		else
		{
			RECT rc;
			GetClientRect(m_hWnd, &rc);
			FillSolidRect(hDC, &rc, GetSysColor(COLOR_BTNFACE));
		}
	}

	HBRUSH OnCtlColorStatic(HDC hDC, HWND hWnd)
	{
		if (IsAppThemed())
		{
			RECT rc;
			GetClientRectRelative(hWnd, m_hTabControl, &rc);
			POINT pt;
			SetWindowOrgEx(hDC, rc.left, rc.top, &pt);
			SendMessage(m_hTabControl, WM_PRINTCLIENT, (WPARAM)hDC, PRF_CLIENT);
			SetWindowOrgEx(hDC, pt.x, pt.y, NULL);
			SetBkColor(hDC, GetSysColor(COLOR_BTNFACE));
			SetBkMode(hDC, TRANSPARENT);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}

		COLORREF clrBackgr = GetSysColor(COLOR_BTNFACE);
		SetBkColor(hDC, clrBackgr);
		RECT rc;
		GetClientRect(hWnd, &rc);
		FillSolidRect(hDC, &rc, clrBackgr);
		return m_hbrBackgr;
	}

	HBRUSH OnCtlColorBtn(HDC hDC, HWND hWnd)
	{
		return OnCtlColorStatic(hDC, hWnd);
	}

	HWND m_hTabControl;
	bool m_bChanged;
	HBRUSH m_hbrBackgr;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// RPropertySheet

class RPropertySheet : public RDialog
{
	friend class RWindow;

public:
	RPropertySheet()
	{
	}

	~RPropertySheet()
	{
		DestroyWindow(m_hWnd);
	}

	template <class DERIVED_TYPE>
	bool Create(HWND hWndParent, const TCHAR *lpszName, const TCHAR *lpszOK, 
			const TCHAR *lpszCancel, const TCHAR *lpszApply, int x, int y, int cx, int cy, 
			bool bResizable = false, bool bContextHelp = false)
	{
		m_lpszOK = lpszOK;
		m_lpszCancel = lpszCancel;
		m_lpszApply = lpszApply;
		m_nActivePage = 0;

		if (!RDialog::Create<DERIVED_TYPE>(hWndParent, lpszName, x, y, cx, cy, 
				bResizable, bContextHelp))
		{
			m_lpszOK = m_lpszCancel = m_lpszApply = NULL;
			ASSERTRETURN(false);
		}

		return true;
	}

	template <class DERIVED_TYPE>
	bool AddPage(RPropertyPage *pPage, const TCHAR *lpszName)
	{
		if (!pPage->Create<DERIVED_TYPE>(m_hWnd, m_tabControl))
			ASSERTRETURN(false);

		m_tabControl.AddItem(lpszName);
		m_pages.Add(pPage);

		if (m_pages == 1)
		{
			ShowWindow(*pPage, SW_SHOW);
			SetWindowPos(*pPage, m_tabControl, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);

			RECT rc;
			GetClientRect(m_hWnd, &rc);
			PostMessage(m_hWnd, WM_SIZE, 0, MAKELONG(rc.right, rc.bottom));
		}
		else
		{
			ShowWindow(*pPage, SW_HIDE);
			SetWindowPos(*pPage, *m_pages[m_pages-1], 0, 0, 0, 0, 
					SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
		}

		return true;
	}

	virtual void ApplyChanges()
	{
		foreach (m_pages, pPage)
		{
			if (pPage->m_bChanged)
				pPage->ApplyChanges();
		}

		PostMessage(m_hWnd, WM_PREFCHANGED);
		PostChildren(m_hWnd, WM_PREFCHANGED);
		PostMessage(GetParent(m_hWnd), WM_PREFCHANGED);

		EnableWindow(m_btnApply, FALSE);
	}

protected:
	LRESULT WndProc(UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (Msg == PSM_CHANGED)
			{OnChanged(); return 0;}

		return RDialog::WndProc(Msg, wParam, lParam);
	}

	bool OnCreate(CREATESTRUCT *pCS)
	{
		if (!RDialog::OnCreate(pCS))
			ASSERTRETURN(false);

		if (!m_tabControl.Create<RTabControl>(m_hWnd, WS_TABSTOP) ||
				!m_btnOK.Create<RButton>(m_hWnd, WS_TABSTOP|BS_DEFPUSHBUTTON, 0, m_lpszOK) ||
				!m_btnCancel.Create<RButton>(m_hWnd, WS_TABSTOP, 0, m_lpszCancel) ||
				!m_btnApply.Create<RButton>(m_hWnd, WS_TABSTOP|WS_DISABLED, 0, m_lpszApply))
			ASSERTRETURN(false);

		return true;
	}

	void OnDestroy()
	{
		m_lpszOK = m_lpszCancel = m_lpszApply = NULL;
		m_pages.SetSize(0);
		m_nActivePage = 0;

		RDialog::OnDestroy();
	}

	void OnSize(DWORD type, WORD cx, WORD cy)
	{
		RDialog::OnSize(type, cx, cy);

		MoveWindow(m_tabControl, DUX(3)+1, DUY(3), cx - DUX(6), cy - DUY(26));
		MoveWindow(m_btnApply, cx - DUX(53), cy - DUY(20), DUX(50), DUY(14));
		MoveWindow(m_btnCancel, cx - DUX(106), cy - DUY(20), DUX(50), DUY(14));
		MoveWindow(m_btnOK, cx - DUX(159), cy - DUY(20), DUX(50), DUY(14));

		if (m_nActivePage < m_pages)
		{
			// Move active page to correct position

			RECT rc;
			GetClientRectRelative(m_tabControl, m_hWnd, &rc);
			m_tabControl.AdjustRect(false, &rc);
			MoveWindow(*m_pages[m_nActivePage], &rc, TRUE);

			// We 'cut a hole' in the tab control to make page visible (page is lower in z-order,
			// but that is intentional so now the tab order is correct)

			GetClientRect(m_tabControl, &rc);
			HRGN hRgn = CreateRectRgnIndirect(&rc);
			m_tabControl.AdjustRect(false, &rc);
			HRGN hPageRgn = CreateRectRgnIndirect(&rc);
			CombineRgn(hRgn, hRgn, hPageRgn, RGN_DIFF);
			DeleteObject(hPageRgn);
			SetWindowRgn(m_tabControl, hRgn, TRUE);
		}

		PostMessage(m_hWnd, WM_PAINT);
		PostChildren(m_hWnd, WM_PAINT);
	}

	void OnCommand(WORD id, WORD notifyCode, HWND hWndControl)
	{
		if (notifyCode == BN_CLICKED)
		{
			// IsDialogMessage sends WM_COMMAND messages with wID set, we don't work with 
			// IDs but with window handles.

			if (id == IDOK)
				hWndControl = m_btnOK;
			else if (id == IDCANCEL)
				hWndControl = m_btnCancel;

			// Handle button input

			if (hWndControl == m_btnApply || hWndControl == m_btnOK)
				ApplyChanges();
			if (hWndControl == m_btnOK || hWndControl == m_btnCancel)
				PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		}

		RDialog::OnCommand(id, notifyCode, hWndControl);
	}

	LRESULT OnNotify(UINT_PTR id, NMHDR *pNMH)
	{
		if (pNMH->code == TCN_SELCHANGE && pNMH->hwndFrom == m_tabControl)
		{
			m_nActivePage = max(min(m_tabControl.GetCurSel(), m_pages - 1), 0);

			if (m_nActivePage < m_pages && !IsWindowVisible(*m_pages[m_nActivePage]))
				ShowWindow(*m_pages[m_nActivePage], SW_SHOW);

			if (m_pages > 1)
				for (INT_PTR i = 0; i < m_pages; ++i)
					if (i != m_nActivePage)
						ShowWindow(*m_pages[i], SW_HIDE);

			RECT rc;
			GetClientRect(m_hWnd, &rc);
			PostMessage(m_hWnd, WM_SIZE, 0, MAKELONG(rc.right, rc.bottom));
		}

		return RDialog::OnNotify(id, pNMH);
	}

	virtual void OnChanged()
	{
		EnableWindow(m_btnApply, TRUE);
	}

	const TCHAR *m_lpszOK, *m_lpszCancel, *m_lpszApply;
	RTabControl m_tabControl;
	RButton m_btnOK, m_btnCancel, m_btnApply;
	RArray<RPropertyPage*> m_pages;
	INT_PTR m_nActivePage;
};

#endif // __RPROPERTYSHEET_H__
