<?php
// place anything you want here to modify $raydium_ogg_file 
// with the desired file (or empty string to stop music playback)

// In this example, I list music/*.ogg and play a random file

$raydium_ogg_file="";
$dir=opendir("music/");
if($dir==false) die("Cannot find \"music/\" directory...");

unset($tbl);
while (($file = readdir($dir)) != false)
    if(substr($file,-4) == ".ogg")
        $tbl[]=$file;


$raydium_ogg_file="music/".$tbl[rand(0,count($tbl)-1)];

echo "Now playing $raydium_ogg_file ...";
?>