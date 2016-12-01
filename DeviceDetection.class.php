<?php
//require('Constant.class.php');
class DeviceDetection {
    private $userAgent = "";
    private $deviceType = 1; 
    private $isMobile = false;
    private $isTablet = false;
    private $isPC = false;
    private $platformName = "";
    private $platformVersion = "";
    private $browserName = "";
    private $browserVersion = "";
    
    private static $provider_instance = null;

    // get device information
    static function getDeviceInfo($userAgent=null) {
        // if no user-Agent, get from request header
        if ($userAgent==null) {
            $userAgent = $_SERVER['HTTP_USER_AGENT'];
        }
        
        // check if provider not created
        if (self::$provider_instance==null) {
            self::$provider_instance = new fiftyonedegrees(); 
        }
        $match = self::$provider_instance->get_match($userAgent);

        // create new object
        $obj = new DeviceDetection();
        $obj->userAgent = $userAgent;
        $obj->isMobile = (strtolower($match->get_value("IsMobile"))=="true");
        $obj->platformName = $match->get_value("PlatformName");
        $obj->platformVersion = $match->get_value("PlatformVersion");
        $obj->browserName = $match->get_value("BrowserName");
        $obj->browserVersion = $match->get_value("BrowserVersion");
//      $obj->screenPixelsWidth = $match->getValue("ScreenPixelsWidth");
//      $obj->screenPixelsHeight = $match->getValue("ScreenPixelsHeight");
        $obj->isTablet = self::checkTablet($userAgent);
        $obj->isPC = (!$obj->isMobile && !$obj->isTablet);
        if ($obj->isTablet) {
            $obj->deviceType = 2; 
        } elseif ($obj->isMobile) {
            $obj->deviceType = 3; 
        } else {
            $obj->deviceType = $deviceType; 
        }
        return $obj;
    }

    // tablet-device detection
    static function checkTablet($sUserAgent) {
        if (empty($sUserAgent)) return false;
        $userAgent = strtolower($sUserAgent);

        $bNotContainMobile = ( strpos($userAgent, "mobile")===false );
        $bAndroid = ( strpos($userAgent, "android")!==false );
        $biPad = ( strpos($userAgent, "ipad")!==false );
        $bKindle = ( strpos($userAgent, "silk")!==false );

        // Android special case
        if ($bAndroid && $bNotContainMobile) {
            return true;
        }
        // Apple special case
        if ($biPad) {
            return true;
        }
        // Kindle Fire special case
        if ($bKindle && $bNotContainMobile) {
            return true;
        }
        // others
        if ( preg_match("/(ipad|playbook|hp-tablet|kindle)/i", $userAgent) ) {
            return true;
        }
        return false;
    }
    
    function getUserAgent() {
        return $this->userAgent;
    }

    function getDeviceType() {
        return $this->deviceType;
    }

    function isMobile() {
        return $this->isMobile;
    }

    function isTablet() {
        return $this->isTablet;
    }

    function isPC() {
        return $this->isPC;
    }

    function getPlatformName() {
        return $this->platformName;
    }

    function getPlatformVersion() {
        return $this->platformVersion;
    }

    function getBrowserName() {
        return $this->browserName;
    }

    function getBrowserVersion() {
        return $this->browserVersion;
    }

    static function getProviderInstance() {
        return self::$provider_instance;
    }
    
}
?>
