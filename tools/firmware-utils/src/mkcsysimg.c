/*
 *  $Id$
 *
 *  Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
 *
 *  This program was based on the code found in various Linux 
 *  source tarballs released by Edimax for it's devices.
 *  Original author: David Hsu <davidhsu@realtek.com.tw>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>     /* for unlink() */
#include <libgen.h>
#include <getopt.h>     /* for getopt() */
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include <endian.h>     /* for __BYTE_ORDER */
#if defined(__CYGWIN__)
#  include <byteswap.h>
#endif

#include "csysimg.h"

#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#  define HOST_TO_LE16(x)	(x)
#  define HOST_TO_LE32(x)	(x)
#  define LE16_TO_HOST(x)	(x)
#  define LE32_TO_HOST(x)	(x)
#else
#  define HOST_TO_LE16(x)	bswap_16(x)
#  define HOST_TO_LE32(x)	bswap_32(x)
#  define LE16_TO_HOST(x)	bswap_16(x)
#  define LE32_TO_HOST(x)	bswap_32(x)
#endif

#define ALIGN(x,y)	((x)+((y)-1)) & ~((y)-1)

#define MAX_NUM_BLOCKS	8
#define MAX_ARG_COUNT	32
#define MAX_ARG_LEN	1024
#define FILE_BUF_LEN	(16*1024)
#define CSYS_PADC	0xFF

#define BLOCK_TYPE_BOOT	0
#define BLOCK_TYPE_CONF	1
#define BLOCK_TYPE_WEBP	2
#define BLOCK_TYPE_CODE	3
#define BLOCK_TYPE_XTRA	4


struct csum_state{
	int	size;
	uint16_t val;
	uint16_t tmp;
	int	odd;
};


struct csys_block {
	int		type;	/* type of the block */

	int		need_file;
	char		*file_name;	/* name of the file */
	uint32_t	file_size;	/* length of the file */

	uint32_t	size;
	int		size_set;
	uint8_t		padc;

	uint32_t	size_hdr;
	uint32_t	size_csum;
	uint32_t	size_avail;

	unsigned char	sig[SIG_LEN];
	uint32_t	addr;
	int		addr_set;
	struct csum_state *css;
};


struct board_info {
	char *model;
	char *name;
	uint32_t flash_size;

	char sig_boot[SIG_LEN];
	char sig_conf[SIG_LEN];
	char sig_webp[SIG_LEN];

	uint32_t boot_size;
	uint32_t conf_size;
	uint32_t webp_size;
	uint32_t webp_size_max;
	uint32_t code_size;
	
	uint32_t addr_code;
	uint32_t addr_webp;
};

#define BOARD(m, n, f, sigb, sigw, bs, cs, ws, ac, aw) {\
	.model = m, .name = n, .flash_size = f<<20, \
	.sig_boot = sigb, .sig_conf = SIG_CONF, .sig_webp = sigw, \
	.boot_size = bs, .conf_size = cs, \
	.webp_size = ws, .webp_size_max = 3*0x10000, \
	.addr_code = ac, .addr_webp = aw \
	}
	
#define BOARD_ADM(m,n,f, sigw) BOARD(m,n,f, ADM_BOOT_SIG, sigw, \
	ADM_BOOT_SIZE, ADM_CONF_SIZE, ADM_WEBP_SIZE, \
	ADM_CODE_ADDR, ADM_WEBP_ADDR)


/*
 * Globals
 */
char *progname;
char *ofname = NULL;
int verblevel = 0;
int size_check = 1;

struct board_info *board = NULL;

struct csys_block *boot_block = NULL;
struct csys_block *conf_block = NULL;
struct csys_block *webp_block = NULL;
struct csys_block *code_block = NULL;

struct csys_block blocks[MAX_NUM_BLOCKS];
int num_blocks = 0;


