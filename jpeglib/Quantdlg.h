#if !defined(AFX_QUANTDLG_H__F0B2BF83_2A33_11D1_8159_444553540000__INCLUDED_)
#define AFX_QUANTDLG_H__F0B2BF83_2A33_11D1_8159_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// QuantDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQuantDlg dialog

class CQuantDlg : public CDialog
{
// Construction
public:
	CQuantDlg(CWnd* pParent = NULL);   // standard constructor

	UINT m_uDither, m_uPalette;
	BOOL m_color;

// Dialog Data
	//{{AFX_DATA(CQuantDlg)
	enum { IDD = IDD_QUANT };
	UINT	m_quantColors;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuantDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQuantDlg)
	afx_msg void OnColor();
	afx_msg void OnGray();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUANTDLG_H__F0B2BF83_2A33_11D1_8159_444553540000__INCLUDED_)
