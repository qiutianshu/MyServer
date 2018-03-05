// MyServerDlg.h : header file
//

#include "afxwin.h"
#if !defined(AFX_MYSERVERDLG_H__3B292600_A5A9_4B95_886D_3584593233FA__INCLUDED_)
#define AFX_MYSERVERDLG_H__3B292600_A5A9_4B95_886D_3584593233FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMyServerDlg dialog

class CMyServerDlg : public CDialog
{
// Construction
public:
	CMyServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMyServerDlg)
	enum { IDD = IDD_MYSERVER_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
    HANDLE  hThread[1];
	//HANDLE hThread[1];
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMyServerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ip;
	CEdit m_port;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSERVERDLG_H__3B292600_A5A9_4B95_886D_3584593233FA__INCLUDED_)
