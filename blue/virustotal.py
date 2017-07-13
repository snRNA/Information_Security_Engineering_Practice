#! /usr/bin/python
# -*- coding: utf-8 -*-

import requests
import sys
import os.path


if(len(sys.argv)<=1):
    print( "lack params!")
    sys.exit(1)

if(os.path.exists(sys.argv[1])!=True):
    print("file not found")
    sys.exit(1)


# 上传文件到virustotal，获取上传的文件的sha256值

params = {'apikey': ''}
files = {'file': (os.path.basename(sys.argv[1]), open(sys.argv[1], 'rb'))}
response = requests.post('https://www.virustotal.com/vtapi/v2/file/scan', files=files, params=params)
json_response = response.json()

#print(json_response)

# 根据获取的文件的sha256，从api请求文件的扫描结果

params = {'apikey': '', 'resource': json_response['resource']}
#params = {'apikey': '', 'resource': '7e25f7f43df851ae777658910b865a1d077e4883a6a8979cbc599729e4e94020'}
headers = {
  "Accept-Encoding": "gzip, deflate",
  "User-Agent" : "gzip,  My Python requests library example client or username"
  }
response = requests.get('https://www.virustotal.com/vtapi/v2/file/report',
  params=params, headers=headers)
report_response = response.json()

#print(report_response)

#print(report_response['scans'])

# 若扫描结构中有检测到威胁警报，则输出

for i in report_response['scans']:
    if(report_response['scans'][i]['detected'] == True):
        print(i +" : " + report_response['scans'][i]['result'])
        break
