// TcpTran.h: interface for the CTcpTran class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPTRAN_H__F3515A70_E030_420C_986B_174D6C0E9E06__INCLUDED_)
#define AFX_TCPTRAN_H__F3515A70_E030_420C_986B_174D6C0E9E06__INCLUDED_



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SOCKETBIND	1
#define SOCKETNOBIND 2
#define SOCKET_TIMEOUT -100

#include "winsock2.h"

#include <string>

using namespace std;

#pragma comment (lib,"ws2_32.lib")

class CTcpTran  
{
public:
	CTcpTran();
	virtual ~CTcpTran();

public:
	static BOOL InitSocketLibray(int lowver,int higver ); //ºÏ—Èsocket∞Ê±æ

public:
	SOCKET	InitSocket( int SocketType, string strBindIp,u_short BindPort,int opt);     
	SOCKET	myaccept(SOCKET s,struct sockaddr* addr,int* addrlen);
	int		mysend(SOCKET sock, const char *buf, int len, int flag,int overtime);
	int		myrecv(SOCKET sock, char *buf, int len, int flag , int overtime,char*EndMark,BOOL soonflag=FALSE);	
	
public:
	SOCKET m_Socket;
	
};

#endif // !defined(AFX_TCPTRAN_H__F3515A70_E030_420C_986B_174D6C0E9E06__INCLUDED_)
