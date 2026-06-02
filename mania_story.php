<?php
// ManiaDrive, CQFD Corp.
// Generates story track list for ingame menu

$ret="";
$completed=0;

$scores=array();


function get_data($mni)
{
$a=file($mni);
if ($a==FALSE)
    {
    $smni='';
    raydium_file_home_path_cpy("data/".$mni,&$smni);
    $a=file($smni);
    if ($a==FALSE)
        {
        echo "Track ".$mni." Not Found\n"; 
        return array("$mni","unknown","0","0");
        }
    }

for($i=0;$i<count($a);$i++)
    {
    $l=$a[$i];
    if($l[0]=='d') // is Data
        {
        $l=substr($l,2);
        $l=explode(';',$l);
        return $l;
        }
    }
return array("$mni","unknown","0","0");
}


function get_best($mni)
{
global $scores;
$best=999999;

foreach($scores as $s)
    {
    $a=explode(";",$s);
    if($a[0]==$mni &&  $a[1]<$best)
        {
        $best=trim($a[1]);
        }
    }
return $best;
}

///////////// main
$story=file($story_file);

$state_file='';
raydium_file_home_path_cpy("mania_drive.state",&$state_file);
if(file_exists($state_file))
    $scores=file($state_file);

$ret="";
$col='2';
foreach($story as $s)
    {
    $s=trim($s);
    $d=get_data($s);
    $b=get_best($s);

    $ret.="$s;^$col".$d[0]."  \n";
    if($b>$d[2])
        $col='4';
    }

if($col=='2')
    $completed=1;

?>
