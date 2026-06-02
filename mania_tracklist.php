<?php
// this file is used by ManiaDrive to get
// track list from CQFD Corp website.
$libfile="rayphp/libfile.php";
$a=@file($libfile);
if ($a==FALSE)
{
    $libfile="../raydium/rayphp/libfile.php"; 
}
require($libfile);
$list="";
$nl="";


$ok=(integer)http_test(); // R3S function
if(!$ok)
    {
    // network unavailable, read from disk
    $list=file_get_contents("mania_tracklist.txt")."";
    //var_dump($list);
    die("Internet connection unavailable.");
    }

$url="http://maniadrive.cqfd-corp.org/score/list.php";
$file=http_download($url);

$file=explode("\n", $file);

foreach($file as $t)
    if($t!="")
    {
    $list.=$nl.trim($t);
    $nl="\n";
    }

// save list to disk
$fp=fopen("mania_tracklist.txt","wb");
fwrite($fp,$list);
fclose($fp);

//var_dump($list);
?>
