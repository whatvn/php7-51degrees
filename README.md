# php7-51degrees

The origin php extension provided by 51Degrees foundation was built on top of swig, which is currently does not support PHP7.
This extension is experimental version, written as raw php-extension, because php-cpp has memory leak problem 


# BUILD




```
phpize
./configure
make
make install 
```


# CONFIGURATION

I tried to keep the same configuration with old 51Degrees php extension, so the configuration file is the same (except extention name)


php.ini

```
extension=fiftydegrees.so
FiftyOneDegreesPatternV3.data_file=/data/51Degrees-LiteV3.2.dat
FiftyOneDegreesPatternV3.property_list=BrowserName,BrowserVersion,PlatformName,PlatformVersion,DeviceType,IsTablet,IsMobile,ScreenPixelsWidth,ScreenPixelsHeight
FiftyOneDegreesPatternV3.cache_size=20000
FiftyOneDegreesPatternV3.pool_size=25
```


# Usage

here is example: 


```php}
<?php

$time_begin = microtime(true);
$my = new fiftydegrees();


for ($i = 0; $i < 1000000; $i++) {
        $m = $my->get_match("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.98 Safari/537.36");
        $m->get_value("PlatformName");
}
echo microtime(true) - $time_begin;

``` 




# CREDITs:

- 51 Degrees libary: https://github.com/51Degrees/Device-Detection


# LICENSE

As always, do whatever you want ;)
I do this to learn thing only