static struct board_info boards[] = {
	/* The original Edimax products */
	BOARD_ADM("BR-6104K", "Edimax BR-6104K", 2, SIG_BR6104K),
	BOARD_ADM("BR-6104KP", "Edimax BR-6104KP", 2, SIG_BR6104KP),
	BOARD_ADM("BR-6114WG", "Edimax BR-6114WG", 2, SIG_BR6114WG),
	BOARD_ADM("BR-6524K", "Edimax BR-6524K", 2, SIG_BR6524K),
	BOARD_ADM("BR-6524KP", "Edimax BR-6524KP", 2, SIG_BR6524KP),
	BOARD_ADM("BR-6524WG", "Edimax BR-6524WG", 4, SIG_BR6524WG),
	BOARD_ADM("BR-6524WP", "Edimax BR-6524WP", 4, SIG_BR6524WP),
	BOARD_ADM("BR-6541K", "Edimax BR-6541K", 2, SIG_BR6541K),
	BOARD_ADM("BR-6541KP", "Edimax BR-6541K", 2, SIG_BR6541KP),
	BOARD_ADM("BR-6541WP", "Edimax BR-6541WP", 4, SIG_BR6541WP),
	BOARD_ADM("EW-7207APg", "Edimax EW-7207APg", 2, SIG_EW7207APg),
	BOARD_ADM("PS-1205UWg", "Edimax PS-1205UWg", 2, SIG_PS1205UWg),
	BOARD_ADM("PS-3205U", "Edimax PS-3205U", 2, SIG_PS3205U),
	BOARD_ADM("PS-3205UWg", "Edimax PS-3205UWg", 2, SIG_PS3205UWg),

	/* Hawking products */
	BOARD_ADM("H2BR4", "Hawking H2BR4", 2, SIG_H2BR4),
	BOARD_ADM("H2WR54G", "Hawking H2WR54G", 4, SIG_H2WR54G),

	/* Planet products */
	BOARD_ADM("XRT-401D", "Planet XRT-401D", 2, SIG_XRT401D),
	BOARD_ADM("XRT-402D", "Planet XRT-402D", 2, SIG_XRT402D),
	
	{.model = NULL}
};

/*
 * Helper routines
 */
void
errmsgv(int syserr, const char *fmt, va_list arg_ptr)
{
	int save = errno;

	fflush(0);
	fprintf(stderr, "%s: error, ", progname);
	vfprintf(stderr, fmt, arg_ptr);
	if (syserr != 0) {
		fprintf(stderr, " (%s)", strerror(save));
	}
	fprintf(stderr, "\n");
}

void
errmsg(int syserr, const char *fmt, ...)
{
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	errmsgv(syserr, fmt, arg_ptr);
	va_end(arg_ptr);
}

void
dbgmsg(int level, const char *fmt, ...)
{
	va_list arg_ptr;
	if (verblevel >= level) {
		fflush(0);
		va_start(arg_ptr, fmt);
		vfprintf(stderr, fmt, arg_ptr);
		fprintf(stderr, "\n");
		va_end(arg_ptr);
	}
}


void
usage(int status)
{
	FILE *stream = (status != EXIT_SUCCESS) ? stderr : stdout;
	struct board_info *board;

	fprintf(stream, "Usage: %s [OPTIONS...] <file>\n", progname);
	fprintf(stream,
"\n"
"Options:\n"
"  -B <board>      create image for the board specified with <board>.\n"
"                  valid <board> values:\n"
	);
	for (board = boards; board->model != NULL; board++){
		fprintf(stream,
"                  %-12s: %s\n",
		 board->model, board->name);
	};
	fprintf(stream,
"  -d              disable check for available space\n"
"                  add boot code to the image\n"
"  -b <file>[:<len>[:<padc>]]\n"
"                  add boot code to the image\n"
"  -c <file>[:<len>[:<padc>]]\n"
"                  add configuration settings to the image\n"
"  -r <file>:[<addr>][:<len>[:<padc>]]\n"
"                  add runtime code to the image\n"
"  -w [<file>:[<addr>][:<len>[:<padc>]]]\n"
"                  add webpages to the image\n"
"  -x <file>[:<len>[:<padc>]]\n"
"                  add extra data at the end of the image\n"
"  -h              show this screen\n"
"Parameters:\n"
"  <file>          write output to the file <file>\n"
	);

	exit(status);
}


/*
 * argument parsing
 */
int
str2u32(char *arg, uint32_t *val)
{
	char *err = NULL;
	uint32_t t;
	
	errno=0;
	t = strtoul(arg, &err, 0);
	if (errno || (err==arg) || ((err != NULL) && *err)) {
		return -1;
	}

	*val = t;
	return 0;
}


