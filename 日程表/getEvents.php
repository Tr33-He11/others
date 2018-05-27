<?php 
  //interface of Fullcalendar  
  define("jsonFile","events.json");
  define("status_success",'{"status":"success"}');
  define("status_failed",'{"status":"failed"}');
  function save_json($obj){
	$fp=fopen(jsonFile,"w");
	if (fp==NULL) return false;
	$data=json_encode($obj);
	fwrite($fp,$data);  
	fclose($fp);
	return true;
  }
  function add_event($newEvent){
        if (strlen($newEvent["title"])==0)return false;
	if ($newEvent==NULL)return false;  
	$data=file_get_contents(jsonFile); 
	$obj=json_decode($data,true);
	$sz=count($obj);  
	$obj[$sz]=$newEvent;
    return save_json($obj);
	return true;
  }
  function remove_event($title,$start){
	$data=file_get_contents(jsonFile);
	$obj=json_decode($data,true);
	$len=count($obj);
	for ($i=0;$i<$len;$i++){
	  if ($obj[$i]["title"]==$title && $obj[$i]["start"]==$start){
	    unset($obj[$i]);
	    return save_json($obj);
	  }
	}
	return false;
  }
  $action=$_GET["action"]; 
  switch ($action){
    case "getAllEvents":                                     // Get All Events
      $fp=file_get_contents("events.json","r"); 
      echo $fp;
      break;
    case "addEvent":
      if (!isset($_GET["title"])||!isset($_GET["start"]))die(status_failed);
	  $event=null;
	  $event["title"]=$_GET["title"];
	  $event["start"]=$_GET["start"];
      if (add_event($event)) echo status_success;else echo status_failed;
      break;
	case "removeEvent":
      if (!isset($_GET["title"])||!isset($_GET["start"]))die(status_failed);
      if (remove_event($_GET["title"],$_GET["start"]))echo status_success;else echo status_failed;
	  break;
    default:
      echo status_failed;
  }

?>
