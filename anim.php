<?php
// This script is able to extract only needed anims from a animated
// tri file and to reverse any anim (for backward run, for example).

// usage: ./anim.php anim.tri stand run -run jump death1
// "-" stands for reverse animation

function find_anim($name)
{
global $anims,$anims_num;

if($name[0]=='-')
    $name=substr($name,1);

for($i=0;$i<$anims_num;$i++)
    if(!strcmp($anims[$i]["name"],$name))
        return $anims[$i];

die("\nanim not found: $name\n");
}

function dump($start,$len,$name)
{
global $fp_start,$fp,$anims_len;

// rewind
fseek($fp,$fp_start,SEEK_SET);

for($i=0;$i<($start*$anims_len);$i++)
    fgets($fp);

// side effect
$len++;

$v=array();
for($i=0;$i<$len;$i++)
    for($j=0;$j<$anims_len;$j++)
        {
        $v[$i].=fgets($fp);
        }

// test - and reverse
if($name[0]=='-')
    $v=array_reverse($v);

// display
for($i=0;$i<count($v);$i++)
    echo $v[$i];

unset($v);
}


if($argv[2]=="") die("usage example: php anim.php anim.tri stand run -run jump death1\n");

$in=$argv[1];
$fp=fopen($in,"rb");

if(!$fp)
    die("cannot open $in\n");

$f=trim(fgets($fp));

if($f!=2)
    die("Not an animated tri file (need version2)");

for($i=2;true;$i++)
    {
    if($argv[$i]=="")
        break;
    $wanted[]=$argv[$i];
    }

//var_dump($anims);
$f=explode(' ',trim(fgets($fp)));
$anims_num=$f[0];
$anims_len=$f[1];

//var_dump($f);
for($i=0;$i<$anims_num;$i++)
    {
    $f=explode(' ',trim(fgets($fp)));
    $anims[$i]["start"]=$f[0];
    $anims[$i]["end"]=$f[1];
    $anims[$i]["name"]=$f[2];
    }

echo "2\n";
echo count($wanted)." ".$anims_len."\n";

$cpt=0;
for($i=0;$i<count($wanted);$i++)
    {
    $a=find_anim($wanted[$i]);
    $l=($a["end"]-$a["start"]);
    echo $cpt." ".($cpt+$l)." ".$wanted[$i]."\n";
    $cpt+=($l+1);
    }

echo fgets($fp);
$fp_start=ftell($fp);

for($i=0;$i<count($wanted);$i++)
    {
    $a=find_anim($wanted[$i]);
    $l=($a["end"]-$a["start"]);
    dump($a["start"],$l,$wanted[$i]);
    }

?>
