/*
 * Hackrf driver, ILIASAM 2024
 */

#ifndef __HACKRF_H
#define __HACKRF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
//#include "rtl-sdr_export.h"

/*-GNSS-SDRLIB----------------------------------------------------------------*/
#include "../../../src/sdr.h"
    
#define HACKRF_SAMPLE_RATE      8e6
#define HACKRF_FREQUENCY        1575420000
#define HACKRF_ASYNC_BUF_NUMBER 32
#define HACKRF_DATABUFF_SIZE    16384

#define RTLSDR_FILE_READ_RATIO	2
extern int hackrf_init(void);
extern void hackrf_quit(void);
extern int hackrf_initconf(void);
extern int hackrf_start(void);
extern void hackrf_exp(uint8_t *buf, int n, char *expbuf);
extern void hackrf_getbuff(uint64_t buffloc, int n, char *expbuf);
extern void fhackrf_pushtomembuf(void);
extern void hackrf_set_rx_buf(uint32_t buf_size);
/*----------------------------------------------------------------------------*/

typedef struct hackrf_dev hackrf_dev_t;


#ifdef __cplusplus
}
#endif

#endif /* __HACKRF_H */
