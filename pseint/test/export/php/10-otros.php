<?php
	$stdin = fopen('php://stdin', 'r');
	echo PHP_EOL;
	fgetc($stdin);
	usleep(100*1000);
	sleep(1);
	echo M_PI, PHP_EOL;
?>
