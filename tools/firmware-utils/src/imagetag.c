/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2008 Axel Gembe <ago@bastart.eu.org>
 * Copyright (C) 2009-2010 Daniel Dickinson <openwrt@cshore.neomailbox.net>
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
#include "imagetag_cmdline.h"

#define DEADCODE			0xDEADC0DE

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

void int2tag(char *tag, uint32_t value) {
  uint32_t network = htonl(value);
  memcpy(tag, (char *)(&network), 4);
}

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

int tagfile(const char *kernel, const char *rootfs, const char *bin, \
			const struct gengetopt_args_info *args, \
			uint32_t flash_start, uint32_t image_offset, \
			uint32_t block_size, uint32_t load_address, uint32_t entry)
{
	struct bcm_tag tag;
	struct kernelhdr khdr;
	FILE *kernelfile = NULL, *rootfsfile = NULL, *binfile = NULL, *cfefile = NULL;
	size_t cfeoff, cfelen, kerneloff, kernellen, rootfsoff, rootfslen, \
	  read, imagelen, rootfsoffpadlen = 0, kernelfslen, kerneloffpadlen = 0, oldrootfslen;
	uint8_t readbuf[1024];
	uint32_t imagecrc = IMAGETAG_CRC_START;
	uint32_t kernelcrc = IMAGETAG_CRC_START;
	uint32_t rootfscrc = IMAGETAG_CRC_START;
	uint32_t kernelfscrc = IMAGETAG_CRC_START;
	uint32_t fwaddr = 0;
	uint8_t crc_val;
	const uint32_t deadcode = htonl(DEADCODE);
	int i;
	int is_pirelli = 0;


	memset(&tag, 0, sizeof(struct bcm_tag));

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

	if ((args->cfe_given) && (args->cfe_arg)) {
	  if (!(cfefile = fopen(args->cfe_arg, "rb"))) {
		fprintf(stderr, "Unable to open CFE file \"%s\"\n", args->cfe_arg);
	  }
	}

	fwaddr = flash_start + image_offset;
	if (cfefile) {
	  cfeoff = flash_start;		  
	  cfelen = getlen(cfefile);
	  /* Seek to the start of the file after tag */
	  fseek(binfile, sizeof(tag), SEEK_SET);
	  
	  /* Write the cfe */
	  while (cfefile && !feof(cfefile) && !ferror(cfefile)) {
		read = fread(readbuf, sizeof(uint8_t), sizeof(readbuf), cfefile);
		fwrite(readbuf, sizeof(uint8_t), read, binfile);
	  }

	} else {
	  cfeoff = 0;
	  cfelen = 0;
	}

	if (!args->root_first_flag) {
	  /* Build the kernel address and length (doesn't need to be aligned, read only) */
	  kerneloff = fwaddr + sizeof(tag);
	  
	  kernellen = getlen(kernelfile);
	  
	  if (!args->kernel_file_has_header_flag) {
		/* Build the kernel header */
		khdr.loadaddr	= htonl(load_address);
		khdr.entry	= htonl(entry);
		khdr.lzmalen	= htonl(kernellen);
		
		/* Increase the kernel size by the header size */
		kernellen += sizeof(khdr);	  
	  }
	  
	  /* Build the rootfs address and length (start and end do need to be aligned on flash erase block boundaries */
	  rootfsoff = kerneloff + kernellen;
	  rootfsoff = (rootfsoff % block_size) > 0 ? (((rootfsoff / block_size) + 1) * block_size) : rootfsoff;
	  rootfslen = getlen(rootfsfile);
	  rootfslen = ( (rootfslen % block_size) > 0 ? (((rootfslen / block_size) + 1) * block_size) : rootfslen );
	  imagelen = rootfsoff + rootfslen - kerneloff + sizeof(deadcode);
	  rootfsoffpadlen = rootfsoff - (kerneloff + kernellen);
	  
	  /* Seek to the start of the kernel */
	  fseek(binfile, kerneloff - fwaddr + cfelen, SEEK_SET);
	  
	  /* Write the kernel header */
	  fwrite(&khdr, sizeof(khdr), 1, binfile);
	  
	  /* Write the kernel */
	  while (kernelfile && !feof(kernelfile) && !ferror(kernelfile)) {
		read = fread(readbuf, sizeof(uint8_t), sizeof(readbuf), kernelfile);
		fwrite(readbuf, sizeof(uint8_t), read, binfile);
	  }

	  /* Write the RootFS */
	  fseek(binfile, rootfsoff - fwaddr + cfelen, SEEK_SET);
	  while (rootfsfile && !feof(rootfsfile) && !ferror(rootfsfile)) {
		read = fread(readbuf, sizeof(uint8_t), sizeof(readbuf), rootfsfile);
		fwrite(readbuf, sizeof(uint8_t), read, binfile);
	  }

	  /* Align image to specified erase block size and append deadc0de */
	  printf("Data alignment to %dk with 'deadc0de' appended\n", block_size/1024);
	  fseek(binfile, rootfsoff + rootfslen - fwaddr + cfelen, SEEK_SET);
	  fwrite(&deadcode, sizeof(uint32_t), 1, binfile);

	  /* Flush the binfile buffer so that when we read from file, it contains
	   * everything in the buffer
	   */
	  fflush(binfile);

	  /* Compute the crc32 of the entire image (deadC0de included) */
	  imagecrc = compute_crc32(imagecrc, binfile, kerneloff - fwaddr + cfelen, imagelen);
	  /* Compute the crc32 of the kernel and padding between kernel and rootfs) */
	  kernelcrc = compute_crc32(kernelcrc, binfile, kerneloff - fwaddr + cfelen, kernellen + rootfsoffpadlen);
	  /* Compute the crc32 of the kernel and padding between kernel and rootfs) */
	  kernelfscrc = compute_crc32(kernelfscrc, binfile, kerneloff - fwaddr + cfelen, kernellen + rootfsoffpadlen + rootfslen + sizeof(deadcode));
	  /* Compute the crc32 of the flashImageStart to rootLength.
	   * The broadcom firmware assumes the rootfs starts the image,
	   * therefore uses the rootfs start to determine where to flash
	   * the image.  Since we have the kernel first we have to give
	   * it the kernel address, but the crc uses the length
	   * associated with this address, which is added to the kernel
	   * length to determine the length of image to flash and thus
	   * needs to be rootfs + deadcode
	   */
	  rootfscrc = compute_crc32(rootfscrc, binfile, kerneloff - fwaddr + cfelen, rootfslen + sizeof(deadcode));

	} else {
	  /* Build the kernel address and length (doesn't need to be aligned, read only) */
	  rootfsoff = fwaddr + sizeof(tag);
	  oldrootfslen = getlen(rootfsfile);
	  rootfslen = oldrootfslen;
	  rootfslen = ( (rootfslen % block_size) > 0 ? (((rootfslen / block_size) + 1) * block_size) : rootfslen );
	  kerneloffpadlen = rootfslen - oldrootfslen;

	  kerneloff = rootfsoff + rootfslen;
	  kernellen = getlen(kernelfile);

	  imagelen = cfelen + rootfslen + kernellen;
	  
	  /* Seek to the start of the kernel */
	  fseek(binfile, kerneloff - fwaddr + cfelen, SEEK_SET);
	  
	  if (!args->kernel_file_has_header_flag) {
		/* Build the kernel header */
		khdr.loadaddr	= htonl(load_address);
		khdr.entry	= htonl(entry);
		khdr.lzmalen	= htonl(kernellen);
		
		/* Write the kernel header */
		fwrite(&khdr, sizeof(khdr), 1, binfile);
	  
		/* Increase the kernel size by the header size */
		kernellen += sizeof(khdr);	  
	  }
	  
	  /* Write the kernel */
	  while (kernelfile && !feof(kernelfile) && !ferror(kernelfile)) {
		read = fread(readbuf, sizeof(uint8_t), sizeof(readbuf), kernelfile);
		fwrite(readbuf, sizeof(uint8_t), read, binfile);
	  }

	  /* Write the RootFS */
	  fseek(binfile, rootfsoff - fwaddr + cfelen, SEEK_SET);
	  while (rootfsfile && !feof(rootfsfile) && !ferror(rootfsfile)) {
		read = fread(readbuf, sizeof(uint8_t), sizeof(readbuf), rootfsfile);
		fwrite(readbuf, sizeof(uint8_t), read, binfile);
	  }

	  /* Flush the binfile buffer so that when we read from file, it contains
	   * everything in the buffer
	   */
	  fflush(binfile);

	  /* Compute the crc32 of the entire image (deadC0de included) */
	  imagecrc = compute_crc32(imagecrc, binfile, sizeof(tag), imagelen);
	  /* Compute the crc32 of the kernel and padding between kernel and rootfs) */
	  kernelcrc = compute_crc32(kernelcrc, binfile, kerneloff - fwaddr + cfelen, kernellen + rootfsoffpadlen);
	  kernelfscrc = compute_crc32(kernelfscrc, binfile, rootfsoff - fwaddr + cfelen, kernellen + rootfslen);
	  rootfscrc = compute_crc32(rootfscrc, binfile, rootfsoff - fwaddr + cfelen, rootfslen);
	}

	/* Close the files */
	if (cfefile) {
	  fclose(cfefile);
	}
	fclose(kernelfile);
	fclose(rootfsfile);

	/* Build the tag */
	strncpy(tag.tagVersion, args->tag_version_arg, sizeof(tag.tagVersion) - 1);
	strncpy(tag.sig_1, args->signature_arg, sizeof(tag.sig_1) - 1);
	strncpy(tag.sig_2, args->signature2_arg, sizeof(tag.sig_2) - 1);
	strncpy(tag.chipid, args->chipid_arg, sizeof(tag.chipid) - 1);
	strncpy(tag.boardid, args->boardid_arg, sizeof(tag.boardid) - 1);
	strcpy(tag.big_endian, "1");
	sprintf(tag.totalLength, "%lu", imagelen);

	if (args->cfe_given) {
	  sprintf(tag.cfeAddress, "%lu", flash_start);
	  sprintf(tag.cfeLength, "%lu", cfelen);
	} else {
	  /* We don't include CFE */
	  strcpy(tag.cfeAddress, "0");
	  strcpy(tag.cfeLength, "0");
	}

	sprintf(tag.kernelAddress, "%lu", kerneloff);
	sprintf(tag.kernelLength, "%lu", kernellen + rootfsoffpadlen);

	if (args->root_first_flag) {
	  sprintf(tag.flashImageStart, "%lu", rootfsoff);
	  sprintf(tag.flashRootLength, "%lu", rootfslen);	  
	} else {
	  sprintf(tag.flashImageStart, "%lu", kerneloff);
	  sprintf(tag.flashRootLength, "%lu", rootfslen + sizeof(deadcode));
	}
	int2tag(tag.rootLength, rootfslen + sizeof(deadcode));

	if (args->rsa_signature_given) {
	    strncpy(tag.rsa_signature, args->rsa_signature_arg, RSASIG_LEN);
	}

	if (args->layoutver_given) {
	    strncpy(tag.flashLayoutVer, args->layoutver_arg, TAGLAYOUT_LEN);
	}

	if (args->info1_given) {
	  strncpy(tag.information1, args->info1_arg, TAGINFO1_LEN);
	}

	if (args->info2_given) {
	  strncpy(tag.information2, args->info2_arg, TAGINFO2_LEN);
	}

	if (args->reserved2_given) {
	  strncpy(tag.reserved2, args->reserved2_arg, 16);
	}

	if (args->altinfo_given) {
	  strncpy(&tag.information1[0], args->altinfo_arg, ALTTAGINFO_LEN);
	}

	if (args->second_image_flag_given) {
	  if (strncmp(args->second_image_flag_arg, "2", DUALFLAG_LEN) != 0) {		
		strncpy(tag.dualImage, args->second_image_flag_arg, DUALFLAG_LEN);
	  }
	}

	if (args->inactive_given) {
	  if (strncmp(args->inactive_arg, "2", INACTIVEFLAG_LEN) != 0) {		
		strncpy(tag.inactiveFlag, args->second_image_flag_arg, INACTIVEFLAG_LEN);
	  }
	}

	for (i = 0; i < NUM_PIRELLI; i++) {
		if (strncmp(args->boardid_arg, pirellitab[i], BOARDID_LEN) == 0) {
			is_pirelli = 1;
			break;
		}
	}

	if ( !is_pirelli ) {
	  int2tag(tag.imageCRC, kernelfscrc);
	} else {
	  int2tag(tag.imageCRC, kernelcrc);
	}

	int2tag(&(tag.rootfsCRC[0]), rootfscrc);
	int2tag(tag.kernelCRC, kernelcrc);
	int2tag(tag.fskernelCRC, kernelfscrc);
	int2tag(tag.headerCRC, crc32(IMAGETAG_CRC_START, (uint8_t*)&tag, sizeof(tag) - 20));

	fseek(binfile, 0L, SEEK_SET);
	fwrite(&tag, sizeof(uint8_t), sizeof(tag), binfile);

    fflush(binfile);
	fclose(binfile);

	return 0;
}

