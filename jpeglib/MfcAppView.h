// MfcAppView.h : interface of the CMfcAppView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFCAPPVIEW_H__7B4DD98F_945B_11D2_815A_444553540000__INCLUDED_)
#define AFX_MFCAPPVIEW_H__7B4DD98F_945B_11D2_815A_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CMfcAppView : public CView
{
protected: // create from serialization only
	CMfcAppView();
	DECLARE_DYNCREATE(CMfcAppView)

// Attributes
public:
	CMfcAppDoc* GetDocument();

// Operations
public:

	// global image params
	BYTE *m_buf;
	UINT m_width;
	UINT m_height;
	UINT m_widthDW;
    bool m_bPause;
	// draw what we've loaded
	void DrawBMP();
	
	// jpg load save
	void LoadJPG(CString fileName);
	void SaveJPG(CString filename, BOOL color);

	void LoadBMP(CString fileName);
	void SaveBMP24(CString fileName);

	// turn 24-bit to 256-color gray scale
	BYTE * MakeColormappedGrayscale(BYTE *inBuf,
								  UINT inWidth,
								  UINT inHeight,
								  UINT inWidthBytes,
								  UINT colors,
								  RGBQUAD* colormap);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcAppView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMfcAppView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMfcAppView)
	afx_msg void OnFileOpen();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSavecolormappedbmp();
	afx_msg void OnFileSavegrayas();
	afx_msg void OnFileGetdimensionsjpg();
	afx_msg void OnPlay();
  	afx_msg void OnStop();
	afx_msg void OnPause(CCmdUI* pCmdUI) ;
  	afx_msg void OnPause();
//    afx_msg void OnViewPause(UINT nID);
  	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MfcAppView.cpp
inline CMfcAppDoc* CMfcAppView::GetDocument()
   { return (CMfcAppDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCAPPVIEW_H__7B4DD98F_945B_11D2_815A_444553540000__INCLUDED_)
