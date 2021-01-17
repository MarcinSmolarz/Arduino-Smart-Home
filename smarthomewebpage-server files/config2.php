<!DOCTYPE html>
<html lang="pl">
<head>
    <meta charset="UTF-8" content='width=device-width, initial-scale=1' name='viewport'/>
    <title>System alarmowy oparty na platformie arduino</title>
	
<!--	<script> 
	/*fetch("report.txt")
	.then(function(response) {
	return response.text();
	}).then(function(text) {
	console.log(text);
	document.querySelector('#report').textContent = text;
	});
	
	fetch("entry.txt")
	.then(function(response) {
	return response.text();
	}).then(function(text) {
	console.log(text);
	document.querySelector('#entryTime').textContent = text;
	});
	
	fetch("exit.txt")
	.then(function(response) {
	return response.text();
	}).then(function(text) {
	console.log(text);
	document.querySelector('#exitTime').textContent = text;	
	}); */
	</script> -->


	
	
	<link href="https://fonts.googleapis.com/css?family=Josefin+Sans&display=swap&subset=latin-ext" rel="stylesheet">
	<style type = "text/css">
	body {
}

div.header {
	font-family: 'Josefin Sans', sans-serif;
	text-align: center;

}



	
nav.nav{
	font-family: 'Josefin Sans', sans-serif;
	text-align: center;
	background: rgba(0,151,156,0.85);	
	margin-top: 2rem;
}
nav ul{
		list-style: none;
}
nav ul li a{
	text-decoration: none;
}

	
nav.nav1{
	font-family: 'Josefin Sans', sans-serif;
	text-align: center;
	background: rgba(255,255,255,0.7);	
	margin-top: 5rem;
}
nav1 ul{
		list-style: none;
}
nav1 ul li a{
	text-decoration: none;
	
}

#button{
	width: 10em;  height: 2em;
}

div.input-group-mb-3{
	text-align: center;
	background: rgba(0,151,240, 0.7);
}
	</style>
	
</head>

<body>
	<div class="header">
		<font color="black"> 
			<figure> <img src="main.jpg" style="width:50%;"> </figure> 
			<h1>Panel konfiguracyjny urządzenia</h1>
			<p> Tutaj skonfigurujesz swoje urządzenie według własnych potrzeb.</p>
			<p> Pamięaj aby trzymać się zakresów podanych w nawiasach. W przeciwnym razie Twoje ustawienia nie zostaną zapisane.</p>
			<p><a href="http://192.168.1.14/select_device.html">Powrót do panelu urządzeń.</a></p>
		</font>
	</div>
	
	<nav class="nav">
		<ul>	
			<form id="ConfigMenu" name="frmText">	
			<br/>
            <li>				
				<label>Czas wejścia (zakres 10-30): <input type="text" name="EntryTimeValue" size="16" maxlength="16" /></label> 
            </li>			
            <li>
				<label>Czas wyjścia (zakres 10-30): <input type="text" name="ExitTimeValue" size="16" maxlength="16" /></label> 
            </li>
			</form>
			<br> <input type="submit" id="button" value="Zatwierdź""  /><br/>
			<br> <input type="submit" id="button" value="Uzbrój" "  />
			 <input type="submit" id="button" value="Rozbrój" "  /> <br/>
				<br/><label>Obecne ustawienia: </label><br/>
				<label> Czas wyjścia: 15</label> <br/> 
				<label> Czas wejcia: 23 </label> <br/>
				<label> Ostatnia aktywność: Uzbrojenie </label> <br/>
		<!--		Czas wejścia: <span id="entryTime"> </span> <br/>
				Czas wyjścia: <span id="exitTime"> </span> <br/> 
				Ostatnia aktywność: <span id="report"> </span><br/>		-->	
		</ul> <br/>
    </nav>
	<br/> <br/> <br/> <br/> <br/> <br/>
	<center> ARDUINO® is trademark of Arduino SA </center>
</body>
</html>