int
str2u16(char *arg, uint16_t *val)
{
	char *err = NULL;
	uint32_t t;

	errno=0;
	t = strtoul(arg, &err, 0);
	if (errno || (err==arg) || ((err != NULL) && *err) || (t >= 0x10000)) {
		return -1;
	}
	
	*val = t & 0xFFFF;
	return 0;
}

int
str2u8(char *arg, uint8_t *val)
{
	char *err = NULL;
	uint32_t t;

	errno=0;
	t = strtoul(arg, &err, 0);
	if (errno || (err==arg) || ((err != NULL) && *err) || (t >= 0x100)) {
		return -1;
	}
	
	*val = t & 0xFF;
	return 0;
}

int
str2sig(char *arg, uint32_t *sig)
{
	if (strlen(arg) != 4)
		return -1;

	*sig = arg[0] | (arg[1] << 8) | (arg[2] << 16) | (arg[3] << 24);
	
	return 0;
}


int
parse_arg(char *arg, char *buf, char *argv[])
{
	int res = 0;
	size_t argl;
	char *tok;
	char **ap = &buf;
	int i;

	memset(argv, 0, MAX_ARG_COUNT * sizeof(void *));

	if ((arg == NULL)) {
		/* no arguments */
		return 0;
	}

	argl = strlen(arg);
	if (argl == 0) {
		/* no arguments */
		return 0;
	}

	if (argl >= MAX_ARG_LEN) {
		/* argument is too long */
		argl = MAX_ARG_LEN-1;
	}

	memcpy(buf, arg, argl);
	buf[argl] = '\0';

	for (i = 0; i < MAX_ARG_COUNT; i++) {
		tok = strsep(ap, ":");
		if (tok == NULL) {
			break;
		}
#if 0
		else if (tok[0] == '\0') {
			break;
		}
#endif
		argv[i] = tok;
		res++;
	}

	return res;
}


int
required_arg(char c, char *arg)
{
	if (arg == NULL || *arg != '-')
		return 0;

	errmsg(0,"option -%c requires an argument\n", c);
	return -1;
}


int
is_empty_arg(char *arg)
{
	int ret = 1;
	if (arg != NULL) {
		if (*arg) ret = 0;
	};
	return ret;
}


void
csum8_update(uint8_t *p, uint32_t len, struct csum_state *css)
{
	for ( ; len > 0; len --) {
		css->val += *p++;
	}
}


uint16_t
csum8_get(struct csum_state *css)
{
	uint8_t t;
	
	t = css->val;
	return ~t + 1;
}


void
csum16_update(uint8_t *p, uint32_t len, struct csum_state *css)
{
	uint16_t t;
	
	if (css->odd) {
		t = css->tmp + (p[0]<<8);
		css->val += LE16_TO_HOST(t);
		css->odd = 0;
		len--;
		p++;
	}
	
	for ( ; len > 1; len -= 2, p +=2 ) {
		t = p[0] + (p[1] << 8);
		css->val += LE16_TO_HOST(t);
	}

	if (len == 1) {
		css->tmp = p[0];
		css->odd = 1;
	}
}


uint16_t
csum16_get(struct csum_state *css)
{
	char pad = 0;

	csum16_update(&pad, 1, css);
	return ~css->val + 1;
}


void
csum_init(struct csum_state *css, int size)
{
	css->val = 0;
	css->tmp = 0;
	css->odd = 0;
	css->size = size;
}


void
csum_update(uint8_t *p, uint32_t len, struct csum_state *css)
{
	switch (css->size) {
	case 1:
		csum8_update(p,len,css);
		break;
	case 2:
		csum16_update(p,len,css);
		break;
	}
}


uint16_t
csum_get(struct csum_state *css)
{
	uint16_t ret;

	switch (css->size) {
	case 1:
		ret = csum8_get(css);
		break;
	case 2:
		ret = csum16_get(css);
		break;
	}

	return ret;
}


/*
 * routines to write data to the output file
 */
int
write_out_data(FILE *outfile, uint8_t *data, size_t len,
		struct csum_state *css)
{
	errno = 0;

	fwrite(data, len, 1, outfile);
	if (errno) {
		errmsg(1,"unable to write output file");
		return -1;
	}

	if (css) {
		csum_update(data, len, css);
	}

	return 0;
}


