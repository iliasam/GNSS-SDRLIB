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
 * Removed STEREO receiver support
 * Added "Simple 8B" receiver support (FX2LP based)
 * Added satellites state page (with acquisition, tracking and nav information). Freq. error is displayed for SBAS satellites.
 * Improved Galileo support (added some new check boxes, changed nav data extraction)
 * Improved displaying spectrum (now it is drawn with lines, not circles)
 * Acquisition threshold is set to 1.8 (comparing to 3.0 in original)
 * Acquisition process is started if tracking is lost
 * Added displaying number of tracking processing cycles in one second.

>> See "Releases" to get executable version for Windows.  
  
GUI interface after start:  
<img src="https://github.com/iliasam/GNSS-SDRLIB/blob/master/Images/p1.png">  
  
  
Spectrum:  
<img src="https://github.com/iliasam/GNSS-SDRLIB/blob/master/Images/p2.png">  

