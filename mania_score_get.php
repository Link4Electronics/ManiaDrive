<?php
// This file is used by ManiaDrive to get
// score from CQFD Corp website.

// Webservice WSDL URL
$url = "http://maniadrive.raydium.org/ecrire/tools/maniadrive/WSManiaDrive.php?wsdl";

// set the proxy according to the configuration database
$proxy = '';
raydium_parser_db_get("Generic-Proxy", &$proxy, "");
$proxy=parse_url($proxy);

try {
  // Create the client instance
  $client = new SoapClient($url, array('proxy_host' => $proxy["host"],
                                       'proxy_port' => (int) $proxy["port"],
                                       )
                           );
  // Call the SOAP method
  $result = $client->ScoreGetTop($track);

  // Display the result
  raydium_log("TOP : ".$result["name"]." => " . $result["score"]);
  
  $str=explode(":",$result["score"]);
  $score=$str[0]*3600 + $str[1]*60 + $str[2] + $str[3]/1000;
  $player = "{$result["name"]}";
}
catch (SoapFault $fault) {
  raydium_log("SOAP Fault: (faultcode: {$fault->faultcode}, faultstring: {$fault->faultstring})");
  $position = 0;
}

?>