int
write_out_padding(FILE *outfile, size_t len, uint8_t padc,
		 struct csum_state *css)
{
	uint8_t buf[512];
	size_t buflen = sizeof(buf);

	memset(buf, padc, buflen);
	while (len > 0) {
		if (len < buflen)
			buflen = len;

		if (write_out_data(outfile, buf, buflen, css))
			return -1;
			
		len -= buflen;
	}

	return 0;
}


int
block_stat_file(struct csys_block *block)
{
	struct stat st;
	int res;

	if (block->file_name == NULL)
		return 0;

	res = stat(block->file_name, &st);
	if (res){
		errmsg(1, "stat failed on %s", block->file_name);
		return res;
	}

	block->file_size = st.st_size;
	return 0;
}


int
block_writeout_hdr(FILE *outfile, struct csys_block *block)
{
	struct csys_header hdr;
	int res;

	if (block->size_hdr == 0)
		return 0;

	/* setup header fields */
	memcpy(hdr.sig, block->sig, 4);
	hdr.addr = HOST_TO_LE32(block->addr);
	hdr.size = HOST_TO_LE32(block->size-block->size_hdr);

	dbgmsg(1,"writing header for block");
	res = write_out_data(outfile, (uint8_t *)&hdr, sizeof(hdr),NULL);
	return res;

}


int
block_writeout_file(FILE *outfile, struct csys_block *block)
{
	char buf[FILE_BUF_LEN];
	size_t buflen = sizeof(buf);
	FILE *f;
	size_t len;
	int res;

	if (block->file_name == NULL)
		return 0;

	if (block->file_size == 0)
		return 0;

	errno = 0;
	f = fopen(block->file_name,"r");
	if (errno) {
		errmsg(1,"unable to open file: %s", block->file_name);
		return -1;
	}

	len = block->file_size;
	while (len > 0) {
		if (len < buflen)
			buflen = len;

		/* read data from source file */
		errno = 0;
		fread(buf, buflen, 1, f);
		if (errno != 0) {
			errmsg(1,"unable to read from file: %s",
				block->file_name);
			res = -1;
			break;
		}

		res = write_out_data(outfile, buf, buflen, block->css);
		if (res)
			break;

		len -= buflen;
	}

	fclose(f);
	return res;
}


int 
block_writeout_data(FILE *outfile, struct csys_block *block)
{
	int res;
	size_t padlen;

	res = block_writeout_file(outfile, block);
	if (res)
		return res;

	/* write padding data if neccesary */
	padlen = block->size_avail - block->file_size;
	dbgmsg(1,"padding block, length=%d", padlen);
	res = write_out_padding(outfile, padlen, block->padc, block->css);

	return res;
}


int 
block_writeout_csum(FILE *outfile, struct csys_block *block)
{
	uint16_t csum;
	int res;

	if (block->size_csum == 0)
		return 0;

	dbgmsg(1,"writing checksum for block");
	csum = HOST_TO_LE16(csum_get(block->css));
	res = write_out_data(outfile, (uint8_t *)&csum, block->size_csum, NULL);

	return res;
}


int
block_writeout(FILE *outfile, struct csys_block *block)
{
	int res;
	struct csum_state css;
	
	res = 0;

	if (block == NULL)
		return res;

	block->css = NULL;

	dbgmsg(2, "writing block, file=%s, file_size=%d, space=%d",
		block->file_name, block->file_size, block->size_avail);
	res = block_writeout_hdr(outfile, block);
	if (res)
		return res;

	if (block->size_csum != 0) {
		block->css = &css;
		csum_init(&css, block->size_csum);
	}

	res = block_writeout_data(outfile, block);
	if (res)
		return res;

	res = block_writeout_csum(outfile, block);
	if (res)
		return res;

	return res;
}


int
write_out_blocks(FILE *outfile)
{
	struct csys_block *block;
	int i, res;

	res = block_writeout(outfile, boot_block);
	if (res)
		return res;

	res = block_writeout(outfile, conf_block);
	if (res)
		return res;

	res = block_writeout(outfile, webp_block);
	if (res)
		return res;

	res = block_writeout(outfile, code_block);
	if (res)
		return res;

	res = 0;
	for (i=0; i < num_blocks; i++) {
		block = &blocks[i];
		
		if (block->type != BLOCK_TYPE_XTRA)
			continue;
			
		res = block_writeout(outfile, block);
		if (res)
			break;
	}
	
	return res;
}


