<?  
  $ret_json=array("ok" => "0","data"=> "");
  /***********************卡密生成与解密************************************/
  function secret_encode(){
    $arr_secret=array();
    $cur_time=time();
    for ($type=0;$type<4;$type++){  
      $secret=substr(sprintf("%09d%01d%01d",$cur_time,rand(0,9),$type),1); 
      //校验码
      $check_code=sprintf("%05d",hexdec(substr(md5($secret),0,5))%100000); 
      $secret=$secret.$check_code; 
      //对secret进行混杂 
      $newVal=array();
      for ($part=0;$part<3;$part++){
        $newVal[$part]=sprintf("%04d",(intval(substr($secret,$part*4,4))+intval(substr($secret,12,4)))%10000);
      }
      $secret=$newVal[0].$newVal[1].$newVal[2].substr($secret,12,4);
      //添加"-" 分割
      $arr_secret[$type]="";
      for ($part=0;$part<4;$part++)
        $arr_secret[$type].=substr($secret,$part*4,4).($part<3?'-':""); 
    }
    return $arr_secret;
  }
  function secret_decode($secret){
    if (!is_string($secret))return null;
    trim($secret); 
    //检查输入 
    if (strlen($secret)!=19)return null;
    for ($i=0;$i<19;$i++){
      if ($i%5!=4)
          if ($secret[$i]<'0'||$secret[$i]>'9'){ 
              return null;
          }
    } 
    $plain="";
    //去掉"-"分隔符
    for ($part=0;$part<4;$part++) $plain.=substr($secret,$part*5,4);  
    //去混杂
    $newVal=array();
    for ($part=0;$part<3;$part++){
      $newVal[$part]=sprintf("%04d",(10000+intval(substr($plain,$part*4,4))-intval(substr($plain,12,4)))%10000);
    }
    $plain=$newVal[0].$newVal[1].$newVal[2].substr($plain,12,4);
    
    //比较校验码
    $check_code=hexdec(substr(md5(substr($plain,0,11)),0,5))%100000;
    if ($check_code!=intval(substr($plain,11,5))){
        //echo $check_code."!=".intval(substr($plain,11,5))."\n";
        return null;
    }
    $type=intval(substr($plain,10,1));
    $gen_time=intval("1".substr($plain,0,9));
    
    $ret=array();
    $ret["type"]=strval($type);
    $ret["create_time"]=strval($gen_time);  
    return $ret;
  } 
  function gen_secret(){        //生成4种卡密，返回数组
    $arr=secret_encode();
    return $arr;
  }
  function dec_secret($secret){ //解析卡密，返回数组。失败返回null
    $arr=secret_decode($secret); 
    return $arr;
  } 
    //卡密验证
  function check_secret($secret){   //检验secret，正确返回True，否则返回False。并写入ret_json
    $arr=dec_secret($secret);
    global $ret_json;
    $ret_json["ok"]="0";
    //卡密不正确
    if (empty($arr)){
       $ret_json["data"]="卡密错误!";
       return False;
    }        
    //卡密被禁用
    if (is_secret_inactive($secret)){
       $ret_json["data"]="此卡密已被管理员禁用!";
       return False;
    }
    //卡密超时
    $create_time=intval($arr["create_time"]);
    $type=$arr["type"];
    $end_time;
    switch ($type){
       case 0: $end_time=1;break;
       case 1: $end_time=7;break;
       case 2: $end_time=31;break;
       case 3: $end_time=31*20;break;
    }
    $end_time=$create_time+$end_time*3600*24;
    if ($end_time<time()){ 
       $ret_json["data"]="此卡密已在".date('Y-m-d H:i:s', $end_time)."超时!";
       return False;
    }
    //可以正常使用 
    $ret_json["ok"]="1";
    $ret_json["data"]=array("create_time" => date('Y-m-d H:i:s', $create_time),"end_time" => date('Y-m-d H:i:s', $end_time));
    return True;
  }
  /************************订单API *******************************/ 
  function get_user_order($secret){  //读取某个用户订单
    $all_orders=json_decode(read_file("orders.json"),true); 
    $orders=array();
    for ($i=0;$i<count($all_orders);$i++){ 
      if ($all_orders[$i]["user"] ==$secret)
         $orders[count($orders)]=$all_orders[$i];
    } 
    return $orders;
  }
  function get_all_order(){            //读取所有订单   
    $all_orders=json_decode(read_file("orders.json"),true); 
    return $all_orders;
  }
  function add_order($secret,$phone){  //添加订单
    $all_orders=get_all_order();
    $sz=count($all_orders);
    $mxid=0;
    for ($i=0;$i<$sz;$i++) if ($mxid<intval($all_orders[$i]["id"])) $mxid=intval($all_orders[$i]["id"]);
    $all_orders[$sz]["id"]=strval($mxid+1);
    $all_orders[$sz]["user"]=$secret;
    $all_orders[$sz]["phone"]=$phone;
    $all_orders[$sz]["time"]=strval(time());
    $all_orders[$sz]["status"]="0";  //0订单还未处理，1以处理
    write_file(json_encode($all_orders),"orders.json");
  }  
  function del_order($order_id){
    $all_orders=get_all_order();
    $new_orders=array();
    $sz=count($all_orders);
    for ($i=0;$i<$sz;$i++) 
      if ($order_id!=intval($all_orders[$i]["id"])){
        $new_orders[count($new_orders)]=$all_orders[$i];
      }
    write_file(json_encode($new_orders),"orders.json");
  }
  function finish_order($order_id){  //成功返回True，失败False
    $all_orders=get_all_order(); 
    $sz=count($all_orders);
    for ($i=0;$i<$sz;$i++) 
      if ($order_id==intval($all_orders[$i]["id"])){ 
         $all_orders[$i]["status"]="1";
         write_file(json_encode($all_orders),"orders.json");
         return True;
      }
    return False;
  }
  /********************卡密禁用功能*************************/
  function secret_active($secret){
    $new_arr=array();
    $inactive_arr=json_decode(read_file("inactive.json"));
    for ($i=0;$i<count($inactive_arr);$i++)
    if ($inactive_arr[$i]!=$secret){
      $new_arr[count($new_arr)]=$inactive_arr[$i];
    }
    write_file(json_encode($new_arr),"inactive.json");
  }
  function secret_inactive($secret){
    if (is_secret_inactive($secret)) return True;
    $inactive_arr=json_decode(read_file("inactive.json"));
    $inactive_arr[count($inactive_arr)]=$secret;
    write_file(json_encode($inactive_arr),"inactive.json");
    return True;
  }
  function is_secret_inactive($secret){ 
    $inactive_arr=json_decode(read_file("inactive.json"));
    for ($i=0;$i<count($inactive_arr);$i++)
    if ($inactive_arr[$i]==$secret) 
        return True;
    return False;
  }
  /**************Admin 密码*********************/
  function is_admin_pass_correct($pass){
    $md5=read_file("admin_pass_md5.data");
    if (md5("(1*(2+3))*4/5".$pass)==$md5) 
        return True;
    else
        return False;
  }
  function set_admin_pass($newpass){ 
    $newmd5=md5("(1*(2+3))*4/5".$newpass);
    write_file($newmd5,"admin_pass_md5.data");
    return True;
  }
  /**************文件读写API********************/
  function read_file($filename){
    global $ret_json;
    $data=file_get_contents($filename);
    if( $data== False){
       $ret_json["ok"]="0";
       $ret_json["data"]="服务器繁忙!";
       myexit($ret_json);
    }
    return $data;
  }
  function write_file($data,$filename){ 
     if($fp = fopen($filename, 'w')) {
        $startTime = microtime();
        do {
           $canWrite = flock($fp, LOCK_EX);
           if(!$canWrite) usleep(round(rand(0, 100)*1000));
        } while ((!$canWrite)&& ((microtime()-$startTime) < 4000));
       if ($canWrite) {
         fwrite($fp, $data);
       }else{ 
         $ret_json["ok"]="0";
         $ret_json["data"]="服务器繁忙!";
         myexit($ret_json);
       }           
       fclose($fp);
      }
  }

  function auth_verify(){       //权限验证。返回0表示无权限，1表示普通用户，返回2表示admin  
    global $ret_json;
    if (isset($_GET["admin"])){
      if (is_admin_pass_correct($_GET["admin"]))
          return 2;
    }
    if (isset($_GET["secret"])){
      if (check_secret($_GET["secret"])) 
        return 1;
      else 
        myexit($ret_json);
    } 
  }
  function myexit($arr){
    echo json_encode($arr);
    exit();
  }
  function user_api(){
    global $ret_json;
    if (!isset($_GET["action"])){//提示登录成功
       $ret_json["ok"]="1";
       $ret_json["data"]="login successful!\n";
       myexit($ret_json);
    } else{ 
       $secret=$_GET["secret"];
       $action=$_GET["action"];
       switch ($action){
           case "submitorder":
             if (isset($_GET["phone"])&&is_numeric($_GET["phone"])){  //xss
               add_order($secret,$_GET["phone"]);
               $ret_json["ok"]="1"; 
               $ret_json["data"]="添加成功!";
             }else{
               $ret_json["ok"]="0"; 
               $ret_json["data"]="电话格式错误";
             }
             myexit($ret_json);
             break;
           
       }
    }
    $ret_json["ok"]="0";
    $ret_json["data"]="未知命令";
    myexit($ret_json);
  }
  function admin_api(){
    global $ret_json;
    if (isset($_GET["action"])){
      $action=$_GET["action"]; 
      switch ($action){
        case "get_all_order":
           $ret_json["ok"]="1";
           $ret_json["data"]=get_all_order();
           myexit($ret_json);
           break;
        case "secret_inactive":
           if (isset($_GET["secret"]))
             secret_inactive($_GET["secret"]);
           $ret_json["ok"]=1;
           myexit($ret_json);           
           break;
        case "secret_active":
           if (isset($_GET["secret"]))
             secret_active($_GET["secret"]);
           $ret_json["ok"]=1;
           myexit($ret_json);
           break;
        case "is_secret_inactive":
           $rst="no secret"; 
           if (isset($_GET["secret"]))
             $rst=sprintf("%d",is_secret_inactive($_GET["secret"])); 
           $ret_json["ok"]=1;
           $ret_json["data"]=$rst;
           myexit($ret_json);
           break;
        case "gen_secret":
           $ret_json["ok"]=1;
           $ret_json["data"]=gen_secret();
           myexit($ret_json);
           bretk;
        case "dec_secret": 
           check_secret($_GET["secret"]);
           myexit($ret_json);//在函数check_secret中已有赋值
       
           break;
      }
      $ret_json["ok"]=0;
      $ret_json["data"]="未知命令";
      myexit($ret_json);
    }
  } 
  /***********************Main函数********************************************************/ 
  $auth=auth_verify();   
  if ($auth==1) user_api();
  if ($auth==2) admin_api();
?>
