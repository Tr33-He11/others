<?php
class WECHARDB{
    public function conn(){      
      $con = mysql_connect("localhost","vcvycy","yourpass");
      if (!$con)return false; 
      mysql_select_db("vcvycy", $con);
      return true;
    }
    public function addUser($OpenId){
      if (!$this->conn())return false;
      $sql="INSERT INTO `wechar`(`OpenId`, `Cmd`, `Response`) VALUES ('".$OpenId."','','')";
      if (!mysql_query($sql))return false;
      return true;
    }
   public function deleteUser($OpenId){
      if (!$this->conn())return false;
      $sql="DELETE FROM `wechar` WHERE OpenId='".$OpenId."'";
      if (!mysql_query($sql))return false;
      return true;
   }
   public function setCmd($OpenId,$Cmd){
      $Cmd=str_replace("\\","\\\\",$Cmd);
      if (!$this->conn())return false;
      $sql="UPDATE `wechar` SET `Cmd`='".$Cmd."' WHERE OpenId='".$OpenId."'";  
      if (!mysql_query($sql))return false;
      return true;
   }
   public function getCmd($OpenId){ 
      if (!$this->conn())return false;
      $sql="SELECT Cmd FROM  `wechar`  WHERE OpenId ='".$OpenId."'"; 
      $result=mysql_query($sql); 
      while ($row=mysql_fetch_array($result)){
        return $row["Cmd"];
      } 
   }
   public function setResponse($OpenId,$Response){
       
      $Response=str_replace("\\","\\\\",$Response);
      if (!$this->conn())return false;
      $sql="UPDATE `wechar` SET `Response`='".$Response."' WHERE OpenId='".$OpenId."'"; 
      if (!mysql_query($sql))return false;
      return true;
   }
   public function getResponse($OpenId){
      if (!$this->conn())return false;
      $sql="SELECT Response FROM  `wechar`  WHERE OpenId ='".$OpenId."'";
      $result=mysql_query($sql);  
      while ($row=mysql_fetch_array($result)){
        return $row["Response"];
      } 
   }  
   public function clearCmdAndResponse($OpenId){ 
      if (!$this->conn())return false;
      $sql="UPDATE `wechar` SET `Cmd`='',`Response`='' WHERE OpenId='".$OpenId."'"; 
      mysql_query($sql); 
   } 
   public function UserId2OpenId($UserId){
      if (!$this->conn())return false;
      $sql="SELECT OpenId FROM  `wechar`  WHERE UserId =".$UserId;
      $result=mysql_query($sql);  
      while ($row=mysql_fetch_array($result)){
        return $row["OpenId"];
      } 
   }   
   public function OpenId2UserId($OpenId){
      if (!$this->conn())return false;
      $sql="SELECT UserId FROM  `wechar`  WHERE OpenId ='".$OpenId."'"; 
      $result=mysql_query($sql);  
      while ($row=mysql_fetch_array($result))
       return $row["UserId"];
   }
} 
?>