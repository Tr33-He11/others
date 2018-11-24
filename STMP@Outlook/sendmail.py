from email.mime.text import MIMEText
import smtplib
import json
import base64
class OutlookSTMP:
    server = "smtp-mail.outlook.com"
    port = 587

    def __init__(self,user,pwd):
        self.smtp = smtplib.SMTP(self.server, self.port)
        self.user = user
        self.pwd = pwd
        # self.smtp.set_debuglevel(1)
        self.login()
        return

    def login(self):
        self.smtp.ehlo()
        self.smtp.starttls()
        self.smtp.ehlo()
        self.smtp.login(self.user,self.pwd)
        return

    def send(self,to,title,content):
        # 发送邮件; 支持html
        print("%s\n%s\n%s" % (to, title, content))
        msg = MIMEText(content, "html", "utf-8")
        msg["Subject"] = title
        self.smtp.sendmail(self.user, [to], msg.as_string())

import sys
if __name__ == "__main__":
    # 命令行调用方法(base64加密，防止参数中的空格等影响)
    if len(sys.argv) == 4:
        outlook = OutlookSTMP("vcvycy@outlook.com", "xxx")
        to = str(base64.b64decode(sys.argv[1]), encoding="utf-8")
        title = str(base64.b64decode(sys.argv[2]), encoding="utf-8")
        content = str(base64.b64decode(sys.argv[3]), encoding="utf-8")
        outlook.send(to, title, content)
        print("cjf@xmu")
    else:
        print("无参数")
    """
    # 普通调用方法
    if outlook.send("352871242@qq.com","zzzzzzzzzzzzz","<i>这是内容</i>"):
       print("yes")
    else:
       print("no")
    """