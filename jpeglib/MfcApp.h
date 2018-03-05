// MfcApp.h : main header file for the MFCAPP application
//

#if !defined(AFX_MFCAPP_H__7B4DD987_945B_11D2_815A_444553540000__INCLUDED_)
#define AFX_MFCAPP_H__7B4DD987_945B_11D2_815A_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMfcAppApp:
// See MfcApp.cpp for the implementation of this class
//

class CMfcAppApp : public CWinApp
{
public:
	CMfcAppApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcAppApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMfcAppApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCAPP_H__7B4DD987_945B_11D2_815A_444553540000__INCLUDED_)
