<!-- Menu definition begin -->
<div style="text-align: left;" id="language_banner">
	<?php
		$locale=$_SERVER['PHP_SELF'];
		if (strpos($locale,"/en/") === false) {
		  $newadress="../en/".substr(strrchr($locale,"/"),1);
		  echo "<a href='$newadress'><img style='border: 0px solid ; width: 24px; height: 24px;' alt='EN' title='EN' src='../img/Flag-en.png'></a>";
		}
		if (strpos($locale,"/fr/") === false) {
		  $newadress="../fr/".substr(strrchr($locale,"/"),1);
		  echo "<a href='$newadress'><img style='border: 0px solid ; width: 24px; height: 24px;' alt='FR' title='FR' src='../img/Flag-fr.png'></a>";
		}
		if (strpos($locale,"/es/") === false) {
		  $newadress="../es/".substr(strrchr($locale,"/"),1);
		  echo "<a href='$newadress'><img style='border: 0px solid ; width: 24px; height: 24px;' alt='ES' title='ES'  src='../img/Flag-es.png'></a>";
		}
		if (strpos($locale,"/it/") === false) {
		  $newadress="../it/".substr(strrchr($locale,"/"),1);
		  echo "<a href='$newadress'><img style='border: 0px solid ; width: 24px; height: 24px;' alt='IT' title='IT'  src='../img/Flag-it.png'></a>";
		}
	?>
	&nbsp;&nbsp;
    <a href="index.php"> <img style="border: 0px solid ; width: 24px; height: 24px;" alt="" src="../img/logo.png">
		<?php
		  $locale=$_SERVER['PHP_SELF'];
		  if (strpos($locale,"/fr/")     !== false) echo "Home";
		  elseif (strpos($locale,"/es/") !== false) echo "Home";
		  elseif (strpos($locale,"/it/") !== false) echo "Home";
		  else echo "Home";
		?></a>
	&nbsp;&nbsp;
    <a href="News.php"> <img style="border: 0px solid ; width: 24px; height: 24px;" alt="" src="../img/bt_news.png">
		<?php
		  $locale=$_SERVER['PHP_SELF'];
		  if (strpos($locale,"/fr/")     !== false) echo "Nouvelles";
		  elseif (strpos($locale,"/es/") !== false) echo "Noticias";
		  elseif (strpos($locale,"/it/") !== false) echo "Notizie";
		  else echo "News";
		?></a>
	&nbsp;&nbsp;
	<a href="ScreenShots.php"> <img style="border: 0px solid ; width: 24px; height: 24px;" alt="" src="../img/bt_krdc.png">
		<?php
		  $locale=$_SERVER['PHP_SELF'];
		  if (strpos($locale,"/fr/")     !== false) echo "Photos d'&#233;cran";
		  elseif (strpos($locale,"/es/") !== false) echo "Capturas";
		  elseif (strpos($locale,"/it/") !== false) echo "Immagini";
		  else echo "Screenshots";
		?></a>
	&nbsp;&nbsp;
	<a href="Download.php"> <img style="border: 0px solid ; width: 24px; height: 24px;" alt="" src="../img/bt_download.png">
		<?php
		  $locale=$_SERVER['PHP_SELF'];
		  if (strpos($locale,"/fr/")     !== false) echo "T&#233;l&#233;chargement";
		  elseif (strpos($locale,"/es/") !== false) echo "Descargas";
		  elseif (strpos($locale,"/it/") !== false) echo "Scarica";
		  else echo "Download";
		?></a>
	&nbsp;&nbsp;
	<a href="Support.php"> <img style="border: 0px solid ; width: 24px; height: 24px;" alt="" src="../img/help.png">
		<?php
		  $locale=$_SERVER['PHP_SELF'];
		  if (strpos($locale,"/fr/")     !== false) echo "Aide";
		  elseif (strpos($locale,"/es/") !== false) echo "Ayuda";
		  elseif (strpos($locale,"/it/") !== false) echo "Aiuto";
		  else echo "Help";
		?></a>
	&nbsp;&nbsp;
	<a href="Contribute.php"> <img style="border: 0px solid ; width: 24px; height: 24px;" alt="" src="../img/bt_contribute.png">
		<?php
		  $locale=$_SERVER['PHP_SELF'];
		  if (strpos($locale,"/fr/")     !== false) echo "Contribuer";
		  elseif (strpos($locale,"/es/") !== false) echo "Contribuir";
		  elseif (strpos($locale,"/it/") !== false) echo "Contributi";
		  else echo "Contribute";
		?></a>
	&nbsp;&nbsp;
	<a href="http://groups.google.com/group/ffdiaporama"> <img style="border: 0px solid ; width: 24px; height: 24px;" alt="" src="../img/bt_forum.png">
		<?php
		  $locale=$_SERVER['PHP_SELF'];
		  if (strpos($locale,"/fr/")     !== false) echo "Forum";
		  elseif (strpos($locale,"/es/") !== false) echo "Foro";
		  elseif (strpos($locale,"/it/") !== false) echo "Forum";
		  else echo "Forum";
		?></a>
	&nbsp;&nbsp;
	<a href="http://tuxfamily.org"> <img src="http://logo.tuxfamily.org/hosted_by_tf2.png" alt="h&Atilde;&copy;berg&Atilde;&copy; librement chez tuxfamily.org"></a>
</div>
<!-- Menu definition end -->
