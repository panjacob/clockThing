<!-- https://clock.panjacob.online/write.php?datetime=01/01/2022&temp=22&hum=76 -->
<?php
$file = 'temp.txt';
$data = "{$_GET['datetime']}, {$_GET['temp']}, {$_GET['hum']}\n";
file_put_contents($file, $data, FILE_APPEND | LOCK_EX);
?>