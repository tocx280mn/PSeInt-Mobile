<?php
	$a = array();
	for ($i=1; $i<=10; ++$i) {
		$a[$i] = $i*10;
	}
	foreach ($a as $elemento) {
		echo $elemento, PHP_EOL;
	}
	$b = array();
	$c = 0;
	foreach ($b as $aux_var_1) {
		foreach ($aux_var_1 as $x) {
			$c = $c+1;
			$x = $c;
		}
	}
	for ($i=3; $i>=1; --$i) {
		for ($j=1; $j<=5; $j+=2) {
			echo $b[$i][$j], PHP_EOL;
		}
	}
?>
