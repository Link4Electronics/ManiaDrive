<?php
// Raydium Server Side repository script
// name this file "index.php", and place data in $data_dir directory.
require("config.inc.php");

if(file_exists(".lock") && $upload_accept)
    $upload_accept=false;

function GorP($var)
{
global $_GET,$_POST;

if(isset($_POST[$var]))
    return $_POST[$var];

if(isset($_GET[$var]))
    return $_GET[$var];

return "";
}

function _opendir($directory)
{
$list=array();
$dir=@opendir($directory);
$i=0;

while($entry=@readdir($dir))
    {
    $key=filemtime($directory.'/'.$entry);
    $key.="-$i";
    $list[$key]=$entry;
    $i++;
    }
@closedir($dir);

krsort($list);
return $list;
}

function _readdir(&$list)
{
$ret=current($list);
next($list);
return $ret;
}

function decompress_file($gz,$final)
{
$fp=gzopen($gz,"r");
if(!$fp) die("FAILED: Cannot open gz file");
while(!gzeof($fp))
    {
    $data.=gzread($fp,128);
    }
gzclose($fp);

$fp=fopen($final,"wb");
if(!$fp)
    {
    echo "FAILED: Cannot create file '$final' on this server";
    return false;
    }
fwrite($fp,$data);
fclose($fp);
unlink($gz);
return true;
}


