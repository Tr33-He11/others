#by cjf@xmu
#VB作业批改
"""
hwaid ：学生ID
hwId  ：作业ID
courseId：课程ID。
一个courseId对应多个hwId.
"""
import requests
import sys
import os
from bs4 import BeautifulSoup
import urllib
from urllib import parse,request 
import html
class XMUWEB:
#private: 
  sess="" 
  hwId2cId={}
  def login(self,usr,pwd):           #登陆
    self.sess=requests.Session()
    url="http://course.xmu.edu.cn/meol/homepage/common/login.jsp"
    param={
      "logintoken":"1508208320009",
      "IPT_LOGINUSERNAME":usr,
      "IPT_LOGINPASSWORD":pwd
    }
    resp=self.sess.post(url,data=param)
    if resp.text.find("个人首页")!=-1: 
      print("[*]%s:登录成功!" %(usr))
      return True
    else: 
      print("login failed!");
      sys.exit(0);  
       
  def getHomeworkAuth(self,hwId):       #批改某个作业需要权限
    url="http://course.xmu.edu.cn/meol/common/hw/hwanswer.jsp?hwtid=%s&restart=true" %(hwId)
    resp=self.sess.get(url)
    if resp.text.find("<TITLE>错误</TITLE>")!=-1:
      print("[!]HomeWork %s权限申请失败..." %(hwId))
      sys.exit(0)
    return 
    
  def getCourseAuth(self,courseId):      #要查看某门课的作业需要权限
    #url="http://course.xmu.edu.cn/meol/lesson/mainJumpPage.jsp?courseId=%s&t=hw" %(courseId) 
    url="http://course.xmu.edu.cn/meol/jpk/course/blended_module/module_menu_lessonmanage.jsp?courseId=%s&curstyle=default" %(courseId)
    resp=self.sess.get(url)
    if resp.text.find("<TITLE>错误</TITLE>")!=-1: 
      print("[!]Course %s 权限申请失败" %(courseId))
      sys.exit(0)
  
  def getStudentList(self,hwId):  #获取学生名单[sid,name,hwaid]
    stuList={}                             #存储 sid-> 姓名+hwaid
    courseId=self.hwId2cId[hwId];
    #print("courseId=%s" %(courseId))
    self.getCourseAuth(courseId)
    for page in range(1,3):
      url="http://course.xmu.edu.cn/meol/common/hw/hwanswer.jsp?hwtid=%s&restart=true&s_gotopage=%d" %(hwId,page)
      resp=self.sess.get(url)
      if resp.text.find("<TITLE>错误</TITLE>")!=-1: 
        print("err")
        sys.exit(0)
        
      soup=BeautifulSoup(resp.text,"html.parser")
      table=soup.find_all("table",attrs={"class":"valuelist"})[0]
      trs=table.find_all("tr")
      for trIdx in range(1,len(trs)):  #从1开始，过滤掉列名
        tr=trs[trIdx]
        input=tr.find_all("input")[0]
        
        sid=input.get("value")[0:14]
        hwaid=input.get("id")[3:]      #前3个为id_前缀
        name=tr.find_all("td")[2].get_text().strip()
        stuList[sid]=(hwaid,name)
        #print("%s %s %s" %(sid,hwaid,name))
    return stuList
    
  def showHomeworks(self,courseId):       #显示courseId的所有作业
    url="http://course.xmu.edu.cn/meol/common/hw/hwtask.jsp?lid="+courseId
    resp=self.sess.get(url)
    soup=BeautifulSoup(resp.text,"html.parser")
    trs=soup.find_all("form",attrs={"name":"form1"})[0].find_all("tr")
    for i in range(1,len(trs)):
      tr=trs[i]
      a=tr.find_all("a",attrs={"class":"infolist"})[0]
      hwName=a.get_text().strip()               #课程名
      href=a.get("href")     
      hwId=href[len(href)-4:len(href)]  #课程ID
      self.hwId2cId[hwId]=courseId
      tds=tr.find_all("td")
      subTot=int(tds[6].get_text())
      needCor=int(tds[7].get_text())
      self.getHomeworkAuth(hwId)        #申请批改当前作业的权限
      print("     [*]【作业ID %s 批改进度%s/%s】 %s" %(hwId,subTot-needCor,subTot,hwName)) 
    
  def showCourses(self):               #显示要批改的课程
    url="http://course.xmu.edu.cn/meol/welcomepage/teacher/interaction_reminder.jsp"
    resp=self.sess.get(url)
    soup=BeautifulSoup(resp.text,"html.parser") 
    all_a=soup.find_all("a",attrs={"target":"_blank"})
    print("\n[*]有以下课程作业要批改")
    courses=set()
    for a in all_a: 
      courseId=a.get("href").split("?")[1]
      courseId=courseId[4:9] 
      courseName=a.get_text();
      courses.add((courseId,courseName)) 
      
    for course in courses:
      print("  [*][课程ID%s] [%s] " %(course[0],course[1])) 
      self.getCourseAuth(course[0])   #先申请查看作业的权限
      self.showHomeworks(course[0])
    print("")
  def strToGbk(self,data):                #按gbk编码，并转为%xx格式
    bstream=data.encode("gbk")
    gbkCode=""
    for b in bstream:
     gbkCode+="%%%x" %(b)  
    return gbkCode
    
  def param_encode(self,param):  #将dict类型的参数转为post/get参数字符串
    ret=""
    for key in param:
      if len(ret)>0 :
        ret+="&"
      ret+=key+"="+param[key]
    return ret  
    
  def setScore(self,hwaid,score,ipt_body,comment):                        
    #print(" [*]【分数批改】 将[%s]的分数设置为%s" %(hwaid,str(score)))
    url="http://course.xmu.edu.cn/meol/common/hw/hwanswer.comment.do.jsp"
    
    cook="JSESSIONID="+self.sess.cookies["JSESSIONID"] 
    
    param=self.param_encode({
      "hwaid":hwaid,
      "jumpseq":"0",
      "IPT_BODY":self.strToGbk("<p>%s </p>" %(ipt_body)),
      "comments":self.strToGbk(comment),
      "fullmark":"100.0",
      "mark":str(score)
    })
    param=bytes(param,encoding="utf-8")
    try:
      req=urllib.request.Request(url,param,{"Cookie":cook,"name":"cjf"})
      resp=urllib.request.urlopen(req)
    except Exception as e:
      do_nothing=""
    return  
  
  def getStuAnswer(self,hwaid):     #读取学生提交的答案。文本答案，不包括提交的文件。
    url="http://course.xmu.edu.cn/meol/common/hw/hwanswer.comment.jsp?hwaid="+hwaid
    resp=self.sess.get(url)
    soup=BeautifulSoup(resp.text,"html.parser") 
    table=soup.find_all("table",attrs={"id":"table2"})[0] 
    inp=table.find("input").get("value")
    txt=html.parser.unescape(inp)
    return BeautifulSoup(txt,"html.parser").get_text()
    #return html.parser.unescape(inp)
    
  def __init__(self):
    return 
  
  def test(self):
    self.setScore(1,97.9,"做的不错，【】","中文编码")
  
 
