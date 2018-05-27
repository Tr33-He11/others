import os
import sys
import XMUWEB 
import time

if __name__=="__main__":
    xmuweb=XMUWEB.XMUWEB()
    xmuweb.login("A23020161153315","pass")   #登陆course平台
    xmuweb.showCourses()                       #显示所有课程和课程下所有作业  
    hwId=input("输入作业ID:")
    list=xmuweb.getStudentList(hwId)           #获取已提交作业的学生。格式为：{"学号1":("hwaid1","姓名1"),"学号2":...} 

    for sid in list:                           #枚举已经提交作业的学生
      hwaid=list[sid][0]                       #学生ID
      name=list[sid][1]                        #学生名字
      print("[姓名]%s [学号]%s [hwaid]%s" %(name,sid,hwaid))
      ans=xmuweb.getStuAnswer(hwaid)           #读取同学hwaid提交的作业(不包括文件)
      print("[作业]\n%s" %(ans)) 
      #以下对当前作业进行批改
      score=float(input("输入当前学生评分:"))
      comment1="做的不错"                      #批改
      comment2="继续努力"                      #评价
      xmuweb.setScore(hwaid,score,comment1,comment2) 