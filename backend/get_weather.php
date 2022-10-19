<?php
require('utilis.php');

$data = file_get_contents("https://api.openweathermap.org/data/2.5/onecall?lat=54.3520&lon=18.6466&appid=81907e8ccece2ef5f71b1778b4007353");
$json = json_decode($data);
$json_current = $json->{'current'};


$json_result = [
"temp" => round($json_current->{"temp"} - 273.15, 1),
"feels_like" => round($json_current->{"feels_like"} - 273.15, 1),
"pressure" => $json_current->{"pressure"},
"humidity" => $json_current->{"humidity"},
"uvi" => $json_current->{"uvi"},
"wind" => $json_current->{"wind_speed"},
"sunrise" => epochToTime($json_current->{"sunrise"}),
"sunset" => epochToTime($json_current->{"sunset"}),
];
$text = json_encode($json_result);
echo $text;
?>