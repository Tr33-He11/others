import win32com.client
import sys

def write_to_access(final):
    #数据库名和表名
    db_name="./第1套-庄老师.mdb"
    tb_name="第1套-庄"
    #连接数据库
    conn = win32com.client.gencache.EnsureDispatch('ADODB.Connection') 
    DSN = 'PROVIDER = Microsoft.ACE.OLEDB.12.0;DATA SOURCE =%s;' %(db_name)
    conn.Open(DSN)
    #数据库表访问
    rs = win32com.client.Dispatch(r'ADODB.Recordset') 
    rs.Open('[' + tb_name + ']', conn, 1, 3)  # 不允许更新，用于查询
     

    #查询与更新  
    rs.MoveFirst()
    idx=0
    while True:
      if rs.EOF:
        break
      else:
        idx+=1
        sid=rs.Fields.Item(0).Value
        if sid in final:
            rs.Fields.Item(7).Value=final[sid][2]
            rs.Fields.Item(8).Value=final[sid][3]
            rs.Fields.Item(9).Value=final[sid][0]
            rs.Fields.Item(10).Value=final[sid][1]
            rs.Update()
            rs.MoveNext()
        else:
            print("erro!\n")
            sys.exit(0)     
        
def getDict():
  rst={}
  f=open("log.txt","r")
  for line in f:
    x=line.split(" ")
    sid  = x[0]
    prob = x[1]
    score= int(float(x[2]))
    if sid not in rst:
       rst[sid]=[]
    rst[sid].append(score) 
  f.close()
  return rst
  
final=getDict()
write_to_access(final) 