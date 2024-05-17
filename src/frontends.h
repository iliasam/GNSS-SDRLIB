#ifndef FRONTENDS_H
#define FRONTENDS_H

/* front end setting */
#define FEND_STEREO   0                /* front end type: NSL STEREO */
#define FEND_GN3SV2   1                /* front end type: SiGe GN3S v2 */
#define FEND_GN3SV3   2                /* front end type: SiGe GN3S v3 */
#define FEND_RTLSDR   3                /* front end type: RTL-SDR */
#define FEND_BLADERF  4                /* front end type: Nuand BladeRF */
#define FEND_SIMPLE8B 5                /* front end type: Simple 8bit */
#define FEND_HACKRF	  6                /* front end type: HackRF */
//FILES
#define FEND_FSTEREO  7                /* front end type: STEREO binary file */
#define FEND_FGN3SV2  8                /* front end type: GN3Sv2 binary file */
#define FEND_FGN3SV3  9                /* front end type: GN3Sv3 binary file */
#define FEND_FRTLSDR  10                /* front end type: RTL-SDR binary file */
#define FEND_FBLADERF 11                /* front end type: BladeRF binary file */
#define FEND_FILE     12               /* front end type: IF file */

#endif //FRONTENDS_H