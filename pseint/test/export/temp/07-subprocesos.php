<?php
	$stdin = fopen('php://stdin', 'r');
	echo funciondoble(5), PHP_EOL;
	noretornanada(3, 9);
	$c = 0;
	porreferencia($c);
	echo $c, PHP_EOL;
	$a = array();
	$b = array();
	recibevector($a);
	recibematriz($b);
	function funciondoble($b) {
		$a = 2*$b;
		return $a;
	}
	function noretornanada($a, $b) {
		echo $a+$b, PHP_EOL;
	}
	function porreferencia(&$b) {
		$b = 7;
	}
	function recibevector($v) {
		global $stdin;
		$v[1] = rtrim(fgets($stdin), PHP_EOL);
	}
	function recibematriz($m) {
		global $stdin;
		$m[1][1] = rtrim(fgets($stdin), PHP_EOL);
	}
?>
