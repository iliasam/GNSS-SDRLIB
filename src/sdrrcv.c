/*------------------------------------------------------------------------------
* sdrrcv.c : SDR receiver functions
*
* Copyright (C) 2014 Taro Suzuki <gnsssdrlib@gmail.com>
*-----------------------------------------------------------------------------*/
#include "sdr.h"

/* sdr receiver initialization -------------------------------------------------
* receiver initialization, memory allocation, file open
* args   : sdrini_t *ini    I   sdr initialization struct
* return : int                  status 0:okay -1:failure
*-----------------------------------------------------------------------------*/

extern int rcvinit(sdrini_t *ini)
{
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(1,0),&wsaData);
#endif

    /* FFT initialization */
    fftwf_init_threads();

    sdrstat.buff=sdrstat.buff2=NULL;

    switch (ini->fend) 
	{
#ifdef GN3S
    /* SiGe GN3S v2/v3 */
    case FEND_GN3SV2: 
    case FEND_GN3SV3: 
        if (gn3s_init()<0) return -1; /* GN3S initialization */

		if (ini->fend == FEND_GN3SV2)
		{
			sdrstat.fendbuffsize = GN3S_BUFFSIZE / 2; /* frontend buff size */
			fgn3s_set_rx_buf(sdrstat.fendbuffsize);
			sdrstat.buffsize = sdrstat.fendbuffsize * 2 * MEMBUFFLEN; /* total */
		}
		if (ini->fend == FEND_GN3SV3)
		{
			sdrstat.fendbuffsize = GN3S_BUFFSIZE; /* frontend buff size */
			fgn3s_set_rx_buf(sdrstat.fendbuffsize);
			sdrstat.buffsize = sdrstat.fendbuffsize * MEMBUFFLEN; /* total */
		}

        /* memory allocation */
		// This buffer is used to store raw data from USB
        sdrstat.buff=(uint8_t*)malloc(sdrstat.buffsize);
        if (NULL==sdrstat.buff) {
            SDRPRINTF("error: failed to allocate memory for the buffer\n");
            return -1;
        }
        break;
    /* GN3S Binary File */
    case FEND_FGN3SV2:
    case FEND_FGN3SV3:
        /* IF file open */
        if ((ini->fp1 = fopen(ini->file1,"rb"))==NULL){
            SDRPRINTF("error: failed to open file : %s\n",ini->file1);
            return -1;
        }
        
		if (ini->fend == FEND_FGN3SV2)
		{
			sdrstat.fendbuffsize = GN3S_BUFFSIZE / 2; /* frontend buff size */
			sdrstat.fendbuffsize = sdrstat.fendbuffsize * GN3S_FILE_READ_RATIO;//increase to make more realtime
			fgn3s_set_rx_buf(sdrstat.fendbuffsize);
			sdrstat.buffsize = sdrstat.fendbuffsize * 2 * MEMBUFFLEN; /* total */
		}
		if (ini->fend == FEND_FGN3SV3)
		{
			sdrstat.fendbuffsize = GN3S_BUFFSIZE; /* frontend buff size */
			sdrstat.fendbuffsize = sdrstat.fendbuffsize * GN3S_FILE_READ_RATIO;//increase to make more realtime
			fgn3s_set_rx_buf(sdrstat.fendbuffsize);
			sdrstat.buffsize = sdrstat.fendbuffsize * MEMBUFFLEN; /* total */
		}

        /* memory allocation */
        sdrstat.buff=(uint8_t*)malloc(sdrstat.buffsize);
        if (NULL==sdrstat.buff) {
            SDRPRINTF("error: failed to allocate memory for the buffer\n");
            return -1;
        }
		SDRPRINTF("Data is taken from file, reading is not real-time\n");
        break;
#endif
#ifdef BLADERF
    /* Nuand bladeRF */
    case FEND_BLADERF:
        if (bladerf_init()<0) return -1; /* bladeRF initialization */

        sdrstat.fendbuffsize=BLADERF_DATABUFF_SIZE; /* frontend buff size */
        sdrstat.buffsize=2*BLADERF_DATABUFF_SIZE*MEMBUFFLEN; /* total */

        /* memory allocation */
        sdrstat.buff=(uint8_t*)malloc(sdrstat.buffsize);
        if (NULL==sdrstat.buff) {
            SDRPRINTF("error: failed to allocate memory for the buffer\n");
            return -1;
        }
        break;
    /* BladeRF Binary File */
    case FEND_FBLADERF:
        /* IF file open */
        if ((ini->fp1 = fopen(ini->file1,"rb"))==NULL){
            SDRPRINTF("error: failed to open file : %s\n",ini->file1);
            return -1;
        }

        sdrstat.fendbuffsize=BLADERF_DATABUFF_SIZE; /* frontend buff size */
        sdrstat.buffsize=2*BLADERF_DATABUFF_SIZE*MEMBUFFLEN; /* total */

        /* memory allocation */
        sdrstat.buff=(uint8_t*)malloc(sdrstat.buffsize);
        if (NULL==sdrstat.buff) {
            SDRPRINTF("error: failed to allocate memory for the buffer\n");
            return -1;
        }
		SDRPRINTF("Data is taken from file, reading is not real-time\n");
        break;
#endif
#ifdef RTLSDR
    /* RTL-SDR */
    case FEND_RTLSDR:
        if (rtlsdr_init()<0) return -1; /* rtlsdr initialization */

        /* frontend buffer size */
        sdrstat.fendbuffsize=RTLSDR_DATABUFF_SIZE; /* frontend buff size */
		rtlsdr_set_rx_buf(sdrstat.fendbuffsize);
        sdrstat.buffsize=2 * RTLSDR_DATABUFF_SIZE * MEMBUFFLEN; /* total */

        /* memory allocation */
        sdrstat.buff=(uint8_t*)malloc(sdrstat.buffsize);
        if (NULL==sdrstat.buff) {
            SDRPRINTF("error: failed to allocate memory for the buffer\n");
            return -1;
        }
        break;
    /* RTL-SDR Binary File */
    case FEND_FRTLSDR:
        /* IF file open */
        if ((ini->fp1 = fopen(ini->file1,"rb"))==NULL){
            SDRPRINTF("error: failed to open file : %s\n",ini->file1);
            return -1;
        }

        /* frontend buffer size */
        sdrstat.fendbuffsize=RTLSDR_DATABUFF_SIZE * RTLSDR_FILE_READ_RATIO; /* frontend buff size */
		rtlsdr_set_rx_buf(sdrstat.fendbuffsize);
        sdrstat.buffsize= 2 * sdrstat.fendbuffsize * MEMBUFFLEN; /* total */
		
        /* memory allocation */
        sdrstat.buff=(uint8_t*)malloc(sdrstat.buffsize);
        if (NULL==sdrstat.buff)
		{
            SDRPRINTF("error: failed to allocate memory for the buffer\n");
			return -1;
        }
		SDRPRINTF("Data is taken from file, reading is not real-time\n");
        break;
#endif

#ifdef HACKRF
		/* HACKRF */
	case FEND_HACKRF:
		if (hackrf_sdr_init() < 0)
			return -1; /* hackrf initialization */

		/* frontend buffer size */
		sdrstat.fendbuffsize = HACKRF_DATABUFF_SIZE / IQ_COEF; /* frontend buff size */
		hackrf_sdr_set_rx_buf(HACKRF_DATABUFF_SIZE);
		sdrstat.buffsize = HACKRF_DATABUFF_SIZE * MEMBUFFLEN; /* total */

		/* memory allocation */
		sdrstat.buff = (uint8_t*)malloc(sdrstat.buffsize);
		if (NULL == sdrstat.buff) 
		{
			SDRPRINTF("error: failed to allocate memory for the buffer\n");
			return -1;
		}

		if (hackrf_sdr_start() < 0)
		{
			SDRPRINTF("error: Can't start hackrf RX\n");
			return -1;
		}
		break;
#endif

	case FEND_SIMPLE8B:
		if (simple_rf_init() < 0) 
			return -1;

		if (ini->fend == FEND_SIMPLE8B)
		{
			sdrstat.fendbuffsize = SIMPLE_RF_BUFFSIZE; /* frontend buff size */
			simple_rf_set_rx_buf(sdrstat.fendbuffsize);
			sdrstat.buffsize = sdrstat.fendbuffsize * MEMBUFFLEN; /* total */
		}

		/* memory allocation */
		sdrstat.buff = (uint8_t*)malloc(sdrstat.buffsize);
		if (NULL == sdrstat.buff) {
			SDRPRINTF("error: failed to allocate memory for the buffer\n");
			return -1;
		}
		break;

    /* File */
    case FEND_FILE:
        /* IF file open (FILE1) */
        if ((ini->fp1 = fopen(ini->file1,"rb"))==NULL){
            SDRPRINTF("error: failed to open file(FILE1): %s\n",ini->file1);
            return -1;
        }
        /* IF file open (FILE2) */
        if (strlen(ini->file2)!=0) {
            if ((ini->fp2 = fopen(ini->file2,"rb"))==NULL){
                SDRPRINTF("error: failed to open file(FILE2): %s\n",ini->file2);
                return -1;
            }
        }
        /* frontend buffer size */
        sdrstat.fendbuffsize=FILE_BUFFSIZE; /* frontend buff size */
        sdrstat.buffsize=FILE_BUFFSIZE*MEMBUFFLEN; /* total */

        /* memory allocation */
        if (ini->fp1!=NULL) {
            sdrstat.buff=(uint8_t*)malloc(ini->dtype[0]*sdrstat.buffsize);
            if (NULL==sdrstat.buff) {
                SDRPRINTF("error: failed to allocate memory for the buffer\n");
                return -1;
            }
        }
        if (ini->fp2!=NULL) {
            sdrstat.buff2=(uint8_t*)malloc(ini->dtype[1]*sdrstat.buffsize);
            if (NULL==sdrstat.buff2) {
                SDRPRINTF("error: failed to allocate memory for the buffer\n");
                return -1;
            }
        }
		SDRPRINTF("Data is taken from file, reading is not real-time\n");
        break;
    default:
        return -1;
    }
    return 0;
}
/* stop front-end --------------------------------------------------------------
* stop grabber of front end
* args   : sdrini_t *ini    I   sdr initialization struct
* return : int                  status 0:okay -1:failure
*-----------------------------------------------------------------------------*/
extern int rcvquit(sdrini_t *ini)
{
    switch (ini->fend) {
#ifdef GN3S
    /* SiGe GN3S v2/v3 */
    case FEND_GN3SV2:
    case FEND_GN3SV3:
        gn3s_quit();
        break;
#endif
#ifdef BLADERF
    /* Nuand bladeRF */
    case FEND_BLADERF:
        bladerf_quit();
        break;
#endif
#ifdef RTLSDR
    /* RTL-SDR */
    case FEND_RTLSDR:
        rtlsdr_quit();
        break;
#endif

#ifdef HACKRF
		/* HACKRF */
	case FEND_HACKRF:
		hackrf_sdr_quit();
		break;
#endif
	case FEND_SIMPLE8B:
		simple_rf_quit();
		break;
    /* Front End Binary File */
    case FEND_FGN3SV2:
    case FEND_FGN3SV3:
    case FEND_FBLADERF:
    case FEND_FRTLSDR:
        if (ini->fp1!=NULL) fclose(ini->fp1); ini->fp1=NULL;
        break;
    /* File */
    case FEND_FILE:
        if (ini->fp1!=NULL) fclose(ini->fp1); ini->fp1=NULL;
        if (ini->fp2!=NULL) fclose(ini->fp2); ini->fp2=NULL;
        break;
    default:
        return -1;
    }
    /* free memory */
    if (NULL!=sdrstat.buff)    free(sdrstat.buff);    sdrstat.buff=NULL;
    if (NULL!=sdrstat.buff2)   free(sdrstat.buff2);   sdrstat.buff2=NULL;
    return 0;
}

