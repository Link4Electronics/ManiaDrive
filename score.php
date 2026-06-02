<?
// this file is used by NewSkyDiver to upload
// scores to CQFD Corp website.
// See how it's easy to cheat ? ... No interest then ;)

// $wind / $breaked_bones / $fly_time / $touching_steps / $score / $version / $name

//die("score upload disabled in score.php !");

$nameE=rawurlencode($name);
$versionE=rawurlencode($version);

$url="http://skydiver.cqfd-corp.org/score/?wind=$wind&breaked_bones=$breaked_bones&fly_time=$fly_time&touching_steps=$touching_steps&score=$score&name=$nameE&version=$versionE";
$res=file($url);

if($res[0]=="OK")
    {
    echo "Score successfuly posted to NewSkyDiver website";
    echo "Visit http://skydiver.cqfd-corp.org/ to compare ;)";
    }
else raydium_log("^cScore posting failed ...."); // echo == raydium_log

?>