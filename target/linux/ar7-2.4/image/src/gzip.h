//bvb#include "timemmap.h"

#define OF(args)  args
#define STATIC static

#define WSIZE 0x8000		/* Slideing window size (defined as var
				 * "window" below) must be at least 32k, 
				 * and a power of two.  This is the
				 * data work window used for input buffer
				 * by the input routine */

typedef unsigned char  uch;
typedef unsigned short ush;
typedef unsigned long  ulg;


static char *output_data;
static ulg output_ptr;

#ifndef NULL
#define NULL 0
#endif

#define NOMEMCPY                /* Does routine memcpy exist? */

//bvb static uch *inbuf;		/* input buffer */
static uch *window;

//bvb static uch outwin[WSIZE];

//bvb static unsigned insize;		/* valid bytes in inbuf */
static unsigned inptr;		/* index of next byte to process in inbuf */

static unsigned outcnt;		/* bytes in output buffer */

/* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define CONTINUATION 0x02 /* bit 1 set: continuation of multi-part gzip file */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define ENCRYPTED    0x20 /* bit 5 set: file is encrypted */
#define RESERVED     0xC0 /* bit 6,7:   reserved */

/* If BMAX needs to be larger than 16, then h and x[] should be ulg. */
#define BMAX 16         /* maximum bit length of any code (16 for explode) */
#define N_MAX 288       /* maximum number of codes in any set */

static char *input_data;

static void *freememstart;
