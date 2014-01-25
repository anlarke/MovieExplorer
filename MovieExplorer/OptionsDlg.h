#pragma once

#include "GeneralPage.h"
#include "DatabasePage.h"
#include "CategoriesPage.h"

class COptionsDlg : public RPropertySheet
{
	friend class RWindow;
	friend class RDialog;

public:
	COptionsDlg();
	~COptionsDlg();

protected:
	bool PreTranslateMessage(MSG *pMsg);
	void OnPrefChanged();

	CGeneralPage m_generalPage;
	CDatabasePage m_databasePage;
	CCategoriesPage m_categoriesPage;
};
