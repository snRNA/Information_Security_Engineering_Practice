#include"MLHT_CLIENT.h"


#define SCREEN_WIDTH (GetSystemMetrics(SM_CXSCREEN)>=1920?1280:(GetSystemMetrics(SM_CXSCREEN)>=1600?1024:800)) //屏幕宽度
#define SCREEN_HEIGHT (GetSystemMetrics(SM_CYSCREEN)>=1080?720:(GetSystemMetrics(SM_CYSCREEN)>=900?768:600))   //屏幕高度
#define SCREEN_X (GetSystemMetrics(SM_CXSCREEN)-SCREEN_WIDTH)/2     //屏幕横坐标
#define SCREEN_Y (GetSystemMetrics(SM_CYSCREEN)-SCREEN_HEIGHT)/2    //屏幕纵坐标


#define BUTTON_WIDTH    (SCREEN_WIDTH/8)		//窗口控制按钮宽
#define BUTTON_HEIGHT   (SCREEN_HEIGHT/12)      //窗口控制按钮高
#define BUTTON_DISTANCE  BUTTON_HEIGHT			//窗口控制按钮纵间隔=窗口控制按钮高+窗口控制按钮间隔
#define BUTTON_X		(SCREEN_WIDTH/10)       //窗口控制最左上按钮与y轴间距
#define BUTTON_Y		(SCREEN_HEIGHT/10)      //窗口控制最左上按钮与x轴间距


#define BUTTON_NUM 11             //窗口控制按钮数


#define BUTTON_CLIENT_LINK    0  //服务器连接按钮编号
#define BUTTON_FILE_UPLOAD    1  //文件上传按钮编号
#define BUTTON_FILE_DOWNLOAD  2  //文件下载按钮编号
#define BUTTON_SHELL_WINDOW   3  //远程桌面开启/关闭按钮编号

#define BUTTON_PROCESS_HIDE   4  //远程桌面开启/关闭按钮编号
#define BUTTON_FILE_HIDE	  5  //远程桌面开启/关闭按钮编号
#define BUTTON_SHELL_INPUT	  6  //远程桌面开启/关闭按钮编号

#define BUTTON_SHELL_LS		  7  //远程桌面开启/关闭按钮编号
#define BUTTON_SHELL_MKDIR    8  //远程桌面开启/关闭按钮编号
#define BUTTON_SHELL_RMDIR    9  //远程桌面开启/关闭按钮编号
#define BUTTON_SHELL_RM		 10  //远程桌面开启/关闭按钮编号


#define SHELL_WIDTH (SCREEN_WIDTH/2)						//SHELL窗口控制按钮宽
#define SHELL_HEIGHT (SCREEN_HEIGHT/2)  					//SHELL窗口控制按钮高
#define SHELL_X BUTTON_X									//SHELL窗口控制最左上按钮与y轴间距
#define SHELL_Y BUTTON_Y+3*(BUTTON_DISTANCE+BUTTON_HEIGHT)  //SHELL窗口控制最左上按钮与x轴间距


extern HWND hwndbutton[BUTTON_NUM];   //窗口按钮


class HackClient{
public:
	MLHT_CLIENT* mc;

	bool processhide;
	bool filehide;

public:
	HackClient(){
		processhide = filehide = false;
	}

	void Init(HWND);						//主窗口初始化
	void ScreenPaint(HWND);					//屏幕绘制
	void CommandExecute(HWND,int);			//指令执行

	char* GetFilename(char* FullPathName);	//获取全路径中的文件名

};