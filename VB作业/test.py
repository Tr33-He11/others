import os 
import zipfile
import rarfile
import shutil
def rmdir():
  for root,dirs,files in os.walk(os.getcwd()):
    for dir in dirs:
      shutil.rmtree(dir) 
      print("删除目录 %s/%s" %(root,dir))
def extract(dir):  #将rar,zip文件解压到dir
  for root,dirs,files in os.walk(dir):
    for file in files:  #所有文件都尝试解压
      cur_cd=os.getcwd()
      os.chdir(root)
      cmd="7z e \"%s\"" %(file)      
      os.system(cmd)
      rmdir()
      os.chdir(cur_cd)
"""
(1)读取rar将文件解压到 rar文件夹。
(2)枚举文件夹，将子文件放入根文件。
"""
dir=input("要解压的目录")
print(dir)
#stuList=extract(dir) 
#print(stuList)