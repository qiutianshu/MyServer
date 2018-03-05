// MyServer.h : main header file for the MYSERVER application
//

#if !defined(AFX_MYSERVER_H__90C98308_8550_4968_8832_E59BD6B8F97F__INCLUDED_)
#define AFX_MYSERVER_H__90C98308_8550_4968_8832_E59BD6B8F97F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMyServerApp:
// See MyServer.cpp for the implementation of this class


class CMyServerApp : public CWinApp
{
public:
	CMyServerApp();



//	HANDLE			hOneInstanceMutex;	// 保证程序只运行一次的互斥对象
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyServerApp)

	public:
	virtual BOOL InitInstance();


	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMyServerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSERVER_H__90C98308_8550_4968_8832_E59BD6B8F97F__INCLUDED_)
