#include "stdafx.h"
#include "MovieExplorer.h"
#include "CategoriesPage.h"

CCategoriesPage::CCategoriesPage()
{
}

CCategoriesPage::~CCategoriesPage()
{
	Destroy(); // otherwise errors... don't know why
}

void CCategoriesPage::ApplyChanges()
{
	GetDB()->CancelUpdate();
	GetDB()->m_categories = m_categories;
	foreach (GetDB()->m_categories, cat)
	{
		foreach (cat.directories, dir)
		{
			dir.pCategory = &cat;
			foreach (dir.movies, mov)
				mov.pDirectory = &dir;
		}
	}
	GetDB()->SyncAndUpdate();

	RPropertyPage::ApplyChanges();
}

void CCategoriesPage::OnCommand(WORD id, WORD notifyCode, HWND hWndControl)
{
	DBCATEGORY *pSelectedCat = NULL;
	if (m_tbtvCats.tv.GetSelectedItem())
		pSelectedCat = &m_categories[m_tbtvCats.tv.GetItemData(m_tbtvCats.tv.GetSelectedItem())];

	//
	// Categories
	//

	if (hWndControl == m_tbtvCats.tb)
	{
		RTreeView &tv = m_tbtvCats.tv;

		if (id == ID_ADD && !tv.GetEditControl())
		{
			DBCATEGORY *pNewCat = m_categories.AddNew();
			foreach (m_categories, cat)
				foreach (cat.directories, dir)
					dir.pCategory = &cat;

			HTREEITEM hItem = tv.InsertItem(_T(""), m_categories-1);
			tv.SelectItem(hItem);
			tv.EditLabel(hItem);

			SetChanged();
		}
		else if (id == ID_DELETE && pSelectedCat)
		{
			HTREEITEM hItem = tv.GetSelectedItem();
			if (!hItem)
				{RPropertyPage::OnCommand(notifyCode, id, hWndControl); return;}
			INT_PTR nItem = tv.GetItemData(hItem);

			HTREEITEM hNextItem = hItem;
			INT_PTR nNextItem = nItem;
			while (hNextItem = tv.GetNextItem(hNextItem))
				tv.SetItemData(hNextItem, nNextItem++);

			m_categories.RemoveAt(nItem);
			foreach (m_categories, cat)
				foreach (cat.directories, dir)
					dir.pCategory = &cat;

			tv.DeleteItem(hItem);
			SetFocus(tv);

			if (m_categories == 0)
				m_tbtvDirs.tv.DeleteAllItems();
			
			SetChanged();
		}
		else if (id == ID_MOVEUP && pSelectedCat)
		{
			SetFocus(tv);

			HTREEITEM hItem = tv.GetSelectedItem();
			HTREEITEM hPrevItem = tv.GetPrevItem(hItem);
			if (!hPrevItem)
				{RPropertyPage::OnCommand(notifyCode, id, hWndControl); return;}

			INT_PTR nItem = tv.GetItemData(hItem);
			INT_PTR nPrevItem = tv.GetItemData(hPrevItem);

			m_categories.Swap(nItem, nPrevItem);
			foreach (m_categories, cat)
				foreach (cat.directories, dir)
					dir.pCategory = &cat;

			tv.SetItemText(hPrevItem, m_categories[nPrevItem].strName);
			tv.SetItemText(hItem, m_categories[nItem].strName);
			tv.SelectItem(hPrevItem);

			SetChanged();
		}
		else if (id == ID_MOVEDOWN && pSelectedCat)
		{
			SetFocus(tv);

			HTREEITEM hItem = tv.GetSelectedItem();
			HTREEITEM hNextItem = tv.GetNextItem(hItem);
			if (!hNextItem)
				{RPropertyPage::OnCommand(notifyCode, id, hWndControl); return;}

			INT_PTR nItem = tv.GetItemData(hItem);
			INT_PTR nNextItem = tv.GetItemData(hNextItem);

			m_categories.Swap(nItem, nNextItem);
			foreach (m_categories, cat)
				foreach (cat.directories, dir)
					dir.pCategory = &cat;

			tv.SetItemText(hNextItem, m_categories[nNextItem].strName);
			tv.SetItemText(hItem, m_categories[nItem].strName);
			tv.SelectItem(hNextItem);

			SetChanged();
		}
	}

	//
	// Directories
	//

	else if (hWndControl == m_tbtvDirs.tb)
	{
		RTreeView &tv = m_tbtvDirs.tv;

		if (id == ID_ADD && pSelectedCat && !tv.GetEditControl())
		{
			DBDIRECTORY *pDir = pSelectedCat->directories.AddNew();
			foreach (pSelectedCat->directories, dir)
				foreach (dir.movies, mov)
					mov.pDirectory = &dir;
			pDir->pCategory = pSelectedCat;
			pDir->strComputerName = GetComputerName();
			//pDir->bList = false;

			HTREEITEM hItem = tv.InsertItem(_T(""), pSelectedCat->directories-1);
			tv.SelectItem(hItem);
			tv.EditLabel(hItem);

			SetChanged();
		}
		else if (id == ID_DELETE && pSelectedCat)
		{
			HTREEITEM hItem = tv.GetSelectedItem();
			if (!hItem)
				{RPropertyPage::OnCommand(notifyCode, id, hWndControl); return;}
			INT_PTR nItem = tv.GetItemData(hItem);

			HTREEITEM hNextItem = hItem;
			INT_PTR nNextItem = nItem;
			while (hNextItem = tv.GetNextItem(hNextItem))
				tv.SetItemData(hNextItem, nNextItem++);

			pSelectedCat->directories.RemoveAt(nItem);
			foreach (pSelectedCat->directories, dir)
				foreach (dir.movies, mov)
					mov.pDirectory = &dir;

			tv.DeleteItem(hItem);
			SetFocus(tv);
			
			SetChanged();
		}
		else if (id == ID_MOVEUP && pSelectedCat)
		{
			SetFocus(tv);

			HTREEITEM hItem = tv.GetSelectedItem();
			HTREEITEM hPrevItem = tv.GetPrevItem(hItem);
			if (!hPrevItem)
				{RPropertyPage::OnCommand(notifyCode, id, hWndControl); return;}

			INT_PTR nItem = tv.GetItemData(hItem);
			INT_PTR nPrevItem = tv.GetItemData(hPrevItem);

			pSelectedCat->directories.Swap(nItem, nPrevItem);
			foreach (pSelectedCat->directories, dir)
				foreach (dir.movies, mov)
					mov.pDirectory = &dir;

			tv.SetItemText(hPrevItem, pSelectedCat->directories[nPrevItem].strPath);
			tv.SetItemText(hItem, pSelectedCat->directories[nItem].strPath);
			tv.SelectItem(hPrevItem);

			SetChanged();
		}
		else if (id == ID_MOVEDOWN && pSelectedCat)
		{
			SetFocus(tv);

			HTREEITEM hItem = tv.GetSelectedItem();
			HTREEITEM hNextItem = tv.GetNextItem(hItem);
			if (!hNextItem)
				{RPropertyPage::OnCommand(notifyCode, id, hWndControl); return;}

			INT_PTR nItem = tv.GetItemData(hItem);
			INT_PTR nNextItem = tv.GetItemData(hNextItem);

			pSelectedCat->directories.Swap(nItem, nNextItem);
			foreach (pSelectedCat->directories, dir)
				foreach (dir.movies, mov)
					mov.pDirectory = &dir;

			tv.SetItemText(hNextItem, pSelectedCat->directories[nNextItem].strPath);
			tv.SetItemText(hItem, pSelectedCat->directories[nItem].strPath);
			tv.SelectItem(hNextItem);

			SetChanged();
		}
	}

	//
	// Edit control
	//

	if (notifyCode == EN_CHANGE && hWndControl == m_eComputerName && pSelectedCat)
	{
		HTREEITEM hItem = m_tbtvDirs.tv.GetSelectedItem();
		if (hItem)
		{
			INT_PTR nItem = m_tbtvDirs.tv.GetItemData(hItem);
			if (nItem >= 0 && nItem < pSelectedCat->directories)
			{
				DBDIRECTORY *pDir = &pSelectedCat->directories[nItem];
				if (pDir->strComputerName != m_eComputerName.GetText())
				{
					pDir->strComputerName = m_eComputerName.GetText();
					SetChanged();
				}
			}
		}
	}

	RPropertyPage::OnCommand(id, notifyCode, hWndControl);
}

