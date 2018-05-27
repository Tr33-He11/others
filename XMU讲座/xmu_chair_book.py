# by cjf @xmu 
import requests
import sys
from bs4 import BeautifulSoup 
import urllib.parse
sess=requests.Session()
hdrs={ 
    "Cache-Control": "max-age=0",
    "Origin": "http://ischoolgu.xmu.edu.cn",
    "Upgrade-Insecure-Requests": "1",
    "Content-Type": "application/x-www-form-urlencoded",
    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.62 Safari/537.36",
    "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8",
    "Referer": "http://ischoolgu.xmu.edu.cn/Default.aspx",
    "Accept-Language": "zh-CN,zh;q=0.9,en;q=0.8,ja;q=0.7,ru;q=0.6,zh-TW;q=0.5"
}
def encode(str):
  ret=""
  for ch in str:
    if (ch>="a" and ch<="z") or (ch>="A" and ch <="Z") or (ch>="0" and ch<="9"):
      ret+=ch 
    else: 
      ret+="%%%02X" %(ord(ch)) 
  return ret
  
def getParam_for_login(usr,pwd):
  global sess
  url="http://ischoolgu.xmu.edu.cn/" 
  resp=sess.get(url)
  soup=BeautifulSoup(resp.text,"html.parser")
  __VIEWSTATE=soup.find(attrs={"id":"__VIEWSTATE"}).get("value")
  __EVENTVALIDATION=soup.find(attrs={"id":"__EVENTVALIDATION"}).get("value") 
  param="__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE="+encode(__VIEWSTATE)+"&__VIEWSTATEGENERATOR=CA0B0334&__EVENTVALIDATION="+encode(__EVENTVALIDATION)+"&userName="+usr+"&passWord="+pwd+"&userType=1&sumbit=%B5%C7%A1%A1%C2%BD"
  return param
  
def urlencode_gbk(kv): 
  rst=""
  for key in kv:
    if len(rst)>0:
      rst+="&" 
    #key
    bStream=bytes(key,encoding="gbk")
    for ch in bStream:
      rst+="%%%02X" %(ch) 
    #val
    rst+="="
    val=kv[key]
    bStream=bytes(val,encoding="gbk")
    for ch in bStream:
      rst+="%%%02X" %(ch)
  return rst
def urlencode_utf8(kv): 
  rst=""
  for key in kv:
    if len(rst)>0:
      rst+="&" 
    #key
    bStream=bytes(key,encoding="utf-8")
    for ch in bStream:
      rst+="%%%02X" %(ch) 
    #val
    rst+="="
    val=kv[key]
    bStream=bytes(val,encoding="utf-8")
    for ch in bStream:
      rst+="%%%02X" %(ch)
  return rst
def getParam_for_book(html_text,submit_name):
  soup=BeautifulSoup(html_text,"html.parser")
  inputs=soup.find_all("input")
  param={submit_name:"预约该讲座"}
  for input in inputs: 
    if input.get("type")=="submit":
      continue
    key=input.get("name")
    val=input.get("value")
    param[key]=val
  return urlencode_gbk(param)
import time
def login(usr,pwd):
  global sess
  url="http://ischoolgu.xmu.edu.cn/Default.aspx"
  param=getParam_for_login(usr,pwd)
  try:
    resp=sess.post(url,data=param,headers=hdrs,timeout=5) 
    if resp.text.find("top.location.href=('admin_main.aspx');")!=-1:
      time_local = time.localtime(time.time())
      dt = time.strftime("%Y-%m-%d %H:%M:%S",time_local)
      log("[*]%s %s" %(dt,usr))
    else:
      log("[!]unknown error")
      sys.exit(0)
  except Exception as e:
    print("[!] %s 超时，正在重新登录... %(usr)")
    login(usr,pwd)
def parseTable(table):
  trs=table.find_all("tr")
  ret=[]
  i=0
  while(i<len(trs)):
    teacher=trs[i+1].find_all("td")[1].get_text() 
    title=trs[i+2].find_all("td")[1].get_text() 
    time=trs[i+7].find_all("td")[1].get_text() 
    addr=trs[i+8].find_all("td")[1].get_text()  
    chairId=trs[i].find("input").get("name"),trs[i].find("input").get("value")
    submit_name=""
    status=2  #0表示可以预约，1表示预约过了，2表示预约时间还没到和表示其他情况
    if len(trs[i+9].find_all("input"))>0:
      submit_name=trs[i+9].find("input").get("name")
      if trs[i+9].find("input").get("value")=="预约该讲座":
        status=0
      else:
        status=1
    else: 
      status=2 
    ret.append((time,addr,teacher,title,chairId,status,submit_name))
    i+=9
    while(i<len(trs) and trs[i].get_text().find("讲座日期")==-1):
      i+=1
  return ret

def sendEmail(to,title,content):
  param={
    "to":to,
    "title":title,
    "content":content
  }
  param=urlencode_utf8(param)
  url="http://xmucs.top:8080/apis/sendmail.php?"+param
  resp=requests.get(url)
  log("[*]发送邮件%s" %(resp.text))
  
def get():
  global sess
  url="http://ischoolgu.xmu.edu.cn/admin_bookChair.aspx"
  resp=sess.get(url)
  if resp.text.find("当前没有可预约的讲座，请密切关注研会主页通知")!=-1:
    log("[*]当前没有讲座可预约")
    ""
  else:
    log("[*]可预约的讲座列表")
    bookTable=BeautifulSoup(resp.text,"html.parser").find(attrs={"id":"bookTable"})
    chairs=parseTable(bookTable)#返回时间，地址，讲师，课题，(讲座key,val),状态(时间没到，取消预约，预约讲座)
    #print(chairs)
    for (time,addr,teacher,title,chair_id_val,status,submit_name) in chairs:       
        if status==2:
          log("    [*] 预约时间还没到或者没名额预约了[%s]" %(title))
        else:
          if status==1:
            log("    [*]之前已经预约了[%s]" %(title))
          else:
            if status==0:
              log("    [*]准备抢讲座 [%s]" %(title))
              param=getParam_for_book(resp.text,submit_name)
              sess.post(url,data=param,headers=hdrs,timeout=5)
              sendEmail("352871242@qq.com","讲座预约成功","时间:【%s】 地址【%s】讲师【%s】 标题【%s】" %(time,addr,teacher,title))
              log("[*]成功抢到讲座[%s],邮件已发送" %(title))
      
def log(data):
  print(data)
  return
  f=open("xmu_chair_book.log","a")
  f.write(data+"\n")
  f.close()

def qry():
  global sess
  url="http://ischoolgu.xmu.edu.cn/admin_chaircheck.aspx"
  resp=sess.get(url)
  soup=BeautifulSoup(resp.text,"html.parser")
  table=soup.find("table",attrs={"id":"selectresult"})
  rst=[]
  trs=table.find_all("tr")
  for i in range(1,len(trs)):
    tr=trs[i]
    tds=tr.find_all("td")
    cTime=tds[0].get_text().strip()
    cTech=tds[1].get_text().strip()
    cTitl=tds[2].get_text().strip()
    cPeri=tds[3].get_text().strip()
    print(" %s %-20s %-80s %s" %(cTime,cTech,cTitl,cPeri))
print("[*]程序开始运行:")
time_local = time.localtime(time.time())
dt = time.strftime("%Y-%m-%d %H:%M:%S",time_local)
print(dt)
while True:
  try:
    login("23020161153315","password")
    #qry()
    get()
  except Exception as e:
    log("except")
  time.sleep(3)
