# Snort 利用 Guardian 联立iptables 进行防御拦截     

 
## 搭建步骤  
  
### snort
 
		apt-get update 
		apt-get install snort(要求输入网段，输入内网网段)  
		
创建规则   
  
		vim /etc/snort/rules/myrule.rules

写入规则 

		alert tcp any any -> any any (msg:"backdoor detected";content: "red"; sid: 12312737;)  
 
在/etc/snort/snort.conf 配置文件中includ规则 （在 step 8 之前）  
 
		include $RULE_PATH/myrule.rules  
  
snort 开启监听   
  
		snort -vd -i eth1 -l /var/log/ -c /etc/snort/snort.conf -A full  # 监听指定的内网网卡  会在/var/log/下面自动创建alert文件

----


### guardian     
  
		wget http://www.chaotic.org/guardian/guardian-1.7.tar.gz  
		tar xzvf  guardian-1.7.tar.gz
		  
		cd guardian-1.7
		cp guardian.pl /usr/local/bin
        cp scripts/iptabls_block.sh /usr/local/bin/guardian_blcok.sh  （先将文件中iptables 指令由INPUT换成FORWARD）
		cp scripts/iptabls_unblock.sh /usr/local/bin/guardian_unblcok.sh  （先将文件中iptables 指令由INPUT换成FORWARD）  
		touch /etc/guardian.target （其中逐行写入内网要保护主机的Ip）

修改guardian 配置文件  

        HostIpAddr   网关内网IP

		Interface    外网网卡  

		LogFile      /var/log/guardian/guardian.log （若暂时没有 要先创建）  
		  
		AlertFile    /var/log/alert （填入snort 报警日志所在）   
		
		TargetFile  /etc/guardian.target
		TimeLimit   20      

安装依赖模块  

		perl -MCPAN -e shell  

在perl控制台中输入  

		insall Perl4::CoreLibs
 
保存好后控制台输入指令 cd 家目录 运行脚本  

		guardian.pl -c guardian-1.7/guardian.conf  (-d 可以运行成调试模式)     
  

  



  

  

