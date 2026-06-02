<?php
// Test if Internet connection is available using Raydium website

// params: $status (integer, output)
// (0 means 'not available', 1 means 'OK')

// This script must be placed in "rayphp" sub-directory.

require("libfile.php");
$status = (http_test()?1:0);
?>
