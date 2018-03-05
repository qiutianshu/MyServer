// QuantDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mfcapp.h"
#include "QuantDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuantDlg dialog


CQuantDlg::CQuantDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuantDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQuantDlg)
	m_quantColors = 64;
	//}}AFX_DATA_INIT

	m_color=TRUE;
}


void CQuantDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuantDlg)
	DDX_Text(pDX, IDC_QUANT_COL, m_quantColors);
	DDV_MinMaxUInt(pDX, m_quantColors, 1, 256);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuantDlg, CDialog)
	//{{AFX_MSG_MAP(CQuantDlg)
	ON_BN_CLICKED(IDC_COLOR, OnColor)
	ON_BN_CLICKED(IDC_GRAY, OnGray)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuantDlg message handlers


void CQuantDlg::OnColor() 
{
	m_color=TRUE;
}

void CQuantDlg::OnGray() 
{
	m_color=FALSE;
}

BOOL CQuantDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// defaults
	CWnd *pWnd = GetDlgItem(IDC_QUANT_COL);
	CString txt;
	txt.Format("%d",m_quantColors);
	pWnd->SetWindowText(txt);
	
	CheckRadioButton(IDC_COLOR, IDC_GRAY, IDC_COLOR);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

