<?php
$MyUsername = "root";  
$MyPassword = ""; 
$MyHostname = "localhost";       
$MyDb = "arduino_alarm_log";

$dbh = mysqli_connect($MyHostname , $MyUsername, $MyPassword, $MyDb) or die("Connection failed");

$selected = mysqli_select_db($dbh, $MyDb);
?>