bool CCategoriesPage::OnCreate(CREATESTRUCT *pCS)
{
	if (!RPropertyPage::OnCreate(pCS))
		ASSERTRETURN(false);

	// Create child controls

	if (
		!m_tbtvCats.Create(m_hWnd, WS_TABSTOP|TVS_FULLROWSELECT|TVS_EDITLABELS|
			TVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE) || 
		!m_stcCats.Create<RStatic>(m_hWnd) ||
		!m_tbtvDirs.Create(m_hWnd, WS_TABSTOP|TVS_FULLROWSELECT|TVS_EDITLABELS, WS_EX_CLIENTEDGE) ||
		!m_stcDirs.Create<RStatic>(m_hWnd) ||
		!m_stcOnlyAvailableOn.Create<RStatic>(m_hWnd) ||
		!m_eComputerName.Create<REdit>(m_hWnd, WS_TABSTOP|ES_AUTOHSCROLL, WS_EX_CLIENTEDGE) ||
		!m_stcYouCanUseTxt.Create<RStatic>(m_hWnd)
		)
		ASSERTRETURN(false);

	m_tbtvCats.tv.SetItemHeight(DUY(13));
	m_tbtvDirs.tv.SetItemHeight(DUY(13));

	// Create icons for tool bars

	RMemoryDC mdc1, mdc2;

	LoadIcon(IDI_ADD, &mdc1);
	mdc2.Create(SCX(16), SCX(16));
	Resize(&mdc2, &mdc1, true);
	m_mdcAdd.Create(SCX(20), SCX(20));
	mdc2.Draw(m_mdcAdd, SCX(2), SCX(2), true);

	LoadIcon(IDI_DELETE, &mdc1);
	mdc2.Create(SCX(16), SCX(16));
	Resize(&mdc2, &mdc1, true);
	m_mdcDelete.Create(SCX(20), SCX(20));
	mdc2.Draw(m_mdcDelete, SCX(2), SCX(2), true);

	LoadIcon(IDI_MOVEUP, &mdc1);
	mdc2.Create(SCX(16), SCX(16));
	Resize(&mdc2, &mdc1, true);
	m_mdcMoveUp.Create(SCX(20), SCX(20));
	mdc2.Draw(m_mdcMoveUp, SCX(2), SCX(2), true);

	LoadIcon(IDI_MOVEDOWN, &mdc1);
	mdc2.Create(SCX(16), SCX(16));
	Resize(&mdc2, &mdc1, true);
	m_mdcMoveDown.Create(SCX(20), SCX(20));
	mdc2.Draw(m_mdcMoveDown, SCX(2), SCX(2), true);

	// Add buttons to tool bars

	m_tbtvCats.tb.AddButton(ID_ADD, &m_mdcAdd);
	m_tbtvCats.tb.AddButton(ID_DELETE, &m_mdcDelete);
	m_tbtvCats.tb.AddButton(ID_MOVEUP, &m_mdcMoveUp);
	m_tbtvCats.tb.AddButton(ID_MOVEDOWN, &m_mdcMoveDown);

	m_tbtvDirs.tb.AddButton(ID_ADD, &m_mdcAdd);
	m_tbtvDirs.tb.AddButton(ID_DELETE, &m_mdcDelete);
	m_tbtvDirs.tb.AddButton(ID_MOVEUP, &m_mdcMoveUp);
	m_tbtvDirs.tb.AddButton(ID_MOVEDOWN, &m_mdcMoveDown);

	// Set text of controls

	OnPrefChanged();

	// Copy categories

	m_categories = GetDB()->m_categories;
	foreach (m_categories, cat)
	{
		foreach (cat.directories, dir)
		{
			dir.pCategory = &cat;
			foreach (dir.movies, mov)
				mov.pDirectory = &dir;
		}
	}

	// Populate tree views

	foreach (m_categories, cat, i)
		m_tbtvCats.tv.InsertItem(cat.strName, i);

	SetFocus(m_tbtvCats.tv); // selects first item too

	return true;
}

