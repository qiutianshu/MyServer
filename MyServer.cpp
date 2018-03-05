// MyServer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MyServer.h"
#include "MyServerDlg.h"
#include "comm.h"

#define REGKEY "{E92B03AB-B707-11d2-9CBD-0000F87A3690}"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyServerApp

BEGIN_MESSAGE_MAP(CMyServerApp, CWinApp)
	//{{AFX_MSG_MAP(CMyServerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyServerApp construction







CMyServerApp::CMyServerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMyServerApp object

CMyServerApp theApp;                     //实例化应用程序


/////////////////////////////////////////////////////////////////////////////
// CMyServerApp initialization



BOOL CMyServerApp::InitInstance()
{

		// 下面的代码防止程序重复被运行 //////////////////
		HANDLE hOneInstanceMutex = ::CreateMutex(NULL, FALSE, _T("MyServer_is_Running"));             //创建互斥体不允许其它进程使用该端口，防止程序重复运行
		if(GetLastError() == ERROR_ALREADY_EXISTS)	
		{	
			hOneInstanceMutex = NULL;
			return FALSE;	
		}
		//////////////////////////////////////////////////

	//写入注册表,开机自启动 
	HKEY hKey; 
	//找到系统的启动项 
	LPCTSTR lpRun = "Software\\Microsoft\\Windows\\CurrentVersion\\Run"; 
	//打开启动项Key 
	long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_WRITE, &hKey); 
	if(lRet == ERROR_SUCCESS) 
	{ 
		char pFileName[MAX_PATH] = {0}; 
		//得到程序自身的全路径 
		DWORD dwRet = GetModuleFileName(NULL, pFileName, MAX_PATH); 
		//添加一个子Key,并设置值 
		lRet = RegSetValueEx(hKey, "AntiSpyware", 0, REG_SZ, (BYTE *)pFileName, dwRet); 
		//关闭注册表 
		RegCloseKey(hKey); 
		if(lRet != ERROR_SUCCESS) 
		{ 
			AfxMessageBox("系统参数错误,不能随系统启动"); 
		} 
	}  


	CMyServerDlg dlg;                             //创建主对话框窗口
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();

	//ShowWindow(dlg,SW_HIDE);
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}



	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

