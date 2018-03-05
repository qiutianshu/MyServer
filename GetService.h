#include <windows.h>
#include  <winsvc.h>

typedef struct tagServiceInfo{
	char  ServiceName[32];
	char  ServiceDetail[128];
	char  ServiceState[16];
    char  ServiceStartType[16];
}SERVICEINFO,*LPSERVICEINFO;



BOOL ListService(std::vector<SERVICEINFO *> *service_vector)
{
	SERVICEINFO *service_tmp = new SERVICEINFO ;
	memset(service_tmp,0,sizeof(SERVICEINFO));
	
   	SC_HANDLE ScManager;
	ENUM_SERVICE_STATUS EnService[512];
	DWORD   cbBufSize=512*sizeof(ENUM_SERVICE_STATUS);
	// size of status buffer
	DWORD lpServicesReturned;
	DWORD   pcbBytesNeeded;           // buffer size needed
	DWORD   lpResumeHandle=0;           // next entry
	CString str,csTemp;
	

	service_vector->clear();

	ScManager=::OpenSCManager(NULL,NULL,SC_MANAGER_ENUMERATE_SERVICE|GENERIC_READ);
	
	if(ScManager==NULL)
	{
		AfxMessageBox("Error Opening Service Mgr");
		return FALSE;
	}
	
	if(::EnumServicesStatus(ScManager,SERVICE_WIN32,SERVICE_STATE_ALL,EnService,cbBufSize,&pcbBytesNeeded,&lpServicesReturned,&lpResumeHandle )==0)
	{
		AfxMessageBox("Error Querrying Service Mgr");
		return FALSE;
	}
	
	
	
	for(int i=0;i<int(lpServicesReturned);i++)
	{
		
		SERVICEINFO *service_tmp = new SERVICEINFO ;
        memset(service_tmp,0,sizeof(SERVICEINFO));
		
		strcpy(service_tmp->ServiceName,EnService[i].lpServiceName);
		strcpy(service_tmp->ServiceDetail,EnService[i].lpDisplayName);
		
		
		
		switch(EnService[i].ServiceStatus.dwCurrentState)
		{
		case SERVICE_PAUSED:
			strcpy(service_tmp->ServiceState,"暂停");		
			break;	// The service is paused. 
			
		case SERVICE_RUNNING:
			strcpy(service_tmp->ServiceState,"已启动");          
			break;// The service is running. 
			
		case SERVICE_STOPPED :     
			strcpy(service_tmp->ServiceState,"停止");  
			break;// The service is stopped
			
		case SERVICE_START_PENDING :
			strcpy(service_tmp->ServiceState,"正在启动");  
		    break;// The service is starting
							  
		case SERVICE_STOP_PENDING :
			strcpy(service_tmp->ServiceState,"正在停止");                                                                      
			break;// The service is stopped
							  
		default:
			strcpy(service_tmp->ServiceState,"未知");       							
			strcpy(service_tmp->ServiceState,"unknown");    
			break;// The service is running.
		}
		
		//block for start type
		{
			SC_HANDLE ScService;					
			QUERY_SERVICE_CONFIG ServiceConfig;
			DWORD CbBufSize, pcbBytesNeeded;
			
			CbBufSize=sizeof(QUERY_SERVICE_CONFIG);
			
			//CString 
			//char *SerState ;;
			
			if((ScService=::OpenService(ScManager,EnService[i].lpServiceName,SERVICE_ALL_ACCESS))==NULL)
			{
				::CloseHandle(ScManager);		
				"Problem in opening service";
				
			}
			
			::QueryServiceConfig(ScService,&ServiceConfig,CbBufSize,&pcbBytesNeeded);
			
			switch(ServiceConfig.dwStartType)
			{
				case	SERVICE_AUTO_START:	strcpy(service_tmp->ServiceStartType,"自动");	break;                           
				case	SERVICE_DEMAND_START:strcpy(service_tmp->ServiceStartType,"手动");	break;
				case	SERVICE_DISABLED :strcpy(service_tmp->ServiceStartType,"禁用");	break;
				default:  strcpy(service_tmp->ServiceStartType,"未知");
					     
			}
			
			
		}/**/
		service_vector->push_back(service_tmp);
	}

//	for(int jj = 0;jj<service_vector.size();jj++)
//	{
//		AfxMessageBox(service_vector[jj]->ServiceName);
//	}
	return true;
}

BOOL KillService(char *chSvrid)
{

    char *strServiceName = chSvrid;

	SC_HANDLE ScManager,ScService;//Declaring the Some USeful Variable
	
	if((ScManager=::OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS))==NULL)
	{
								
		//preparing Error Variable
		//AfxMessageBox("Error Opening System Manager");
		return false;
							 
	}
	
	if((ScService=::OpenService(ScManager,strServiceName,SERVICE_ALL_ACCESS))==NULL)
	{
		//preparing Error Variable
		//AfxMessageBox("Error Opening Service");
		return false;
								
	}
	
	if(::DeleteService(ScService)==0)
	{
		//preparing Error Variable
		//AfxMessageBox("Error Deleting Service");
		return false;
								
	}
	else
	{
		//MessageBox("Service Marked For Deletion\nDeleted After Computer Restart");
		return true;						
	}    
}

