<?php
require('utilis.php');

$data = file_get_contents("https://api.openweathermap.org/data/2.5/onecall?lat=54.3520&lon=18.6466&appid=81907e8ccece2ef5f71b1778b4007353&lang=pl");
$json = json_decode($data);
$json_current = $json->{'current'};
$descriptionDirty = $json_current->{"weather"}[0]->{"description"};
$descriptionClean =  str_replace(array('ą', 'ć', 'ę', 'ł', 'ń', 'ó', 'ś', 'ź', 'ż'), array('a', 'c', 'e', 'l', 'n', 'o', 's', 'z', 'z'), $descriptionDirty);
$descriptionBig = ucfirst($descriptionClean);
$descriptionArr = explode(" ", $descriptionBig, 2);
$desc1 = $descriptionArr[0];
$desc2 = count($descriptionArr) == 1 ? "" : $descriptionArr[1];

$json_result = [
    "temp" => number_format(round($json_current->{"temp"} - 273.15, 1), 1),
    "feels_like" => number_format(round($json_current->{"feels_like"} - 273.15, 1), 1),
    "pressure" => $json_current->{"pressure"},
    "humidity" => $json_current->{"humidity"},
    "uvi" => $json_current->{"uvi"},
    "wind" => number_format($json_current->{"wind_speed"}, 1),
    "sunrise" => epochToTime($json_current->{"sunrise"}),
    "sunset" => epochToTime($json_current->{"sunset"}),
    "clouds" => $json_current->{"clouds"},
    "desc1" => $desc1,
    "desc2" => $desc2,
];
$text = json_encode($json_result);
echo $text;
