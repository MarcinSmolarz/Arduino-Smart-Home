<?php
    include("dbconnect.php");
    $conn = new mysqli("localhost", "root", "");	
	
	$sql= "INSERT INTO arduino_alarm_log.reports (Typ) VALUES ('".$_GET['alert']."') ";
	
	$conn->query($sql);
?>
