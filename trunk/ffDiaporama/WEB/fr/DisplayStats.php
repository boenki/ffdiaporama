<?php
	$user= "ffdiaporama_stats";		//l'utilisateur de la base de données MySQL
	$pwd = "ffdiaporama";			//le mot de passe de la base de données MySQL
	$db  = "ffdiaporama_stats";		//le nom de la base de données MySQL
	$host= "sql";					//l'adresse de la base de données MySQL
	$port= "3306";					//Port du serveur de bd

	echo "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n";
	echo "<html lang=\"fr\">\n";
	echo "<head>\n";
	echo "  <title>Statistiques de telechargement</title>\n";
	echo "  <meta content=\"text/html; charset=UTF-8\" http-equiv=\"Content-Type\">\n";
	echo "  <link href=\"../ffDiaporama_style.css\" rel=\"stylesheet\" type=\"text/css\">\n";
	echo "</head>\n";
	echo "<body>\n";

	include "../ffDiaporama_menu.php";

	echo "  <div id=\"banner\">\n";
	echo "    <img checked=\"true\" style=\"width: 48px; height: 48px;\" src=\"../img/icon_download.png\" align=\"MIDDLE\">\n";
	echo "    Statistiques de t&#233;l&#233;chargement";
	echo "  </div>\n";
	echo "  <div id=\"content\">\n";
	echo "    <br>\n";
	echo "    <table border=\"1\" cellpadding=\"2\" cellspacing=\"0\"><tbody>\n";
	echo "      <tr>\n";
	echo "        <td style=\"background-color: rgb(0, 0, 102); color: rgb(255, 255, 255);\"><big>Date</td>\n";
	echo "        <td style=\"background-color: rgb(0, 0, 102); color: rgb(255, 255, 255);\"><big>Version</td>\n";
	echo "        <td style=\"background-color: rgb(0, 0, 102); color: rgb(255, 255, 255);\"><big>Windows</td>\n";
	echo "        <td style=\"background-color: rgb(0, 0, 102); color: rgb(255, 255, 255);\"><big>Ubuntu32</td>\n";
	echo "        <td style=\"background-color: rgb(0, 0, 102); color: rgb(255, 255, 255);\"><big>Ubuntu64</td>\n";
	echo "        <td style=\"background-color: rgb(0, 0, 102); color: rgb(255, 255, 255);\"><big>Fed14-32</td>\n";
	echo "        <td style=\"background-color: rgb(0, 0, 102); color: rgb(255, 255, 255);\"><big>Fed14-64</td>\n";
	echo "        <td style=\"background-color: rgb(0, 0, 102); color: rgb(255, 255, 255);\"><big>Fed15-32</td>\n";
	echo "        <td style=\"background-color: rgb(0, 0, 102); color: rgb(255, 255, 255);\"><big>Fed15-64</td>\n";
	echo "        <td style=\"background-color: rgb(0, 0, 102); color: rgb(255, 255, 255);\"><big>Total</td>\n";
	echo "      </tr>\n";

	// Connexion MySQL
	$conn = mysql_connect($host,$user,$pwd)	or die(mysql_error());
	mysql_select_db($db,$conn) 				or die(mysql_error());

	$curdate  = "";
	$version  = "";
	$windows  = 0;
	$ubuntu32 = 0;
	$ubuntu64 = 0;
	$fc1432   = 0;
	$fc1464   = 0;
	$fc1532   = 0;
	$fc1564   = 0;
	$total    = 0;

	$result  = mysql_query("SELECT * FROM DownloadsCompter ORDER BY FileName");
	while($row = mysql_fetch_array($result)) {
		$newdate = $row['FileName'];
		if (strpos($newdate,"/") !== false) $newdate=substr($newdate,0,strpos($newdate,"/"));
		if ($newdate !== $curdate) {
			if ($curdate !== "") {
				echo "      <tr>\n";
				echo "        <td>".$curdate."</td>\n";
				echo "        <td style=\"text-align: right;\">".$version."</td>\n";
				echo "        <td style=\"text-align: right;\">".$windows."</td>\n";
				echo "        <td style=\"text-align: right;\">".$ubuntu32."</td>\n";
				echo "        <td style=\"text-align: right;\">".$ubuntu64."</td>\n";
				echo "        <td style=\"text-align: right;\">".$fc1432."</td>\n";
				echo "        <td style=\"text-align: right;\">".$fc1464."</td>\n";
				echo "        <td style=\"text-align: right;\">".$fc1532."</td>\n";
				echo "        <td style=\"text-align: right;\">".$fc1564."</td>\n";
				echo "        <td style=\"text-align: right;\">".$total."</td>\n";
				echo "      </tr>\n";
			}
			$curdate  = $newdate;
			$windows  = 0;
			$ubuntu32 = 0;
			$ubuntu64 = 0;
			$fc1432   = 0;
			$fc1464   = 0;
			$fc1532   = 0;
			$fc1564   = 0;
			$total    = 0;
		}
		if (strpos($row['FileName'],"_setup.exe") !== false) {
			$windows =$windows+$row['Downloaded'];
			$version =substr(strrchr($row['FileName'],"/"),1);
			$version =substr(strchr($row['FileName'],"."),1);
			$version =substr($version,0,strpos($version,"_setup.exe"));
		}
		if (strpos($row['FileName'],"_setup_NoA.exe") !== false) {
			$windows =$windows+$row['Downloaded'];
			$version =substr(strrchr($row['FileName'],"/"),1);
			$version =substr(strchr($row['FileName'],"."),1);
			$version =substr($version,0,strpos($version,"_setup_NoA.exe"));
		}
		if (strpos($row['FileName'],"_i386.deb") !== false)			$ubuntu32=$row['Downloaded'];
		if (strpos($row['FileName'],"_amd64.deb") !== false)		$ubuntu64=$row['Downloaded'];
		if (strpos($row['FileName'],".fc14.i686.rpm") !== false)	$fc1432  =$row['Downloaded'];
		if (strpos($row['FileName'],".fc14.x86_64.rpm") !== false)	$fc1464  =$row['Downloaded'];
		if (strpos($row['FileName'],".fc15.i686.rpm") !== false)	$fc1532  =$row['Downloaded'];
		if (strpos($row['FileName'],".fc15.x86_64.rpm") !== false)	$fc1564  =$row['Downloaded'];
  		$total=$total+$row['Downloaded'];
	}
	if ($curdate !== "") {
		echo "      <tr>\n";
		echo "        <td>".$newdate."</td>\n";
		echo "        <td style=\"text-align: right;\">".$version."</td>\n";
		echo "        <td style=\"text-align: right;\">".$windows."</td>\n";
		echo "        <td style=\"text-align: right;\">".$ubuntu32."</td>\n";
		echo "        <td style=\"text-align: right;\">".$ubuntu64."</td>\n";
		echo "        <td style=\"text-align: right;\">".$fc1432."</td>\n";
		echo "        <td style=\"text-align: right;\">".$fc1464."</td>\n";
		echo "        <td style=\"text-align: right;\">".$fc1532."</td>\n";
		echo "        <td style=\"text-align: right;\">".$fc1564."</td>\n";
		echo "        <td style=\"text-align: right;\">".$total."</td>\n";
		echo "      </tr>\n";
	}

	mysql_close($conn);

	echo "    </tbody></table>\n";
    echo "    <br><hr style=\"width: 100%; height: 2px;\">&#169; The ffDiaporama Project\n";
	echo "  </div>\n";
	echo "</body>\n";
	echo "</html>\n";
	exit();
?>

