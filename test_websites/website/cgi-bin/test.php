<!-- <?php
    $stdin = fopen('php://stdin', 'r');
    $input = fgets($stdin);
	$input = strtoupper($input);
    echo "Received: " . $input;
    fclose($stdin);
?> -->

<?php
    // Read from request body using php://input
    $input = file_get_contents('php://input');
	$prefix = 'char=';
    if (strpos($input, $prefix) === 0) {
        $input = substr($input, strlen($prefix));
    }
	$input = strtoupper($input);
	echo "Received: " . $input;
?>

