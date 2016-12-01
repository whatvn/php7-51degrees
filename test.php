<?php
require('DeviceDetection.class.php');
$time_begin = microtime(true);
echo "round 1\n";
$d1 = DeviceDetection::getDeviceInfo("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.98 Safari/537.36");
for ($i = 0; $i < 1000; $i++) {
	echo $d1->getPlatformName();;
	echo "\n";
	echo $d1->getDeviceType();;
	echo "\n";
}

echo "round 2\n";

$d2 = DeviceDetection::getDeviceInfo("HTC_HD2_T8585 Opera/9.70 (Windows NT 5.1; U; de)");
for ($i = 0; $i < 1000; $i++) {
        echo $d2->getPlatformName();;
        echo "\n";
        echo $d2->getDeviceType();;
	echo "\n";
}
$time_end = microtime(true);
//echo $time_end - $time_begin;
echo microtime(true) -  $time_begin;
echo "\n";


