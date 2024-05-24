/*------------------------------------------------------------------------------
* hackrf.c : HackRF functions
*
* Copyright (C) 2024 ILIASAM
*-----------------------------------------------------------------------------*/
#include "../../../src/sdr.h"

#include "hackrf/hackrf.h"
#include "pthread/pthread.h"

#include <chrono>
#include <time.h>

hackrf_device *dev = NULL;

static uint32_t hackrf_read_buf_size = 0;
static pthread_mutex_t m_u_mutex;

int hackrf_sdr_initconf(void);


//******************************************************************************


int hackrf_rx_callback(hackrf_transfer * transfer)
{
	if (sdrstat.stopflag)
		return 0;

	int new_bytes_cnt = transfer->valid_length;
	uint8_t *buf_p = transfer->buffer;
	
	mlock(hbuffmtx);
	//Copy buf_p -> sdrstat.buff
	memcpy(&sdrstat.buff[(sdrstat.buffcnt % MEMBUFFLEN) * HACKRF_DATABUFF_SIZE],
		buf_p, HACKRF_DATABUFF_SIZE);
	unmlock(hbuffmtx);

	mlock(hreadmtx);
	sdrstat.buffcnt++;
	unmlock(hreadmtx);

	/*
	total_cnt += new_bytes_cnt;
	if (sdrstat.buffcnt % 200 == 0)
	{
		using namespace std::chrono;
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		auto nanosec = t1.time_since_epoch();
		auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(nanosec);
		long long msec = milliseconds.count();

		SDRPRINTF("TEST %lu time %lu \n", total_cnt, msec);
	}
	*/
	
	return 0;
}

/* hackrf initialization -------------------------------------------------------
* search front end and initialization
* args   : none
* return : int                  status 0:okay -1:failure
*-----------------------------------------------------------------------------*/
extern int hackrf_sdr_init(void) 
{
    int dev_index=0;
	pthread_mutex_init(&m_u_mutex, 0);

	/* open hackrf */
	int res = hackrf_init();
	if (res != HACKRF_SUCCESS)
	{
		SDRPRINTF("hackrf_init() failed.");
		return -1;
	}

	res = hackrf_open(&dev);
	if (res != HACKRF_SUCCESS)
	{
		SDRPRINTF("Failed to open hackrf device. %s\n", hackrf_error_name((hackrf_error)res));
		return -1;
	}

    /* set configuration */
	res = hackrf_sdr_initconf();
    if (res < 0) 
	{
        SDRPRINTF("error: failed to initialize hackrf %s\n", hackrf_error_name((hackrf_error)res));
        return -1;
    }

    return 0;
}

extern int hackrf_sdr_start(void)
{
	int res = 0;
	pthread_mutex_lock(&m_u_mutex);
	res = hackrf_start_rx(dev, hackrf_rx_callback, NULL);
	if (res != HACKRF_SUCCESS)
	{
		SDRPRINTF("hackrf_start_rx() failed: %s (%d)\n",
			hackrf_error_name((hackrf_error)res), res);
		pthread_mutex_unlock(&m_u_mutex);
		return res;
	}
	pthread_mutex_unlock(&m_u_mutex);
	return 0;
}



/* stop front-end --------------------------------------------------------------
* stop grabber of front end
* args   : none
* return : none
*-----------------------------------------------------------------------------*/
extern void hackrf_sdr_quit(void) 
{
	pthread_mutex_lock(&m_u_mutex);

	int result;
	result = hackrf_stop_rx(dev);
	if (result != HACKRF_SUCCESS)
	{
		SDRPRINTF("hackrf_stop_rx() failed: %s (%d)\n",
			hackrf_error_name((hackrf_error)result), result);
		pthread_mutex_unlock(&m_u_mutex);
		exit(1);
	}
	pthread_mutex_unlock(&m_u_mutex);

	hackrf_close(dev);
	pthread_mutex_destroy(&m_u_mutex);
}


