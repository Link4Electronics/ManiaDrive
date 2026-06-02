<?php
// lists files from the first valid repository

// params: $filter (string, input), $size (integer, input)
//         $list (string, output), $status (integer, output)

// This script must be placed in "rayphp" sub-directory.

require("libfile.php");

$status=0; // sets status to "error", by default
$repos=read_repositories_file("repositories.list");
$list="";

for($i=0;$i<count($repos);$i++)
{
  $r = $repos[$i];
  if(valid_entry($r)) 
    {
      if( ($data=list_repos($filter,$size,$r)) )
        {
          if(strlen($data)+1>=$size)
            {
              echo "$r : Not enough memory";
            }
          else
            {
              $list="$data";
              $status=1;
              exit(0);
            }
        }
    }
}
// Something is wrong
die("No valid repository found for listing, abording.");
?>
