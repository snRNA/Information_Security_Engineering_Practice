#! /usr/bin/python
# -*- coding: utf-8 -*-

import requests
import sys
import os.path
import hashlib
import time


def writeLog(result):
    if (os.path.exists("virustotal.log")!=True):
        f = open("virustotal.log","w+")
        item = "%s\t%s\t%s\t%s\t%s\t%s\t%s\n" % ("time","file","vt_rate","vt_McAfee","vt_Qihoo-360","vt_kaspersky","vt_url")
        f.write(item)
        f.close()
    f = open("virustotal.log","a+")
    item = "%s\t%s\t%.2f\t%s\t%s\t%s\t%s\n" %(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()),sys.argv[1],result['positives']/result['total'],result['scans']['McAfee']['result'],result['scans']['Qihoo-360']['result'],result['scans']['Kaspersky']['result'],result['permalink'])
    f.write(item)
    f.close()


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

    return report_response

    # if (report_response['response_code'] == 1):
    #     for i in report_response['scans']:
    #         if (report_response['scans'][i]['detected'] == True):
    #             print(i + " : " + report_response['scans'][i]['result'])
    #             return True
    #             #break
    # else:
    #     return False

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

result = getScanReport(md5)

if (result['response_code'] == 1):
    print(result)
    print("Need to write log")
    writeLog(result)
    ## write log to file

else:  ## no result online ,need to upload files to analyze
    json_response = uploadfile(sys.argv[1])
    print(json_response)
    time.sleep(10)
    result = getScanReport(json_response['resource'])
    while result['response_code'] !=1:
        time.sleep(15)
        result = getScanReport(json_response['resource'])
        print(result)

    if result['response_code'] == 1:
        for i in result['scans']:
                if (result['scans'][i]['detected'] == True):
                    print(i + " : " + result['scans'][i]['result'])
                    print("Need to write log")
                    writeLog(result)
                    #return True
                    #break


# if(getScanReport(md5)):
#
#     exit(0)
# else:
#     json_response = uploadfile(sys.argv[1])
#
#     time.sleep(15)
#     if (getScanReport(json_response['resource']) == True):
#         exit(0)
#     else:
#         print('Safe file!')




