<?php
/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/
// Sketchup embeded Collada 1.4.1 file converter to raydium 1 .tri format file.
// This script convert only the first Scene.
// Run recursively into the scene hierarchy.
// Export all meshes in one .tri file.
// Size of generated file is generaly big, number of texture can
// exceed raydium capacity on big models.
//
// $file_name (input kmz/dae/tri file name) $force_scale(input convert scale applied to all meshes) $verbose(input verbose level).


if (empty($file_name))
    {
    //$file_name="test.tri";
    //$file_name="test.dae";
    //$file_name="city.dae";
    //$file_name="tele.kmz";
    $file_name="tests.kmz";
	if (empty($file_name))
		die("kmz_2_tri: ERROR: empty file name! the world will explode !");
    }

if (empty($force_scale))
    $force_scale=1;
//$force_scale = $force_scale *0.0254; //Sketcup output data in inches, working with meters
if (empty($verbose))
    $verbose=0;

if (!file_exists($file_name))
    {
    $file_name="";
    die("kmz_2_tri: ERROR: File '$file_name' not found.");
    }

dprint(0,"kmz_2_tri: Converting: ".$file_name);
if ($force_face)
    dprint(0,"kmz_2_tri: Forcing single face creation.");

$dest_path=dirname($file_name);

$dae_file=dae_file_name($file_name);
$source_path=dirname($dae_file);
$xml_file=convert_xml_iso($dae_file);

$dom = new DomDocument;

if (!$dom->load($xml_file))
    {
    $file_name="";
    die("kmz_2_tri: Invalid dae/xml file. Please try again.");
    }

$xpath = new DOMXpath($dom);
$res=$xpath->query("//COLLADA/*");
if ($res->length==0)
    {
    echo "kmz_2_tri: ERROR: Xpath doesn't work. Verify utf8 encoding, schema definition in dae header ...\n";
    echo "kmz_2_tri: Sorry script can't go further as is.\n";
    $file_name="";
    die(-1);
    }
$units=$dom->getElementsByTagName("unit");
if ($units->length>=1)
    {
    $unit=$units->item(0);
    $scale = $unit->getAttribute("meter");
    $force_scale = $force_scale * $scale;
    echo "Found unit ".$scale." using ".$force_scale." scale factor";
    }

$out_file=$dest_path."/".filename($file_name).".tri";
dprint(0,"kmz_2_tri: Output file: ".$out_file);
$ftri=fopen($out_file,"w");
fprintf($ftri,"1\n");

$visual_scene_url=get_attribute("/scene/instance_visual_scene","url");
$scene_nodes= find_attribute("/library_visual_scenes/visual_scene","id",$visual_scene_url);
$scene_nodes=get_childs($scene_nodes);

$matrix=set_identity();
$n=export_nodes($scene_nodes,$matrix,$scene_nodes);

fclose($ftri);
//kmz_unzip_remove();
$file_name=$ftri;
die(0);

//******************************************************************************
function export_nodes($nodes,$matrixin,$from_node)
    {

    foreach($nodes as $node)
        {
        if ($node->nodeType==XML_TEXT_NODE) continue;
        if ($node->tagName=="node")
            {
            dprint(4,"kmz_2_tri:->Entering Sub Node:".$node->getAttribute("name"));
            export_nodes(get_childs($node),$matrixin,$node);
            dprint(4,"kmz_2_tri:<-Leaving Sub Node:".$node->getAttribute("name"));
            }
        if ($node->tagName=="instance_node")
            {
            $nodep=$node->parentNode;
            $matrix = set_identity();
            $matrix = matrix($nodep,$matrix);
            $matrix = translate($nodep,$matrix);
            $matrix = rotate($nodep,$matrix);
            $matrix = scale($nodep,$matrix);
            $matrixout=mat_mul($matrixin,$matrix);

            $nnode=get_attribute("","url",$node);

            $nnode=find_attribute("/library_nodes/node","id",$nnode);
            $nnodes=get_childs($nnode);
            export_nodes($nnodes,$matrixout,$node);
            }
        if ($node->tagName=="instance_geometry")
            {
            $nodep=$node->parentNode;
            $matrix = set_identity();
            $matrix = matrix($nodep,$matrix);
            $matrix = translate($nodep,$matrix);
            $matrix = rotate($nodep,$matrix);
            $matrix = scale($nodep,$matrix);
            $matrixout=mat_mul($matrixin,$matrix);

            $geom=get_attribute("","url",$node);
            $meshs=find_attribute("/library_geometries/geometry","id",$geom);
            $mesh=find_one("mesh",$meshs);
            dprint(2,"kmz_2_tri:Exporting mesh:".$meshs->getAttribute("name"));
            $nverts=export_mesh($mesh,$matrixout,$node);
            dprint(2,"kmz_2_tri: ".$meshs->getAttribute("name")." ".$nverts." vertices.");
            }
        }
    }
