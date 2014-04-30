#pragma once

class CGeneralPage : public RPropertyPage
{
	friend class RWindow;

public:
	CGeneralPage();
	~CGeneralPage();
	virtual void ApplyChanges();

protected:
	bool OnCreate(CREATESTRUCT *pCS);
	void OnSize(DWORD type, WORD cx, WORD cy);
	void OnPrefChanged();
	void OnMeasureItem(UINT_PTR id, MEASUREITEMSTRUCT *pMIS);
	void OnDrawItem(UINT_PTR id, DRAWITEMSTRUCT *pDIS);
	void OnCommand(WORD id, WORD notifyCode, HWND hWndControl);

	RButton m_grpInterface, m_grpSearch, m_chkNormalizeRatings, m_chkSearchInstantly, 
			m_chkSearchLiterally, m_chkSearchStoryline, m_chkAutoCategories;
	RStatic m_stcLanguage, m_stcTheme;
	RComboBox m_cbLanguage, m_cbTheme;
	struct CONFIGFILE {RString strFilePath, strName;};
	RObArray<CONFIGFILE> m_langFiles;
	RObArray<CONFIGFILE> m_themeFiles;
};
