<?php
// gets file from repositories listed in repositories.list

// params: $filename (string, input), $depends (integer, input)
//         $status (integer, output)

// This script must be placed in "rayphp" sub-directory.

require("libfile.php");

$status=0; // sets status to "error", by default

//$filename="buggy.tri"; // used when debugging outside of Raydium

$repos=read_repositories_file("repositories.upload");

if($depends)
{
  $deps = depends_find($filename);
}
else
{
  $deps[] = $filename;
}

// for each file to upload
for($j=0;$j<count($deps);$j++)
{
  filename_cut($deps[$j],$file,$path);

  if(!file_exists($path.$file) || !is_readable($path.$file))
    {
      echo "Cannot upload '$path$file': file do not exist or invalid rights";
      continue;
    }

  echo "Using repositories to upload '$file' file...";
  
  // for each repository
  for($i=0;$i<count($repos);$i++)
    {
      $r = $repos[$i];
      if(valid_entry($r))
        {
          // http or ftp ?
          $type=parse_url($r);
          $type=$type["scheme"];
          
          if(($type=="ftp" ||
              $type=="ftps" )
             && ftp_upload($r,$path.$file,$file))
            {
              touch($path.$file);
              $status++;
              break;
            }
          
          if(($type=="http" ||
              $type=="https" )
             && http_upload($r,$path.$file,$file))
            {
              touch($path.$file);
              $status++;
              break;
            }   
        }
    }
}

if($status==count($deps))
{
  echo "All files uploaded ($status)";
  return;
}    

if($status==0)
{
  echo "No file uploaded";
  return;
}    

echo "Only $status/".count($deps)." file(s) uploaded";

?>
