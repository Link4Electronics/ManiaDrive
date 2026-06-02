<?php
// gets file from repositories listed in repositories.list

// params: $filename (string, input), $force (integer, input),
//         $status (integer, output)

// This script must be placed in "rayphp" sub-directory.

require("libfile.php");

$status=0;

filename_cut($filename,$file,$path);
echo "Using repositories to get '$file' file";
$repos=read_repositories_file("repositories.list");

// foreach repository
for($i=0;$i<count($repos);$i++)
{
  $r = $repos[$i];
  if(valid_entry($r))
    {
      if(update_file($file,$r,$path.$file,$force))
        {
          $status=1;
          return;
        }
    }
}
// If we get here, something went wrong
die("No valid repository found for this file, aborting.");
?>
