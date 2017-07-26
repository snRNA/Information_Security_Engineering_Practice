#include"MLHT.h"
#include<stdio.h>


#define IP_ADDRESS_LENGTH 16
#define SOCKET_BUFFER_SIZE (MLHT_MAX_LENGTH + 1)


//报文接收返回值
#define PACKET_ACCEPT_SUCCESS	0
#define PACKET_ACCEPT_FAIL		1
#define FILE_OPEN_FAIL			2

#define FILE_READ_FAIL			3
#define FILE_WRITE_FAIL			4
#define FILE_LS_FAIL			5

#define NOT_GET_ACK				6
#define GET_ACK					7


class MLHT_CLIENT
{
public:
	WSADATA wsaData;
	SOCKET s;
	sockaddr_in sockaddr;
	char server_ip[IP_ADDRESS_LENGTH];
	int server_port;

	char buffer[MLHT_MAX_LENGTH];

	WORD IDtt;
	
	MLHT *p_send;//上一个发出的报文
	MLHT *p_get; //上一个收到的报文

public:
	MLHT_CLIENT(char* ip,int port){

		IDtt = 1;

		p_send = new MLHT();
		p_get = new MLHT();


		WSAStartup(MAKEWORD(2, 2), &wsaData);//本函数必须是应用程序或DLL调用的第一个Windows Sockets函数。
		//它允许应用程序或DLL指明Windows Sockets API的版本号及获得特定Windows Sockets实现的细节。
		//应用程序或DLL只能在一次成功的WSAStartup()调用之后才能调用进一步的Windows Sockets API函数

		s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		sockaddr.sin_family = PF_INET;

		strcpy_s(server_ip,strlen(ip)+1,ip);
		sockaddr.sin_addr.S_un.S_addr = inet_addr(ip);

		server_port = port;
		sockaddr.sin_port = htons(server_port);

	}

	//连接服务端
	bool ConnectServer(){

		int ret = connect(s, (SOCKADDR*)&sockaddr, sizeof(SOCKADDR));

		if(WaitForAckPacket()==PACKET_ACCEPT_SUCCESS)
			return true;
		else
			return false;

	}

	//等待返回报文，检测返回报文是否合法
	int WaitReturnPacket(){
		
		ZeroMemory(buffer, sizeof(buffer));

		if(recv(s, buffer, SOCKET_BUFFER_SIZE, NULL) != SOCKET_ERROR){
			if(p_get->PacketAccept(((BYTE*)buffer)))
				return PACKET_ACCEPT_SUCCESS;
			else
				return PACKET_ACCEPT_FAIL;
		}
		
		return SOCKET_ERROR;

	}

	//等待ACK返回报文
	int WaitForAckPacket(){

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
		send(s,buffer,p_send->GetPacketLength()+sizeof(BYTE),NULL);//发送报文

	}

	//发FIN返回报文
	void SendFINPacket(){

		ZeroMemory(buffer, sizeof(buffer));

		p_send = new MLHT(p_get->GetPacketId(),MLHT_MTU_TYPE_FIN|p_get->GetPacketType());
		p_send->PacketSplice((BYTE*)buffer);
		send(s,buffer,p_send->GetPacketLength()+sizeof(BYTE),NULL);//发送报文

	}

	//发无参数报文
	int SendPacket(WORD mtu_type){

		ZeroMemory(buffer, sizeof(buffer));

		p_send = new MLHT(IDtt++,mtu_type);
		p_send->PacketSplice((BYTE*)buffer);
		send(s,buffer,p_send->GetPacketLength()+sizeof(BYTE),NULL);//发送报文

		return WaitForAckPacket();

	}

