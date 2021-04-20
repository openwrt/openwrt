// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * uimage_sgehdr.c : add 96 bytes of extra header information after the normal tail of uimage header
 * this is an edited version of uimage_padhdr.c
 *
 * Copyright (C) 2019 NOGUCHI Hiroshi <drvlabo@gmail.com>
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <zlib.h>


/* from u-boot/include/image.h */
#define IH_NMLEN		32	/* Image Name Length		*/
#define SGE_PRODUCTLEN 	64	/* sge_Product Length		*/
#define SGE_VERSIONLEN 	16	/* sge Version Length		*/
#define OrignalHL		64	/* Orignal Header Length	*/

/*
 * SGE format image header,
 * all data in network byte order (aka natural aka bigendian).
 */
typedef struct image_header {
	uint32_t	ih_magic;	/* Image Header Magic Number	*/
	uint32_t	ih_hcrc;	/* Image Header CRC Checksum	*/
	uint32_t	ih_time;	/* Image Creation Timestamp	*/
	uint32_t	ih_size;	/* Image Data Size		*/
	uint32_t	ih_load;	/* Data	 Load  Address		*/
	uint32_t	ih_ep;		/* Entry Point Address		*/
	uint32_t	ih_dcrc;	/* Image Data CRC Checksum	*/
	uint8_t		ih_os;		/* Operating System		*/
	uint8_t		ih_arch;	/* CPU architecture		*/
	uint8_t		ih_type;	/* Image Type			*/
	uint8_t		ih_comp;	/* Compression Type		*/
	uint8_t		ih_name[IH_NMLEN];	/* Image Name		*/
	uint8_t		sgeih_p[SGE_PRODUCTLEN];	/* sge_Product		*/
	uint8_t		sgeih_sv[SGE_VERSIONLEN];	/* sge Software Version		*/
	uint8_t		sgeih_hv[SGE_VERSIONLEN];	/* sge Hardware Version		*/
} image_header_t;


/* default padding size */
#define	IH_PAD_BYTES		(96)


static void usage(char *prog)
{
	fprintf(stderr,
		"%s -i <input_uimage_file> -o <output_file> -m <model> -h <hardware version> -s <software version> \n",
		prog);
}

int main(int argc, char *argv[])
{
	struct stat statbuf;
	u_int8_t *filebuf;
	int ifd;
	int ofd;
	ssize_t rsz;
	u_int32_t crc_recalc;
	image_header_t *imgh;
	int opt;
	char *infname = NULL;
	char *outfname = NULL;
	char *model = NULL;
	char *hversion = NULL;
	char *sversion = NULL;
	int padsz = IH_PAD_BYTES;
	int ltmp;

	while ((opt = getopt(argc, argv, "i:o:m:h:s:")) != -1) {
		switch (opt) {
		case 'i':
			infname = optarg;
			break;
		case 'o':
			outfname = optarg;
			break;
		case 'm':
			model = optarg;
			break;
		case 'h':
			hversion = optarg;
			break;
		case 's':
			sversion = optarg;
			break;
		default:
			break;
		}
	}

	if (!infname || !outfname) {
		usage(argv[0]);
		exit(1);
	}

	ifd = open(infname, O_RDONLY);
	if (ifd < 0) {
		fprintf(stderr,
			"could not open input file. (errno = %d)\n", errno);
		exit(1);
	}

	ofd = open(outfname, O_WRONLY | O_CREAT, 0644);
	if (ofd < 0) {
		fprintf(stderr,
			"could not open output file. (errno = %d)\n", errno);
		exit(1);
	}

	if (fstat(ifd, &statbuf) < 0) {
		fprintf(stderr,
			"could not fstat input file. (errno = %d)\n", errno);
		exit(1);
	}

	filebuf = malloc(statbuf.st_size + padsz);
	if (!filebuf) {
		fprintf(stderr, "buffer allocation failed\n");
		exit(1);
	}

	rsz = read(ifd, filebuf, OrignalHL);
	if (rsz != OrignalHL) {
		fprintf(stderr,
			"could not read input file (errno = %d).\n", errno);
		exit(1);
	}

	memset(&(filebuf[OrignalHL]), 0, padsz);

	rsz = read(ifd, &(filebuf[sizeof(*imgh)]),
				statbuf.st_size - OrignalHL);
	if (rsz != (int32_t)(statbuf.st_size - OrignalHL)) {
		fprintf(stderr,
			"could not read input file (errno = %d).\n", errno);
		exit(1);
	}

	imgh = (image_header_t *)filebuf;

	imgh->ih_hcrc = 0;
	
	strncpy(imgh->sgeih_p, model, sizeof(imgh->sgeih_p));
	strncpy(imgh->sgeih_sv, sversion, sizeof(imgh->sgeih_sv));
	strncpy(imgh->sgeih_hv, hversion, sizeof(imgh->sgeih_hv));
	
	crc_recalc = crc32(0, filebuf, sizeof(*imgh));
	imgh->ih_hcrc = htonl(crc_recalc);

	rsz = write(ofd, filebuf, statbuf.st_size + padsz);
	if (rsz != (int32_t)statbuf.st_size + padsz) {
		fprintf(stderr,
			"could not write output file (errnor = %d).\n", errno);
		exit(1);
	}

	return 0;
}
