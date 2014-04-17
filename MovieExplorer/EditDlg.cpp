#include "stdafx.h"
#include "MovieExplorer.h"
#include "EditDlg.h"

CEditDlg::CEditDlg(HWND hWndParent, DBMOVIE* pMov) : m_pMov(pMov)
{
	VERIFY(Create<CEditDlg>(hWndParent, GETSTR(IDS_EDITMOVIE), 0, 0, DUX(250), DUY(173), true));
	VERIFY(ShowModal<CEditDlg>(true));
}

CEditDlg::~CEditDlg()
{
}

void CEditDlg::OnCommand(WORD id, WORD notifyCode, HWND hWndControl)
{
	RDialog::OnCommand(id, notifyCode, hWndControl);

	switch (id)
	{
	case IDOK:
		OnOK();
		break;
	case IDCANCEL:
		PostMessage(m_hWnd, WM_CLOSE);
		break;
	}
}

bool CEditDlg::OnCreate(CREATESTRUCT *pCS)
{
	if (!RDialog::OnCreate(pCS))
		return false;

	// Create controls

	if (!m_btnOK.Create<RButton>(m_hWnd, WS_TABSTOP|BS_DEFPUSHBUTTON, 0, GETSTR(IDS_OK), 0, 0, 0, 0, IDOK) ||
			!m_btnCancel.Create<RButton>(m_hWnd, WS_TABSTOP, 0, GETSTR(IDS_CANCEL), 0, 0, 0, 0, IDCANCEL) ||
			!m_stcFileName.Create<RStatic>(m_hWnd, 0, 0, GETSTR(IDS_FILENAME) + _T(":")) ||
			!m_stcFileName2.Create<RStatic>(m_hWnd, 0, 0, m_pMov->strFileName) ||
			!m_stcFileSize.Create<RStatic>(m_hWnd, 0, 0, GETSTR(IDS_FILESIZE) + _T(":")) ||
			!m_stcFileSize2.Create<RStatic>(m_hWnd, 0, 0, SizeToString(m_pMov->fileSize)) ||
			!m_grpInformation.Create<RButton>(m_hWnd, BS_GROUPBOX, 0, GETSTR(IDS_INFORMATION)) ||
			!m_stcIMDb.Create<RStatic>(m_hWnd, 0, 0, _T("imdb.com:")) ||
			!m_eIMDb.Create<REdit>(m_hWnd, WS_TABSTOP|ES_AUTOHSCROLL, WS_EX_CLIENTEDGE, m_pMov->strIMDbID) ||
			!m_stcMovieMeter.Create<RStatic>(m_hWnd, 0, 0, _T("moviemeter.nl:")) ||
			!m_eMovieMeter.Create<REdit>(m_hWnd, WS_TABSTOP|ES_AUTOHSCROLL, WS_EX_CLIENTEDGE, m_pMov->strMovieMeterID) ||
			!m_grpIdentification.Create<RButton>(m_hWnd, BS_GROUPBOX, 0, GETSTR(IDS_IDENTIFICATION)) ||
			!m_chkSeenMovie.Create<RButton>(m_hWnd, BS_AUTOCHECKBOX|WS_TABSTOP, 0, _T(" ") + GETSTR(IDS_SEENMOVIE)) ||
			!m_chkHideMovie.Create<RButton>(m_hWnd, BS_AUTOCHECKBOX|WS_TABSTOP, 0, _T(" ") + GETSTR(IDS_HIDEMOVIE)) ||
			!m_grpOptionalProperties.Create<RButton>(m_hWnd, BS_GROUPBOX, 0, GETSTR(IDS_OPTIONALPROP)))
		ASSERTRETURN(false);

	m_chkSeenMovie.SetCheck(m_pMov->bSeen);
	m_chkHideMovie.SetCheck(m_pMov->bHide);

	SetFocus(m_btnOK);
	return true;
}

