/* inflate.c -- Not copyrighted 1992 by Mark Adler
   version c10p1, 10 January 1993 */

/*
 * Adapted for booting Linux by Hannu Savolainen 1993
 * based on gzip-1.0.3
 *
 * Nicolas Pitre <nico@visuaide.com>, 1999/04/14 :
 *   Little mods for all variable to reside either into rodata or bss segments
 *   by marking constant variables with 'const' and initializing all the others
 *   at run-time only.  This allows for the kernel uncompressor to run
 *   directly from Flash or ROM memory on embeded systems.
 */

#include <linux/config.h>
#include "gzip.h"
#include "LzmaDecode.h"

/* Function prototypes */
unsigned char get_byte(void);
int tikernelunzip(int,char *[], char *[]);
static int tidecompress(uch *, uch *);

void kernel_entry(int, char *[], char *[]);
void (*ke)(int, char *[], char *[]); /* Gen reference to kernel function */
void (*prnt)(unsigned int, char *);		/* Gen reference to Yamon print function */
void printf(char *ptr);			/* Generate our own printf */

int tikernelunzip(int argc, char *argv[], char *arge[])
{
	extern unsigned int _ftext;
	extern uch kernelimage[];
	uch *in, *out;
	int status;

	printf("Launching kernel decompressor.\n");

	out = (unsigned char *) LOADADDR;
	in = &(kernelimage[0]);

	status = tidecompress(in, out);

	if (status == 0) {
		printf("Kernel decompressor was successful ... launching kernel.\n");

		ke = ( void(*)(int, char *[],char*[]))kernel_entry;
		(*ke)(argc,argv,arge);

		return (0);
	} else {
		printf("Error in decompression.\n");
		return(1);
	}
}

#if 0
char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
void print_i(int i)
{
	int j;
	char buf[11];

	buf[0] = '0';
	buf[1] = 'x';
	buf[10] = 0;
	
	for (j = 0; j < 8; j++)
	{
		buf[2 + 7 - j] = hex[i & 0xf];
		i = i >> 4;
	}

	printf(buf);
}
#endif

int tidecompress(uch *indata, uch *outdata)
{
	extern unsigned int workspace;
	extern unsigned char kernelimage[], kernelimage_end[];
	unsigned int i;  /* temp value */
	unsigned int lc; /* literal context bits */
	unsigned int lp; /* literal pos state bits */
	unsigned int pb; /* pos state bits */
	unsigned int osize; /* uncompressed size */
	unsigned int wsize; /* window size */
	unsigned int insize = kernelimage_end - kernelimage;
	int status;
	
	output_ptr = 0;
	output_data = outdata;
	input_data = indata;

	/* lzma args */
	i = get_byte();
	lc = i % 9, i = i / 9;
	lp = i % 5, pb = i / 5;

	/* skip rest of the LZMA coder property */
	for (i = 0; i < 4; i++)
		get_byte();
	
	/* read the lower half of uncompressed size in the header */
	osize = ((unsigned int)get_byte()) +
		((unsigned int)get_byte() << 8) +
		((unsigned int)get_byte() << 16) +
		((unsigned int)get_byte() << 24);

	/* skip rest of the header (upper half of uncompressed size) */
	for (i = 0; i < 4; i++)
		get_byte();
	
	i = 0;
	wsize = (LZMA_BASE_SIZE + (LZMA_LIT_SIZE << (lc + lp))) * sizeof(CProb);

	if ((status = LzmaDecode((unsigned char *) &workspace, wsize, lc, lp, pb,
		indata + 13, insize - 13, (unsigned char *) output_data, osize, &i)) == LZMA_RESULT_OK)
			return 0;

	return status;
}


void printf(char *ptr)
{
	unsigned int *tempptr = (unsigned int  *)0x90000534;
	prnt = ( void (*)(unsigned int, char *)) *tempptr;
	(*prnt)(0,ptr);
}

unsigned char get_byte()
{
	unsigned char c;
	
	c = *input_data;
	input_data++;

	return c;
}

