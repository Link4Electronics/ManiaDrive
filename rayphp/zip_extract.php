<?php
// Extract a zip file to the specified directory

// params: $file (string, input), $dest (string, input), $status (integer, output)

$status=0;

$zip = new ZipArchive;

$res=$zip->open($file);
if($res!==TRUE)
	die("packages: ERROR: Unable to open $file from RayPHP");

$res=$zip->extractTo($dest);
if(!$res)
	die("packages: ERROR: Unable to extract '$file' to '$dest'");

$zip->close();

$status=1;
echo "packages: '$file' extracted to '$dest'";

?>