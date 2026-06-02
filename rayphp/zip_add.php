<?php
// Add a file to the specified zip package

// params: $file_name (string, input), $file_full_path (string, input), $zip_file (string, input), $status (integer, output)

$status=0;

//echo getcwd();

$zip = new ZipArchive;

$res=$zip->open($zip_file,ZipArchive::CREATE);
if($res!==TRUE)
    die("packages: ERROR: Unable to create $zip_file from RayPHP");

if ($file_full_path=="RAYDIUM_PACKAGE_READONLY")
    {
    $stat=$zip->statName($file_full_path);
    if($stat==FALSE)
        $zip->addFromString($file_full_path,".");
    $zip->deleteName("RAYDIUM_PACKAGE_READWRITE");
    $zip->close();
    $status=1;
    return;
    }

if ($file_full_path=="RAYDIUM_PACKAGE_READWRITE")
    {
    $stat=$zip->statName($file_full_path);
    if($stat==FALSE)
        $zip->addFromString($file_full_path,".");
    $zip->deleteName("RAYDIUM_PACKAGE_READONLY");
    $zip->close();
    $status=1;
    return;
    }


if($status==0)
    {
    $stats=stat($file_full_path);
    $mtime_f=$stats["mtime"];
    $ltf=localtime($mtime_f,TRUE);
    if ($ltf["tm_isdst"]==1)
        $mtime_f=$mtime_f+mktime(0,0,0,1,1,1970,0);

    $statz=$zip->statName($file_name);
    $mtime_z=$statz["mtime"];
    $ltz=localtime($mtime_z,TRUE);

    //echo "file:".$mtime_f.":".$ltf["tm_isdst"].": === Zip:".$mtime_z.":".$ltz["tm_isdst"].": ";
    if($statz==FALSE)
        {
        $res=$zip->addFile($file_full_path,$file_name);
        echo "packages: ".$file_name." package: Added to '$zip_file'";
        if(!$res)
            die("packages: ERROR: Unable to add '$file_name' to '$zip_file'");
        }
    else if ($mtime_f > ($mtime_z+1)) // One second imprecision on zip file time.
        {
        $res=$zip->addFile($file_full_path,$file_name);
        echo "packages: ".$file_name." Refreshed into '$zip_file'";
        if(!$res)
            die("packages: ERROR: Unable to refresh '$file_name' to '$zip_file'");
        }
    else echo "packages: ".$file_name." Already in '$zip_file'";
    $zip->close();
    $status=1;
    }

?>