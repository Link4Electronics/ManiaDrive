<?php
// set the proxy according to the configuration database
$proxy = '';
raydium_parser_db_get("Generic-Proxy", &$proxy, "");
$GLOBALS['raydium_proxy'] = $proxy;

// returns a newer content for a file (using HTTP repository)
function update_file($file,$repos,$local,$force)
{
  $file=rawurlencode($file);
  $req="$repos?file=$file&type=getDate";
  $d=http_download($req);
  if($d == false)
    {
      echo "FAILED: Cannot get date on $file ($repos)";
      return false;
    }
  
  $ld=@filemtime($local);
  
  if($ld>=$d && !$force && $d!=false && $ld!=false)
    {
      echo "Local file is the same or newer, aborting. ($repos)";
      return false;
    }
  $req="$repos?file=$file&type=getGzip";
  $data=http_download($req);
  // we got nothing
  if($data===false)
    {
      echo "FAILED: Cannot retrieve $file ($repos)";
      return false;
    }

  // if we got an error message
  if(substr($data,0,6)=="FAILED")
    {
      // not very safe on a rogue server
      echo $data." ($repos)";
      return false;
    }
  
  echo "OK ($repos)";
  
  // let's update the file with data
  $data=gzdecode($data);
  $fp=@fopen($local,"wb");
  if(!$fp) die("FAILED: Cannot create output file '$path$file', aborting.");
  fwrite($fp,$data);
  fclose($fp);
  return true;
}

