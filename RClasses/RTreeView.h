#ifndef __RTREEVIEW_H__
#define __RTREEVIEW_H__

class RTreeView : public RControl
{
	friend class RWindow;
	friend class RControl;

public:
	RTreeView()
	{
	}

	~RTreeView()
	{
	}

	template <class DERIVED_TYPE>
	bool Create(HWND hWndParent, DWORD style = 0, DWORD exStyle = 0, int x = 0, int y = 0, 
			int cx = 0, int cy = 0, UINT_PTR id = 0, void *pParam = NULL)
	{
		if (!RControl::Create<DERIVED_TYPE>(WC_TREEVIEW, hWndParent, WS_CHILD|WS_VISIBLE|
				WS_CLIPCHILDREN|WS_CLIPSIBLINGS|style, exStyle, NULL, x, y, cx, cy, id, pParam))
			ASSERTRETURN(false);

		SetFont(GetDialogFont());

#ifdef _UXTHEME_H_
		if (IsAppThemed())
			SetWindowTheme(m_hWnd, L"Explorer", NULL);
#endif

		return true;
	}

	HTREEITEM InsertItem(const TCHAR *lpszText, LPARAM lParam = 0, HTREEITEM hParent = TVI_ROOT, 
			HTREEITEM hInsertAfter = TVI_LAST)
	{
		TVINSERTSTRUCT insert;
		insert.hParent = hParent;
		insert.hInsertAfter = hInsertAfter;
		insert.item.mask = TVIF_TEXT|TVIF_PARAM;
		insert.item.pszText = (TCHAR*)lpszText;
		insert.item.cchTextMax = (int)_tcslen(lpszText);
		insert.item.lParam = lParam;
		
		return (HTREEITEM)SendMessage(m_hWnd, TVM_INSERTITEM, 0, (LPARAM)&insert);
	}

	bool SelectItem(HTREEITEM hItem)
	{
		return SendMessage(m_hWnd, TVM_SELECTITEM, TVGN_CARET, (LPARAM)hItem);
	}

	bool DeleteItem(HTREEITEM hItem)
	{
		return SendMessage(m_hWnd, TVM_DELETEITEM, 0, (LPARAM)hItem);
	}

	bool DeleteAllItems()
	{
		return SendMessage(m_hWnd, TVM_DELETEITEM, 0, NULL);
	}

	int SetItemHeight(int nHeight)
	{
		return (int)SendMessage(m_hWnd, TVM_SETITEMHEIGHT, nHeight);
	}

	HTREEITEM GetSelectedItem()
	{
		return (HTREEITEM)SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_CARET, 0);
	}

	RString GetItemText(HTREEITEM hItem)
	{
		ASSERT(hItem);
		RString str;
		TVITEM tvi;
		ZeroMemory(&tvi, sizeof(TVITEM));
		tvi.mask = TVIF_HANDLE|TVIF_TEXT;
		tvi.hItem = hItem;
		tvi.cchTextMax = 256;
		tvi.pszText = str.GetBuffer(256);
		VERIFY(SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&tvi));
		str.ReleaseBuffer();
		return str;
	}

	bool SetItemText(HTREEITEM hItem, const TCHAR *lpszText)
	{
		ASSERT(hItem);
		ASSERT(lpszText);
		
		TVITEM tvi;
		ZeroMemory(&tvi, sizeof(TVITEM));
		tvi.mask = TVIF_HANDLE|TVIF_TEXT;
		tvi.hItem = hItem;
		tvi.pszText = (TCHAR*)lpszText;
		
		return SendMessage(m_hWnd, TVM_SETITEM, 0, (LPARAM)&tvi);
	}

	LPARAM GetItemData(HTREEITEM hItem)
	{
		ASSERT(hItem);
		TVITEM tvi;
		memset(&tvi, 0, sizeof(TVITEM));
		tvi.mask = TVIF_HANDLE|TVIF_PARAM;
		tvi.hItem = hItem;
		VERIFY(SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&tvi));
		return tvi.lParam;
	}

	bool SetItemData(HTREEITEM hItem, LPARAM lParam)
	{
		ASSERT(hItem);
		TVITEM tvi;
		memset(&tvi, 0, sizeof(TVITEM));
		tvi.mask = TVIF_HANDLE|TVIF_PARAM;
		tvi.hItem = hItem;
		tvi.lParam = lParam;
		return SendMessage(m_hWnd, TVM_SETITEM, 0, (LPARAM)&tvi);
	}

	HWND EditLabel(HTREEITEM hItem)
	{
		SetFocus(m_hWnd);
		return (HWND)SendMessage(m_hWnd, TVM_EDITLABEL, 0, (LPARAM)hItem);
	}

	HTREEITEM GetNextItem(HTREEITEM hItem, UINT nCode = TVGN_NEXT)
	{
		ASSERT(hItem);
		return (HTREEITEM)SendMessage(m_hWnd, TVM_GETNEXTITEM, nCode, (LPARAM)hItem);
	}

	HTREEITEM GetPrevItem(HTREEITEM hItem)
	{
		ASSERT(hItem);
		return (HTREEITEM)SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_PREVIOUS, (LPARAM)hItem);
	}

	HTREEITEM GetRootItem()
	{
		ASSERT(IsWindow(m_hWnd));
		return (HTREEITEM)SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_ROOT, 0);
	}

	HWND GetEditControl()
	{
		//if (!m_hWnd || !IsWindow(m_hWnd))
		//	return NULL;
		return (HWND)SendMessage(m_hWnd, TVM_GETEDITCONTROL, 0, 0);
	}
};

class RTreeViewFF : public RControlFF<RTreeView>
{
	friend class RWindow;
	friend class RControl;

public:
	RTreeViewFF()
	{
	}

	~RTreeViewFF()
	{
	}
};

#endif // __RTREEVIEW_H__