function export_mesh($mesh,$matrix,$from_node)
    {
    global $vxyz,$vxyz_offset,$vnxyz,$vnxyz_offset,$uv_array,$uv_offset,$has_uv;
    global $faces_indexes,$interleave;
    global $force_face;
    $nverts=0;
    $triangles=find("triangles",$mesh);
	$skip=0;
    foreach($triangles as $triangle)
        {
        if ($force_face)
            if ($skip)
                {
                $skip=0;
                //dprint(0,"Skipping faces");
                continue;
                }
                
        $texture = texture($triangle,$from_node);
        $input_vectors=find("input",$triangle);
        $interleave=interleave($input_vectors);
        $vxyz=$vnxyz=$uv_array=array();
        parse_inputs($mesh,$input_vectors,$vxyz,$vxyz_offset,$vnxyz,$vnxyz_offset,$uv_array,$uv_offset,$has_uv);
        $vxyz=vertices_transform($vxyz,$matrix);

        $faces_indexes=find_one("p",$triangle);
        $faces_indexes=get_value($faces_indexes);
        $faces_indexes=explode(" ",$faces_indexes);
        $n=(sizeof($faces_indexes)) /$interleave;
        for ($indx=0;$indx<$n;$indx++)
            write_vert($indx,$texture);
        $nverts+=$n;
		$skip++;
        }

    $polys=find("polylist",$mesh);
    foreach($polys as $poly)
        {
        $texture = texture($poly,$from_node);
        $input_vectors=find("input",$poly);
        $interleave=interleave($input_vectors);
        $vxyz=$vnxyz=$uv_array=array();
        parse_inputs($mesh,$input_vectors,$vxyz,$vxyz_offset,$vnxyz,$vnxyz_offset,$uv_array,$uv_offset,$has_uv);
        $vxyz=vertices_transform($vxyz,$matrix);

        $faces_nvertex=find_one("vcount",$poly);
        $faces_nvertex=get_value($faces_nvertex);
        $faces_indexes=find_one("p",$poly);
        $faces_indexes=get_value($faces_indexes);
        $faces_nvertex=explode(" ",$faces_nvertex);
        $faces_indexes=explode(" ",$faces_indexes);

        $j=0;
        foreach($faces_nvertex as $face_n)
            {
            if ($face_n==4)
                {
                $indx=array(0,1,2,2,3,0);
                for ($i=0;$i<6;$i++)
                    {
                    //Creating to triangle 0-1-2 and 2-3-0
                    $v=$j+$indx[$i];
                    write_vert($v,$texture);
                    }
                $j+=4;
                $nverts+=4;
                }
            if ($face_n==3)
                {
                for ($i=0;$i<3;$i++)
                    write_vert($j+$i,$texture);
                $j+=3;
                $nverts+=3;
                }
            }
        }
    return $nverts;
    }

function interleave($inputs)
    {
    $max_offset=0;
    foreach($inputs as $input)
    $max_offset = max($max_offset,$input->getAttribute("offset"));
    return $max_offset+1;
    }

