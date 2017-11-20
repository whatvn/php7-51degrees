# NOTE: 

The original php extension for **51Degrees** is now supported PHP7, please use that extension instead to have support directly from 51Degrees's people. ( [51Degrees](https://github.com/51Degrees/Device-Detection) ). 




# php7-51degrees

The origin php extension provided by 51Degrees foundation was built on top of swig, which is currently does not support PHP7.
This extension is experimental version, written as raw php-extension, because my last extension built with php-cpp has memory leak problem 


# BUILD


```
phpize
./configure
make
make test
make install 
```


# CONFIGURATION

I tried to keep the same configuration with old 51Degrees php extension, so the configuration file is the same (except extention name)


php.ini

```bash
extension=fiftyonedegrees.so
FiftyOneDegreesPatternV3.data_file=/data/51Degrees-LiteV3.2.dat
FiftyOneDegreesPatternV3.property_list=BrowserName,BrowserVersion,PlatformName,PlatformVersion,DeviceType,IsTablet,IsMobile,ScreenPixelsWidth,ScreenPixelsHeight
FiftyOneDegreesPatternV3.cache_size=20000
FiftyOneDegreesPatternV3.pool_size=25
```


# Usage



```php

<?php
$time_begin = microtime(true);
echo "round 1\n";
$d1 = fiftyonedegrees::getMatch();
$d1->setUserAgent("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.98 Safari/537.36");
for ($i = 0; $i < 1000; $i++) {
	echo $d1->getValue("PlatformName");
	echo "\n";
	echo $d1->getValue("DeviceType");
	echo "\n";
}

echo "round 2\n";

$d2 = new fiftyonedegrees();
$d2->setUserAgent("HTC_HD2_T8585 Opera/9.70 (Windows NT 5.1; U; de)");
for ($i = 0; $i < 1000; $i++) {
        echo $d2->getValue("PlatformName");
        echo "\n";
        echo $d2->getValue("DeviceType");
	echo "\n";
}
$time_end = microtime(true);
//echo $time_end - $time_begin;
echo microtime(true) -  $time_begin;
echo "\n";

``` 




# CREDITs:

- 51 Degrees libary: https://github.com/51Degrees/Device-Detection


# LICENSE

As always, do whatever you want ;)
I do this to learn thing only



