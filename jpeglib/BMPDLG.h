// BMPDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBMPDlg dialog

class CBMPDlg : public CDialog
{
// Construction
public:
	CBMPDlg(CWnd* pParent = NULL);   // standard constructor
	int m_bits;

// Dialog Data
	//{{AFX_DATA(CBMPDlg)
	enum { IDD = IDD_CMAP_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBMPDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBMPDlg)
	afx_msg void OnBmp1bits();
	afx_msg void OnBmp4bit();
	afx_msg void OnBmp8bit();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
