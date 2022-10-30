<?php
require('utilis.php');
if (!isset($_GET["from"])) echo file_get_contents("http://www.clock.panjacob.online/temp.txt");


$from = $_GET["from"];
$to = $_GET["to"];

$from_time = strtotime("24-09-2022 16:40");
$to_time = strtotime("24-09-2022 23:16");

if ($from_time > $to_time) {
    $temp = $to_time;
    $to_time = $from_time;
    $from_time = $temp;
}

function dateIntToString($dateInt)
{
    $current_date = getDate($dateInt);

    $day = str_pad("{$current_date["mday"]}", 2, "0", STR_PAD_LEFT);
    $month = str_pad("{$current_date["mon"]}", 2, "0", STR_PAD_LEFT);
    $year_int = $current_date["year"] - 2000;
    $year = str_pad("{$year_int}", 2, "0", STR_PAD_LEFT);
    $hour = str_pad("{$current_date["hours"]}", 2, "0", STR_PAD_LEFT);
    $minute = str_pad("{$current_date["minutes"]}", 2, "0", STR_PAD_LEFT);
    $date_string = "$day-$month-$year $hour-$minute";

    return $date_string;
}

$myfile = fopen("temp.txt", "r") or die("Unable to open file!");
$data = fread($myfile, filesize("temp.txt"));
fclose($myfile);

$data_arr = explode("\n", $data);
$result_arr = [];
foreach ($data_arr as $index => &$value) {
    $value_arr = explode(',', $value);
    $value_date = $value_arr[0];
    $dateExtended = substr($value_date, 0, 6) . "20" . substr($value_date, 6);
    $value_time = strtotime($dateExtended);
    if ($value_time >= $from_time && $value_time <= $to_time) {
        array_push($result_arr, $value_arr);
    }    
}
$text = json_encode($result_arr);
echo $text;
