import sys
import os 
import time
def log(txt):
  f=open("log.txt","a")
  print(txt)
  f.write(txt+"\n")
  f.close()
  
def start(stuList,frm_name,vbp_name):  #处理某题所有提交 
  for sid in stuList:
    vbp_path="./vb/%s/%s.vbp" %(sid,vbp_name)
    frm_path="./vb/%s/%s.frm" %(sid,frm_name) 
    print("[*]学号%s" %(sid))
    print("[*]正在打开vb项目")
    #打开工程
    try:
      os.startfile(vbp_path)
    except Exception as e:
      print("[*]无法打开项目%s" %(vbp_path))
      input("")
    #打开代码
    try:
      frm_f=open(frm_path)
      frm_content=frm_f.read()
      print("作业内容:"+frm_content)
      frm_f.close()
    except Exception as e:
      print("[*]文件%s处理失败%s" %(frm_path))
      input() 
    #评分并写入log
    while True:
      try:
        score=float(input("给分"))
        break
      except Exception as e:
        print("[*]重新输入分数")
        
    log("%s %s %s" %(sid,frm_name,score))
    input("\n[*]回车继续下一个...")
  
def main():  
  frm_vbp={"编程题1":"工程3","编程题2":"工程4","设计题1":"工程2","设计题2":"工程2"}
  #读取学号
  stuList=[]
  for root,dirs,files in os.walk("vb"):
    if root!="vb": 
      sid = root.split("\\")[1]
      stuList.append(sid) 
  #
  for problem in frm_vbp:
    start(stuList,problem,frm_vbp[problem])
  
if __name__ == "__main__":
   main()