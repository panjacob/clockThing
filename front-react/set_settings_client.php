<?php
require('utilis.php');

$file = 'settings_client.txt';
$data = "{$_GET['power']}, {$_GET['color']}";
file_put_contents($file, $data, LOCK_EX);
?>