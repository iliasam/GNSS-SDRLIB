/*------------------------------------------------------------------------------
* hackrf.c : HackRF functions
*
* Copyright (C) 2024 ILIASAM
*-----------------------------------------------------------------------------*/
#include "../../../src/sdr.h"

static hackrf_dev_t *dev=NULL;

static uint32_t hackrf_read_buf_size = 0;

/* hackrf stream callback  -----------------------------------------------------
* callback for receiving RF data
*-----------------------------------------------------------------------------*/
void stream_callback_hackrf(unsigned char *buf, uint32_t len, void *ctx)
{
    /* copy stream data to global buffer */
    mlock(hbuffmtx);
    memcpy(&sdrstat.buff[(sdrstat.buffcnt%MEMBUFFLEN)*2*HACKRF_DATABUFF_SIZE],
        buf,2*HACKRF_DATABUFF_SIZE);
    unmlock(hbuffmtx);

    mlock(hreadmtx);
    sdrstat.buffcnt++;
    unmlock(hreadmtx);

   // if (sdrstat.stopflag) 
    //    hackrf_cancel_async(dev);
}
/* hackrf initialization -------------------------------------------------------
* search front end and initialization
* args   : none
* return : int                  status 0:okay -1:failure
*-----------------------------------------------------------------------------*/
extern int hackrf_init(void) 
{
    int ret,dev_index=0;;

    /* open hackrf */
    //dev_index = verbose_device_search("0");
    //ret = hackrf_open(&dev, (uint32_t)dev_index);
    ret = -1;
    if (ret<0) {
        SDRPRINTF("error: failed to open hackrf device #%d.\n",dev_index);
        return -1;
    }

    /* set configuration */
    //ret=hackrf_initconf();
    if (ret<0) {
        SDRPRINTF("error: failed to initialize hackrf\n");
        return -1;
    }

    return 0;
}
/* stop front-end --------------------------------------------------------------
* stop grabber of front end
* args   : none
* return : none
*-----------------------------------------------------------------------------*/
extern void hackrf_quit(void) 
{
    //hackrf_cancel_async(dev);
    //hackrf_close(dev);
}
/* hackrf configuration function -----------------------------------------------
* load configuration file and setting
* args   : none
* return : int                  status 0:okay -1:failure
*-----------------------------------------------------------------------------*/
extern int hackrf_initconf(void) 
{
    int ret;
    /*
    
    // Set the sample rate 
    ret=verbose_set_sample_rate(dev,RTLSDR_SAMPLE_RATE);
    if (ret<0) {
        SDRPRINTF("error: failed to set samplerate\n");
        return -1;
    }

    // Set the frequency
    ret=verbose_set_frequency(dev,RTLSDR_FREQUENCY);
    if (ret<0) {
        SDRPRINTF("error: failed to set frequency\n");
        return -1;
    }

    // Enable automatic gain 
    ret=verbose_auto_gain(dev);
    if (ret<0) {
        SDRPRINTF("error: failed to set automatic gain\n");
        return -1;
    }

    // set ppm offset 
    ret=verbose_ppm_set(dev,sdrini.hackrfppmerr);
    if (ret<0) {
        SDRPRINTF("error: failed to set ppm\n");
        return -1;
    }
    */

    return 0;
}
/* start grabber ---------------------------------------------------------------
* start grabber of front end
* args   : none
* return : int                  status 0:okay -1:failure
*-----------------------------------------------------------------------------*/
extern int hackrf_start(void) 
{
    int ret;
    
    /*
    // reset endpoint before we start reading from it (mandatory)
    ret=verbose_reset_buffer(dev);
    if (ret<0) {
        SDRPRINTF("error: failed to reset buffers\n");
        return -1;
    }

    // start stream and stay there until we kill the stream 
    ret=hackrf_read_async(dev,stream_callback_hackrf,
        NULL,RTLSDR_ASYNC_BUF_NUMBER,2*hackrf_read_buf_size);

    if (ret<0&&!sdrstat.stopflag) {
        SDRPRINTF("error: failed to read in async mode\n");
        return -1;
    }
    */

    return 0;
}
/* data expansion --------------------------------------------------------------
* get current data buffer from memory buffer
* args   : int16_t *buf     I   bladeRF raw buffer
*          int    n         I   number of grab data
*          char   *expbuf   O   extracted data buffer
* return : none
*-----------------------------------------------------------------------------*/
extern void hackrf_exp(uint8_t *buf, int n, char *expbuf)
{
    int i;

    for (i=0;i<n;i++) 
    {
        expbuf[i]=(char)(buf[i]);
    }
}
/* get current data buffer -----------------------------------------------------
* get current data buffer from memory buffer
* args   : uint64_t buffloc I   buffer location
*          int    n         I   number of grab data
*          char   *expbuf   O   extracted data buffer
* return : none
*-----------------------------------------------------------------------------*/
extern void hackrf_getbuff(uint64_t buffloc, int n, char *expbuf)
{
    uint64_t membuffloc=2*buffloc%(MEMBUFFLEN*2* hackrf_read_buf_size);
    int nout;
    n=2*n;
    nout=(int)((membuffloc+n)-(MEMBUFFLEN*2* hackrf_read_buf_size));

    mlock(hbuffmtx);
    if (nout>0) 
    {
        hackrf_exp(&sdrstat.buff[membuffloc],n-nout,expbuf);
        hackrf_exp(&sdrstat.buff[0],nout,&expbuf[n-nout]);
    } 
    else 
    {
        hackrf_exp(&sdrstat.buff[membuffloc],n,expbuf);
    }
    unmlock(hbuffmtx);
}


extern void hackrf_set_rx_buf(uint32_t buf_size)
{
	hackrf_read_buf_size = buf_size;
}
