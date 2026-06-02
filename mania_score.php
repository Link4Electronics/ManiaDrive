<?php
// This file is used by ManiaDrive to upload
// scores to CQFD Corp website.

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
  $result = $client->ScoreAddNew($name, $track, $score, $version);

  // Display the result
  $result++;
  raydium_log('Debug : Level=' . $result);
  $position= (int) $result;
}
catch (SoapFault $fault) {
  raydium_log("SOAP Fault: (faultcode: {$fault->faultcode}, faultstring: {$fault->faultstring})");
  $position = 0;
}

?>
