/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2008 Axel Gembe <ago@bastart.eu.org>
 * Copyright (C) 2009 Daniel Dickinson <crazycshore@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <netinet/in.h>

#include "bcm_tag.h"

#define IMAGETAG_MAGIC1			"Broadcom Corporatio"
#define IMAGETAG_MAGIC2			"ver. 2.0"
#define IMAGETAG_VER			"6"
#define IMAGETAG_DEFAULT_LOADADDR	0x80010000
#define DEFAULT_FW_OFFSET		0x10000
#define DEFAULT_FLASH_START		0xBFC00000
#define DEFAULT_FLASH_BS		(64 * 1024)
#define DEADCODE			0xDEADC0DE

union int2char {
  uint32_t input;
  char output[4];
};

/* Convert uint32_t CRC to bigendian and copy it into a character array */
#define int2tag(tag, value)  intchar.input = htonl(value);	\
	  memcpy(tag, intchar.output, sizeof(union int2char))

/* Kernel header */
struct kernelhdr {
	uint32_t		loadaddr;	/* Kernel load address */
	uint32_t		entry;		/* Kernel entry point address */
	uint32_t		lzmalen;	/* Compressed length of the LZMA data that follows */
};

static char pirellitab[NUM_PIRELLI][BOARDID_LEN] = PIRELLI_BOARDS;

