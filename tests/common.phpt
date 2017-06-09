--TEST--
fiftyonedegrees()
--SKIPIF--
<?php if(!extension_loaded("fiftyonedegrees")) print "skip"; ?>
--INI--
FiftyOneDegreesPatternV3.data_file=/data/51Degrees-LiteV3.2.dat
--FILE--
<?php
$macOs = fiftyonedegrees::getMatch();
$macOs->setUserAgent("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.98 Safari/537.36");
echo $macOs->getValue("PlatformName"), "\n";
echo $macOs->getValue("IsMobile"), "\n";

$windowsMobile = new fiftyonedegrees();
$windowsMobile->setUserAgent("HTC_HD2_T8585 Opera/9.70 (Windows NT 5.1; U; de)");
echo $windowsMobile->getValue("PlatformName"), "\n";
echo $windowsMobile->getValue("IsMobile"), "\n";
?>
--EXPECT--
Mac OS X
False
Windows Mobile
True
