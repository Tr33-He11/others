from selenium import webdriver
from selenium.webdriver.common.keys import Keys
import time
import json
import urllib
import sys
import requests
import os
import warnings
#参数
global p4p_id
global csrf_token
global g_cookie
global main_page
global g_sess
g_sess=requests.Session()
main_page="http://www2.alibaba.com/manage_ad_keyword.htm"
#函数
#(0)login()               登录并记录cookie和一些参数在全局变量中
#(1)get_top5(item_id)     返回一个数组，长度<=5，记录top5的价格float[12.1,10.2]..
#(2)get_items()           返回json解析后的字符串。且加上其排名。
'''
 返回格式
 {"costLimit":"150.00","currentPage":1,"data":[
   {    ...
        "avgClick":"0.00%",
        "avgCost":"0.00",
        "avgPrice":"9.8",
        "basePrice":"3.0", 
        "id":76803037478, 
        "keyword":"artificial grass tile", 
        "spend":"0.00", 
    }
    ]}
'''
#(3)show_items(items)           显示包含json字符串
#(4)get_rank(data)              将items_data解析为obj,并加入rank
#(5)update_price(item_id,price,basePrice)  #(str,float,float)
#(6)update_items(items_obj)    #更新所有价格
def get_special_param(data,key):
  cnt=2
  idx=data.find(key)
  while (cnt):
    if (data[idx]=="'"):
      cnt-=1
    idx+=1
  val=""
  while (data[idx]!="'"):
    val+=data[idx]
    idx+=1
  return val
  
def init_param(cookies):
  global p4p_id
  global csrf_token
  global main_page
  global g_cookie
  global g_sess
  
  print("[*]正在初始化参数...") 
  #保存cookie
  g_cookie={} 
  for cookie in cookies:
    g_cookie[cookie["name"]]=cookie["value"] 
    
  #main_page_src=requests.post(main_page,cookies=g_cookie).text;
  main_page_src=g_sess.post(main_page,cookies=g_cookie).text;
  p4p_id=get_special_param(main_page_src,"_dt_p4p_id_")
  csrf_token=get_special_param(main_page_src,"_csrf_token_") 
  print("[*]参数初始化成功")
  return 

def has_login(driver):
  global main_page  
  while True:
    if driver.current_url == main_page: 
      break;
    #print(driver.current_url);
    time.sleep(1)  
  print("[*]登录成功!")
def login(driver,usr,pwd):                                                  #登录成功后返回cookie
  print("[*]正在登录(可能需要手动输入验证码)...");
  login_url="https://login.alibaba.com/?flag=1&return_url=http%3A%2F%2Fwww2.alibaba.com%2Fmanage_ad_keyword.htm"  #url返回到main_page
  login_iframe_id="alibaba-login-box"
  login_user_id="fm-login-id"
  login_pass_id="fm-login-password"
  #
  driver.get(login_url)
  driver.switch_to_default_content();
  driver.switch_to_frame(login_iframe_id)         #转到iframe中
  ''' 
  #以下代码太慢了
  #user_ele=driver.find_element_by_id(login_user_id); 
  #user_ele.send_keys(usr)
  #pass_ele=driver.find_element_by_id(login_pass_id)
  #pass_ele.send_keys(pwd);
  '''
  driver.execute_script("document.getElementById('%s').value='%s';" %(login_user_id,usr));
  driver.execute_script("document.getElementById('%s').value='%s';" %(login_pass_id,pwd));
  #driver.find_element_by_id("fm-login-submit").click()
  #driver.switch_to_default_content()
  
  has_login(driver)
  init_param(driver.get_cookies())
  return 
  

def get_top5(item_id): 
  global p4p_id
  global csrf_token
  global g_cookie
  global g_sess
  #url
  url="http://www2.alibaba.com/asyGetTopFivePrice.do"
  #参数
  param={
    "json": '{"id":"%s"}' %(item_id),
    "_dt_p4p_id_" : "%s"  %(p4p_id),
    "_csrf_token_": "%s"  %(csrf_token)
  }  
  #cookie
  #content=requests.post(url,data=param,cookies=g_cookie,verify=False).text
  content=g_sess.post(url,data=param,cookies=g_cookie,verify=False).text
  obj=json.loads(content) 
  fobj=[]
  for x in obj["data"]:
    fobj.append(float(x))
  return  fobj 

def get_items(): 
  global p4p_id
  global csrf_token
  global g_cookie
  global g_sess
  #url
  url="https://www2.alibaba.com/asyGetAdKeyword.do"
  #参数
  json_val='{"status":"all","cost":"all","click":"all","exposure":"all","cpc":"all","qsStar":"all","kw":"","isExact":"N","date":7,"tagId":-1,"delayShow":false,"recStrategy":1,"recType":"recommend"}'
  param={
    "json": json_val,
    "_dt_p4p_id_" : "%s"  %(p4p_id),
    "_csrf_token_": "%s"  %(csrf_token)
  }  
  #cookie 
  #data=requests.post(url,data=param,cookies=g_cookie,verify=False).text;
  data=g_sess.post(url,data=param,cookies=g_cookie,verify=False).text;
  return get_rank(data)
  
