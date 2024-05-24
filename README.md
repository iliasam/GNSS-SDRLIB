GNSS-SDRLIB
===============================================================================  

An Open Source GNSS Software Defined Radio Library

This is a fork of "GNSS-SDRLIB", original is here: https://github.com/taroz/GNSS-SDRLIB  

Author of original "GNSS-SDRLIB" is:
Taro Suzuki  
E-Mail: <gnsssdrlib@gmail.com>
HP: <http://www.taroz.net>

===========================================================   

This fork has such changes:  
 * Ported to VS 2017
 * **Added HackRF support**
 * Removed STEREO receiver support
 * Added "Simple 8B" receiver support (FX2LP based)
 * Added satellites state page (with acquisition, tracking and nav information)
 * Added single satellite state window with IQ-constellation plot. Double click at the needed sat. in list to open this window
 * Improved Galileo support (added more satellites, changed nav data extraction)
 * Added more Beidou satellites
 * Improved displaying spectrum (now it is drawn with lines, not circles)
 * Improved displaying tracking information (not simple ABS value is draw)
 * Acquisition threshold can be set by user
 * Acquisition process is started if tracking is lost
 * Added displaying number of tracking processing cycles in one second.

>> See "Releases" to get executable version for Windows.  
  
GUI interface after start:  
<img src="https://github.com/iliasam/GNSS-SDRLIB/blob/master/Images/p1.png">  
  
  
Spectrum:  
<img src="https://github.com/iliasam/GNSS-SDRLIB/blob/master/Images/p2.png">  
  
GUI interface befor start:  
<img src="https://github.com/iliasam/GNSS-SDRLIB/blob/master/Images/p3.png">  

