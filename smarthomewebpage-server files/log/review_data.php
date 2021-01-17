<?php 
    include('dbconnect.php'); 
?>
<html>
<head>
    <title>Raporty systemu alarmowego Arduino</title>
</head>
<body>
<h1>Raporty systemu alarmowego Arduino</h1>

<table border="5">
<tr>
<td class="table_titles"> <center> <b> Nr </b> </center>  </td>
 <td class="table_titles"> <center> <b> Typ alarmu </b> </center>  </td> 
 <td class="table_titles"> <center> <b> Data </b> </center > </td>	
</tr>


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