static uint32_t crc32tab[256] = {
	0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
	0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
	0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
	0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
	0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
	0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
	0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
	0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
	0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
	0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
	0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
	0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
	0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
	0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
	0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
	0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
	0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
	0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
	0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
	0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
	0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
	0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
	0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
	0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
	0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
	0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
	0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
	0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
	0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
	0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
	0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
	0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

uint32_t crc32(uint32_t crc, uint8_t *data, size_t len)
{
	while (len--)
		crc = (crc >> 8) ^ crc32tab[(crc ^ *data++) & 0xFF];

	return crc;
}

uint32_t compute_crc32(uint32_t crc, FILE *binfile, size_t compute_start, size_t compute_len)
{
	uint8_t readbuf[1024];
	size_t read;

	fseek(binfile, compute_start, SEEK_SET);

	/* read block of 1024 bytes */
	while (binfile && !feof(binfile) && !ferror(binfile) && (compute_len >= sizeof(readbuf))) {
		read = fread(readbuf, sizeof(uint8_t), sizeof(readbuf), binfile);
		crc = crc32(crc, readbuf, read);
		compute_len = compute_len - read;
	}

	/* Less than 1024 bytes remains, read compute_len bytes */
	if (binfile && !feof(binfile) && !ferror(binfile) && (compute_len > 0)) {
		read = fread(readbuf, sizeof(uint8_t), compute_len, binfile);
		crc = crc32(crc, readbuf, read);
	}

	return crc;
}

size_t getlen(FILE *fp)
{
	size_t retval, curpos;

	if (!fp)
		return 0;

	curpos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	retval = ftell(fp);
	fseek(fp, curpos, SEEK_SET);

	return retval;
}

int tagfile(const char *kernel, const char *rootfs, const char *bin,
	    const char *boardid, const char *chipid, const uint32_t fwaddr,
	    const uint32_t loadaddr, const uint32_t entry,
	    const char *ver, const char *magic2, const uint32_t flash_bs,
	    const char *rsignature, const char *layoutver)
{
	struct bcm_tag tag;
	struct kernelhdr khdr;
	FILE *kernelfile = NULL, *rootfsfile = NULL, *binfile;
	size_t kerneloff, kernellen, rootfsoff, rootfslen, read, imagelen, rootfsoffpadlen, kernelfslen;
	uint8_t readbuf[1024];
	uint32_t imagecrc = IMAGETAG_CRC_START;
	uint32_t kernelcrc = IMAGETAG_CRC_START;
	uint32_t rootfscrc = IMAGETAG_CRC_START;
	uint32_t kernelfscrc = IMAGETAG_CRC_START;
	const uint32_t deadcode = htonl(DEADCODE);
        union int2char intchar;
        int i;
        int is_pirelli = 0;

	memset(&tag, 0, sizeof(struct bcm_tag));

	if (strlen(boardid) >= sizeof(tag.boardid)) {
		fprintf(stderr, "Board id is too long!\n");
		return 1;
	}

	/* Likewise chipid */
	if (strlen(chipid) >= sizeof(tag.chipid)) {
		fprintf(stderr, "Chip id is too long!\n");
		return 1;
	}

	if (!kernel || !rootfs) {
		fprintf(stderr, "imagetag can't create an image without both kernel and rootfs\n");
	}

	if (kernel && !(kernelfile = fopen(kernel, "rb"))) {
		fprintf(stderr, "Unable to open kernel \"%s\"\n", kernel);
		return 1;
	}

	if (rootfs && !(rootfsfile = fopen(rootfs, "rb"))) {
		fprintf(stderr, "Unable to open rootfs \"%s\"\n", rootfs);
		return 1;
	}

	if (!bin || !(binfile = fopen(bin, "wb+"))) {
		fprintf(stderr, "Unable to open output file \"%s\"\n", bin);
		return 1;
	}

	/* Build the kernel address and length (doesn't need to be aligned, read only) */
	kerneloff = fwaddr + sizeof(tag);
	kernellen = getlen(kernelfile);

	/* Build the kernel header */
	khdr.loadaddr	= htonl(loadaddr);
	khdr.entry	= htonl(entry);
	khdr.lzmalen	= htonl(kernellen);

	/* Increase the kernel size by the header size */
	kernellen += sizeof(khdr);

	/* Build the rootfs address and length (start and end do need to be aligned on flash erase block boundaries */
	rootfsoff = kerneloff + kernellen;
	rootfsoff = (rootfsoff % flash_bs) > 0 ? (((rootfsoff / flash_bs) + 1) * flash_bs) : rootfsoff;
	rootfslen = getlen(rootfsfile);
	rootfslen = ( (rootfslen % flash_bs) > 0 ? (((rootfslen / flash_bs) + 1) * flash_bs) : rootfslen );
	imagelen = rootfsoff + rootfslen - kerneloff + sizeof(deadcode);
	rootfsoffpadlen = rootfsoff - (kerneloff + kernellen);

	/* Seek to the start of the kernel */
	fseek(binfile, kerneloff - fwaddr, SEEK_SET);

	/* Write the kernel header */
	fwrite(&khdr, sizeof(khdr), 1, binfile);

	/* Write the kernel */
	while (kernelfile && !feof(kernelfile) && !ferror(kernelfile)) {
		read = fread(readbuf, sizeof(uint8_t), sizeof(readbuf), kernelfile);
		fwrite(readbuf, sizeof(uint8_t), read, binfile);
	}

	/* Write the RootFS */
	fseek(binfile, rootfsoff - fwaddr, SEEK_SET);
	while (rootfsfile && !feof(rootfsfile) && !ferror(rootfsfile)) {
		read = fread(readbuf, sizeof(uint8_t), sizeof(readbuf), rootfsfile);
		fwrite(readbuf, sizeof(uint8_t), read, binfile);
	}

	/* Align image to specified erase block size and append deadc0de */
	printf("Data alignment to %dk with 'deadc0de' appended\n", flash_bs/1024);
	fseek(binfile, rootfsoff + rootfslen - fwaddr, SEEK_SET);
	fwrite(&deadcode, sizeof(uint32_t), 1, binfile);

	/* Flush the binfile buffer so that when we read from file, it contains
         * everything in the buffer
	 */
	fflush(binfile);

	/* Compute the crc32 of the entire image (deadC0de included) */
	imagecrc = compute_crc32(imagecrc, binfile, kerneloff - fwaddr, imagelen);
        /* Compute the crc32 of the kernel and padding between kernel and rootfs) */
	kernelcrc = compute_crc32(kernelcrc, binfile, kerneloff - fwaddr, kernellen + rootfsoffpadlen);
        /* Compute the crc32 of the kernel and padding between kernel and rootfs) */
	kernelfscrc = compute_crc32(kernelfscrc, binfile, kerneloff - fwaddr, kernellen + rootfsoffpadlen + rootfslen + sizeof(deadcode));
	/* Compute the crc32 of the flashImageStart to rootLength.
         * The broadcom firmware assumes the rootfs starts the image,
         * therefore uses the rootfs start to determine where to flash
         * the image.  Since we have the kernel first we have to give
         * it the kernel address, but the crc uses the length
         * associated with this address, which is added to the kernel
         * length to determine the length of image to flash and thus
         * needs to be rootfs + deadcode
         */
	rootfscrc = compute_crc32(rootfscrc, binfile, kerneloff - fwaddr, rootfslen + sizeof(deadcode));

	/* Close the files */
	fclose(kernelfile);
	fclose(rootfsfile);

	/* Build the tag */
	strncpy(tag.tagVersion, ver, TAGVER_LEN);
	strncpy(tag.sig_1, IMAGETAG_MAGIC1, sizeof(tag.sig_1) - 1);
	strncpy(tag.sig_2, magic2, sizeof(tag.sig_2) - 1);
	strcpy(tag.chipid, chipid);
	strcpy(tag.boardid, boardid);
	strcpy(tag.big_endian, "1");
	sprintf(tag.totalLength, "%lu", imagelen);

	/* We don't include CFE */
	strcpy(tag.cfeAddress, "0");
	strcpy(tag.cfeLength, "0");

        sprintf(tag.kernelAddress, "%lu", kerneloff);
	sprintf(tag.kernelLength, "%lu", kernellen + rootfsoffpadlen);
        sprintf(tag.flashImageStart, "%lu", kerneloff);
	sprintf(tag.rootLength, "%lu", rootfslen + sizeof(deadcode));

	if (rsignature) {
	    strncpy(tag.rsa_signature, rsignature, RSASIG_LEN);
	}

        if (layoutver) {
	    strncpy(tag.flashLayoutVer, layoutver, TAGLAYOUT_LEN);
	}

	for (i = 0; i < NUM_PIRELLI; i++) {
		if (strncmp(boardid, pirellitab[i], BOARDID_LEN) == 0) {
			is_pirelli = 1;
			break;
		}
	}

	if ( !is_pirelli ) {
		int2tag(tag.imageCRC, imagecrc);
	} else {
	        int2tag(tag.imageCRC, kernelcrc);
	}
        int2tag(tag.kernelCRC, kernelcrc);
        int2tag(tag.rootfsCRC, rootfscrc);
	int2tag(tag.fskernelCRC, kernelfscrc);
	int2tag(tag.headerCRC, crc32(IMAGETAG_CRC_START, (uint8_t*)&tag, sizeof(tag) - 20));

	fseek(binfile, 0L, SEEK_SET);
	fwrite(&tag, sizeof(uint8_t), sizeof(tag), binfile);

	fclose(binfile);

	return 0;
}

int main(int argc, char **argv)
{
        int c, i;
	char *kernel, *rootfs, *bin, *boardid, *chipid, *magic2, *ver, *tagid, *rsignature, *layoutver;
	uint32_t flashstart, fwoffset, loadaddr, entry;
	uint32_t fwaddr, flash_bs;
	int tagidfound = 0;

	kernel = rootfs = bin = boardid = chipid = magic2 = ver = rsignature = layoutver = NULL;
	entry = 0;

	flashstart = DEFAULT_FLASH_START;
	fwoffset = DEFAULT_FW_OFFSET;
	loadaddr = IMAGETAG_DEFAULT_LOADADDR;
	flash_bs = DEFAULT_FLASH_BS;

	printf("Broadcom image tagger - v1.0.0\n");
	printf("Copyright (C) 2008 Axel Gembe\n");
	printf("Copyright (C) 2009-2010 Daniel Dickinson\n");

	while ((c = getopt(argc, argv, "i:f:o:b:c:s:n:v:m:k:l:e:h:r:y:")) != -1) {
		switch (c) {
			case 'i':
				kernel = optarg;
				break;
			case 'f':
				rootfs = optarg;
				break;
			case 'o':
				bin = optarg;
				break;
			case 'b':
				boardid = optarg;
				break;
			case 'c':
				chipid = optarg;
				break;
			case 's':
				flashstart = strtoul(optarg, NULL, 16);
				break;
			case 'n':
				fwoffset = strtoul(optarg, NULL, 16);
				break;
			case 'v':
				ver = optarg;
				break;
			case 'm':
				magic2 = optarg;
				break;
			case 'k':
				flash_bs = strtoul(optarg, NULL, 16);
				break;
			case 'l':
				loadaddr = strtoul(optarg, NULL, 16);
				break;
			case 'e':
				entry = strtoul(optarg, NULL, 16);
				break;
		        case 'r':
			        rsignature = optarg;
				break;
		        case 'y':
			        layoutver = optarg;
				break;
			case 'h':
			default:
				fprintf(stderr, "Usage: imagetag <parameters>\n\n");
				fprintf(stderr, "	-i <kernel>		- The LZMA compressed kernel file to include in the image\n");
				fprintf(stderr, "	-f <rootfs>		- The RootFS file to include in the image\n");
				fprintf(stderr, "	-o <bin>		- The output file\n");
				fprintf(stderr, "	-b <boardid>		- The board id to set in the image (i.e. \"96345GW2\")\n");
				fprintf(stderr, "	-c <chipid>		- The chip id to set in the image (i.e. \"6345\")\n");
				fprintf(stderr, "	-s <flashstart> 	- Flash start address (i.e. \"0xBFC00000\"\n");
				fprintf(stderr, "	-n <fwoffset>   	- \n");
				fprintf(stderr, "	-v <version>		- \n");
				fprintf(stderr, "	-m <magic2>		- \n");
				fprintf(stderr, "	-k <flash_bs>		- flash erase block size\n");
				fprintf(stderr, "	-l <loadaddr>		- Address where the kernel expects to be loaded (defaults to 0x80010000)\n");
				fprintf(stderr, "	-e <entry>		- Address where the kernel entry point will end up\n");
				fprintf(stderr, "       -r <signature>          - vendor specific signature, for those that need it");
				fprintf(stderr, "       -y <layoutver>          - Flash Layout Version (2.2x code versions need this)");
				fprintf(stderr, "	-h			- Displays this text\n\n");
				return 1;
		}
	}

	if (!boardid || !chipid) {
		fprintf(stderr, "You need to specify the board (-b) and chip id (-c)!\n");
		return 1;
	}

	if (entry == 0) {
		fprintf(stderr, "You need to specify the kernel entry (-e)\n");
		return 1;
	}

	/* Fallback to defaults */

	fwaddr = flashstart + fwoffset;

	if (!magic2) {
		magic2 = malloc(sizeof(char) * 14);
		if (!magic2) {
			perror("malloc");
			return 1;
		}
		strcpy(magic2, IMAGETAG_MAGIC2);
	}

	if (!ver) {
		ver = malloc(sizeof(char) * 4);
		if (!ver) {
			perror("malloc");
			return 1;
		}
		strcpy(ver, IMAGETAG_VER);
	}


	return tagfile(kernel, rootfs, bin, boardid, chipid, fwaddr, loadaddr, entry, ver, magic2, flash_bs, rsignature, layoutver);
}
