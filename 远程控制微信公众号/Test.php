<?php  
$host = "120.36.250.190";  
$port = 8888;  
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)or die("Could not create  socket\n"); // ����Socket  
$connection = socket_connect($socket, $host, $port) or die("Could not connet server\n");    //  ����  
socket_write($socket, "hello socket") or die("Write failed\n"); // ���ݴ��� �������������Ϣ  
socket_write($socket,"chenjianfeng",12);
while ($buff = socket_read($socket, 1024, PHP_NORMAL_READ)) {  
    echo("Response was:" . $buff . "\n");  
}  
socket_close($socket);  
?>