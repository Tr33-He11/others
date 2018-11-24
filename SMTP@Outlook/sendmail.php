<?php 
  $rst=array("error_code"=>"-1","data"=>"参数有问题,to,title,content");   
  if (isset($_GET["to"]) && isset($_GET["title"]) && isset($_GET["content"])){ 
      $to = base64_encode($_GET["to"]);
      $title = base64_encode($_GET["title"]);
      $content = base64_encode($_GET["content"]);
      $cmd="python sendmail.py $to $title $content";
      $ret=exec($cmd); 
      // echo $cmd;
      if ($ret=="cjf@xmu"){
         $rst["error_code"]=0; 
         $rst["data"]="邮件发送成功";
      }
      else{
         $rst["error_code"]=-2; 
         $rst["data"]="发送失败:$ret";
      }
  }
  die(json_encode($rst));
?>