struct board_info *
find_board(char *model)
{
	struct board_info *ret;
	struct board_info *board;

	ret = NULL;
	for (board = boards; board->model != NULL; board++){
		if (strcmp(model, board->model) == 0) {
			ret = board;
			break;
		}
	};

	return ret;
}


int
parse_opt_board(char ch, char *arg)
{

	dbgmsg(1,"parsing board option: -%c %s", ch, arg);

	if (board != NULL) {
		errmsg(0,"only one board option allowed");
		return -1;
	}

	if (required_arg(ch, arg))
		return -1;

	board = find_board(arg);
	if (board == NULL){
		errmsg(0,"invalid/unknown board specified: %s", arg);
		return -1;
	}

	return 0;
}


int
parse_opt_block(char ch, char *arg)
{
	char buf[MAX_ARG_LEN];
	char *argv[MAX_ARG_COUNT];
	int argc;
	char *p;
	struct csys_block *block;
	int i;

	if ( num_blocks > MAX_NUM_BLOCKS ) {
		errmsg(0,"too many blocks specified");
		return -1;
	}

	block = &blocks[num_blocks];
	
	/* setup default field values */
	block->need_file = 1;
	block->padc = 0xFF;
	
	switch (ch) {
	case 'b':
		if (boot_block) {
			errmsg(0,"only one boot block allowed");
			break;
		}
		block->type = BLOCK_TYPE_BOOT;
		boot_block = block;
		break;
	case 'c':
		if (conf_block) {
			errmsg(0,"only one config block allowed");
			break;
		}
		block->type = BLOCK_TYPE_CONF;
		conf_block = block;
		break;
	case 'w':
		if (webp_block) {
			errmsg(0,"only one web block allowed");
			break;
		}
		block->type = BLOCK_TYPE_WEBP;
		block->size_hdr = sizeof(struct csys_header);
		block->size_csum = 1;
		block->need_file = 0;
		webp_block = block;
		break;
	case 'r':
		if (code_block) {
			errmsg(0,"only one runtime block allowed");
			break;
		}
		block->type = BLOCK_TYPE_CODE;
		block->size_hdr = sizeof(struct csys_header);
		block->size_csum = 2;
		code_block = block;
		break;
	case 'x':
		block->type = BLOCK_TYPE_XTRA;
		break;
	default:
		errmsg(0,"unknown block type \"%c\"", ch);
		return -1;
	}

	argc = parse_arg(arg, buf, argv);

	i = 0;
	p = argv[i++];
	if (!is_empty_arg(p)) {
		block->file_name = strdup(p);
		if (block->file_name == NULL) {
			errmsg(0,"not enough memory");
			return -1;
		}
	} else if (block->need_file){
		errmsg(0,"no file specified in %s", arg);
		return -1;
	}

	if (block->size_hdr) {
		p = argv[i++];
		if (!is_empty_arg(p)) {
			if (str2u32(p, &block->addr) != 0) {
				errmsg(0,"invalid start address in %s", arg);
				return -1;
			}
			block->addr_set = 1;
		}
	}

	p = argv[i++];
	if (!is_empty_arg(p)) {
		if (str2u32(p, &block->size) != 0) {
			errmsg(0,"invalid block size in %s", arg);
			return -1;
		}
		block->size_set = 1;
	}

	p = argv[i++];
	if (!is_empty_arg(p) && (str2u8(p, &block->padc) != 0)) {
		errmsg(0,"invalid paddig character in %s", arg);
		return -1;
	}

	num_blocks++;

	return 0;
}


