--TEST--
fiftyonedegrees()
--SKIPIF--
<?php if(!extension_loaded("fiftyonedegrees")) print "skip"; ?>
--INI--
FiftyOneDegreesPatternV3.data_file=/data/51Degrees-LiteV3.2.dat
--FILE--
<?php
$deviceDetection = new fiftyonedegrees();
$macOs = $deviceDetection->get_match("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.98 Safari/537.36");

echo $macOs->get_value("PlatformName"), "\n";
echo $macOs->get_value("IsMobile"), "\n";
$windowsMobile = $deviceDetection->get_match("HTC_HD2_T8585 Opera/9.70 (Windows NT 5.1; U; de)");
echo $windowsMobile->get_value("PlatformName"), "\n";
echo $windowsMobile->get_value("IsMobile"), "\n";
?>
--EXPECT--
Mac OS X
False
Windows Mobile
True