function head()
{
if(file_exists(".welcome"))
    {
    $str=file(".welcome");
    $str=$str[0];
    }
else
    $str="R3S <a href=\"http://raydium.org/\">Raydium</a> data repository";
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
    <title>Raydium 3D Game Engine</title>
    <style type="text/css">
    <!--BODY {color: #424242; font-family: Verdana,Arial,Helvetica,sans-serif,monospace; margin: 0; padding: 0;  }
    A.blk {color: black;}
    A {color: #F19137;}
    A:HOVER {color: #227CBF;}
    .topbanner {background-color: #FFCC00; border: 0; border-bottom: 1px dashed #5E5E5E; text-align: right;margin: 0; height: 15px; font-size: x-small; padding: 0;}
    .topbanner A {color: Black;}.topbanner A:HOVER {color: #F19137; text-decoration: none;}
    .topbanner UL {list-style: none; border: 0; margin: 0;}
    .topbanner LI {display: inline; margin: 3px;}
    #contenu {margin: 0 10% 0 170px; position: absolute; left:5px; top: 45px; width: 800px;}
    .publi_bloc { border-bottom: 2px dotted #FFCC00;  margin-bottom: 20px;}
    .publi_head {border-bottom: 1px dashed #A9A9A9;  border-left: 10px solid #FFCC00;}
    .publi_head h2 { margin: 0px;  padding-left: 10px;}
    .publi_head h2 a { color: #424242;  text-decoration: none;  }
    .publi_head h2 a:hover { color: #727272;  text-decoration: none;  }
    .publi_info {text-align: right; color: #A9A9A9;}
    .publi_info a {color: #A9A9A9;  text-decoration: none;}
    .publi_info a:hover {color: #696969;  text-decoration: none;}
    .publi_corps{ padding: 10px;}
    IMG {border: 1px solid; margin: 5px;}
    .tables {background: #f3f3f3;border-collapse:collapse;margin-left: auto; margin-right: auto;}
    .tables TD {border-style: solid; border-color:black; border-width:1px; text-align: center; padding-left: 5px; padding-right: 5px;}
    .redfont { color: #dd0000;}
    .greenfont { color: #00dd00;}
    .noborder { border : 0;}
    .border_one { border: 1px dashed #ACACAC; background-color: #EEEEEE; }-->
    </style>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-15">
</head>
<body>
    <div class="topbanner">
        <ul>
            <li>Raydium Server Side Scripts (R3S) - Data Repository for Raydium 3D Game Engine - <a href="http://raydium.org/">http://raydium.org</a></li>
        </ul>
    </div>
    <div id="contenu">
    <div class="contenu">
    <div class="publi_bloc">
    <div class="publi_head">
    <strong>&nbsp;R3S message</strong> &gt; home<h2><?php echo $str;?></h2>
    </div>
    <div class="publi_corps">
<?php
}

function tail()
{
echo "</div></div></div></div></body></html>";
}

function home()
{
global $data_dir,$upload_accept;

head();

echo "<h3>Available files:</h3>\n";
echo "<table border=0 cellpadding=2>";

if ($dh = _opendir($data_dir))
    {
    while (($file = _readdir($dh)) !== false)
        {
        if($file[0]==".") continue;
        $size=filesize($data_dir.$file);
        $total_size+=$size;
        echo "<tr><td><b><a href=\"?type=getBin&file=$file\">$file</a></b></td><td align=right>$size byte(s)</td><td>&nbsp;</td><td>".date("Y-m-d H:i:s",filemtime($data_dir.$file))."</td></tr>";
        }
    }
echo "<tr><td>&nbsp;</td></tr>";
echo "<tr><td><b>Total size</b></td><td align=right>".sprintf("%.2f",$total_size/1024/1024)." MB</td></tr";
echo "</table><br>";
echo "You can add this URL in your <i>rayphp/repositories.*</i> files.<br>";
if($upload_accept) $up="supports"; else $up="does not supports";
echo "This server $up data uploading.<br><br>";

tail();
}



function main($file,$type,$username,$password,$data)
{
global $data_dir,$upload,$upload_accept,$brute_force_delay;
global $HTTP_POST_FILES;

if($type=="")
    {
    home();
    return;
    }



$file=rawurldecode($file);
$file=str_replace("/","",$file);
$file=$data_dir.$file;

if($type=="listRepos")
    {
    if($file==$data_dir)
        $file="$data_dir/*";
    foreach((array)glob($file) as $file)
        {
        if($file[0]!='.')
            echo trim(str_replace("/","",substr($file,strlen($data_dir))))."\n";
        }
    return;
    }

// For all next operations, consider $file as mandatory ...
if($file=="") return;

if($type=="putGzip")
    {
    if(!$upload_accept)
        {
        echo "FAILED: Upload is not activated on this server !";
        return;
        }

    sleep($brute_force_delay);

    $username=rawurldecode($username);
    $password=rawurldecode($password);

    if($username!=$upload["user"] || $password!=$upload["pass"])
        {
        echo "FAILED: invalid user/pass ($username/$password)";
        return;
        }
    $filegz=$file.".tmp.gz";
    if(file_exists($_FILES["data"]["tmp_name"]))
        {
        move_uploaded_file($_FILES["data"]["tmp_name"], $filegz);
        }
    else die("FAILED: Cannot find data in this request");

    if(!decompress_file($filegz,$file)) return;
    chmod($file,0664);

    echo "+ SUCCESS: file uploaded";
    return;
    }

if(!file_exists($file)) die("FAILED: file not found");

if($type=="getGzip")
    {
    $fp=@fopen($file,"rb");
    if(!$fp) die("FAILED: file not found");
    $data=fread($fp,filesize($file));
    fclose($fp);

    $tmp=tempnam("./","delme");
    $fp=@gzopen($tmp,"wb");
    if(!$fp) return;
    gzwrite($fp,$data);
    gzclose($fp);
    //$dat=gzencode($data);
    //echo $dat;
    //echo date("s");
    readfile($tmp);
    unlink($tmp);
    //echo $tmp;
    }

if($type=="getDate")
    {
    echo filemtime($file);
    }

if($type=="getBin")
    {
    header('Content-type: application/octet-stream');
    header('Content-Transfer-Encoding: Binary');
    header('Content-length: '.filesize($file));
    header('Content-Disposition: attachment; filename="'.basename($file).'"');
    readfile($file);
    }

} // end main()


main(GorP("file"),GorP("type"),GorP("username"),GorP("password"),GorP("data"));
