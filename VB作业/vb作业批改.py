import os
import sys
import XMUWEB
import Evaluator
import time

#先下载所有作业的打包文件到本地解压后，再调用这个函数读取提交的作业。

def readHomework(dir):         #格式{"sid":{"file1":"content",..}},"sid2":...}
  if dir.find("homework")==-1:
    dir+="\\homework"
  stuList={}
  for root,dirs,files in os.walk(dir): 
     if root.split("\\")[-2]=="homework": 
        underLineIdx=len(root)-1
        while root[underLineIdx]!="_":
          underLineIdx-=1
        #记录以下信息
        sid=root[underLineIdx-14:underLineIdx]  #学号
        name=root[underLineIdx+1:len(root)]     #姓名
        file_list={}                            #文件列表(前缀删了)+文件内容 
        for file in files:  
          idx=len(file)-1 
          stripFilename=file 
          
          file_content=""         #当后缀名为vbp或者frm时，读取其数据
          suffix=os.path.splitext(file)[1] 
          if suffix==".vbp" or suffix==".frm":
            fobj=open(root+"/"+file,"r")
            try:
              file_content=fobj.read()
            except Exception as e:
              print(suffix)
              print(file)
              file_content=""
            fobj.close()
          file_list[stripFilename.lower()]=file_content 
        stuList[sid]=file_list
  return stuList

dir=input("输入实验打包解压后的目录：")
stuList=readHomework(dir)                  #格式{"sid":{"file1":"content",..}},"sid2":...} 
web=XMUWEB.XMUWEB()
#courseId=input("输入课程ID:")
hwId=input("输入作业ID:")
list=web.getStudentList(hwId)         #获取{"sid":("hwaid","name"),"sid2":...}
print("[*]课程名单:%d  <--> 作业名单 一一对应%d " %(len(list),len(stuList)))

for sid in stuList:
  if not (sid in list):
    print("%s 在web没有找到对应的" %(sid))
    sys.exit(0)

newStuList=[]
idx=1
#***********
debug=False
for sid in stuList: 
  hwaid=list[sid][0]
  name=list[sid][1]
  file_list=stuList[sid] 
  txtAns=web.getStuAnswer(hwaid)
  file_list["txtAns"]=txtAns
  
  print("\n************************[*%d] %s[%s]***********************" %(idx,name,hwaid))
  score,comment=Evaluator.homework(file_list,dir,debug) 
  #newStuList.append(hwaid,score,comment,"再接再厉"+Evaluator.nice())
  print(" %s ------分数为:%s %s" %(name,score,comment))
  idx+=1
  #if not debug:
  #web.setScore(hwaid,score,"","再接再厉"+Evaluator.nice())   
  #time.sleep(3)