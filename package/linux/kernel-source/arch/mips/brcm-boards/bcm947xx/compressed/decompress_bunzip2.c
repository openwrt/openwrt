/* vi: set sw=4 ts=4: */
/*	Small bzip2 deflate implementation, by Rob Landley (rob@landley.net).

	Based on bzip2 decompression code by Julian R Seward (jseward@acm.org),
	which also acknowledges contributions by Mike Burrows, David Wheeler,
	Peter Fenwick, Alistair Moffat, Radford Neal, Ian H. Witten,
	Robert Sedgewick, and Jon L. Bentley.

	This code is licensed under the LGPLv2:
		LGPL (http://www.gnu.org/copyleft/lgpl.html
*/

/*
	Size and speed optimizations by Manuel Novoa III  (mjn3@codepoet.org).

	More efficient reading of huffman codes, a streamlined read_bunzip()
	function, and various other tweaks.  In (limited) tests, approximately
	20% faster than bzcat on x86 and about 10% faster on arm.

	Note that about 2/3 of the time is spent in read_unzip() reversing
	the Burrows-Wheeler transformation.  Much of that time is delay
	resulting from cache misses.

	I would ask that anyone benefiting from this work, especially those
	using it in commercial products, consider making a donation to my local
	non-profit hospice organization in the name of the woman I loved, who
	passed away Feb. 12, 2003.

		In memory of Toni W. Hagan

		Hospice of Acadiana, Inc.
		2600 Johnston St., Suite 200
		Lafayette, LA 70503-3240

		Phone (337) 232-1234 or 1-800-738-2226
		Fax   (337) 232-1297

		http://www.hospiceacadiana.com/

	Manuel
 */

/* May 21, 2004        Manuel Novoa III
 * Modified to load a bzip'd kernel on the linksys wrt54g.
 *
 * May 30, 2004
 * Further size reduction via inlining and disabling len check code.
 */

/**********************************************************************/

/* Note... the LED code is specific to the v2.0 (and GS?) unit. */
#undef  ENABLE_LEDS
/* #define ENABLE_LEDS  1 */

/* Do we want to bother with checking the bzip'd data for errors? */
#undef  ENABLE_BUNZIP_CHECKING
/* #define ENABLE_BUNZIP_CHECKING  1 */

/**********************************************************************/
/* #include <bcm4710.h> */
#define BCM4710_FLASH		0x1fc00000	/* Flash */

#define KSEG0                   0x80000000
#define KSEG1                   0xa0000000

#define KSEG1ADDR(a)            ((((unsigned)(a)) & 0x1fffffffU) | KSEG1)

/* The following cache code was taken from the file bcm4710_cache.h
 * which was necessarily GPL as it was used to build the linksys
 * kernel for the wrt54g. */

#warning icache [l]size hardcoded

#define icache_size				8192
#define ic_lsize				  16

#define Index_Invalidate_I		0x00

#define cache_unroll(base,op)	\
	__asm__ __volatile__(		\
		".set noreorder;\n"		\
		".set mips3;\n"			\
		"cache %1, (%0);\n"		\
		".set mips0;\n"			\
		".set reorder\n"		\
		:						\
		: "r" (base),			\
		  "i" (op));

static __inline__ void blast_icache(void)
{
	unsigned long start = KSEG0;
	unsigned long end = (start + icache_size);

	while(start < end) {
		cache_unroll(start,Index_Invalidate_I);
		start += ic_lsize;
	}
}

/**********************************************************************/
#ifndef INT_MAX
#define INT_MAX (((1 << 30)-1)*2 + 1)
#endif
/**********************************************************************/
#ifdef ENABLE_BUNZIP_CHECKING

#define REBOOT		do {} while (1)

#else

#define REBOOT		((void) 0)

#endif
/**********************************************************************/
#ifdef ENABLE_LEDS

#define LED_POWER_ON		0x02 /* OFF == flashing */
#define LED_DMZ_OFF			0x80
#define LED_WLAN_OFF		0x01

