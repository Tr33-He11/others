import os 
import zipfile
import rarfile
import shutil 
def extract(dir):  #将rar,zip文件解压到dir
  
  for root,dirs,files in os.walk(dir):
    for file in files:  #所有文件都尝试解压
      cur_cd=os.getcwd()
      os.chdir(root)
      cmd="7z e \"%s\"" %(file)      
      os.system(cmd)  
      os.chdir(cur_cd) 
  for root,dirs,files in os.walk(dir): #删除空文件夹
    for d in dirs:
      os.system("rmdir \"%s\\%s\"" %(root,d))
"""
(1)读取rar将文件解压到 rar文件夹。
(2)枚举文件夹，将子文件放入根文件。
"""
dir=input("要解压的目录")
stuList=extract(dir) 
#print(stuList)