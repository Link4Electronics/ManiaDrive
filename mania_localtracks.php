<?php
// < $list
// this scripts search for local "user" tracks
$tracks=array();

function trim_value(&$val)
{
$val=trim($val);
}

function search_here($dir)
{
global $tracks;

if (!$handle = opendir($dir))
    die("");

while( ($file = readdir($handle)) !== false )
    {
    $ext=substr($file,-4,4);
    if($ext!==".mni" || $file=="dyn_track.mni")
        continue;
    $tracks[]=$file;
    }

closedir($handle);
}

$data_dir='';
raydium_file_home_path_cpy("data",&$data_dir);

search_here(".");
search_here($data_dir);

// let's clean things, now
$off=file("mania_drive.story.beg");
if ($off==false){
    raydium_file_home_path_cpy("data/mania_drive.story.beg",$data_dir);
    $off=file($data_dir);
}
$off2=file("mania_drive.story.pro");
if ($off2==false){
    raydium_file_home_path_cpy("data/mania_drive.story.pro",$data_dir);
    $off2=file($data_dir);
}
$off=array_merge($off,$off2);

//var_dump($tracks);

array_walk($off,'trim_value');
$tracks=array_diff($tracks,$off);
$tracks=array_unique($tracks);
sort($tracks);


$list=implode("\n",$tracks)."";
?>
