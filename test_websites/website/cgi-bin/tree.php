<?php
    error_reporting(E_ALL);
    ini_set('display_errors', 1);

	// Check if the 'char' and 'size' parameters are set in the query string
    if (isset($_GET['char']) && isset($_GET['size'])) {
        // Get the parameters from the query string
        $char = $_GET['char'];
        $size = $_GET['size'];

		if ($size <= 0) {
			echo "Please provide a positive integer for 'size'.";
			ob_flush();
			flush();
			exit(1);
		}
        // Draw the top part of the diamond
        for ($i = 0; $i < $size; $i++) {
            for ($j = $size - $i; $j > 1; $j--) {
                echo '&nbsp;';
            }

            for ($j = 0; $j <= $i * 2; $j++) {
                echo $char;
            }

            echo "<br>";
            ob_flush();
            flush();
        }

        // Draw the bottom part of the diamond
        for ($i = 0; $i < $size - 1; $i++) {
            echo '&nbsp;';
        }

        echo $char . $char;
        ob_flush();
        flush();
    } else {
        echo "Please provide a character as 'char' and a size as 'size' in the query parameters.";
        ob_flush();
        flush();
		exit(1);
    }
?>

