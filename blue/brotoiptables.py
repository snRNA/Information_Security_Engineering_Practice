#!/usr/bin/python
# encoding=utf-8
# Filename: monitorLog.py
import os
import commands
import signal
import subprocess
import time
import re

logFile="/var/log/bro/current/notice.log"

def monitorLog(logFile):
    matchip=list()
    stoptime = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time() + 10))
    # 判断文件是否存在

    popen = subprocess.Popen('tail -f ' + logFile, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    pid = popen.pid
    print('Popen.pid:' + str(pid))
    while True:
        line = popen.stdout.readline().strip()
        # 判断内容是否为空
        if line:
            print(line)
            match = re.compile('(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)')
            words = match.findall(line)
            if words:
                del words[1:]
                print(words[0])
                if words[0] not in matchip:
                    matchip.append(words[0])
                    rule = 'iptables -A INPUT -s ' + words[0] + ' -j DROP'
                    print(rule)
                    a,b = commands.getstatusoutput(rule)
                    print(a)
                    print(b)
        # 当前时间
            thistime = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time()))
            # if thistime >= stoptime:
            #     # 终止子进程
            #     popen.kill()
            #     print '杀死subprocess'
            #     break
        # time.sleep(2)


if __name__ == '__main__':
    while True:
        if os.path.exists(logFile):
            monitorLog(logFile)