/* hackrf configuration function -----------------------------------------------
* load configuration file and setting
* args   : none
* return : int                  status 0:okay -1:failure
*-----------------------------------------------------------------------------*/
extern int hackrf_sdr_initconf(void) 
{
    int res;

	unsigned int samp_rate_hz = (unsigned int)sdrini.f_sf[0];
	double center_freq_hz = (double)sdrini.f_cf[0];

	// Set the sample rate 
	res = hackrf_set_sample_rate(dev, samp_rate_hz);
	if (res != HACKRF_SUCCESS)
	{
		SDRPRINTF("ERROR: Failed to set sample rate: %lu\n", samp_rate_hz);
		return res;
	}

	res = hackrf_set_freq(dev, (uint64_t)center_freq_hz);
	if (res != HACKRF_SUCCESS)
	{
		SDRPRINTF("ERROR: Failed to set center frequency: %llu\n", (uint64_t)center_freq_hz);
		return res;
	}

	uint8_t amp_gain_on = 0;
	uint16_t lna_gain = 40;
	uint16_t vga_gain = 40;

	lna_gain = (((lna_gain + 7) / 8) * 8);
	vga_gain = (((vga_gain + 1) / 2) * 2);

	if (lna_gain > 40)
		lna_gain = 40;
	if (vga_gain > 62)
		vga_gain = 62;

	res = 0;
	if (amp_gain_on)
		res = hackrf_set_amp_enable(dev, amp_gain_on);
	if (vga_gain)
		res |= hackrf_set_vga_gain(dev, vga_gain);
	if (lna_gain)
		res |= hackrf_set_lna_gain(dev, lna_gain);
	if (res != 0)
	{
		SDRPRINTF("ERROR: Can't set gains\n");
		return -1;
	}

	res = hackrf_set_antenna_enable(dev, 1);
	if (res != HACKRF_SUCCESS)
	{
		SDRPRINTF("ERROR: Failed to enable bias tee, result: %li\n", res);
		return res;
	}

	/*
	res = hackrf_set_baseband_filter_bandwidth(dev, 2e6);
	if (res != HACKRF_SUCCESS)
	{
		printf("hackrf_baseband_filter_bandwidth_set() failed: \n");
		return -1;
	}
	*/

    /*
    // set ppm offset 
    ret=verbose_ppm_set(dev,sdrini.hackrfppmerr);
    if (ret<0) {
        SDRPRINTF("error: failed to set ppm\n");
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
extern void hackrf_sdr_exp(uint8_t *buf, int n, char *expbuf)
{
	memcpy(expbuf, buf, n);
}
/* get current data from buffer -----------------------------------------------------
* get current data buffer from memory buffer
* args   : uint64_t buffloc I   buffer location, in samples
*          int    n         I   number of  data - number of needed samples
*          char   *expbuf   O   extracted data buffer
* return : none
*-----------------------------------------------------------------------------*/
extern void hackrf_sdr_getbuff(uint64_t buffloc, int n_samples, char *expbuf)
{
	uint64_t buf_length_bytes = MEMBUFFLEN * hackrf_read_buf_size;

	uint64_t membuffloc_bytes = (buffloc * IQ_COEF) % buf_length_bytes;
	int n_bytes = IQ_COEF * n_samples;
	int nout_bytes = (int)((membuffloc_bytes + n_bytes) - buf_length_bytes);

    mlock(hbuffmtx);
    if (nout_bytes > 0)
    {
        hackrf_sdr_exp(&sdrstat.buff[membuffloc_bytes], n_bytes - nout_bytes, expbuf);
        hackrf_sdr_exp(&sdrstat.buff[0], nout_bytes, &expbuf[n_bytes - nout_bytes]);
    } 
    else 
    {
        hackrf_sdr_exp(&sdrstat.buff[membuffloc_bytes], n_bytes, expbuf);
    }
    unmlock(hbuffmtx);
}


extern void hackrf_sdr_set_rx_buf(uint32_t buf_size)
{
	hackrf_read_buf_size = buf_size;
}
