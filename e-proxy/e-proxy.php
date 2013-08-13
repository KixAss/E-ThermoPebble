<?
	define("MSG_TYPE", 1);
	define("MSG_STATUS", 2);

	define("MSG_TEMP_CURRENT", 20);
	define("MSG_TEMP_STATUS", 21);
	define("MSG_TEMP_ACTION", 22);

	define("MSG_ERROR_DESCRIPTION", 9999);

	define("MSG_TYPE_INIT", 0);
	define("MSG_TYPE_TEMP", 2);
	define("MSG_TYPE_ERROR", 999);

	header("Content-type: application/jscon");

	$post = file_get_contents('php://input');

	$payload = json_decode($post, true);

	function dieError($error)
	{
		exit(json_encode(array(MSG_TYPE => MSG_TYPE_ERROR, MSG_ERROR_DESCRIPTION => $error)));
	}

	if ($_SERVER["X-Pebble-ID"] == "36K012")
	{
		$post = "username=&password=&remember=1";
	}
	else
	{
		dieError("Unknown pebble ID");
	}



	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	$ch = curl_init("https://portal.icy.nl/login"); //definieer de link

	curl_setopt($ch, CURLOPT_POST, true);	//zet het type op post
	curl_setopt($ch, CURLOPT_POSTFIELDS, $post); 	//stuur de gegevens uit het formulier door naar de link

	curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
	curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-type: application/x-www-form-urlencoded; charset=UTF-8'));

	$data = json_decode(curl_exec($ch));

	$token = $data->token;

	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	$ch = curl_init("https://portal.icy.nl/data"); 	//definieer de link
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);		
	curl_setopt($ch, CURLOPT_HTTPHEADER, array('Session-Token: ' . $token));
	$data = json_decode(curl_exec($ch)); 


	$control = $data->configuration[0];
	$config = ($control >> 5 & 7);


	$temp = $data->temperature1;
	$uid = $data->uid;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if ($payload[MSG_TEMP_ACTION] > 0)
	{
		if ($payload[MSG_TEMP_ACTION] < 3) // && $config == 1)
		{
			if ($payload[MSG_TEMP_ACTION] == 1)
			{
				$temp -= 0.5; 
			}
			else
			{
				$temp += 0.5; 
			}

			$ch = curl_init("https://portal.icy.nl/data");			//definieer de link
			curl_setopt($ch, CURLOPT_POST, true);			//zet het type op post
			curl_setopt($ch, CURLOPT_POSTFIELDS, "uid=".$uid."&temperature1=" . $temp); //stuur de gegevens uit het formulier door naar de link
			curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
			curl_setopt($ch, CURLOPT_HTTPHEADER, array('Session-Token: ' . $token, 'Content-type: application/x-www-form-urlencoded; charset=UTF-8'));
			curl_exec($ch);

			$data->temperature1 = $temp;
		}
		else
		{

			//echo "oud: " . $config;

			if ($payload[MSG_TEMP_ACTION] != 3)
			{
				$new_config = 1;
			}
			elseif ($payload[MSG_TEMP_ACTION] == 4)
			{
				$new_config = 5;
			}
			else
			{
				$new_config = $config + 1;
				if ($new_config > 2) $new_config = 0;
			}

			//exit("nieuw: " . $new_config);
			$post = "";
			$post .= "uid=" . $uid . "&";
			$post .= "temperature1=" . $temp;
			$post .= "&configuration%5B%5D=" . (($control & 31) + ($new_config << 5 & 224));
			$post .= "&configuration%5B%5D=" . $data->configuration[1];
			$post .= "&configuration%5B%5D=" . $data->configuration[2];
			$post .= "&configuration%5B%5D=" . $data->configuration[3];
			$post .= "&configuration%5B%5D=" . $data->configuration[4];
			$post .= "&configuration%5B%5D=" . $data->configuration[5];
			$post .= "&configuration%5B%5D=" . $data->configuration[6];
			$post .= "&configuration%5B%5D=" . $data->configuration[7];
			$post .= "&configuration%5B%5D=" . $data->configuration[8];
			$post .= "&configuration%5B%5D=" . $data->configuration[9];
			$post .= "&configuration%5B%5D=" . $data->configuration[10];
			$post .= "&configuration%5B%5D=" . $data->configuration[11];


			$ch = curl_init("https://portal.icy.nl/data");			//definieer de link
			curl_setopt($ch, CURLOPT_POST, true);			//zet het type op post
			curl_setopt($ch, CURLOPT_POSTFIELDS, $post); //stuur de gegevens uit het formulier door naar de link
			curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
			curl_setopt($ch, CURLOPT_HTTPHEADER, array('Session-Token: ' . $token, 'Content-type: application/x-www-form-urlencoded; charset=UTF-8'));
			curl_exec($ch);

			$config = $new_config;

		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	$output[MSG_TEMP_CURRENT] = $data->temperature2 . "* / " . $data->temperature1 . "*";
	$output[MSG_TEMP_STATUS] = $config;
?>