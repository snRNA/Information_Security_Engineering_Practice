#include "HACK_CLIENT.h"


HWND hwndbutton[BUTTON_NUM];//窗口按钮


HackClient *hackclient=new HackClient();


LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
	switch (msg){
	case WM_CREATE:
		hackclient->Init(hwnd);
		break;
	case WM_PAINT:
		hackclient->ScreenPaint(hwnd);
		break;
	case WM_COMMAND:
		switch(HIWORD(wParam)){
		case BN_CLICKED:
			for(int i=0;i<BUTTON_NUM;i++)
				if((HWND)lParam == hwndbutton[i]){
					hackclient->CommandExecute(hwnd,i);
					break;
				}
			break;
		}
		break;
	case WM_DESTROY:
		ExitProcess(0);
	default:
		break;
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

int WINAPI WinMain(HINSTANCE hinstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){
	//注册主窗口类 
	WNDCLASS wc;
	if(!hPrevInstance){
		wc.style = CS_HREDRAW | CS_VREDRAW;                     
		wc.lpfnWndProc = MainWndProc;    
		wc.cbClsExtra = 0;                 
		wc.cbWndExtra = 0;               
		wc.hInstance = hinstance;      
		wc.hIcon = LoadIcon(NULL,IDI_APPLICATION);    
		wc.hCursor = LoadCursor(NULL,IDC_ARROW);                        //鼠标为箭头
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);         //黑色背景     
		wc.lpszMenuName =  "WndMenu";    
		wc.lpszClassName = "WndClass";                                  //主窗口类名
		RegisterClass(&wc); 
	}

   /********************初始化主窗口句柄hMainWnd********************/
	HWND hMainWnd;
	hMainWnd=CreateWindow( 
		"WndClass",                                                      //窗口类名
		"哔哩哔哩(゜-゜)つロ 干杯~-bilibili ",                                       //窗口标题名
		WS_OVERLAPPEDWINDOW,                                             //窗口风格
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,             
		NULL,               
		hinstance,          
		NULL);
	ShowWindow(hMainWnd, SW_SHOWNORMAL); 
	UpdateWindow(hMainWnd); 

   /********************消息循环结构********************/    
	MSG msg;
	while(GetMessage(&msg,NULL,0,0))//获取消息，填充msg结构体
    {
   		TranslateMessage(&msg);//翻译键盘消息
		DispatchMessage(&msg);//向窗口函数发送消息，让窗口函数处理
	}
	return msg.wParam;
}