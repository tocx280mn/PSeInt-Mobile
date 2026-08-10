<?php
	$c = 0;
	while ($c<=10) {
		$c = $c+1;
		echo $c, PHP_EOL;
	}
	$c = 0;
	do {
		$c = $c+1;
		echo $c, PHP_EOL;
	} while ($c!=10);
	$c = 0;
	do {
		$c = $c+1;
		echo $c, PHP_EOL;
	} while ($c<10);
	if ($c==10) {
		echo 'Si', PHP_EOL;
	}
	if ($c==10) {
		echo 'Si', PHP_EOL;
	} else {
		echo 'No', PHP_EOL;
	}
?>
