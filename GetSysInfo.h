#include "stdafx.h"
#include "windows.h"
#include "GetHDSerial.h"

#define BUFSIZE 16

bool GetClientSystemInfo(SYSTEMINIT& sysinfo)
{
	/////get computer name///////
	TCHAR computerbuf[256];
	DWORD computersize=256;
	memset(computerbuf,0,256);
	if(!GetComputerName(computerbuf,&computersize))
		return false;
	computerbuf[computersize]=0;
	sysinfo.computer[0]=0;
	strcat(sysinfo.computer,"计算机: ");
	strcat(sysinfo.computer,computerbuf);
	///////get user name/////////
	TCHAR userbuf[256];
	DWORD usersize=256;
	memset(userbuf,0,256);
	if(!GetUserName(userbuf,&usersize))
		return false;
	userbuf[usersize]=0;
	sysinfo.user[0]=0;
	strcat(sysinfo.user,"用户名: ");
	strcat(sysinfo.user,userbuf);
	
	
	///////get version//////////
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	OSVERSIONINFOEX   osviex;
    sysinfo.os[0]=0;
	memset(&osviex,0,sizeof(OSVERSIONINFOEX));
	osviex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if(GetVersionEx((LPOSVERSIONINFO)&osviex)==0)
	{
		OutputDebugString("GetVersionEx Error");
		return FALSE;
	}

	switch(osviex.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		switch(osviex.dwMajorVersion)
		{
		case 4:
			if(osviex.dwMinorVersion == 0)
               strcat(sysinfo.os,"Microsoft Windows NT 4");
			break;
		case 5:
			if(osviex.dwMinorVersion == 0)
			{
                strcat(sysinfo.os,"Microsoft Windows 2000 ");
			}
			else if(osviex.dwMinorVersion == 1)
			{
				strcat(sysinfo.os,"Windows XP ");
			}
			else if(osviex.dwMinorVersion == 2)
			{
               strcat(sysinfo.os,"Windows 2003 ");
			}
			
		}
		break;
	}


	//////////////////get HardDisk serial /////////////////////////////////////////
	char *temp;
	sysinfo.HDSerial[0]=0;
    CGetHDSerial HDSerial;  // 创建实例   
    temp=HDSerial.GetHDSerial(); // 得到硬盘序列号    
    strcat(sysinfo.HDSerial,temp);

	////////////////get cpu info//////////////////
	sysinfo.processor[0]=0;
	//strcat(sysinfo.processor,"CPU: ");
	HKEY hKey;
	char szcpuinfo[80];
	DWORD dwBufLen=80;
	RegOpenKeyEx( HKEY_LOCAL_MACHINE,
	   "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
	   0, KEY_QUERY_VALUE, &hKey );
	RegQueryValueEx( hKey, "VendorIdentifier", NULL, NULL,
	   (LPBYTE)szcpuinfo, &dwBufLen);
	szcpuinfo[dwBufLen]=0;
	//strcat(sysinfo.processor,szcpuinfo);
	//strcat(sysinfo.processor," ");
	memset(szcpuinfo,0,80);
	dwBufLen=80;
	RegQueryValueEx( hKey, "Identifier", NULL, NULL,
	   (LPBYTE)szcpuinfo, &dwBufLen);
	szcpuinfo[dwBufLen]=0;
	//strcat(sysinfo.processor,szcpuinfo);
	DWORD f;
	dwBufLen=8;
	RegQueryValueEx( hKey, "~MHz", NULL, NULL,
	   (LPBYTE)&f, &dwBufLen);
	char hz[10];
	sprintf(hz," %dMHZ",f);
	strcat(sysinfo.processor,hz);
	RegCloseKey(hKey);
	
	/////////////get mem size////////////
	MEMORYSTATUS ms;
	GlobalMemoryStatus(&ms);
	char membuf[256];//物理内存: 
//	sprintf(membuf,"总物理内存:%dMB,可用内存:%dMB (占%.2f%s)",ms.dwTotalPhys/1024/1024,ms.dwAvailPhys/1024/1024
//		,(double)ms.dwAvailPhys/ms.dwTotalPhys*100,"%");
    sprintf(membuf,"%dMB",ms.dwTotalPhys/1024/1024);
	sysinfo.mem[0]=0;
	strcpy(sysinfo.mem,membuf);
	return true;
}