#define LED_CODE_0			(LED_POWER_ON | LED_DMZ_OFF | LED_WLAN_OFF)
#define LED_CODE_1			(LED_POWER_ON | LED_DMZ_OFF)
#define LED_CODE_2			(LED_POWER_ON               | LED_WLAN_OFF)
#define LED_CODE_3			(LED_POWER_ON)

#define SET_LED_ERROR(X) \
	do { \
	*(volatile u8*)(KSEG1ADDR(0x18000064))=(X & ~LED_POWER_ON); \
	*(volatile u8*)(KSEG1ADDR(0x18000068))=0; /* Disable changes */ \
	REBOOT; \
	} while (0)

#define SET_LED(X)	*(volatile u8*)(KSEG1ADDR(0x18000064))=X;


typedef unsigned char u8;

#else

#define SET_LED_ERROR(X)		REBOOT
#define SET_LED(X)				((void)0)

#endif

/**********************************************************************/

/* Constants for huffman coding */
#define MAX_GROUPS			6
#define GROUP_SIZE   		50		/* 64 would have been more efficient */
#define MAX_HUFCODE_BITS 	20		/* Longest huffman code allowed */
#define MAX_SYMBOLS 		258		/* 256 literals + RUNA + RUNB */
#define SYMBOL_RUNA			0
#define SYMBOL_RUNB			1

/* Status return values */
#define RETVAL_OK						0
#define RETVAL_LAST_BLOCK				(-1)
#define RETVAL_NOT_BZIP_DATA			(-2)
#define RETVAL_UNEXPECTED_INPUT_EOF		(-3)
#define RETVAL_UNEXPECTED_OUTPUT_EOF	(-4)
#define RETVAL_DATA_ERROR				(-5)
#define RETVAL_OUT_OF_MEMORY			(-6)
#define RETVAL_OBSOLETE_INPUT			(-7)

/* Other housekeeping constants */
#define IOBUF_SIZE			4096

/* This is what we know about each huffman coding group */
struct group_data {
	/* We have an extra slot at the end of limit[] for a sentinal value. */
	int limit[MAX_HUFCODE_BITS+1],base[MAX_HUFCODE_BITS],permute[MAX_SYMBOLS];
	int minLen, maxLen;
};

/* Structure holding all the housekeeping data, including IO buffers and
   memory that persists between calls to bunzip */
typedef struct {
	/* State for interrupting output loop */
	int writeCopies,writePos,writeRunCountdown,writeCount,writeCurrent;
	/* I/O tracking data (file handles, buffers, positions, etc.) */
#if defined(ENABLE_BUNZIP_CHECKING)
	int /*in_fd,out_fd,*/ inbufCount,inbufPos /*,outbufPos*/;
#else
	int /*in_fd,out_fd,inbufCount,*/ inbufPos /*,outbufPos*/;
#endif
	unsigned char *inbuf /*,*outbuf*/;
	unsigned int inbufBitCount, inbufBits;
	/* The CRC values stored in the block header and calculated from the data */
#ifdef ENABLE_BUNZIP_CHECKING
	unsigned int crc32Table[256],headerCRC, totalCRC, writeCRC;
	/* Intermediate buffer and its size (in bytes) */
	unsigned int *dbuf, dbufSize;
#else
	unsigned int *dbuf;
#endif
	/* These things are a bit too big to go on the stack */
	unsigned char selectors[32768];			/* nSelectors=15 bits */
	struct group_data groups[MAX_GROUPS];	/* huffman coding tables */
} bunzip_data;

static int get_next_block(bunzip_data *bd);

/**********************************************************************/
/* Undo burrows-wheeler transform on intermediate buffer to produce output.
   If start_bunzip was initialized with out_fd=-1, then up to len bytes of
   data are written to outbuf.  Return value is number of bytes written or
   error (all errors are negative numbers).  If out_fd!=-1, outbuf and len
   are ignored, data is written to out_fd and return is RETVAL_OK or error.
*/

