<?php

// > $last
// < $track

if(!file_exists("mania_server_tracks.txt"))
    {
    echo "You need 'mania_server_tracks.txt' simple text track list file ! Server is disabled.";
    die("");
    }

$tracks=file("mania_server_tracks.txt");

do{
$last++;
if($last>=count($tracks))
    $last=0;

$track="".trim($tracks[$last]);
}while($track=="");

?>
