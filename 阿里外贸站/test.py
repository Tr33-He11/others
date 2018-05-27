import requests
import time
s=requests.Session()
cook={"cjf":"name","age":"30"}
url="http://turnback.top/test.php"
requests.utils.add_dict_to_cookiejar(s.cookies,cook) 
while True:
  content=s.post(url)
  print("yes");
  time.sleep(1);