function parse_inputs($mesh,$input_vectors,&$vxyz,&$vxyz_offset,&$vnxyz,&$vnxyz_offset,&$uv_array,&$uv_offset,&$has_uv)
    {

    $has_uv=False;
    foreach($input_vectors as $input)
        {
        if (get_attribute("","semantic",$input)=="VERTEX")
            {
            $vxyz_offset=get_attribute("","offset",$input);

            $v1=get_attribute("","source",$input);
            $v=find_attribute("vertices","id",$v1,$mesh);

            $inputvs=find("input",$v);
        	foreach($inputvs as $inputv)
        		{
        		if (get_attribute("","semantic",$inputv )=="POSITION")
        			{
        			$vs=get_attribute("","source",$inputv);
        			$source=find_attribute("source","id" ,$vs,$mesh);
        			$float_array=find_one("float_array",$source);
        			$vcount=get_attribute("","count",$float_array);
        			$vposvals=get_value($float_array);
        			$xyzpos=explode(" ",$vposvals);

        			$j=0;
        			for ($i=0;$i<$vcount;$i+=3)
        				{
        				$vxyz[$j][0]=$xyzpos[$i];
        				$vxyz[$j][1]=$xyzpos[$i+1];
        				$vxyz[$j][2]=$xyzpos[$i+2];
        				$j++;
        				}
        			}

        		if (get_attribute("","semantic",$inputv )=="NORMAL")
        			{
        			$vnxyz_offset=0;
        			$vs=get_attribute("","source",$inputv);
        			$source=find_attribute("source","id" ,$vs,$mesh);
        			$float_array=find_one("float_array",$source);
        			$vcount=get_attribute("","count",$float_array);
        			$vnormvals=get_value($float_array);
        			$nxyzpos=explode(" ",$vnormvals);

        			$j=0;
        			for ($i=0;$i<$vcount;$i+=3)
        				{
        				$vnxyz[$j][0]=$nxyzpos[$i];
        				$vnxyz[$j][1]=$nxyzpos[$i+1];
        				$vnxyz[$j][2]=$nxyzpos[$i+2];
        				$j++;
        				}

        			}
        		}
        	}


        if (get_attribute("","semantic",$input)=="NORMAL")
            {
            $vnxyz_offset=get_attribute("","offset",$input);
            $v1=get_attribute("",'source',$input);

            $float_array=find_attribute("source","id",$v1,$mesh);
            $float_array=find_one("float_array",$float_array);
            $vcount=get_attribute("","count",$float_array);
            $vnormvals=get_value($float_array);
            $nxyzpos=explode(" ",$vnormvals);

            $j=0;
            for ($i=0;$i<$vcount;$i+=3)
                {
                $vnxyz[$j][0]=$nxyzpos[$i];
                $vnxyz[$j][1]=$nxyzpos[$i+1];
                $vnxyz[$j][2]=$nxyzpos[$i+2];
                $j++;
                }

            }
        if (get_attribute("","semantic",$input)=="TEXCOORD")
            {
            $uv_offset=get_attribute("","offset",$input);
            $v1=get_attribute("",'source',$input);

            $float_array=find_attribute("source","id",$v1,$mesh);
            $float_array=find_one("float_array",$float_array);
            $vcount=get_attribute("","count",$float_array);
            $uvvals=get_value($float_array);
            $uv=explode(" ",$uvvals);

            $j=0;
            for ($i=0;$i<$vcount;$i+=2)
                {
                $uv_array[$j][0]=$uv[$i];
                $uv_array[$j][1]=$uv[$i+1];
                $j++;
                }
            $has_uv=True;
            }
        }
    }
function texture($node,$from_node)
    {
    global $source_path,$dest_path,$file_name;
    $mat_id=get_attribute("",'material',$node);
    if ($mat_id==NULL || $mat_id=="")
        {
        return "rgb(0.6,0.6,0.6)";
        }
    $target=find_attribute("bind_material/technique_common/instance_material","symbol",$mat_id,$from_node);
    $target=get_attribute("","target",$target);

    $effect=find_attribute("/library_materials/material","id",$target);
    $effect=find_one("instance_effect",$effect);
    $url=get_attribute("","url",$effect);

    $effect=find_attribute("/library_effects/effect","id",$url);

    $profile=find_one("*/technique/lambert/diffuse/*",$effect);


    if ($profile->tagName=='color')
        {
        $rgb=$profile->nodeValue;
        $rgb=explode(' ',$rgb);
        return sprintf("rgb(%3.3f,%3.3f,%3.3f)",$rgb[0],$rgb[1],$rgb[2]);
        }

    if ($profile->tagName=='texture')
        {
        $sid=get_attribute("","texture",$profile);
        $sampler=find_attribute("*/newparam","sid",$sid,$effect);///surface/init_from

        $sid=get_value($sampler);
        $surface=find_attribute("*/newparam","sid",$sid,$effect);
        $sid=get_value($surface);
        $image=find_attribute("/library_images/image","id",$sid);
        $image=get_value($image);

        $source_file_name=$source_path."/".$image;

        $dest_file_name=$dest_path."/".filename($file_name)."_".filename($image).".tga";

        if (!file_exists($dest_file_name))
            {
            convtga($source_file_name,$dest_file_name);
            }

        if (file_exists($dest_file_name))
            return basename($dest_file_name);
        else
            return "rgb(0.2,0.2,0.2)";

        return $image;
        }
    }