LRESULT CCategoriesPage::OnNotify(UINT_PTR id, NMHDR *pNMH)
{
	HTREEITEM hSelectedCat = m_tbtvCats.tv.GetSelectedItem();

	INT_PTR nSelectedCat = -1;
	if (hSelectedCat)
		nSelectedCat = m_tbtvCats.tv.GetItemData(hSelectedCat);

	DBCATEGORY *pSelectedCat = NULL;
	if (nSelectedCat >= 0 && nSelectedCat < m_categories)
		pSelectedCat = &m_categories[nSelectedCat];

	//
	// Categories tree view
	//

	if (pNMH->hwndFrom == m_tbtvCats.tv)
	{
		if (pNMH->code == TVN_SELCHANGED)
		{
			m_tbtvDirs.tv.DeleteAllItems();
			if (pSelectedCat)
			{
				foreach (pSelectedCat->directories, dir, i)
					m_tbtvDirs.tv.InsertItem(dir.strPath, i);
			}
		}
		else if (pNMH->code == NM_DBLCLK)
		{
			VERIFY(m_tbtvCats.tv.EditLabel(m_tbtvCats.tv.GetSelectedItem()));
		}
		else if (pNMH->code == TVN_ENDLABELEDIT)
		{
			NMTVDISPINFO *pDI = (NMTVDISPINFO*)pNMH;

			// Text is NULL when canceled or empty

			if (!pDI->item.pszText)
			{
				if (m_tbtvCats.tv.GetItemText(pDI->item.hItem).IsEmpty())
					PostMessage(m_tbtvCats.tv, TVM_EDITLABEL, 0, (LPARAM)pDI->item.hItem);
				return RPropertyPage::OnNotify(id, pNMH);
			}

			// Not accepting empty name

			if (_tcscmp(pDI->item.pszText, _T("")) == 0)
			{
				PostMessage(m_tbtvCats.tv, TVM_EDITLABEL, 0, (LPARAM)pDI->item.hItem);
				return RPropertyPage::OnNotify(id, pNMH);
			}

			// Check if name already exists

			HTREEITEM hItem = m_tbtvCats.tv.GetRootItem();
			while (hItem)
			{
				if (_tcsicmp(m_tbtvCats.tv.GetItemText(hItem), pDI->item.pszText) == 0)
				{
					PostMessage(m_tbtvCats.tv, TVM_EDITLABEL, 0, (LPARAM)pDI->item.hItem);
					return RPropertyPage::OnNotify(id, pNMH);
				}
				hItem = m_tbtvCats.tv.GetNextItem(hItem);
			}

			// Change the name of the category

			DBCATEGORY *pCat = &m_categories[pDI->item.lParam];
			if (!pCat)
				{ASSERT(false); return RPropertyPage::OnNotify(id, pNMH);}

			pCat->strName = pDI->item.pszText;
			pCat->strName.Trim();
			_tcscpy(pDI->item.pszText, pCat->strName);

			SetChanged();

			return TRUE;

		}
		else if (pNMH->code == NM_SETFOCUS)
		{
			EnableWindow(m_eComputerName, FALSE);
		}
	}

	//
	// Directories tree view
	//

	else if (pNMH->hwndFrom == m_tbtvDirs.tv)
	{
		RTreeView *pTV = &m_tbtvDirs.tv;

		if (pNMH->code == TVN_SELCHANGED)
		{
			HTREEITEM hItem = pTV->GetSelectedItem();
			if (hItem && pSelectedCat)
			{
				INT_PTR nItem = pTV->GetItemData(hItem);
				if (nItem >= 0 && nItem < pSelectedCat->directories)
				{
					DBDIRECTORY *pDir = &pSelectedCat->directories[nItem];
					m_eComputerName.SetText(pDir->strComputerName);
				}
			}
			else
				m_eComputerName.SetText(NULL);
		}
		else if (pNMH->code == NM_CLICK || pNMH->code == NM_RCLICK)
		{
			SetFocus(*pTV);
		}
		else if (pNMH->code == NM_DBLCLK)
		{
			HTREEITEM hItem = pTV->GetSelectedItem();
			if (hItem)
				pTV->EditLabel(hItem);
		}
		else if (pNMH->code == TVN_ENDLABELEDIT && pSelectedCat)
		{
			NMTVDISPINFO *pDI = (NMTVDISPINFO*)pNMH;

			// Text is NULL when canceled or empty

			if (!pDI->item.pszText)
			{
				if (pTV->GetItemText(pDI->item.hItem).IsEmpty())
					PostMessage(*pTV, TVM_EDITLABEL, 0, (LPARAM)pDI->item.hItem);
				return RPropertyPage::OnNotify(id, pNMH);
			}

			// Not accepting empty text

			if (_tcscmp(pDI->item.pszText, _T("")) == 0)
			{
				PostMessage(*pTV, TVM_EDITLABEL, 0, (LPARAM)pDI->item.hItem);
				return RPropertyPage::OnNotify(id, pNMH);
			}

			// Check if text already exists

			HTREEITEM hItem = m_tbtvDirs.tv.GetRootItem();
			while (hItem)
			{
				if (_tcsicmp(m_tbtvDirs.tv.GetItemText(hItem), pDI->item.pszText) == 0)
				{
					PostMessage(m_tbtvDirs.tv, TVM_EDITLABEL, 0, (LPARAM)pDI->item.hItem);
					return RPropertyPage::OnNotify(id, pNMH);
				}
				hItem = m_tbtvDirs.tv.GetNextItem(hItem);
			}

			// Change text

			DBDIRECTORY *pDir = &pSelectedCat->directories[pDI->item.lParam];
			if (!pDir)
				{ASSERT(false); return RPropertyPage::OnNotify(id, pNMH);}

			pDir->strPath = pDI->item.pszText;
			pDir->strPath.Trim();
			_tcscpy(pDI->item.pszText, pDir->strPath);

			//pDir->bList = !DirectoryExists(CorrectPath(pDir->strPath));

			SetChanged();

			return TRUE;
		}
		else if (pNMH->code == NM_SETFOCUS)
		{
			EnableWindow(m_eComputerName, TRUE);
		}
	}

	return RPropertyPage::OnNotify(id, pNMH);
}

