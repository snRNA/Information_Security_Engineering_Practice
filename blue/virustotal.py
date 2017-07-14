#! /usr/bin/python
# -*- coding: utf-8 -*-

import requests
import sys
import os.path
import hashlib
import time


def getMD5(filename):
    f = open(filename, 'rb')
    m = hashlib.md5()
    while True:
        data = f.read(4096)
        if not data:
            break
        m.update(data)
    return m.hexdigest()

def getScanReport(resource):
    params = {'apikey': '4f00ad19447f5c9269e00dcccf173ac458860bb0a8763e8e87af66dddcff962b','resource':resource}
    headers = {
        "Accept-Encoding": "gzip, deflate",
        "User-Agent": "gzip,  My Python requests library example client or username"
    }
    response = requests.get('https://www.virustotal.com/vtapi/v2/file/report',
                           params=params, headers=headers)
    report_response = response.json()

    if (report_response['response_code'] == 1):
        for i in report_response['scans']:
            if (report_response['scans'][i]['detected'] == True):
                print(i + " : " + report_response['scans'][i]['result'])
                return True
                #break
    else:
        return False

def uploadfile(filename):
    params = {'apikey': '4f00ad19447f5c9269e00dcccf173ac458860bb0a8763e8e87af66dddcff962b'}
    files = {'file': (os.path.basename(filename), open(filename, 'rb'))}
    response = requests.post('https://www.virustotal.com/vtapi/v2/file/scan', files=files, params=params)
    json_response = response.json()
    return json_response


if(len(sys.argv)<=1):
    print( "lack params!")
    sys.exit(1)

if(os.path.exists(sys.argv[1])!=True):
    print("file not found")
    sys.exit(1)

md5 = getMD5(sys.argv[1])

if(getScanReport(md5)):
    exit(0)
else:
    json_response = uploadfile(sys.argv[1])
    time.sleep(15)
    if (getScanReport(json_response['resource']) == True):
        exit(0)
    else:
        print('Safe file!')