static __inline__ int read_bunzip(bunzip_data *bd, char *outbuf, int len)
{
	const unsigned int *dbuf;
	int pos,current,previous,gotcount;
#ifdef ENABLE_LEDS
	int led_state = LED_CODE_2;
#endif

	/* If last read was short due to end of file, return last block now */
	if(bd->writeCount<0) return bd->writeCount;

	gotcount = 0;
	dbuf=bd->dbuf;
	pos=bd->writePos;
	current=bd->writeCurrent;

	/* We will always have pending decoded data to write into the output
	   buffer unless this is the very first call (in which case we haven't
	   huffman-decoded a block into the intermediate buffer yet). */

	if (bd->writeCopies) {
		/* Inside the loop, writeCopies means extra copies (beyond 1) */
		--bd->writeCopies;
		/* Loop outputting bytes */
		for(;;) {
#if 0		/* Might want to enable this if passing a limiting size. */
/* #ifdef ENABLE_BUNZIP_CHECKING */
			/* If the output buffer is full, snapshot state and return */
			if(gotcount >= len) {
				bd->writePos=pos;
				bd->writeCurrent=current;
				bd->writeCopies++;
				return len;
			}
#endif
			/* Write next byte into output buffer, updating CRC */
			outbuf[gotcount++] = current;
#ifdef ENABLE_BUNZIP_CHECKING
			bd->writeCRC=(((bd->writeCRC)<<8)
						  ^bd->crc32Table[((bd->writeCRC)>>24)^current]);
#endif
			/* Loop now if we're outputting multiple copies of this byte */
			if (bd->writeCopies) {
				--bd->writeCopies;
				continue;
			}
decode_next_byte:
			if (!bd->writeCount--) break;
			/* Follow sequence vector to undo Burrows-Wheeler transform */
			previous=current;
			pos=dbuf[pos];
			current=pos&0xff;
			pos>>=8;
			/* After 3 consecutive copies of the same byte, the 4th is a repeat
			   count.  We count down from 4 instead
			 * of counting up because testing for non-zero is faster */
			if(--bd->writeRunCountdown) {
				if(current!=previous) bd->writeRunCountdown=4;
			} else {
				/* We have a repeated run, this byte indicates the count */
				bd->writeCopies=current;
				current=previous;
				bd->writeRunCountdown=5;
				/* Sometimes there are just 3 bytes (run length 0) */
				if(!bd->writeCopies) goto decode_next_byte;
				/* Subtract the 1 copy we'd output anyway to get extras */
				--bd->writeCopies;
			}
		}
#ifdef ENABLE_BUNZIP_CHECKING
		/* Decompression of this block completed successfully */
		bd->writeCRC=~bd->writeCRC;
		bd->totalCRC=((bd->totalCRC<<1) | (bd->totalCRC>>31)) ^ bd->writeCRC;
		/* If this block had a CRC error, force file level CRC error. */
		if(bd->writeCRC!=bd->headerCRC) {
			bd->totalCRC=bd->headerCRC+1;
			return RETVAL_LAST_BLOCK;
		}
#endif
	}

#ifdef ENABLE_LEDS
	if (led_state == LED_CODE_2) {
		led_state = LED_CODE_1;
	} else {
		led_state = LED_CODE_2;
	}
	SET_LED(led_state);
#endif

	/* Refill the intermediate buffer by huffman-decoding next block of input */
	/* (previous is just a convenient unused temp variable here) */
	previous=get_next_block(bd);
#ifdef ENABLE_BUNZIP_CHECKING
	if(previous) {
		bd->writeCount=previous;
		return (previous!=RETVAL_LAST_BLOCK) ? previous : gotcount;
	}
	bd->writeCRC=0xffffffffUL;
#else
	if (previous) return gotcount;
#endif
	pos=bd->writePos;
	current=bd->writeCurrent;
	goto decode_next_byte;
}

