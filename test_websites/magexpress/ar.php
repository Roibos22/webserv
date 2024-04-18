<?php
    // Check if the 'num1' and 'num2' parameters are set in the query string
    if (isset($_GET['num1']) && isset($_GET['num2'])) {
        // Get the numbers from the query string
        $num1 = $_GET['num1'];
        $num2 = $_GET['num2'];

        // Calculate the sum
        $sum = $num1 + $num2;

        // Output the sum
        echo "The sum of $num1 and $num2 is $sum.";
    } else {
        echo "Please provide two numbers as query parameters 'num1' and 'num2'.";
    }
?>