void CEditDlg::OnSize(DWORD type, WORD cx, WORD cy)
{
	RDialog::OnSize(type, cx, cy);

	int y = DUY(4);
	MoveWindow(m_grpInformation, DUX(4), y, cx - 2*DUX(4), DUY(38));
	y += DUY(12);
	MoveWindow(m_stcFileName, DUX(12), y+DUY(1)+1, DUX(60), DUY(10));
	MoveWindow(m_stcFileName2, DUX(12) + DUX(62), y+DUY(1)+1, cx - DUX(82), DUY(10));
	y += DUY(12);
	MoveWindow(m_stcFileSize, DUX(12), y+DUY(1)+1, DUX(60), DUY(10));
	MoveWindow(m_stcFileSize2, DUX(12) + DUX(62), y+DUY(1)+1, cx - DUX(82), DUY(10));

	y += DUY(18);
	MoveWindow(m_grpIdentification, DUX(4), y, cx - 2*DUX(4), DUY(42));
	y += DUY(12);
	MoveWindow(m_stcIMDb, DUX(12), y+DUY(1)+1, DUX(60), DUY(10));
	MoveWindow(m_eIMDb, DUX(12) + DUX(62), y, cx - DUX(82), DUY(12));
	y += DUY(14);
	MoveWindow(m_stcMovieMeter, DUX(12), y+DUY(1)+1, DUX(60), DUY(10));
	MoveWindow(m_eMovieMeter, DUX(12) + DUX(62), y, cx - DUX(82), DUY(12));

	y += DUY(19);
	MoveWindow(m_grpOptionalProperties, DUX(4), y, cx - 2*DUX(4), DUY(40));
	y += DUY(12);
	MoveCheckBox(m_chkSeenMovie, DUX(12), y);
	y += DUY(14);
	MoveCheckBox(m_chkHideMovie, DUX(12), y);

	MoveWindow(m_btnCancel, cx - DUX(54), cy - DUY(20), DUX(50), DUY(14));
	MoveWindow(m_btnOK, cx - DUX(108), cy - DUY(20), DUX(50), DUY(14));

	PostMessage(m_hWnd, WM_PAINT);
	PostChildren(m_hWnd, WM_PAINT);
}

void CEditDlg::OnSizing(DWORD side, RECT *pRect)
{
	pRect->bottom = pRect->top + DUY(175);
	RDialog::OnSizing(side, pRect);
}

void CEditDlg::OnOK()
{
	if (m_pMov->strIMDbID != m_eIMDb.GetText() ||
			m_pMov->strMovieMeterID != m_eMovieMeter.GetText())
	{
		m_pMov->fIMDbRating = m_pMov->fIMDbRatingMax = 0.0f;
		m_pMov->fRating = m_pMov->fRatingMax = 0.0f;
		m_pMov->nMetascore = -1;
		m_pMov->nEpisode = -1; m_pMov->nSeason = -1;
		m_pMov->strEpisodeName.Empty(); m_pMov->strAirDate.Empty();
		m_pMov->bType = DB_TYPE_MOVIE;
		m_pMov->nIMDbVotes = m_pMov->nVotes = 0;
		m_pMov->strTitle.Empty(); m_pMov->strYear.Empty(); m_pMov->strCountries.Empty(); 
		m_pMov->strGenres.Empty(); m_pMov->strRuntime.Empty(); m_pMov->strStoryline.Empty();
		m_pMov->strDirectors.Empty(); m_pMov->strWriters.Empty(); m_pMov->strStars.Empty();
		m_pMov->posterData.SetSize(0); 
		for (int i = 0; i < DBI_STAR_NUMBER; i++)
		{
			m_pMov->strActorId[i].Empty();
			m_pMov->actorImageData[i] = NULL;
		}
	}

	m_pMov->strIMDbID = m_eIMDb.GetText();
	m_pMov->strMovieMeterID = m_eMovieMeter.GetText();
	m_pMov->bSeen = m_chkSeenMovie.GetCheck();
	m_pMov->bHide = m_chkHideMovie.GetCheck();

	GetDB()->Update(m_pMov);

	PostMessage(GetMainWnd(), WM_DBUPDATED);
	PostMessage(m_hWnd, WM_CLOSE);
}