void CCategoriesPage::OnSize(DWORD type, WORD cx, WORD cy)
{
	RPropertyPage::OnSize(type, cx, cy);

	int x = 0, y = 0, cx2 = cx / 3;
	MoveStatic(m_stcCats, x + DUX(4), y + DUY(6));
	MoveWindow(m_tbtvCats, x + DUX(4), y + DUY(18), cx2 - DUX(8), cy - DUY(24));

	x = cx / 3; cx2 = (2 * cx) / 3;
	MoveStatic(m_stcDirs, x + DUX(2), y + DUY(6));
	MoveWindow(m_tbtvDirs, x + DUX(2), y + DUY(18), cx2 - DUX(6), cy - DUY(61));
	MoveStatic(m_stcOnlyAvailableOn, x + DUX(2), cy - DUY(38));
	MoveWindow(m_eComputerName, x + DUX(78), cy - DUY(40), cx2 - DUX(82)-1, DUY(12));
	MoveWindow(m_stcYouCanUseTxt, x + DUX(2), cy - DUY(24), cx2 - DUX(6), DUY(20));

	PostMessage(m_hWnd, WM_PAINT);
	PostChildren(m_hWnd, WM_PAINT);
}

void CCategoriesPage::OnPrefChanged()
{
	RPropertyPage::OnPrefChanged();

	m_stcCats.SetText(GETSTR(IDS_CATEGORIES) + _T(":"));
	m_stcDirs.SetText(GETSTR(IDS_DIRECTORIES) + _T(":"));
	m_stcOnlyAvailableOn.SetText(GETSTR(IDS_ONLYAVAILABLEON) + _T(":"));
	m_stcYouCanUseTxt.SetText(GETSTR(IDS_YOUCANUSETXT));

	Invalidate(m_hWnd);
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	OnSize(0, (WORD)rc.right, (WORD)rc.bottom);
}
