<?php

for ($loop=0; $loop<1000; $loop++) {

echo "loop " . $loop . "\n";

// Connecting, selecting database
//$link = mysql_connect('dbshards_host', 'root', '') or die('Could not connect: ' . mysql_error());
//mysql_select_db('testsuite') or die('Could not select database');

$link = mysql_connect('localhost', 'root', '') or die('Could not connect: ' . mysql_error());
mysql_select_db('dbs_junit_0001a') or die('Could not select database');

list($usec, $sec) = explode(' ', microtime());
$script_start = (float) $sec + (float) $usec;


for ($i=0; $i<1000; $i++) {
  //$result = mysql_query("SELECT 1");
  $result = mysql_query("SELECT * FROM country");
  //$result = mysql_query("SELECT id, name FROM country WHERE id = 1 AND name != 'whatever'");

  if (!$result) {
    die("Query failed: " . mysql_error() . " error number: " . mysql_errno());
  }

  $num_columns = mysql_num_fields ($result);
  $num_rows = mysql_num_rows($result);
  while ($row = mysql_fetch_row($result)) {
    echo $row[0] . "\n";
  }
  mysql_free_result($result);


}

list($usec, $sec) = explode(' ', microtime());
$script_end = (float) $sec + (float) $usec;

$elapsed_time = round($script_end - $script_start, 5);

echo "<p>" . $elapsed_time . "</p>";

mysql_close($link);

} // end of loop

echo "<p>Done.</p>";
?>

