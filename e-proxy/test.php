<?
	define("MSG_TYPE", 1);
	define("MSG_STATUS", 2);

	define("MSG_TEMP_CURRENT", 20);
	define("MSG_TEMP_STATUS", 21);
	define("MSG_TEMP_ACTION", 22);

	define("MSG_ERROR_DESCRIPTION", 9999);


	define("MSG_TYPE_INIT", 0);
	define("MSG_TYPE_ONOFF", 1);
	define("MSG_TYPE_TEMP", 2);
	define("MSG_TYPE_ERROR", 999);


	$post = json_encode(
						array(MSG_TYPE => MSG_TYPE_TEMP, MSG_TEMP_ACTION => 0)
						);

	$ch = curl_init("e-proxy.php"); 
	//definieer de link

	curl_setopt($ch, CURLOPT_POST, true);
	//zet het type op post

	curl_setopt($ch, CURLOPT_POSTFIELDS, $post); 
	//stuur de gegevens uit het formulier door naar de link

	curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
			
	curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-type: application/json'));

	$data = curl_exec($ch); 
	//Zet de output op het scherm  

	echo $data;
?>