<?php
/*
    方倍工作室 http://www.cnblogs.com/txw1958/
    CopyRight 2013 www.fangbei.org  All Rights Reserved
*/

define("TOKEN", "weixin");                             //我们自己定义的token
$wechatObj = new wechatCallbackapiTest();
if (isset($_GET['echostr'])) {
    $wechatObj->valid();
}else{
    $wechatObj->responseMsg();
}

class wechatCallbackapiTest
{
    public function valid()
    {
        $echoStr = $_GET["echostr"];
        if($this->checkSignature()){
            header('content-type:text');
            echo $echoStr;
            exit;
        }
    }

    private function checkSignature()
    {
        $signature = $_GET["signature"];
        $timestamp = $_GET["timestamp"];
        $nonce = $_GET["nonce"];

        $token = TOKEN;
        $tmpArr = array($token, $timestamp, $nonce);
        sort($tmpArr, SORT_STRING);
        $tmpStr = implode( $tmpArr );
        $tmpStr = sha1( $tmpStr );

        if( $tmpStr == $signature ){
            return true;
        }else{
            return false;
        }
    }

    public function responseMsg()
    {
        $postStr = $GLOBALS["HTTP_RAW_POST_DATA"];
//save postStr
$url="http://turnback.top/wechar/fileio.php?data=MessageReceived".$postStr;  
$html = file_get_contents($url);  

        if (!empty($postStr)){
            $postObj = simplexml_load_string($postStr, 'SimpleXMLElement', LIBXML_NOCDATA);
            $fromUsername = $postObj->FromUserName;
            $toUsername = $postObj->ToUserName;
            $msgType=$postObj->MsgType;
            $keyword = trim($postObj->Content);
            $responseStr="";
            switch ($msgType){
			  case "event":                  //事件处理
                $Event=$postObj->Event;
                switch ($Event){
                    case "subscribe":
					       $addUrl="http://turnback.top/wechar/WecharUserAPI.php?action=addUser&user=".$fromUsername;
						   $html_userId=file_get_contents($addUrl);
                           $responseStr="欢迎订阅!你的ID为：".$html_userId."这个Id输入在python脚本中，以便识别用户。功能：1、执行命令，如cd、shutdown等cmd命令 2、下载文件:如\"下载c:\\1.txt\"，可得到下载地址";
                           break;
                    case "unsubscribe":
					       $deleteUrl="http://turnback.top/wechar/WecharUserAPI.php?action=deleteUser&user=".$fromUsername;
						   $html=file_get_contents($deleteUrl);
                           $responseStr="退订";
                           break; 
				    default: $responseStr="未知事件";
                }
                break;
             case "text":                  //信息处理
              if($keyword == "?" || $keyword == "？")
                {
                $responseStr=date("Y-m-d H:i:s",time()+8*60*60); 
              }else{  
			    $addCMDUrl="http://turnback.top/wechar/WecharUserAPI.php?action=addCMD&user=".$fromUsername."&cmd=".urlencode($keyword);
				$html=file_get_contents($addCMDUrl);
                $responseStr = $html;
                if (strlen($responseStr)<5){
$chatUrl="http://api.douqq.com/?key=R2xoTz1WV2VYZ0JkT3FoUHQ4d1hRVWQ9eEFVQUFBPT0&msg=".urlencode($keyword);
                 $responseStr=file_get_contents($chatUrl);
               }
              }
              break;
             case "voice":
               $recog=$postObj->Recognition;
               $addCMDUrl="http://turnback.top/wechar/WecharUserAPI.php?action=addCMD&user=".$fromUsername."&cmd=".urlencode($recog);
               $responseStr=file_get_contents($addCMDUrl);
               if (strlen($responseStr)<5){
                 $chatUrl="http://api.douqq.com/?key=R2xoTz1WV2VYZ0JkT3FoUHQ4d1hRVWQ9eEFVQUFBPT0&msg=".urlencode($recog);
                 $responseStr=file_get_contents($chatUrl);
               }
               $responseStr="识别:".$recog."~~".$responseStr;
               break;
            }
            $textTpl = "<xml>
                        <ToUserName><![CDATA[%s]]></ToUserName>
                        <FromUserName><![CDATA[%s]]></FromUserName>
                        <CreateTime>%s</CreateTime>
                        <MsgType><![CDATA[%s]]></MsgType>
                        <Content><![CDATA[%s]]></Content>
                        <FuncFlag>0</FuncFlag>
                        </xml>";
            $resultStr = sprintf($textTpl, $fromUsername, $toUsername, time(), "text", $responseStr);
            echo $resultStr; 
        }else{ 
            echo "mark";
            exit;
        }
    }
}
?>