/**********************************************************************/
/* WARNING!!! Must be the first function!!! */

void load_and_run(unsigned long ra)
{
	int dbuf[900000];	/* Maximum requred */
	bunzip_data bd;

	unsigned int i;
#ifdef ENABLE_BUNZIP_CHECKING
	unsigned int j, c;
	int r;
#endif
	char *p;

#ifdef ENABLE_LEDS
	*(volatile u8*)(KSEG1ADDR(0x18000068))=0x83; /* Allow all bits to change */
	SET_LED(LED_CODE_0);
#endif

/* 	memset(&bd,0,sizeof(bunzip_data)); */
	p = (char *) &bd;
	for (i = 0 ; i < sizeof(bunzip_data) ; i++) {
		p[i] = 0;
	}

	/* Find start of flash and adjust for pmon partition. */
	p = ((char *) KSEG1ADDR(BCM4710_FLASH)) + 0x10000;

	SET_LED(LED_CODE_1);
	/* Find the start of the bzip'd data. */
	while ((p[0]!='B') || (p[1]!='Z') || (p[2]!='h') /*|| (p[3]!='9')*/) ++p;
	SET_LED(LED_CODE_2);

	/* Setup input buffer */
	bd.inbuf=p+4;		/* Skip the "BZh9" header. */
#ifdef ENABLE_BUNZIP_CHECKING
	bd.inbufCount=INT_MAX;
	/* Init the CRC32 table (big endian) */
	for(i=0;i<256;i++) {
		c=i<<24;
		for(j=8;j;j--)
			c=c&0x80000000 ? (c<<1)^0x04c11db7 : (c<<1);
		bd.crc32Table[i]=c;
	}

	bd.dbufSize=900000;
#endif
	bd.dbuf=dbuf;

	/* Actually do the bunzip */
#ifdef ENABLE_BUNZIP_CHECKING
	r = read_bunzip(&bd, ((char *) LOADADDR), INT_MAX);
	if (r > 0) {
		if (bd.headerCRC==bd.totalCRC) {
			SET_LED(LED_CODE_3);
			{
				int code = LED_WLAN_OFF;
				int i, j;
				for (j=0 ; j < 4 ; j++) {
					for (i=0; i<(1<<27) ; i++) {}
					SET_LED(code);
					code ^= LED_WLAN_OFF;
				}
			}
			blast_icache();
			/* Jump to load address */
			((void (*)(void)) LOADADDR)();
		} else {
			SET_LED_ERROR(LED_CODE_3);
		}
	} else {
		SET_LED_ERROR(LED_CODE_2);
	}
#else
	read_bunzip(&bd, ((char *) LOADADDR), INT_MAX);
	blast_icache();
	/* Jump to load address */
	((void (*)(void)) LOADADDR)();
#endif
}

/**********************************************************************/
/* Return the next nnn bits of input.  All reads from the compressed input
   are done through this function.  All reads are big endian */
static unsigned int get_bits(bunzip_data *bd, char bits_wanted)
{
	unsigned int bits=0;

	/* If we need to get more data from the byte buffer, do so.  (Loop getting
	   one byte at a time to enforce endianness and avoid unaligned access.) */
	while (bd->inbufBitCount<bits_wanted) {
		/* If we need to read more data from file into byte buffer, do so */
#ifdef ENABLE_BUNZIP_CHECKING
		if(bd->inbufPos==bd->inbufCount) {
			SET_LED_ERROR(LED_CODE_0);
		}
#endif
		/* Avoid 32-bit overflow (dump bit buffer to top of output) */
		if(bd->inbufBitCount>=24) {
			bits=bd->inbufBits&((1<<bd->inbufBitCount)-1);
			bits_wanted-=bd->inbufBitCount;
			bits<<=bits_wanted;
			bd->inbufBitCount=0;
		}
		/* Grab next 8 bits of input from buffer. */
		bd->inbufBits=(bd->inbufBits<<8)|bd->inbuf[bd->inbufPos++];
		bd->inbufBitCount+=8;
	}
	/* Calculate result */
	bd->inbufBitCount-=bits_wanted;
	bits|=(bd->inbufBits>>bd->inbufBitCount)&((1<<bits_wanted)-1);

	return bits;
}

