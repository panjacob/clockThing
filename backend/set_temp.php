<?php
require('utilis.php');

$file = 'temp.txt';
$dateX = date('d-m-y H:i');
$data = "{$dateX}, {$_GET['temp']}, {$_GET['hum']}\n";
file_put_contents($file, $data, FILE_APPEND | LOCK_EX);
?>