function convtga($img,$image)
    {
    if(!is_callable(gd_info))
        return;

    $flip=False;
    dprint(0,"Converting and scaling:".basename($img)." to ".$image);
    $ext=pathinfo($img);
    if ($ext['extension']=='jpg')
        {
        $ims=imagecreatefromjpeg($img);
        }
    if ($ext['extension']=='png')
        {
        $ims=imagecreatefrompng($img);
        $flip=True;
        }

    $sx=imagesx($ims);
    $sy=imagesy($ims);

    $psx=pow(2,floor(log($sx,2)));
    $psy=pow(2,floor(log($sy,2)));

    $imr=imagecreatetruecolor($psx,$psy);
    imagealphablending($imr,False);

    imagecopyresampled($imr,$ims ,0,0,0,0,$psx ,$psy ,$sx ,$sy);


    $imd=fopen($image,"wb");
    $fh=sprintf("\000\000\002\000\000\000\000\000\000\000\000\000%c%c%c%c\040\000",$psx%256,floor($psx/256),$psy%256,floor($psy/256));
    fwrite($imd,$fh,18);
    for ($i=0;$i<$psy;$i++)
        {
        for ($j=0;$j<$psx;$j++)
            {

            $rgb = imagecolorat($imr, $j, $psy-$i-1);

            $a = ($rgb >> 24) &0xFF;
            $r = ($rgb >> 16) & 0xFF;
            $g = ($rgb >> 8) & 0xFF;
            $b = $rgb & 0xFF;
            if ($a==0)
                $a=255;
            else
                $a=0;
            $d=sprintf("%c%c%c%c",$b,$g,$r,$a);
            fwrite($imd,$d,4);
            }
        //dprint($i);
        }

    fclose($imd);
    }

function vertices_transform($vixyz,$matrix)
    {
    for ($i=0;$i<sizeof($vixyz);$i++)
        {
        for ($j=0;$j<3;$j++)
            {
            $vxyz[$i][$j]=$matrix[$j][0]*$vixyz[$i][0]+$matrix[$j][1]*$vixyz[$i][1]+$matrix[$j][2]*$vixyz[$i][2]+$matrix[$j][3];
            }
        }
    return $vxyz;
    }
function write_vert($indx,$material_name)
    {
    global $faces_indexes,$vxyz_offset,$vnxyz_offset,$uv_offset;
    global $vxyz,$vnxyz,$uv_array,$interleave,$has_uv;
    global $ftri,$force_scale;

    $vert_indx=$faces_indexes[$indx*$interleave+$vxyz_offset];
    $vert_nindex=$faces_indexes[$indx*$interleave+$vnxyz_offset];
    if ($has_uv)
        $vert_uv_indx=$faces_indexes[$indx*$interleave+$uv_offset];

    $li=sprintf("%+9.9f %+9.9f %+9.9f ",$vxyz[$vert_indx][0]*$force_scale,$vxyz[$vert_indx][1]*$force_scale,$vxyz[$vert_indx][2]*$force_scale);
    $li.=sprintf("%+6.6f %+6.6f %+6.6f ",$vnxyz[$vert_nindex][0],$vnxyz[$vert_nindex][1],$vnxyz[$vert_nindex][2]);
    if ($has_uv)
        $li.=sprintf("%+6.6f %+6.6f ",$uv_array[$vert_uv_indx][0],$uv_array[$vert_uv_indx][1]);
    else
        $li.=sprintf("%+6.6f %+6.6f ",0,0);
    $li.=$material_name;

    fprintf($ftri,$li."\n");
    }

function set_identity()
    {
    for ($i=0;$i<4;$i++)
        for ($j=0;$j<4;$j++)
            if ($i==$j)
                $matrix[$i][$j]=1;
            else
                $matrix[$i][$j]=0;
    return $matrix;
    }