/* Unpacks the next block and sets up for the inverse burrows-wheeler step. */

static int get_next_block(bunzip_data *bd)
{
	struct group_data *hufGroup;
#ifdef ENABLE_BUNZIP_CHECKING
	int dbufCount,nextSym,dbufSize,groupCount,*base,*limit,selector,
		i,j,k,t,runPos,symCount,symTotal,nSelectors,byteCount[256];
#else
	int dbufCount,nextSym,/*dbufSize,*/groupCount,*base,*limit,selector,
		i,j,k,t,runPos,symCount,symTotal,nSelectors,byteCount[256];
#endif
	unsigned char uc, symToByte[256], mtfSymbol[256], *selectors;
	unsigned int *dbuf,origPtr;

	dbuf=bd->dbuf;
#ifdef ENABLE_BUNZIP_CHECKING
	dbufSize=bd->dbufSize;
#endif
	selectors=bd->selectors;
	/* Read in header signature and CRC, then validate signature.
	   (last block signature means CRC is for whole file, return now) */
	i = get_bits(bd,24);
	j = get_bits(bd,24);
#ifdef ENABLE_BUNZIP_CHECKING
	bd->headerCRC=get_bits(bd,32);
	if ((i == 0x177245) && (j == 0x385090)) return RETVAL_LAST_BLOCK;
	if ((i != 0x314159) || (j != 0x265359)) return RETVAL_NOT_BZIP_DATA;
	/* We can add support for blockRandomised if anybody complains.  There was
	   some code for this in busybox 1.0.0-pre3, but nobody ever noticed that
	   it didn't actually work. */
	if(get_bits(bd,1)) return RETVAL_OBSOLETE_INPUT;
	if((origPtr=get_bits(bd,24)) > dbufSize) return RETVAL_DATA_ERROR;
#else
	get_bits(bd,32);
	if ((i == 0x177245) && (j == 0x385090)) return RETVAL_LAST_BLOCK;
	get_bits(bd,1);
	origPtr=get_bits(bd,24);
#endif
	/* mapping table: if some byte values are never used (encoding things
	   like ascii text), the compression code removes the gaps to have fewer
	   symbols to deal with, and writes a sparse bitfield indicating which
	   values were present.  We make a translation table to convert the symbols
	   back to the corresponding bytes. */
	t=get_bits(bd, 16);
	symTotal=0;
	for (i=0;i<16;i++) {
		if(t&(1<<(15-i))) {
			k=get_bits(bd,16);
			for(j=0;j<16;j++)
				if(k&(1<<(15-j))) symToByte[symTotal++]=(16*i)+j;
		}
	}
	/* How many different huffman coding groups does this block use? */
	groupCount=get_bits(bd,3);
#ifdef ENABLE_BUNZIP_CHECKING
	if (groupCount<2 || groupCount>MAX_GROUPS) return RETVAL_DATA_ERROR;
#endif
	/* nSelectors: Every GROUP_SIZE many symbols we select a new huffman coding
	   group.  Read in the group selector list, which is stored as MTF encoded
	   bit runs.  (MTF=Move To Front, as each value is used it's moved to the
	   start of the list.) */
#ifdef ENABLE_BUNZIP_CHECKING
	if(!(nSelectors=get_bits(bd, 15))) return RETVAL_DATA_ERROR;
#else
	nSelectors=get_bits(bd, 15);
#endif
	for(i=0; i<groupCount; i++) mtfSymbol[i] = i;
	for(i=0; i<nSelectors; i++) {
		/* Get next value */
#ifdef ENABLE_BUNZIP_CHECKING
		for(j=0;get_bits(bd,1);j++) if (j>=groupCount) return RETVAL_DATA_ERROR;
#else
		for(j=0;get_bits(bd,1);j++) ;
#endif
		/* Decode MTF to get the next selector */
		uc = mtfSymbol[j];
		for(;j;j--) mtfSymbol[j] = mtfSymbol[j-1];
		mtfSymbol[0]=selectors[i]=uc;
	}
	/* Read the huffman coding tables for each group, which code for symTotal
	   literal symbols, plus two run symbols (RUNA, RUNB) */
	symCount=symTotal+2;
	for (j=0; j<groupCount; j++) {
		unsigned char length[MAX_SYMBOLS],temp[MAX_HUFCODE_BITS+1];
		int	minLen,	maxLen, pp;
		/* Read huffman code lengths for each symbol.  They're stored in
		   a way similar to mtf; record a starting value for the first symbol,
		   and an offset from the previous value for everys symbol after that.
		   (Subtracting 1 before the loop and then adding it back at the end is
		   an optimization that makes the test inside the loop simpler: symbol
		   length 0 becomes negative, so an unsigned inequality catches it.) */
		t=get_bits(bd, 5)-1;
		for (i = 0; i < symCount; i++) {
			for(;;) {
#ifdef ENABLE_BUNZIP_CHECKING
				if (((unsigned)t) > (MAX_HUFCODE_BITS-1))
					return RETVAL_DATA_ERROR;
#endif
				/* If first bit is 0, stop.  Else second bit indicates whether
				   to increment or decrement the value.  Optimization: grab 2
				   bits and unget the second if the first was 0. */
				k = get_bits(bd,2);
				if (k < 2) {
					bd->inbufBitCount++;
					break;
				}
				/* Add one if second bit 1, else subtract 1.  Avoids if/else */
				t+=(((k+1)&2)-1);
			}
			/* Correct for the initial -1, to get the final symbol length */
			length[i]=t+1;
		}
		/* Find largest and smallest lengths in this group */
		minLen=maxLen=length[0];
		for(i = 1; i < symCount; i++) {
			if(length[i] > maxLen) maxLen = length[i];
			else if(length[i] < minLen) minLen = length[i];
		}
		/* Calculate permute[], base[], and limit[] tables from length[].
		 *
		 * permute[] is the lookup table for converting huffman coded symbols
		 * into decoded symbols.  base[] is the amount to subtract from the
		 * value of a huffman symbol of a given length when using permute[].
		 *
		 * limit[] indicates the largest numerical value a symbol with a given
		 * number of bits can have.  This is how the huffman codes can vary in
		 * length: each code with a value>limit[length] needs another bit.
		 */
		hufGroup=bd->groups+j;
		hufGroup->minLen = minLen;
		hufGroup->maxLen = maxLen;
		/* Note that minLen can't be smaller than 1, so we adjust the base
		   and limit array pointers so we're not always wasting the first
		   entry.  We do this again when using them (during symbol decoding).*/
		base=hufGroup->base-1;
		limit=hufGroup->limit-1;
		/* Calculate permute[].  Concurently, initialize temp[] and limit[]. */
		pp=0;
		for(i=minLen;i<=maxLen;i++) {
			temp[i]=limit[i]=0;
			for(t=0;t<symCount;t++) 
				if(length[t]==i) hufGroup->permute[pp++] = t;
		}
		/* Count symbols coded for at each bit length */
		for (i=0;i<symCount;i++) temp[length[i]]++;
		/* Calculate limit[] (the largest symbol-coding value at each bit
		 * length, which is (previous limit<<1)+symbols at this level), and
		 * base[] (number of symbols to ignore at each bit length, which is
		 * limit minus the cumulative count of symbols coded for already). */
		pp=t=0;
		for (i=minLen; i<maxLen; i++) {
			pp+=temp[i];
			/* We read the largest possible symbol size and then unget bits
			   after determining how many we need, and those extra bits could
			   be set to anything.  (They're noise from future symbols.)  At
			   each level we're really only interested in the first few bits,
			   so here we set all the trailing to-be-ignored bits to 1 so they
			   don't affect the value>limit[length] comparison. */
			limit[i]= (pp << (maxLen - i)) - 1;
			pp<<=1;
			base[i+1]=pp-(t+=temp[i]);
		}
		limit[maxLen+1] = INT_MAX; /* Sentinal value for reading next sym. */
		limit[maxLen]=pp+temp[maxLen]-1;
		base[minLen]=0;
	}
	/* We've finished reading and digesting the block header.  Now read this
	   block's huffman coded symbols from the file and undo the huffman coding
	   and run length encoding, saving the result into dbuf[dbufCount++]=uc */

	/* Initialize symbol occurrence counters and symbol Move To Front table */
	for(i=0;i<256;i++) {
		byteCount[i] = 0;
		mtfSymbol[i]=(unsigned char)i;
	}
	/* Loop through compressed symbols. */
	runPos=dbufCount=symCount=selector=0;
	for(;;) {
		/* Determine which huffman coding group to use. */
		if(!(symCount--)) {
			symCount=GROUP_SIZE-1;
#ifdef ENABLE_BUNZIP_CHECKING
			if(selector>=nSelectors) return RETVAL_DATA_ERROR;
#endif
			hufGroup=bd->groups+selectors[selector++];
			base=hufGroup->base-1;
			limit=hufGroup->limit-1;
		}
		/* Read next huffman-coded symbol. */
		/* Note: It is far cheaper to read maxLen bits and back up than it is
		   to read minLen bits and then an additional bit at a time, testing
		   as we go.  Because there is a trailing last block (with file CRC),
		   there is no danger of the overread causing an unexpected EOF for a
		   valid compressed file.  As a further optimization, we do the read
		   inline (falling back to a call to get_bits if the buffer runs
		   dry).  The following (up to got_huff_bits:) is equivalent to
		   j=get_bits(bd,hufGroup->maxLen);
		 */
		while (bd->inbufBitCount<hufGroup->maxLen) {
#ifdef ENABLE_BUNZIP_CHECKING
			if(bd->inbufPos==bd->inbufCount) {
				j = get_bits(bd,hufGroup->maxLen);
				goto got_huff_bits;
			}
#endif
			bd->inbufBits=(bd->inbufBits<<8)|bd->inbuf[bd->inbufPos++];
			bd->inbufBitCount+=8;
		};
		bd->inbufBitCount-=hufGroup->maxLen;
		j = (bd->inbufBits>>bd->inbufBitCount)&((1<<hufGroup->maxLen)-1);
got_huff_bits:
		/* Figure how how many bits are in next symbol and unget extras */
		i=hufGroup->minLen;
		while(j>limit[i]) ++i;
		bd->inbufBitCount += (hufGroup->maxLen - i);
		/* Huffman decode value to get nextSym (with bounds checking) */
#ifdef ENABLE_BUNZIP_CHECKING
		if ((i > hufGroup->maxLen)
			|| (((unsigned)(j=(j>>(hufGroup->maxLen-i))-base[i]))
				>= MAX_SYMBOLS))
			return RETVAL_DATA_ERROR;
#else
		j=(j>>(hufGroup->maxLen-i))-base[i];
#endif
		nextSym = hufGroup->permute[j];
		/* We have now decoded the symbol, which indicates either a new literal
		   byte, or a repeated run of the most recent literal byte.  First,
		   check if nextSym indicates a repeated run, and if so loop collecting
		   how many times to repeat the last literal. */
		if (((unsigned)nextSym) <= SYMBOL_RUNB) { /* RUNA or RUNB */
			/* If this is the start of a new run, zero out counter */
			if(!runPos) {
				runPos = 1;
				t = 0;
			}
			/* Neat trick that saves 1 symbol: instead of or-ing 0 or 1 at
			   each bit position, add 1 or 2 instead.  For example,
			   1011 is 1<<0 + 1<<1 + 2<<2.  1010 is 2<<0 + 2<<1 + 1<<2.
			   You can make any bit pattern that way using 1 less symbol than
			   the basic or 0/1 method (except all bits 0, which would use no
			   symbols, but a run of length 0 doesn't mean anything in this
			   context).  Thus space is saved. */
			t += (runPos << nextSym); /* +runPos if RUNA; +2*runPos if RUNB */
			runPos <<= 1;
			continue;
		}
		/* When we hit the first non-run symbol after a run, we now know
		   how many times to repeat the last literal, so append that many
		   copies to our buffer of decoded symbols (dbuf) now.  (The last
		   literal used is the one at the head of the mtfSymbol array.) */
		if(runPos) {
			runPos=0;
#ifdef ENABLE_BUNZIP_CHECKING
			if(dbufCount+t>=dbufSize) return RETVAL_DATA_ERROR;
#endif

			uc = symToByte[mtfSymbol[0]];
			byteCount[uc] += t;
			while(t--) dbuf[dbufCount++]=uc;
		}
		/* Is this the terminating symbol? */
		if(nextSym>symTotal) break;
		/* At this point, nextSym indicates a new literal character.  Subtract
		   one to get the position in the MTF array at which this literal is
		   currently to be found.  (Note that the result can't be -1 or 0,
		   because 0 and 1 are RUNA and RUNB.  But another instance of the
		   first symbol in the mtf array, position 0, would have been handled
		   as part of a run above.  Therefore 1 unused mtf position minus
		   2 non-literal nextSym values equals -1.) */
#ifdef ENABLE_BUNZIP_CHECKING
		if(dbufCount>=dbufSize) return RETVAL_DATA_ERROR;
#endif
		i = nextSym - 1;
		uc = mtfSymbol[i];
		/* Adjust the MTF array.  Since we typically expect to move only a
		 * small number of symbols, and are bound by 256 in any case, using
		 * memmove here would typically be bigger and slower due to function
		 * call overhead and other assorted setup costs. */
		do {
			mtfSymbol[i] = mtfSymbol[i-1];
		} while (--i);
		mtfSymbol[0] = uc;
		uc=symToByte[uc];
		/* We have our literal byte.  Save it into dbuf. */
		byteCount[uc]++;
		dbuf[dbufCount++] = (unsigned int)uc;
	}
	/* At this point, we've read all the huffman-coded symbols (and repeated
       runs) for this block from the input stream, and decoded them into the
	   intermediate buffer.  There are dbufCount many decoded bytes in dbuf[].
	   Now undo the Burrows-Wheeler transform on dbuf.
	   See http://dogma.net/markn/articles/bwt/bwt.htm
	 */
	/* Turn byteCount into cumulative occurrence counts of 0 to n-1. */
	j=0;
	for(i=0;i<256;i++) {
		k=j+byteCount[i];
		byteCount[i] = j;
		j=k;
	}
	/* Figure out what order dbuf would be in if we sorted it. */
	for (i=0;i<dbufCount;i++) {
		uc=(unsigned char)(dbuf[i] & 0xff);
		dbuf[byteCount[uc]] |= (i << 8);
		byteCount[uc]++;
	}
	/* Decode first byte by hand to initialize "previous" byte.  Note that it
	   doesn't get output, and if the first three characters are identical
	   it doesn't qualify as a run (hence writeRunCountdown=5). */
	if(dbufCount) {
#ifdef ENABLE_BUNZIP_CHECKING
		if(origPtr>=dbufCount) return RETVAL_DATA_ERROR;
#endif
		bd->writePos=dbuf[origPtr];
	    bd->writeCurrent=(unsigned char)(bd->writePos&0xff);
		bd->writePos>>=8;
		bd->writeRunCountdown=5;
	}
	bd->writeCount=dbufCount;

	return RETVAL_OK;
}
