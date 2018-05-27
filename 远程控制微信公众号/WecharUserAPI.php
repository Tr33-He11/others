<?php  
/*
  get参数，action，user，cmd(只用于3),response(用于5)
         1、addUser:    添加用户,user为openId,返回userId
         2、deleteUser: 删除用户,user为openId。
	     3、addCMD：   添加命令,user为openId,另有cmd参数。
         4、polling     :  轮询，获取命令，删除命令,user为userId,返回cmd数组。
         5、response  ：电脑端执行完命令后的回应,
*/
  include "WecharDB.php";
  $action=$_GET["action"];
  $user=$_GET["user"]; 
  $dao = new WECHARDB();
  switch ($action){
    case "addUser": 
      $OpenId=$user;
      $dao->addUser($OpenId);
      echo $dao->OpenId2UserId($OpenId);
	  break;
	case "deleteUser":  
      $OpenId=$user;
      $dao->deleteUser($OpenId);
	  break;
	case "addCMD":
      $OpenId=$user; 
        $dao->setCmd($OpenId,$_GET["cmd"]);
   	    sleep(3); //等待电脑执行	 	
        $res=$dao->getResponse($OpenId);
        if (strlen($res)>0)
        echo $res;
        else 
        echo "请确认PC脚本以运行并且网络状况良好";
        $dao->clearCmdAndResponse($OpenId); 
	  break;
	case "polling":  
       $OpenId=$dao->UserId2OpenId($user); 
       if (strlen($dao->getResponse($OpenId))==0) 
       echo trim($dao->getCmd($OpenId));
	  break; 
    case "response":
       $OpenId=$dao->UserId2OpenId($user); 
       $resp=$_GET["response"];
       if($resp==""){
          $resp="unknown error";
       }
       $dao->setResponse($OpenId,gbk2utf8($resp)); 
	  break;
    default:
	  exit();
	 
  }  
function gbk2utf8($data)
{
  if(is_array($data))
    {
        return array_map('gbk2utf8', $data);
    }
  return iconv('gbk','utf-8',$data);
}
?>  