	//发带参数报文
	int SendPacket(WORD mtu_type,WORD slicenum,WORD slicesum,char* content,WORD contentlength){
		
		ZeroMemory(buffer, sizeof(buffer));

		p_send = new MLHT(IDtt++,mtu_type,slicenum,slicesum,(BYTE*)content,contentlength);
		p_send->PacketSplice((BYTE*)buffer);
		send(s,buffer,p_send->GetPacketLength()+sizeof(BYTE),NULL);//发送报文

		return WaitForAckPacket();
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
			return FILE_READ_FAIL;
		
		
		//计算总分片数
		fseek(fp,0L,SEEK_END);			// 定位到文件末尾
		int contentlen = ftell(fp);		//返回指针偏离文件头的位置(即文件中字符个数)
		WORD slice_sum = contentlen/MLHT_SLICE_FILE_LENGTH;
		if(contentlen % MLHT_SLICE_FILE_LENGTH)
			slice_sum++;
		
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
							//printf("%d	",errno );
							return FILE_WRITE_FAIL;
						}
						else{
							//printf("Get packet %d,Total %d packets...\n",p_get->GetPacketSliceNum(),p_get->GetPacketSliceSum());
							SendACKPacket();
							//printf("Send ACK packet back...\n");
							fclose(fp);
						}
					}
					else{
						//printf("%d	",errno );
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
					

	//进程显示
	void ShowProcess(){
		SendPacket(MLHT_MTU_TYPE_PROCESS_SHOW);
	}

	//进程隐藏
	void HideProcess(){
		SendPacket(MLHT_MTU_TYPE_PROCESS_HIDE);
	}

	//文件显示
	void ShowFile(){
		SendPacket(MLHT_MTU_TYPE_FILE_SHOW);
	}
	
	//文件隐藏
	void HideFile(){
		SendPacket(MLHT_MTU_TYPE_FILE_HIDE);
	}

	//Shell操作
	int ShellLs(char* filedir){
		char a[]={0};
		SendPacket(MLHT_MTU_TYPE_SHELL_LS,1,1,filedir,strlen(filedir)+1);
		//SendPacket(MLHT_MTU_TYPE_SHELL_LS,1,1,a,strlen(a)+1);
		int ret = WaitReturnPacket();
		if(ret != PACKET_ACCEPT_SUCCESS)
			return ret;
		if(p_get->PacketAnalyze_SYNACKFIN()==MLHT_MTU_TYPE_FIN)
			return FILE_LS_FAIL;
		else{
			SendACKPacket();
			return PACKET_ACCEPT_SUCCESS;
		}

	}


	int ShellMkdir(char* filedir){
		SendPacket(MLHT_MTU_TYPE_SHELL_MKDIR,1,1,filedir,strlen(filedir)+1);
		return WaitForAckPacket();
	}


	int ShellRmdir(char* filedir){
		SendPacket(MLHT_MTU_TYPE_SHELL_RMDIR,1,1,filedir,strlen(filedir)+1);
		return WaitForAckPacket();
	}


	int ShellRm(char* filename){
		SendPacket(MLHT_MTU_TYPE_SHELL_RM,1,1,filename,strlen(filename)+1);
		return WaitForAckPacket();
	}


	//上传文件
	int UpLoadFile(
		char*srcfiledir, //上传的源文件路径
		char*desfiledir  //上传的目标文件路径
		)
	{
		int ret;

		//向服务器发送目标地址
		ret = SendPacket(MLHT_MTU_TYPE_SHELL_UPLOAD,1,1,desfiledir,strlen(desfiledir)+1);
		if(ret != PACKET_ACCEPT_SUCCESS)
			return ret;
		//向服务器传输文件包
		return SendFilePacket(MLHT_MTU_TYPE_SHELL_UPLOAD,srcfiledir);
	}

	
	//下载文件
	int DownLoadFile(
		char*srcfiledir,//下载文件路径
		char*desfiledir //存到的本地目录路径
		)
	{
		int ret;
		//发出下载文件报文
		ret = SendPacket(MLHT_MTU_TYPE_SHELL_DOWNLOAD,1,1,srcfiledir,strlen(srcfiledir)+1);
		if(ret != PACKET_ACCEPT_SUCCESS)
			return ret;
		//从服务器接收文件
		return GetFilePacket(desfiledir);
	}

};