def get_rank(data):
  global p4p_id
  global csrf_token
  global g_cookie
  global g_sess
  #url
  url="https://www2.alibaba.com/asyGetKeywordEstimateRank.do"
  #参数
  obj=json.loads(data)
  ids=""
  keywords=""
  items=obj["data"]
  for item in items: 
    if ids!="":
      ids+=","
      keywords+=","
    ids+=str(item["id"])
    keywords+=item["keyword"]
  json_val='{"ids":"%s","keywords":"%s"}' %(ids,keywords)

  param={
    "json": json_val,
    "_dt_p4p_id_" : "%s"  %(p4p_id),
    "_csrf_token_": "%s"  %(csrf_token)
  }  
  #cookie 
  #content=requests.post(url,data=param,cookies=g_cookie,verify=False).text;
  content=g_sess.post(url,data=param,cookies=g_cookie,verify=False).text;
  content=json.loads(content)
  #if (content["errorMsg"]!="")  
  for idx in range(0,len(content["returnData"]["rank"])):
    items[idx]["rank"]=content["returnData"]["rank"][idx]
  return obj
  
def update_price(item_id,price,basePrice):
  global p4p_id
  global csrf_token
  global g_cookie
  global g_sess
  #url
  url="https://www2.alibaba.com/asyBatchUpdateWordsPrice.do"
  #参数
  json_val='{"basePrice":"%.1f","kwIds":%s,"cmd":"set","price":"%.1f"}' %(basePrice,item_id,price)
  param={
    "json": json_val,
    "_dt_p4p_id_" : "%s"  %(p4p_id),
    "_csrf_token_": "%s"  %(csrf_token)
  }  
  #cookie 
  #data=requests.post(url,data=param,cookies=g_cookie,verify=False).text; 
  data=g_sess.post(url,data=param,cookies=g_cookie,verify=False).text; 
  obj=json.loads(data)
  if (obj["errorMsg"]!=""):
    print("[!]update price %s failed %s!" %(item_id,obj["errorMsg"]))
    return False
  return True

def update_items(obj):
  print("\n[*]----------------------------PRICE UPDATE------------------------------------")
  format="    [%2s]  %-30s  %8s  %8s  %8s  %s"
  print(format %("--","Keyword","OldPrice","NewPrice","BasePrice","TOP5")) 
  print("\n")
  items=obj["data"]
  idx=0
  for item in items:
    id        = item["id"]
    price     = float(item["price"])
    rank      = int(item["rank"])
    basePrice = float(item["basePrice"]) 
    top5      = get_top5(id)
    new_price = 0 
    if int(item["qsStar"]) < 3:
      continue                                      #三星以上才要调整
    while (len(top5)<5):                            #不足5个补满
      top5.append(basePrice)
    
    if top5[4]>=20.0:            #第5名高于20元时输入底价
      new_price=basePrice 
    else: 
      new_price = top5[4]                                  #抢第五名
      if top5[0]-new_price <10.0 or top5[0]<new_price*1.5:     #上升4名 
        new_price=top5[0]
      else: 
        if top5[1]-new_price <5.0 or top5[1]<new_price*1.3:    #上升3名
          new_price =top5[1]
        else:
          if top5[2]-new_price <3.0 or top5[2]<new_price*1.2: #上升2名
            new_price=top5[2]
          else:
            if top5[3]-new_price <1.0 or top5[3]<new_price*1.1:#上升1名
              new_price = top5[3]
    idx+=1 
    print(format %(idx,item["keyword"],price,new_price,basePrice,top5)) 
    if new_price != price :   
      update_price(id,new_price,basePrice) 
  return 
  
def translate_rank(rank):
  r=int(rank)
  if (r==-1):
    return "Others"
  if (r==0):
    return "--"
  return "%d" %(r)
def show_items(obj): 
  print("\n[*]-------------当前价格信息----------------------------第%d页" %(obj["currentPage"]))
  items=obj["data"]
  format="    [%02s]  %-30s %5s  %15s %8s  %8s %15s"
  print(format  %("--","Keywords","Price","Rank","Spend","qsStar","Status\n")) 
  idx=0
  for item in items:
    idx+=1
    print(format %(idx,item["keyword"],item["price"],translate_rank(item["rank"]),item["spend"],item["qsStar"],item["adKeywordDO"]["status"]))
 
def display_items(data):  #显示json数据 
  print("[*]当前keywords详情")
  obj=json.loads(data)
  idx=0
  for item in obj:
    idx+=1
    print("  [%02d] %-30s %10s %10s  %s" %(idx,item["keyword"],item["price"],item["price_rank"],"top5")) #item["top5"]))
'''
def set_price(driver):
  ad_keyword_url="http://www2.alibaba.com/manage_ad_keyword.htm"
  driver.get(ad_keyword_url)
'''
def main():
  #登录并记录cookie和一些参数在全局变量中
  
  #if len(sys.argv) <3 :
  #  print("缺少用户名/密码。用法: xx.py username password")
  #  return 
  warnings.filterwarnings("ignore")
  driver = webdriver.Chrome() 
  #login(driver,sys.argv[1],sys.argv[2]) 
  login(driver,"cn200021818","pass") 
  driver.close() 
  while True:
    items_obj=get_items()                  #读取所有keywords
    update_items(items_obj)                #更新所有价格
    item_obj=get_items()                   #DEBUG重新读取，看看是否修改成功
    show_items(items_obj)                  #显示所有keywords 
    print("[*]--------当前时间[%s],15分钟后更新----------" %time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time())))
    time.sleep(60*15)
main() 