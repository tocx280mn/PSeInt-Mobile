<?php
	$stdin = fopen('php://stdin', 'r');
	fscanf($stdin, "%f", $c);
	switch ($c) {
	case 1:
		echo '1', PHP_EOL;
		break;
	case 3:
	case 5:
	case 9:
		echo '3, 5 o 9', PHP_EOL;
		break;
	case 7:
		echo '7', PHP_EOL;
		break;
	default:
		echo 'ni 1, ni 3, ni 5, ni 7, ni 9', PHP_EOL;
	}
?>