function mat_mul($ma,$mb)
    {
    for ($i=0;$i<4;$i++)
        for ($j=0;$j<4;$j++)
            $mr[$i][$j]=$ma[$i][0]*$mb[0][$j]+$ma[$i][1]*$mb[1][$j]+$ma[$i][2]*$mb[2][$j]+$ma[$i][3]*$mb[3][$j];
    return $mr;

    }

function matrix($node,$matrix)
    {
    $matrix1=find_node("matrix",$node);
    if ($matrix1==NULL)
        return $matrix;
    $matrix1=$matrix1->nodeValue;
    $matrix2=$matrix1;
    do
        {
        $matrix1=$matrix2;
        $matrix2=str_replace("  "," ",$matrix2);
        $matrix2=str_replace("\n"," ",$matrix2);
        $matrix2=str_replace("\r"," ",$matrix2);
        }
    while ($matrix2!=$matrix1);
    $matrix1=explode(' ',trim($matrix2));

    for ($i=0;$i<4;$i++)
        for ($j=0;$j<4;$j++)
            $matrix[$i][$j]=$matrix1[$i*4+$j];
    return $matrix;
    }
function translate($node,$matrix)
    {
    $mesh_postag=find_node("translate",$node);
    if ($mesh_postag===NULL)
        return $matrix;
    $mesh_pos=explode(" ",$mesh_postag->nodeValue);
    for ($i=0;$i<3;$i++)
        $matrix[$i][3]=$mesh_pos[$i];
    return $matrix;
    }
function rotate($node,$matrix)
    {
    $mesh_rots=find_nodes("rotate",$node);
    $rx=$ry=$rz=0;
    if ($mesh_rots===NULL)
        return $matrix;
    foreach($mesh_rots as $rot)
        {
        $axes=$rot->nodeValue;
        $axes=explode(" ",$axes);
        if ($axes[0]!=0) $rx=$axes[3]*pi()/180;
        if ($axes[1]!=0) $ry=$axes[3]*pi()/180;
        if ($axes[2]!=0) $rz=$axes[3]*pi()/180;
        }

    $sx = sin($rx);             // Setup - une seule fois
    $cx = cos($rx);
    $sy = sin($ry);
    $cy = cos($ry);
    $sz = sin($rz);
    $cz = cos($rz);
    $matrix[0][0]=$cz*$cy;
    $matrix[0][1]=$cz*$sy*$sx-$sz*$cx;
    $matrix[0][2]=$sz*$sx+$cz*$sy*$cx;
    $matrix[1][0]=$sz*$cy;
    $matrix[1][1]=$cz*$cx+$sz*$sy*$sx;
    $matrix[1][2]=$sz*$sy*$cx-$cz*$sx;
    $matrix[2][0]=-$sy;
    $matrix[2][1]=$cy*$sx;
    $matrix[2][2]=$cy*$cx;
    return $matrix;
    }
function scale($node,$matrix)
    {
    $mesh_scale=find_node("scale",$node);
    if ($mesh_scale===NULL)
        return $matrix;

    $mesh_scale=explode(" ",$mesh_scale->nodeValue);
    for ($i=0;$i<3;$i++)
        for ($j=0;$j<3;$j++)
            $matrix[$i][$j]=$matrix[$i][$j]*$mesh_scale[$j];
    return $matrix;
    }

function get_value($node)
    {
    return trim($node->nodeValue);
    }
function get_attribute($path,$attribute,$rel=NULL)
    {
    if ($path!="")
        $rel=find_one($path,$rel);
    $res=$rel->getAttribute($attribute);
    $res=explode('#',$res);
    $res=$res[count($res)-1];
    return $res;
    }
function get_childs($node)
    {
    $res = find("child::*",$node);
    return $res;
    }
function find($query,$rel=NULL)
    {
    global $xpath;
    if ($rel===NULL)
        $ret=$xpath->query("/COLLADA".$query);
    else
        $ret=$xpath->query($query,$rel);
    return $ret;
    }
function find_attribute($where,$attribute,$value,$rel=NULL)
    {
    $query=$where."[@".$attribute."='".$value."']";
    $res=find_one($query,$rel);
    return $res;
    }
