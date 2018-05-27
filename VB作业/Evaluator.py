import random
import sys
def endOfLine():
   charList=[",","..",",,","-"]
   return charList[random.randint(0,len(charList)-1)]
def nice():
   charList=["~.~","-.-","O(∩_∩)O~","ヽ(ˋДˊ)ノ ","ヽ(ˋ▽ˊ)ノ"," °(°ˊДˋ°) °",
    "┑(~▽~)┍ ","┑(~Д~)┍"," ┑(~。。~)┍ ","╮(╯Д╰)╭ ╮","(╯▽╰)╭"]
    
   return charList[random.randint(0,len(charList)-1)]
   
def getcontent(file_list,file):
    for f in file_list:             #在file_list中查找当前文件是否存在(大小写不敏感+"-"替换成"_"
       f2=f.strip().lower().replace("-","_")
       f3=file.strip().lower().replace("-","_")
       if f2.find(f3)!=-1:
         return file_list[f]
    return ""     
def getscore(file_list,xfile_list):  #file_list为提交的文件名+文件内容  xfile_list为所需的文件名和关键字
   score=100.0
   comment="" 
   for file in xfile_list:
     #当前文件得分
     cur_file_score=xfile_list[file][0]
     #在提交的文件中找到对应的文件
     find=False
     file_submit=None
     for f in file_list:             #在file_list中查找当前文件是否存在(大小写不敏感+"-"替换成"_"
       f2=f.strip().lower().replace("-","_")
       f3=file.strip().lower().replace("-","_")
       if f2.find(f3)!=-1:
         find=True  
         file_submit=f
         
     if find:
       #枚举关键字，看看file_list是否包含
       for idx in range(1,len(xfile_list[file]),1):
         key=xfile_list[file][idx].lower()
         if file_list[file_submit].lower().find(key)==-1: 
           score-=1
           comment+="缺少关键字"+key+";"
     else:
       comment+="文件\"%s\"没提交%s<br>" %(file,endOfLine())
       score-=cur_file_score 
   if score<70:
     score=70 
   if comment=="": 
     comment="实验做的不错"+nice() 
   return (score,comment)
def homework1(file_list):
   xfile_list={
     "p1.vbp":[4],
     "p1.frm":[4],
     "p2.vbp":[4],
     "p2.frm":[4],
     "p3_1.vbp":[4],
     "p3_1.frm":[4],
     "p3_1.frx":[4],
     "p3_2.vbp":[4],
     "p3_2.frm":[4],
     "p3_2.frx":[4]
     }
   return getscore(file_list,xfile_list)
def homework4(file_list):
   xfile_list={ 
     "p3_4.frm":[4],
     "p3_4.vbp":[4],
     "p3_5.vbp":[4],
     "p3_5.frm":[4],
     "p3_6.frm":[4]
     }
   return getscore(file_list,xfile_list)   
xfile_list5={ 
     "图形1.frm":[4,"print","tab"],
     "图形2.frm":[4,"print","spc"],
     "p41.frm":[4],
     "p42.frm":[4],
     "p44.frm":[4],
     "p415.frm":[4],
     "p416.frm":[4,"x各位数字之和"]
     }
xfile_list6={
     "txtAns":[5,"2500"],
     "4_3.frm":[5,"\"不及格\"","\"及格\"","优秀","60","90"],
     "4_3.vbp":[2],
     "4-20.frm":[4,"Print","200"],
     "4-20.vbp":[2],
     "4_21.frm":[4,"print","*"],
     "4_21.vbp":[2],
     "4_22.frm":[4,"\"*","InputBox","for","next"],
     "4_22.vbp":[2],
     "4_31.frm":[4,"if","then"]
}
xfile_list7={
     "txtAns":[2,"B","40"],
     "p46.frm":[4,"for","99","Mod","print"],
     "p47.frm":[4,"next","999","5","then"],
     "p48.frm":[4,"127","-","Mod","chr"],
     "p49.frm":[4,"15000","<","+","print"],
     "p410.frm":[4,"InputBox","print"],
     "p411.frm":[4,"chr","print"],
     "p412.frm":[4,"10","+","*","for","print"]
}
xfile_list8={
     "txtAns":[2,"D","B"],
     "p51.frm":[10,"MultiLine       =   -1  'True",
                #"Java程序设计",
                "VC程序设计",
                #"VB程序设计",
                "任选课",
                #"限选课",
                "网络技术基础",
                "数据结构",
                #"多媒体技术",
                "你选择的课程",
                ".Value",
                "if",
                ".text"],
     "p55.frm":[10,"停止",
                "移动",
                "计时器应用示例",
                "VB.Timer",
                #"VB.CommandButton",
                "VB.Label",
                "Form_Load",
                "Timer()",
                ".left",
                "3500"
                ],
     "p56.frm":[9,"经过时间",
                "结束时间",
                "开始时间",
                "停止",
                "Timer()",
                "Interval",
                "秒",
                "enable"
                ]
}
xfile_list9={ 
     "p61.frm":[5,"+","print","inputbox","next"],
     "p62.frm":[5,"<",">","InputBox","print","for"],
     "p63.frm":[5,"InputBox","print","Mod","\\"],
     "p64.frm":[5,"10","9999","\\","mod","print"],
     "p65.frm":[5,"rnd","print","for","then"]
}
xfile_list10={ 
     "p91.frm":[6,"Microsoft.Jet.OLEDB"," MSAdodcLib.Adodc","客户地址","客户电话","CompanyName"],
     "p92.frm":[6,"Recordset.Find","MoveFirst","客户地址","客户名称","客户电话"],
     "p93.frm":[6,"客户名称","继续","客户地址","Timer","MoveNext","Recordset.EOF"],
     "p94.frm":[6,"城市",".DataGrid","00A0C91110ED","00AA004BB851","客户地址","客户电话"]
}
xfile_list11={ 
     "p95.frm":[10,"D:\\Nwind.mdb","Fields(\"UnitPrice\")","Fields(\"Quantity\")","1000","500","MoveNext","0.1","0.05","OrderDetails"],
     "p96.frm":[8,"D:\\Nwind.mdb","MoveNext","bof","eof","delete","then","OrderDetails"],
     "p97.frm":[7,"Fields(\"Discount\")","find","print","Fields(\"ProductID\")","MoveNext","OrderDetails"]
}
xfile_list12={ 
     "P73.frm":[8,"Function","1949","2049","Mod","100","400","then","next"],
     "P75.frm":[8,"Function","^","100","999","print","for","if"],
     "P76.frm":[6,"500","if","mod","print","sqr"],
     "P79.frm":[6,"Function","mod","+","then","print"]
}
def homework(file_list,hw_dir,debug): 
   #print(file_list["txtAns"])
   xfile_list=xfile_list12
   
   score,comment=getscore(file_list,xfile_list)
   if score<100 and comment.find("function")!=-1:
     score+=1
   #小于90或为100的人工处理 
   if not debug and score==1100:
    print("score=%s comment=%s" %(score,comment))
    for file in xfile_list:
      print("------------%s----------" %(file))
      content=getcontent(file_list,file)
      if content!="":        
        print(content)
        input("*********continue*******")
    score=input("输入新的分数 【%d】：" %(score))
    #comment=input("输入comment")
   return score,comment