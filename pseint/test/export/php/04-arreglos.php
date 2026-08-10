<?php
	$stdin = fopen('php://stdin', 'r');
	$a = array();
	$b = array();
	$c = array();
	$d = array();
	fscanf($stdin, "%d", $n);
	$e = array();
	$a[1] = rtrim(fgets($stdin), PHP_EOL);
	$a[10] = $a[1];
	echo $b[10][10], PHP_EOL;
?>
