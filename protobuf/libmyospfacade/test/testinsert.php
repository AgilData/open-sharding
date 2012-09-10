<?php

// Connecting, selecting database
$link = mysql_connect('dbshards_host', 'root', '') or die('Could not connect: ' . mysql_error());
mysql_select_db('testsuite') or die('Could not select database');

//$link = mysql_connect('localhost', 'root', '') or die('Could not connect: ' . mysql_error());
//mysql_select_db('dbs_junit_0001a') or die('Could not select database');

list($usec, $sec) = explode(' ', microtime());
$script_start = (float) $sec + (float) $usec;


for ($i=0; $i<10000; $i++) {
#mysql_query("INSERT INTO country (name) values ('Test')");
mysql_query("INSERT INTO country_sharded (name) values ('Test')");
}

list($usec, $sec) = explode(' ', microtime());
$script_end = (float) $sec + (float) $usec;

$elapsed_time = round($script_end - $script_start, 5);

echo "<p>" . $elapsed_time . "</p>";

mysql_close($link);
echo "<p>Done.</p>";
?>