/* grab current data -----------------------------------------------------------
* push data to memory buffer from front end
* args   : sdrini_t *ini    I   sdr initialization struct
* return : int                  status 0:okay -1:failure
*-----------------------------------------------------------------------------*/
extern int rcvgrabdata(sdrini_t *ini)
{
    unsigned long buffcnt=0;

    switch (ini->fend) 
	{
#ifdef GN3S
    /* SiGe GN3S v2/v3 */
    case FEND_GN3SV2:
    case FEND_GN3SV3:
        if (gn3s_pushtomembuf()<0) {
            SDRPRINTF("error: GN3S Buffer overrun...\n");
            return -1;
        }
        break;
    /* GN3S Binary File */
    case FEND_FGN3SV2:
    case FEND_FGN3SV3: 
        fgn3s_pushtomembuf(); /* copy to membuffer */
        sleepms(5);
        break;
#endif
	case FEND_SIMPLE8B:
		if (simple_rf_pushtomembuf() < 0) {
			SDRPRINTF("error: Simple Frontend Buffer overrun...\n");
			return -1;
		}
		break;
#ifdef BLADERF
    /* Nuand BladeRF */
    case FEND_BLADERF:
        if (bladerf_start()<0) {
            SDRPRINTF("error: bladeRF...\n");
            return -1;
        }
        break;
    /* BladeRF Binary File */
    case FEND_FBLADERF: 
        fbladerf_pushtomembuf(); /* copy to membuffer */
        sleepms(5);
        break;
#endif
#ifdef RTLSDR
    /* RTL-SDR */
    case FEND_RTLSDR:
        if (rtlsdr_start()<0) {
            SDRPRINTF("error: rtlsdr...\n");
            return -1;
        }
        break;
    /* RTL-SDR Binary File */
    case FEND_FRTLSDR: 
        frtlsdr_pushtomembuf(); /* copy to membuffer */
        sleepms(5);
        break;/* File */
#endif

#ifdef HACKRF
		/* HACKRF */
	case FEND_HACKRF:
		/*
		if (hackrf_sdr_start() < 0) {
			SDRPRINTF("error: hackrf...\n");
			return -1;
		}
		*/
		break;
#endif
    case FEND_FILE:
        file_pushtomembuf(); /* copy to membuffer */
        sleepms(5);
        break;
    default:
        return -1;
    }
    return 0;
}
/* grab current buffer ---------------------------------------------------------
* get current data buffer from memory buffer
* args   : sdrini_t *ini    I   sdr initialization struct
*          uint64_t buffloc I   buffer location, samples
*          int    n         I   number of SAMPLES of data data to get
*          int    ftype     I   front end type (FTYPE1 or FTYPE2)
*          int    dtype     I   data type (DTYPEI or DTYPEIQ)
*          char   *expbuff  O   extracted data buffer
* return : int                  status 0:okay -1:failure
*-----------------------------------------------------------------------------*/
extern int rcvgetbuff(sdrini_t *ini, uint64_t buffloc, int n_samples, int ftype,
                      int dtype, char *expbuf)
{
	if (n_samples < 0)
		return -1;

    switch (ini->fend) 
	{
#ifdef GN3S
    /* SiGe GN3S v2 */
    case FEND_GN3SV2:
        gn3s_getbuff_v2(buffloc, n_samples,dtype,expbuf);
        break;
    /* SiGe GN3S v3 */
    case FEND_GN3SV3:
        gn3s_getbuff_v3(buffloc, n_samples,dtype,expbuf);
        break;
    /* GN3Sv2/v3 Binary File */
    case FEND_FGN3SV2: 
    case FEND_FGN3SV3: 
        fgn3s_getbuff(buffloc, n_samples,dtype,expbuf);
        break;
#endif
	case FEND_SIMPLE8B:
		simple_rf_getbuf(buffloc, n_samples, dtype, expbuf);
		break;
#ifdef BLADERF
    /* Nuand BladeRF */
    case FEND_BLADERF:
        bladerf_getbuff(buffloc, n_samples,expbuf);
        break;    
    /* BladeRF Binary File */
    case FEND_FBLADERF: 
        bladerf_getbuff(buffloc, n_samples,expbuf);
        break;
#endif
#ifdef RTLSDR
    /* RTL-SDR */
    case FEND_RTLSDR:
        rtlsdr_getbuff(buffloc, n_samples,expbuf);
        break;    
    /* RTL-SDR Binary File */
    case FEND_FRTLSDR: 
        rtlsdr_getbuff(buffloc, n_samples,expbuf);
        break;
#endif

#ifdef HACKRF
		/* HACKRF */
	case FEND_HACKRF:
		hackrf_sdr_getbuff(buffloc, n_samples, expbuf);
		break;
#endif
    /* File */
    case FEND_FILE:
        file_getbuff(buffloc, n_samples, ftype, dtype, expbuf);
        break;
    default:
        return -1;
    }
    return 0;
}
/* push data to memory buffer --------------------------------------------------
* post-processing function: push data to memory buffer
* args   : none
* return : none
*-----------------------------------------------------------------------------*/
extern void file_pushtomembuf(void) 
{
    size_t nread1=0,nread2=0;

    mlock(hbuffmtx);
    if(sdrini.fp1!=NULL) {
        nread1=fread(&sdrstat.buff[(sdrstat.buffcnt%MEMBUFFLEN)*
            sdrini.dtype[0]*FILE_BUFFSIZE],1,sdrini.dtype[0]*FILE_BUFFSIZE,
            sdrini.fp1);
    }
    if(sdrini.fp2!=NULL) {
        nread2=fread(&sdrstat.buff2[(sdrstat.buffcnt%MEMBUFFLEN)*
            sdrini.dtype[1]*FILE_BUFFSIZE],1,sdrini.dtype[1]*FILE_BUFFSIZE,
            sdrini.fp2);
    }
    unmlock(hbuffmtx);

    if ((sdrini.fp1!=NULL&&(int)nread1<sdrini.dtype[0]*FILE_BUFFSIZE)||
        (sdrini.fp2!=NULL&&(int)nread2<sdrini.dtype[1]*FILE_BUFFSIZE)) {
        sdrstat.stopflag=ON;
        SDRPRINTF("end of file!\n");
    }

    mlock(hreadmtx);
    sdrstat.buffcnt++;
    unmlock(hreadmtx);
}
/* get current data buffer from IF file ----------------------------------------
* post-processing function: get current data buffer from memory buffer
* args   : uint64_t buffloc I   buffer location
*          int    n         I   number of grab data 
*          int    ftype     I   front end type (FTYPE1 or FTYPE2)
*          int    dtype     I   data type (DTYPEI or DTYPEIQ)
*          char   *expbuff  O   extracted data buffer
* return : none
*-----------------------------------------------------------------------------*/
extern void file_getbuff(uint64_t buffloc, int n, int ftype, int dtype, 
                         char *expbuf)
{
    uint64_t membuffloc=dtype*buffloc%(MEMBUFFLEN*dtype*FILE_BUFFSIZE);
    int nout;

    n=dtype*n;
    nout=(int)((membuffloc+n)-(MEMBUFFLEN*dtype*FILE_BUFFSIZE));

    mlock(hbuffmtx);
    if (ftype==FTYPE1) {
        if (nout>0) {
            memcpy(expbuf,&sdrstat.buff[membuffloc],n-nout);
            memcpy(&expbuf[(n-nout)],&sdrstat.buff[0],nout);
        } else {
            memcpy(expbuf,&sdrstat.buff[membuffloc],n);
        }
    }
    if (ftype==FTYPE2) {
        if (nout>0) {
            memcpy(expbuf,&sdrstat.buff2[membuffloc],n-nout);
            memcpy(&expbuf[(n-nout)],&sdrstat.buff2[0],nout);
        } else {
            memcpy(expbuf,&sdrstat.buff2[membuffloc],n);
        }
    }
    unmlock(hbuffmtx);
}
