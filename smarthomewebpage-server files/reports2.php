<!DOCTYPE html>
<?php 
    include('log/dbconnect.php'); 
?>
<html lang="pl">
<head>
    <meta charset="UTF-8" content='width=device-width, initial-scale=1' name='viewport'/>
    <title>System alarmowy oparty na platformie arduino</title>
	<link href="https://fonts.googleapis.com/css?family=Josefin+Sans&display=swap&subset=latin-ext" rel="stylesheet">
	<link rel="stylesheet" href="style.css">
</head>

<body>
	<div class="header">
		<font color="black"> 
			<figure> <img src="images/main.jpg" style="width:50%;"> </figure>
			<h1>Spis raportów aktywności</h1>
		</font>
	</div>
	
	<nav class="nav">
		<ul>			
            <li>
				<br> Aby wrócić na stronę główną kliknij  
                <a href="home.html">tutaj.</a>
            </li>			
            <li>
				<br>Aby przejść do panelu zarządzania urządzeniami kliknij 
                <a href="select_device.html">tutaj.</a>
            </li> <br>
		</ul>
    </nav>
	
	<div class="header">
		<font color="black">
			<p> Spis raportów z ostatniej aktywności urządzeń:</p>
		</font>
	</div>
	<center>
	<table>
<tr>
<td class="table_titles"> <center> <b> Nr </b> </center>  </td>
 <td class="table_titles"> <center> <b> Typ alarmu </b> </center>  </td> 
 <td class="table_titles"> <center> <b> Data </b> </center > </td>	
</tr>
	</center>


<?php 
    $result = mysqli_query($dbh, "SELECT * FROM reports #ORDER BY ID ASC" );   
    while( $row = mysqli_fetch_array($result))
    {
	echo '<tr>';
        echo '   <td>'.$row["ID"].'</td>';  
        echo '   <td>'.$row["Typ"].'</td>';  
        echo '   <td>'.$row["data_zgloszenia"].'</td>';  
        echo '</tr>';
	} 
?>

</table>
	
</body>
</html>