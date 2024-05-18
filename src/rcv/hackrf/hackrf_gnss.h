/*
 * Hackrf driver, ILIASAM 2024
 */

#ifndef __HACKRF_GNSS_H
#define __HACKRF_GNSS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "hackrf/hackrf.h"

/*-GNSS-SDRLIB----------------------------------------------------------------*/
#include "../../../src/sdr.h"
    
#define HACKRF_DATABUFF_SIZE    262144 //see hackrf.c TRANSFER_BUFFER_SIZE

//Number of bytes to store one sample (I+Q=2)
#define IQ_COEF		2

#define RTLSDR_FILE_READ_RATIO	2
extern int hackrf_sdr_init(void);
extern void hackrf_sdr_quit(void);
extern int hackrf_sdr_start(void);
extern void hackrf_sdr_exp(uint8_t *buf, int n, char *expbuf);
extern void hackrf_sdr_getbuff(uint64_t buffloc, int n, char *expbuf);
//extern void fhackrf_pushtomembuf(void);
extern void hackrf_sdr_set_rx_buf(uint32_t buf_size);
/*----------------------------------------------------------------------------*/

typedef struct hackrf_dev hackrf_dev_t;


#ifdef __cplusplus
}
#endif

#endif /* __HACKRF_GNSS_H */
