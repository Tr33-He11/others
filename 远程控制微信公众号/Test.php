<?php  
$host = "120.36.250.190";  
$port = 8888;  
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)or die("Could not create  socket\n"); // 创建Socket  
$connection = socket_connect($socket, $host, $port) or die("Could not connet server\n");    //  连接  
socket_write($socket, "hello socket") or die("Write failed\n"); // 数据传送 向服务器发送消息  
socket_write($socket,"chenjianfeng",12);
while ($buff = socket_read($socket, 1024, PHP_NORMAL_READ)) {  
    echo("Response was:" . $buff . "\n");  
}  
socket_close($socket);  
?>