int
process_blocks(void)
{
	struct csys_block *block;
	uint32_t size_avail;
	int i;
	int res;

	/* collecting stats */
	for (i = 0; i < num_blocks; i++) {
		block = &blocks[i];
		res = block_stat_file(block);
		if (res)
			return res;
	}

	size_avail = board->flash_size;

	/* bootloader */
	block = boot_block;
	if (block) {
		if (block->size_set) {
			board->boot_size= block->size;
		} else {
			block->size = board->boot_size;
		}
		if (block->size > size_avail) {
			errmsg(0,"boot block is too big");
			return -1;
		}
	}
	size_avail -= board->boot_size;

	/* configuration data */
	block = conf_block;
	if (block) {
		if (block->size_set) {
			board->conf_size = block->size;
		} else {
			block->size = board->conf_size;
		}
		if (block->size > size_avail) {
			errmsg(0,"config block is too big");
			return -1;
		}

	}
	size_avail -= board->conf_size;

	/* webpages */
	block = webp_block;
	if (block) {
		if (block->size_set == 0)
			block->size = board->webp_size;
		board->webp_size = block->size;
		if (block->size > board->webp_size_max) {
			errmsg(0,"webpages block is too big");
			return -1;
		}
		memcpy(block->sig, board->sig_webp, 4);
		if (block->addr_set == 0)
			block->addr = board->addr_webp;
	}
	size_avail -= board->webp_size;

	/* runtime code */
	block = code_block;
	if (block) {
		if (block->size_set == 0) {
			block->size =ALIGN(block->file_size+ block->size_hdr +
				block->size_csum, 0x10000);
		}
		board->code_size = block->size;
		if (board->code_size > size_avail) {
			errmsg(0,"code block is too big");
			if (size_check)
				return -1;
		}
		memcpy(code_block->sig, SIG_CSYS, 4);
		if (code_block->addr_set == 0)
			code_block->addr = board->addr_code;
	}
	size_avail -= board->code_size;

	for (i = 0; i < num_blocks; i++) {
		block = &blocks[i];

		if (block->type != BLOCK_TYPE_XTRA)
			continue;

		if (block->size_set == 0)
			block->size = ALIGN(block->file_size, 0x10000);

		if (block->size > size_avail) {
			errmsg(0,"no space for file %s",
				block->file_name);
			if (size_check)
				return -1;
		}

		size_avail -= block->size;
	}

	for (i = 0; i < num_blocks; i++) {
		block = &blocks[i];

		block->size_avail = block->size - block->size_hdr -
			block->size_csum;

		if (block->size_avail < block->file_size) {
			errmsg(0,"file %s is too big, size=%d, avail=%d",
				block->file_name, block->file_size, 
				block->size_avail);
			res = -1;
			break;
		}
	}

	return res;
}


int
main(int argc, char *argv[])
{
	int optinvalid = 0;   /* flag for invalid option */
	int c;
	int res = EXIT_FAILURE;

	FILE *outfile;
	
	progname=basename(argv[0]);

	opterr = 0;  /* could not print standard getopt error messages */
	while ( 1 ) {
		optinvalid = 0;
		
		c = getopt(argc, argv, "b:c:dB:hr:vw:x:");
		if (c == -1)
			break;

		switch (c) {
		case 'b':
		case 'c':
		case 'r':
		case 'x':
			optinvalid = parse_opt_block(c,optarg);
			break;
		case 'w':
			if (optarg != NULL && *optarg == '-') {
				/* rollback */
				optind--;
				optarg = NULL;
			}
			optinvalid = parse_opt_block(c,optarg);
			break;
		case 'd':
			size_check = 0;
			break;
		case 'B':
			optinvalid = parse_opt_board(c,optarg);
			break;
		case 'v':
			verblevel++;
			break;
		case 'h':
			usage(EXIT_SUCCESS);
			break;
		default:
			optinvalid = 1;
			break;
		}
		if (optinvalid != 0 ){
			errmsg(0, "invalid option: -%c", optopt);
			goto out;
		}
	}

	if (optind == argc) {
		errmsg(0, "no output file specified");
		goto out;
	}

	ofname = argv[optind++];

	if (optind < argc) {
		errmsg(0, "invalid option: %s", argv[optind]);
		goto out;
	}
	

	if (board == NULL) {
		errmsg(0, "no board specified");
		goto out;
	}

	if (process_blocks() != 0) {
		goto out;
	}

	outfile = fopen(ofname, "w");
	if (outfile == NULL) {
		errmsg(1, "could not open \"%s\" for writing", ofname);
		goto out;
	}

	if (write_out_blocks(outfile) != 0)
		goto out_flush;

	dbgmsg(1,"Image file %s completed.", ofname);

	res = EXIT_SUCCESS;

out_flush:
	fflush(outfile);
	fclose(outfile);
	if (res != EXIT_SUCCESS) {
		unlink(ofname);
	}
out:
	return res;
}
