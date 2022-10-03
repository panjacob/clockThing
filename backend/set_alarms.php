<?php
require('utilis.php');

$file = 'alarms.txt';
$data = "{$_GET['data']}";
file_put_contents($file, $data, LOCK_EX);
?>