#include "stdafx.h"
#include "MovieExplorer.h"
#include "OptionsDlg.h"

COptionsDlg::COptionsDlg()
{
	VERIFY(Create<COptionsDlg>(GetMainWnd(), GETSTR(IDS_OPTIONS), GETSTR(IDS_OK), 
		GETSTR(IDS_CANCEL), GETSTR(IDS_APPLY), 0, 0, DUX(300), DUY(256), true));
	VERIFY(AddPage<CGeneralPage>(&m_generalPage, GETSTR(IDS_GENERAL)));
	VERIFY(AddPage<CDatabasePage>(&m_databasePage, GETSTR(IDS_DATABASE)));
	VERIFY(AddPage<CCategoriesPage>(&m_categoriesPage, GETSTR(IDS_CATEGORIES)));
	VERIFY(ShowModal<COptionsDlg>(true));
}

COptionsDlg::~COptionsDlg()
{
}

bool COptionsDlg::PreTranslateMessage(MSG *pMsg)
{
	if (m_categoriesPage.m_tbtvCats.tv.GetEditControl() ||
			m_categoriesPage.m_tbtvDirs.tv.GetEditControl())
		return false;

	return RPropertySheet::PreTranslateMessage(pMsg);
}

void COptionsDlg::OnPrefChanged()
{
	RDialog::OnPrefChanged();

	m_tabControl.SetItemText(0, GETSTR(IDS_GENERAL));
	m_tabControl.SetItemText(1, GETSTR(IDS_DATABASE));
	m_tabControl.SetItemText(2, GETSTR(IDS_CATEGORIES));

	m_btnOK.SetText(GETSTR(IDS_OK));
	m_btnCancel.SetText(GETSTR(IDS_CANCEL));
	m_btnApply.SetText(GETSTR(IDS_APPLY));
}
