<?php
// Converts DirectX mesh file format (.x) to Raydium file format (.tri).

// This script assume (for now) that indexes for normals 
// and uv texturing informations are THE SAME as vertices indexes.

// each object must use one texture only (for now)

// v0.6

//////
// THIS FILE IS NOW DEPRECATED !
// To import 3DS/X files, see http://wiki.raydium.org/wiki/Convert3dsTri
//////

$in=file($argv[1]);
set_time_limit(0);
$no_normals=false; // there is an autodetect feature.

function float($in)
{
$ret=$in;
if(strstr($in,"NAN")!=false) $ret=0;
return($ret);
}

function plop($i)
{
// Comment this line to disable debug traces
//echo "$i\n";
}

function find($what, $from, $in, $quiet=false)
{

for($i=$from;$i<count($in);$i++)
 {
 $tmp=explode(' ',$in[$i]);
  for($j=0;$j<count($tmp);$j++)
   {
   if(trim($tmp[$j])==$what) 
    { /* echo "c:$i\n"; */ return ($i); }
   }
 }
if($quiet) return(-1);
die("\"$what\" not found !\n");
}

function find_char($what, $from, $in)
{
for($i=$from;$i<count($in);$i++)
 {
 if(strstr($in[$i],$what)!=FALSE) { /* echo "c:$i\n"; */ return($i); }
 }
return(-1);
}

echo "1\n";

function export_a_mesh($in, $from)
{
global $textures,$no_normals;

// find mesh
plop("Starts mesh search");
$n=find("Mesh",$from,$in,true); // Mesh start
$meshpos=$n+1;
if($n<0) return(0);
$cpt=find_char(";",$n,$in); // Mesh vertex count

$vertices=floatval($in[$cpt]); // number of vertices
//echo "----------- Vertices: $vertices\n";

for($n=0,$i=($cpt+1) ; $i < (1+$vertices+$cpt) ; $i++,$n++)
 {
 $tmp=explode(";",trim($in[$i]));
 $vert[$n]["vx"]=trim($tmp[0]);
 $vert[$n]["vy"]=trim($tmp[1]);
 $vert[$n]["vz"]=trim($tmp[2]); 
 }
$cpt+=$n+1;



$cpt=find_char(";",$cpt,$in); // find triangles description
$ntri=floatval($in[$cpt]); // number of triangles
//echo "----------- Indexes : $ntri\n";

for($n=0,$i=($cpt+1) ; $i < (1+$ntri+$cpt) ; $i++,$n+=3)
 {
 $tmp=explode(";",trim($in[$i]));
 $topo=trim($tmp[0]);
 if($topo!=3) die("Not a triangle ! ($topo vertices, line ".($i+1).")\n");
 $tmp=explode(",",$tmp[1]);
 $t1=trim($tmp[0]);
 $t2=trim($tmp[1]);
 $t3=trim($tmp[2]);
 
 $index[$n+0]=$t1;
 $index[$n+1]=$t2;
 $index[$n+2]=$t3;
 }
$cpt=$i;

// Normals
if(!$no_normals)
{
 plop("Normals");
 $n=find("MeshNormals",$meshpos,$in,true); // Mesh normals
 plop("Normals found");
  if($n>=0)
  {
  $cpt=find_char(";",$cpt,$in); // Mesh normals count

  for($n=0,$i=($cpt+1) ; $i < (1+$vertices+$cpt) ; $i++,$n++)
   {
   $tmp=explode(";",trim($in[$i]));
   $vert[$n]["nx"]=float(trim($tmp[0]));
   $vert[$n]["ny"]=float(trim($tmp[1]));
   $vert[$n]["nz"]=float(trim($tmp[2])); 
   }
 $cpt=$i;
 }
 else $no_normals=true;
}
plop("MaterielList");
$cpt=find("MeshMaterialList",$meshpos,$in); // Mesh texture
$cpt=find_char("{",$cpt+1,$in);
$mat=$in[$cpt];
$mat=str_replace("{","", str_replace("}","",$mat));
$mat=trim($mat);

// Material may be defined inside MaterialList and not declared the same way:
$embd=explode(" ",trim($in[$cpt]));
if($embd[0]=="Material")
 {
 unset($mat);
 for($j=1;$j<count($embd);$j++)
    $mat.=(" ".$embd[$j]);
 $mat=str_replace("{","", str_replace("}","",$mat));
 $mat=trim($mat);
 }
//echo "+++++++-$mat-\n";


plop("MeshTextureCoords");
$cpt=find("MeshTextureCoords",$meshpos,$in); // Mesh UV
$cpt=find_char(";",$cpt,$in); // Mesh UV count

for($n=0,$i=($cpt+1) ; $i < (1+$vertices+$cpt) ; $i++,$n++)
 {
 //echo "*".trim($in[$i])."*\n";
 $tmp=explode(";",trim($in[$i]));
 $vert[$n]["u"]=trim($tmp[0]);
 $vert[$n]["v"]=trim($tmp[1]);
 $vert[$n]["tex"]=$mat;
 }
$cpt=$i;


for($i=0;$i<$ntri*3;$i++)
 {
 echo $vert[$index[$i]]["vx"];
 echo " ";
 echo $vert[$index[$i]]["vy"];
 echo " ";
 echo $vert[$index[$i]]["vz"];
 echo " ";
 if(trim($vert[$index[$i]]["nx"])=="")
  {
  // no normal were found
  $vert[$index[$i]]["nx"]=1;
  $vert[$index[$i]]["ny"]=1;
  $vert[$index[$i]]["nz"]=1;
  }
 echo $vert[$index[$i]]["nx"];
 echo " ";
 echo $vert[$index[$i]]["ny"];
 echo " ";
 echo $vert[$index[$i]]["nz"];
 echo " ";
 echo $vert[$index[$i]]["u"];
 echo " ";
 echo $vert[$index[$i]]["v"];
 echo " ";
 $a=$textures[$vert[$index[$i]]["tex"]];
 if($a!="") echo $a; else echo "rgb(0.5,0.5,0.5)";
// echo $vert[$index[$i]]["tex"];
 echo "\n";
 }


return ($cpt);
}

// find materials
unset($ret);
while( ($ret=find("Material",$ret,$in,true)) >0 )
{
$mat=trim($in[$ret]);
$mat=str_replace("{","",$mat);
$prem=strpos($mat," ");
$mat=substr($mat,$prem); // delete "Material" in $mat string
$mat=trim($mat);
//echo "###########-$mat-\n";

$ret++;
$end=find_char("}",$ret,$in);
$occ=find("TextureFilename",$ret,$in,true);
if($occ<=$end) 
 {
 $start=find_char("\"",$occ,$in); // Texture filename must use " char.
 $tex=str_replace("\"","",$in[$start]);
 $tex=str_replace("{","",$tex);
 $tex=str_replace("}","",$tex);
 $tex=str_replace(";","",$tex);
 $tex=strtolower(trim($tex));
 $textures[$mat]=$tex; 
// echo "************-$tex-[$mat]\n";
 } else $textures[$mat]="rgb(1,0,0)";
}

//var_dump($textures);
//die();

// Exports all
$ind=0;
do{
$ind=export_a_mesh($in,$ind);
}while($ind>0);

?>