int main(int argc, char **argv)
{
    int c, i;
	char *kernel, *rootfs, *bin;
	uint32_t flash_start, image_offset, block_size, load_address, entry;
	flash_start = image_offset = block_size = load_address = entry = 0;
	struct gengetopt_args_info parsed_args;

	kernel = rootfs = bin = NULL;

	if (cmdline_parser(argc, argv, &parsed_args)) {
	  exit(1);
	}

	printf("Broadcom 63xx image tagger - v2.0.0\n");
	printf("Copyright (C) 2008 Axel Gembe\n");
	printf("Copyright (C) 2009-2010 Daniel Dickinson\n");
	printf("Licensed under the terms of the Gnu General Public License\n");

	kernel = parsed_args.kernel_arg;
	rootfs = parsed_args.rootfs_arg;
	bin = parsed_args.output_arg;
	if (strlen(parsed_args.tag_version_arg) >= TAGVER_LEN) {
	  fprintf(stderr, "Error: Tag Version (tag_version,v) too long.\n");
	  exit(1);
	}
	if (strlen(parsed_args.boardid_arg) >= BOARDID_LEN) {
	  fprintf(stderr, "Error: Board ID (boardid,b) too long.\n");
	  exit(1);
	}
	if (strlen(parsed_args.chipid_arg) >= CHIPID_LEN) {
	  fprintf(stderr, "Error: Chip ID (chipid,c) too long.\n");
	  exit(1);
	}
	if (strlen(parsed_args.signature_arg) >= SIG1_LEN) {
	  fprintf(stderr, "Error: Magic string (signature,a) too long.\n");
	  exit(1);
	}
	if (strlen(parsed_args.signature2_arg) >= SIG2_LEN) {
	  fprintf(stderr, "Error: Second magic string (signature2,m) too long.\n");
	  exit(1);
	}
	if (parsed_args.layoutver_given) {
	  if (strlen(parsed_args.layoutver_arg) > FLASHLAYOUTVER_LEN) {
		fprintf(stderr, "Error: Flash layout version (layoutver,y) too long.\n");
		exit(1);
	  }
	}
	if (parsed_args.rsa_signature_given) {
	  if (strlen(parsed_args.rsa_signature_arg) > RSASIG_LEN) {
		fprintf(stderr, "Error: RSA Signature (rsa_signature,r) too long.\n");
		exit(1);
	  }
	}

	if (parsed_args.info1_given) {
	  if (strlen(parsed_args.info1_arg) >= TAGINFO1_LEN) {
		fprintf(stderr, "Error: Vendor Information 1 (info1) too long.\n");
		exit(1);
	  }
	}

	if (parsed_args.info2_given) {
	  if (strlen(parsed_args.info2_arg) >= TAGINFO2_LEN) {
		fprintf(stderr, "Error: Vendor Information 2 (info2) too long.\n");
		exit(1);
	  }
	}

	if (parsed_args.altinfo_given) {
	  if (strlen(parsed_args.altinfo_arg) >= ALTTAGINFO_LEN) {
		fprintf(stderr, "Error: Vendor Information 1 (info1) too long.\n");
		exit(1);
	  }
	}
	flash_start = strtoul(parsed_args.flash_start_arg, NULL, 16);
	image_offset = strtoul(parsed_args.image_offset_arg, NULL, 16);
	block_size = strtoul(parsed_args.block_size_arg, NULL, 16);

	if (!parsed_args.kernel_file_has_header_flag) {
	  load_address = strtoul(parsed_args.load_addr_arg, NULL, 16);
	  entry = strtoul(parsed_args.entry_arg, NULL, 16);
	  if (load_address == 0) {
		fprintf(stderr, "Error: Invalid value for load address\n");
	  }
	  if (entry == 0) {
		fprintf(stderr, "Error: Invalid value for entry\n");
	  }
	}
	
	return tagfile(kernel, rootfs, bin, &parsed_args, flash_start, image_offset, block_size, load_address, entry);
}
