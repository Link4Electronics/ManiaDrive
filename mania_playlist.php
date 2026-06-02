<?php
// place anything you want here to modify $raydium_ogg_file 
// with the desired file (or empty string to stop music playback)

// In this example, I list mania_music/*.ogg and play a random file

$raydium_ogg_file="";
$rep="mania_music/";
$dir=opendir($rep);
if($dir==false) die("Cannot find \"$rep\" directory...");

unset($tbl);
while (($file = readdir($dir)) != false)
    if(substr($file,-4) == ".ogg")
        $tbl[]=$file;


$raydium_ogg_file=$rep.$tbl[rand(0,count($tbl)-1)];

if($rep!=$raydium_ogg_file)
    echo "Now playing $raydium_ogg_file ...";
else
    {
    echo "Cannot find any music";
    $raydium_ogg_file="";
    }

?>