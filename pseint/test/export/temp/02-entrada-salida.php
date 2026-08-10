<?php
	$stdin = fopen('php://stdin', 'r');
	settype($a, 'integer');
	settype($b, 'float');
	settype($c, 'string');
	settype($d, 'boolean');
	fscanf($stdin, "%d", $a);
	fscanf($stdin, "%f", $b);
	$c = rtrim(fgets($stdin), PHP_EOL);
	fscanf($stdin, "%d", $d);
	echo $a, PHP_EOL;
	echo $b, PHP_EOL;
	echo $c, PHP_EOL;
	echo $d, PHP_EOL;
	fscanf($stdin, "%d", $a);
	fscanf($stdin, "%f", $b);
	$c = rtrim(fgets($stdin), PHP_EOL);
	fscanf($stdin, "%d", $d);
	echo $a, $b, $c, $d, PHP_EOL;
	echo 'Esta linea no lleva enter al final';
?>