function find_one($tag,$rel=NULL)
    {
    $r=find($tag,$rel);
    if ($r->length>1)
        {
        dprint(0,"Structural problem, found ".$r->length." tags ".$tag." only one expected.");
        }
    return $r->item(0);
    }
function find_node($tag,$rel=NULL)
    {
    $r=find($tag,$rel);
    if ($r->length==0)
        {
        return NULL;
        }
    return $r->item(0);
    }
function find_nodes($tag,$rel=NULL)
    {
    $r=find($tag,$rel);
    if ($r->length==0)
        {
        return NULL;
        }
    return $r;
    }

function convert_xml_iso($f)
    {
    //Quick and dirty utf8 change for xpath !!!!!
    $fin=fopen($f,"r");
    $xml_file=dirname($f)."/".filename($f).".xml";
    $fout=fopen($xml_file,"w");
    $c=fgets($fin);
    $c="<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
    fputs($fout,$c);
    $c=fgets($fin);
    $c="<COLLADA>\n";
    fputs($fout,$c);
    while (!feof($fin))
        {
        $c=fgets($fin);
        fputs($fout,$c);
        }
    fclose($fin);
    fclose($fout);
    return $xml_file;
    }
function dae_file_name($file_name)
    {
    $file_name=clean_path($file_name);

    if (extension($file_name)==='kmz')
        {
    	kmz_unzip_remove();
        $source_path=kmz_unzip($file_name);
        $dae_file=$source_path."/models/".filename($file_name).".dae";
    	if (!file_exists($dae_file))
    		{
    		$dae_file=$source_path."/models/untitled.dae";
    		if (!file_exists($dae_file))
    			{
    			printf ("Can't find dae file. Sorry");
    			die(-1);
    			}
    		}
        }
    if (extension($file_name)==='dae')
        {
        $dae_file=$file_name;
        }
    if (extension($file_name)==='tri')
        {
        dprint(0,"Already .tri file.");
        die(0);
        }
    if (extension($file_name)==='xml')
        {
        $dae_file=$file_name;
        }
    return $dae_file;
    }
function clean_path($p)
    {
    while (($pos=strpos($p,"\\"))!==False)
        $p[$pos]='/';
    return $p;
    }
function extension($f)
    {
    return pathinfo($f,PATHINFO_EXTENSION);
    }
function filename($f)
    {
    return pathinfo($f,PATHINFO_FILENAME);
    }

function dprint($lvl,$a)
    {
    global $verbose;
    if ($lvl<=$verbose)
        {
        echo $a;
        //echo " <br>\n";
        }
    }
function xmlprint($a)
    {
    $Body = $a;//->ownerDocument->documentElement->firstChild;
    $Document = new DOMDocument();
    $Document->appendChild($Document->importNode($Body,true));
    dprint($Document->saveHTML());
    }

function kmz_unzip($kmz_filen)
    {

    $tmpfile = tempnam("kmz","");
    $path = dirname($tmpfile);
    unlink($tmpfile);
    dprint(2,"kmz_2_tri: Working temp dir:".$path);
    $pathkmz=$path."/kmz";
    // Sous php edit ne pas oublier d'ajouter l'extension zip_php dans php.ini
    $zip= new ZipArchive;
    $zip->open($kmz_filen);
    $zip->extractTo($pathkmz);
    $zip->close();

    return $pathkmz;
    }
function kmz_unzip_remove()
    {
    $tmpfile = tempnam("kmz","");
    $path = dirname($tmpfile);
    unlink($tmpfile);
    $pathkmz=$path."/kmz";
    recursive_remove_directory($pathkmz);
    }
function recursive_remove_directory($directory, $empty=FALSE)
    {
    if (substr($directory,-1) == '/')
        $directory = substr($directory,0,-1);
    if (!file_exists($directory) || !is_dir($directory))
        return FALSE;
    elseif(is_readable($directory))
        {
        $handle = opendir($directory);
        while (FALSE !== ($item = readdir($handle)))
            {
            if ($item != '.' && $item != '..')
                {
                $path = $directory.'/'.$item;
                if (is_dir($path))
                    recursive_remove_directory($path);
                else
                    unlink($path);
                }
            }
        closedir($handle);
        if ($empty == FALSE)
            {
            if (!rmdir($directory))
                return FALSE;
            }
        }
    return TRUE;
    }
?>
