from selenium import webdriver
from selenium.webdriver.common.keys import Keys
import time
import json
import urllib
import sys
import requests
import os
import warnings

class P4P:
  #attribute
  __session          =None
  __param_csrf_token =""
  __param_p4p_id     =""
  main_page          ="http://www2.alibaba.com/manage_ad_keyword.htm"
  #method
  def __private_login(self,usr,pwd):
      driver = webdriver.Chrome() 
      print("[*]正在登录(可能需要手动输入验证码)...");
      login_url="https://login.alibaba.com/?flag=1&return_url=http%3A%2F%2Fwww2.alibaba.com%2Fmanage_ad_keyword.htm"  #url返回到main_page
      login_iframe_id="alibaba-login-box"
      login_user_id="fm-login-id"
      login_pass_id="fm-login-password"
      #
      driver.get(login_url)
      driver.switch_to_default_content();
      driver.switch_to_frame(login_iframe_id)         #转到iframe中 
      driver.execute_script("document.getElementById('%s').value='%s';" %(login_user_id,usr));
      driver.execute_script("document.getElementById('%s').value='%s';" %(login_pass_id,pwd));
  def __init__(self,usr,pwd): 
      #warnings.filterwarnings("ignore")
      self.__private_login(usr,pwd)

x=P4P("cn200021818","pass")