// Retrieves a file from the internet using HTTP. Return the file as a string
function http_download($url)
{
  $ch = curl_init();
  if (defined($GLOBALS['raydium_proxy']))
    curl_setopt ($ch, CURLOPT_PROXY, $GLOBALS['raydium_proxy']);
  curl_setopt ($ch, CURLOPT_URL, $url);
  curl_setopt ($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt ($ch, CURLOPT_FOLLOWLOCATION, 1);
  curl_setopt ($ch, CURLOPT_TIMEOUT, 60);

  $result = curl_exec ($ch);
  if (curl_errno($ch)) 
    {
      print "ERROR downloading file: " . curl_error($ch);
      return false;
    } 
  else 
    {
      curl_close($ch);
      return $result;
    }
}



function read_repositories_file_internal($repos,&$repos_list)
{
  $list=@file($repos);
  if(count($list)==0)
    {
    echo "Cannot open repository file: $repos";
    return $repos_list;
    }

  // let's clean up the list
  for($i=0;$i<count($list);$i++)
    {
      $list[$i]=trim($list[$i]);
    }
  $repos_list = array_unique (array_merge($repos_list,$list));
  return $repos_list;
}

// Get repositories from a given (local) file
function read_repositories_file($repos)
{
global $raydium_php_rayphp_path;
$repos_list=array();

$tmp="";
raydium_file_home_path_cpy($repos,&$tmp);

if(file_exists($tmp))
    read_repositories_file_internal($tmp,$repos_list);

read_repositories_file_internal($raydium_php_rayphp_path."/".$repos,$repos_list);

return $repos_list;
}

// Get the file list from a given repository
function list_repos($filter,$size,$repos)
{
  $filter=rawurlencode($filter);
  
  $req="$repos?file=$filter&type=listRepos";
  $d=http_download($req);
  if($d===false) 
    return false;

  echo "Listing from '$repos'";
  return $d;
}

// Sets file and path from a full filename
function filename_cut($filename,&$file,&$path)
{
  $t=explode("/",$filename);
  $file=$t[count($t)-1];
  $t[count($t)-1]="";
  $path=implode("/",$t);
}

// Is it a non-empty and non-comment line ?
function valid_entry($r)
{
  if($r[0]!='#' && strlen(trim($r))>0)
    return true;
  else
    return false;
}

// unzip a given input
function gzdecode($in)
{
  $tmp="";
  raydium_file_home_path_cpy("tmp.tmp.gz",&$tmp);
  $fp=fopen($tmp,"wb");
  if(!$fp) 
    {
    echo "ERROR: Can't create '$tmp' file !";
    return false;
    }
  fwrite($fp,$in);
  fclose($fp);

  $fp=gzopen($tmp,"rb");
  if(!$fp) return false;
  while(!gzeof($fp))
    {
      $data.=gzread($fp,128);
    }
  gzclose($fp);
  unlink($tmp);
  return $data;
}

function ftp_upload($repos,$local,$distant)
{
  $url=parse_url($repos);

  $conn_id = ftp_connect($url["host"],$url["port"]);
  $login_result = ftp_login($conn_id, $url["user"], $url["pass"]);
  ftp_pasv($conn_id, true);
  
  if (ftp_put($conn_id,$url["path"].$distant, $local, FTP_ASCII))
    {
      echo "$repos: SUCCESS";
      ftp_close($conn_id);
      return true;
    }
  else
    {
      echo "Failed contacting $repos";
      ftp_close($conn_id);
      return false;
    }
}

function http_upload($repos,$local,$distant)
{
  // creating temporary files is a little crappy, but it works

  $data=@file_get_contents($local);
  $data=gzencode($data);

  $zfilename = $local . ".gz";
  $fp=fopen($zfilename,"wb");
  if(!$fp) 
    {
      print "FAILED: Cannot create zipped file $zfilename";
      return false;
    }
  fwrite($fp,$data);
  fclose($fp);


  $url=parse_url($repos);
  $user=rawurlencode($url["user"]);
  $pass=rawurlencode($url["pass"]);
  $path=$url["path"];
  $host=$url["host"];
  
  $post = Array();
  $post[ 'file' ] = $distant;
  $post[ 'type' ] = "putGzip";
  $post[ 'username' ] = $user;
  $post[ 'password' ] = $pass;
  $post[ 'data' ] = "@".$zfilename;

  $ch = curl_init();
  if (defined($GLOBALS['raydium_proxy']))
    curl_setopt ($ch, CURLOPT_PROXY, $GLOBALS['raydium_proxy']);
  curl_setopt($ch, CURLOPT_URL, "http://".$host.$path );
  curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
  curl_setopt($ch, CURLOPT_POST, 1 );
  // seems no need to tell it enctype='multipart/data' it already knows
  curl_setopt($ch, CURLOPT_POSTFIELDS, $post );
  $response = curl_exec( $ch ); 

  unlink($zfilename);

  if($response[0]=='+')
    {
      return true;
    }
  else
    {
      echo "HTTP reply: $response";
      return false;
    }
}

// supported files : (depends_xxx style)
function depends_tri($filename)
{
  $fp=fopen($filename,"rt");
  if($fp==false)
    {
      echo "Cannot open $filename";
      return array();
    }
  
  fgets($fp, 4096); // skip first line (version number)
  
  while (!feof($fp)) 
    {
      $line = trim(fgets($fp, 4096));
      $line=explode(" ",$line);
      $tex=trim($line[count($line)-1]);
      if(substr($tex,0,4)!="rgb(")
        {
          $tex=trim($tex);
          $ok=false;

          if(strpos($tex,';') && !$ok) // is multitextured with ";" ?
            {
            $ok=true;
            $texs=explode(";",$tex);
            if(strlen($texs[0])) $ret[]=$texs[0]; // yes, and it's a "detail texture"
            if(strlen($texs[1]))  // yes and it's a lightmap
                {
                if(strpos($texs[1],"|")!==false)
                    {
                    $lm=explode("|",$texs[1]);
                    $ret[]=$lm[2];
                    }
                else
                    $ret[]=$texs[1];
                }
            }

          if(strpos($tex,'#') && !$ok) // is an environment map ? (or texture slots for shaders)
            {
            $ok=true;
            $texs=explode("#",$tex);
            for($z=0;$z<count($texs);$z++)
                $ret[]=$texs[$z];
            }
            
          if(!$ok && strlen($tex))
            $ret[]=$tex;

          $ret=@array_values(array_unique($ret));
        }
    }
  fclose($fp);
  $ret[]=$filename;

  return $ret;
}

function depends_prt($filename)
{
  $f=@file($filename);
  for($i=0;$i<count($f);$i++)
    {
      $line=trim($f[$i]);
      if($line[0]=='/' && $line[1]=='/') continue;
    
      if(strpos($line,"texture") === false) continue;

      echo $line;
      $t=explode('=',$line);
      $t=trim($t[1]);
      $t=str_replace('"',"",$t);
      $t=str_replace(';',"",$t);
      $ret[]=$t;
    }

  $ret[]=$filename;
  return $ret;
}


function depends_find($filename)
{
  $tbl=explode(".",$filename);
  $ext=trim($tbl[count($tbl)-1]);
  if($ext=="tri") return depends_tri($filename);
  if($ext=="prt") return depends_prt($filename);

  // else ..
  $ret[]=$filename;
  return $ret;
}

/////////////////// Network section

// tests if the website is reacheable.
function http_test()
{
  $url="http://raydium.org/ping.php";
  $ch = curl_init();
  if (defined($GLOBALS['raydium_proxy']))
    curl_setopt ($ch, CURLOPT_PROXY, $GLOBALS['raydium_proxy']);

  curl_setopt ($ch, CURLOPT_URL, $url);
  curl_setopt ($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt ($ch, CURLOPT_FOLLOWLOCATION, 1);
  curl_setopt ($ch, CURLOPT_TIMEOUT, 3);

  $result = curl_exec ($ch);
  if (curl_errno($ch))
  {
    print "HTTP connection test failed: " . curl_error($ch);
    return false;
  }
  else
  {
  curl_close($ch);
  return true;
  }
}

?>
