// TcpTran.cpp: implementation of the CTcpTran class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TcpTran.h"


CTcpTran::CTcpTran()
{
	m_Socket = INVALID_SOCKET;
}

CTcpTran::~CTcpTran()
{

}



BOOL CTcpTran::InitSocketLibray(int lowver,int higver )     
{
	WORD wVersion =0 ;
	int	 errret = -1;
	WSADATA wsaData;
	
	wVersion = MAKEWORD(lowver,higver);
	errret = WSAStartup(wVersion,&wsaData);
	
	if( LOBYTE( wsaData.wVersion) != 2 ||
		HIBYTE( wsaData.wVersion) !=2 )
	{
		MessageBox(NULL,"winsocket库版本低","提示",MB_OK);
		return FALSE;
	}
	
	return TRUE;
}





SOCKET	CTcpTran::InitSocket( int SocketType, string strBindIp,u_short BindPort,int opt)
{
	SOCKET socketid = INVALID_SOCKET;
	socketid = socket(PF_INET,SOCK_STREAM,0);      //建立tcp   socket
	SOCKADDR_IN sockStruct;
	sockStruct.sin_family = AF_INET; //使用TCP/IP协议

	if( strBindIp.empty() )
	{
		sockStruct.sin_addr.S_un.S_addr = INADDR_ANY;
		
	}else
	{
		sockStruct.sin_addr.S_un.S_addr = inet_addr(strBindIp.c_str());	
	}
    
	
    sockStruct.sin_port = htons(BindPort);   //转换为网络字节

	if( SocketType == SOCKETNOBIND )
	{
		if(connect(socketid,(LPSOCKADDR)&sockStruct,sizeof(sockStruct)) == SOCKET_ERROR)
		{
			//OutputDebugString("连接错误！");
			closesocket(socketid);
			shutdown(socketid,2);
			socketid = INVALID_SOCKET;
		}
		
		m_Socket = socketid;
		
	}else if( SocketType == SOCKETBIND )    //绑定ip
	{
		if(bind(socketid,(sockaddr*)&sockStruct,sizeof(sockaddr_in)) == SOCKET_ERROR)
		{
			closesocket(socketid);    
			socketid = INVALID_SOCKET;
			  
		}else
		{
			if( listen(socketid,SOMAXCONN) == SOCKET_ERROR )
			{
				closesocket(socketid);
				socketid = INVALID_SOCKET;
			}
		}
		
		m_Socket = socketid;
	}
    
	return socketid;
}	


SOCKET	CTcpTran::myaccept(SOCKET s,struct sockaddr* addr,int* addrlen)
{
	SOCKET accpsocket  = INVALID_SOCKET;
	accpsocket = accept(s,addr,addrlen);
	return accpsocket;
}



int CTcpTran::myrecv(SOCKET sock, char *buf, int len, int flag , int overtime ,char*EndMark,BOOL soonflag)
{
	int		ret;
	int		nLeft = len;
	int		idx	 = 0;
	int		nCount = 0;
	fd_set readfds;
	struct timeval  timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 500;
	DWORD s_time = GetTickCount();
	
	while ( nLeft > 0 )
	{
		//接收消息
		MSG msg;
		PeekMessage(&msg, NULL,  0, 0, PM_REMOVE) ;
		if(msg.message == WM_QUIT)
			return 0;
		
		FD_ZERO( &readfds );
		FD_SET( sock , &readfds );
		if( select( 0 , &readfds , NULL , NULL , &timeout ) == SOCKET_ERROR )
		{
			return SOCKET_ERROR;
		}
		
		DWORD e_time = GetTickCount( );
		if  ( !FD_ISSET( sock , &readfds ) )
		{
			if( e_time - s_time > overtime*1000 ) 
				return SOCKET_TIMEOUT;
			else
				continue;
		}
		
		ret = recv( sock, &buf[idx], nLeft, flag );
		
		if( soonflag == TRUE )
		{
			return ret;
		}
		
		s_time = e_time ; // 只要有数据就重新置初始时间值
		
		if ( ret <= 0 )
		{
			int		LastError = GetLastError();
			if ( ( -1 == ret ) && ( WSAETIMEDOUT	  == LastError ) )
				continue;
			if ( ( -1 == ret ) && ( WSAEWOULDBLOCK	  == LastError ) )
			{
				if ( nCount < 2000 )
				{
					Sleep( 10 );
					nCount++;
					continue;
				}
			}
			return ret;
		}
		nCount	=	0;
		
		nLeft	-= ret;
		idx		+= ret;

		if( EndMark != NULL && idx>5)
		{
			if( strstr(buf+(idx-5),EndMark) != NULL )
			{
				break;
			}
		}
	}

	return idx;
}


int	CTcpTran::mysend(SOCKET sock, const char *buf, int len, int flag,int overtime)
{
	int		ret;
	int		nLeft = len;
	int		idx	 = 0;
	
	fd_set readfds;
	struct timeval  timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 500;
	DWORD s_time = GetTickCount();
	
	while ( nLeft > 0 )
	{
		MSG msg;
		PeekMessage(&msg, NULL,  0, 0, PM_REMOVE) ;
		if(msg.message == WM_QUIT)
			return 0;
		
		FD_ZERO( &readfds );
		FD_SET( sock , &readfds );
		
		int errorret   = select( 0 , NULL, &readfds, NULL , &timeout );
		
		if( errorret == SOCKET_ERROR )
		{
			OutputDebugString("mysendEx SOCKET 错误");
			return SOCKET_ERROR;
		}
		
		DWORD e_time = GetTickCount( );
		if  ( !FD_ISSET( sock , &readfds ) )
		{
			
			if( e_time - s_time > overtime*1000 ) 
			{
				
				OutputDebugString("mysendEx发送数据超时");
				return 0;
			}
			else
			{
				//OutputDebugString("发送数据FD_ISSET 超时");
				continue;
			}
		}
		
		ret = send( sock, &buf[idx], nLeft, flag );
		
		if ( ret <= 0 )
		{
			return ret;
		}
		
		nLeft	-= ret;
		idx		+= ret;
	}
	
	return len;
}
