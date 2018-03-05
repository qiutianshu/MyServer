// BMPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mfcapp.h"
#include "BMPDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBMPDlg dialog


CBMPDlg::CBMPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBMPDlg::IDD, pParent)
{
	m_bits = 8;

	//{{AFX_DATA_INIT(CBMPDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBMPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBMPDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBMPDlg, CDialog)
	//{{AFX_MSG_MAP(CBMPDlg)
	ON_BN_CLICKED(IDC_BMP_1BITS, OnBmp1bits)
	ON_BN_CLICKED(IDC_BMP_4BIT, OnBmp4bit)
	ON_BN_CLICKED(IDC_BMP_8BIT, OnBmp8bit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBMPDlg message handlers

void CBMPDlg::OnBmp1bits() 
{
	m_bits = 1;
}

void CBMPDlg::OnBmp4bit() 
{
	m_bits = 4;
}

void CBMPDlg::OnBmp8bit() 
{
	m_bits = 8;
}

BOOL CBMPDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CheckRadioButton(IDC_BMP_8BIT, IDC_BMP_1BITS, IDC_BMP_8BIT);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
