/*
 * WRT400n - Firmware Generation Creator
 *
 * Creates a firmware image for the Linksys WRT400n router,
 * that can be uploaded via the firmware upload page,
 * from a kernel image file and root fs file
 *
 *	Author: Sandeep Mistry
 *	Author: Yousong Zhou
 *		- remove size limit on kernel and rootfs part
 */
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cyg_crc.h"

// Prelimiary test shows that the OEM upgrade program only checks checksum of
// the whole firmware image, ignoring specific checksums of kernel and rootfs
// part.  But we are computing crc32 sums of fixed length of data here in lieu
// of read_fw utility found in OEM SDK
#define CRC_SZ_KERNEL 0x100000
#define CRC_SZ_ROOTFS 0x2fffc4
// OEM firmware has size limit on "linux" and "rootfs" partition.  That's also
// the limit for factory firmware images as fwupgrade utility in the OEM system
// needs to write kernel and rootfs to those two partitions (/dev/mtd2 and
// /dev/mtd3 to be exact)
#define SZ_MAX_KERNEL (0x180000 - 0x40000)
#define SZ_MAX_ROOTFS (0x7b0000 - 0x18000)


// Header format:
//
//		GPL Tarball: http://downloads.linksysbycisco.com/downloads/WRT400N_1.0.01.19_US.tar,0.gz
//		File: WRT400N_1.0.01.19_US/FW_WRT400N_1.0.01.19_US_20081229/GTK/user/include/fw_upgrade.h
//
//		Description
//			- checksum: 			CRC32 of kernel and root fs, back to back
//			- magic:				GMTKRT400N
//			- kernel_length:		kernel length in bytes
//			- kernel_upgrade_flag:	should we upgrade the kernel - set to 1
//			- rootfs_length:		root fs length in byte
//			- rootfs_upgrade_flag:	should we upgrade the root fs - set to 1
//			- kernel_checksum: 		Gary S. Brown's 32 bit CRC algorithm for kernel, with remaining bits
//									set to 0xFF upto 0x100000 bytes (total length)
//			- rootfs_checksum:		Gary S. Brown's 32 bit CRC algorithm for root fs, with remaining bits
//									set to 0xFF upto 0x2FFFC4 bytes	(total length)
//			- fw_totalsize:			total firmware image file length (header length + kernel length + root fs length)
//			- reserved[4]:			reserved ??? - set to all 0xFF
struct imghdr_t
{
	uint32_t		checksum;		/* CRC32 */
	uint8_t			magic[12];		/* The value of GTIMG_MAGIC */
	uint32_t		kernel_length;		/* The length of the kernel image */
	//uint32_t		kernel_entry_point;	/* Kernel's entry point for RedBoot's information */
	uint32_t		kernel_upgrade_flag;	/* Set to 1 if we need to upgrade the kernel parition of the Flash */
	uint32_t		rootfs_length;		/* The length of the rootfs image */
	//uint32_t		rootfs_entry_point;	/* Not in use */
	uint32_t		rootfs_upgrade_flag;	/* Set to 1 if we need to upgrade the rootfs parition of the Flash */

	// Add 3 items by Vic Yu, 2006-05/10
	uint32_t		kernel_checksum;
	uint32_t		rootfs_checksum;
	uint32_t		fw_totalsize;
	uint32_t		reserved[4];
};

struct file_info {
	char		*filename;
	int			filesize;
	int			fd;
	uint32_t	crc32sum;
	int			crc_sz;
};

int get_file_info(struct file_info *fi, uint32_t *crc32_acc)
{
	int fd;
	int filesize = 0;
	uint32_t crc32sum = 0;
	uint32_t _crc32_acc;
	int crc_rem = fi->crc_sz;
	int sz;
	char buf[1024];

	if (crc32_acc)
		_crc32_acc = *crc32_acc;
	else
		_crc32_acc = 0;

	fd = open(fi->filename, O_RDONLY);
	if(fd < 0) {
		fprintf(stderr, "error: opening '%s'\n", fi->filename);
		return -1;
	}
	while (true) {
		sz = read(fd, buf, sizeof(buf));
		if (sz > 0) {
			if (crc32_acc)
				_crc32_acc = cyg_crc32_accumulate(_crc32_acc, buf, sz);
			if (crc_rem > 0) {
				if (crc_rem < sz) {
					crc32sum = cyg_crc32_accumulate(crc32sum, buf, crc_rem);
					crc_rem = 0;
				} else {
					crc32sum = cyg_crc32_accumulate(crc32sum, buf, sz);
					crc_rem -= sz;
				}
			}
			filesize += sz;
		} else if (sz == 0) {
			break;
		} else {
			fprintf(stderr, "error read '%s'", strerror(errno));
			return -1;
		}
	}

	if (crc_rem) {
		memset(buf, 0xff, sizeof(buf));
		while (crc_rem > 0) {
			if (crc_rem > sizeof(buf)) {
				crc32sum = cyg_crc32_accumulate(crc32sum, buf, sizeof(buf));
				crc_rem -= sizeof(buf);
			} else {
				crc32sum = cyg_crc32_accumulate(crc32sum, buf, crc_rem);
				crc_rem = 0;
				break;
			}
		}
	}

	fi->fd = fd;
	fi->filesize = filesize;
	fi->crc32sum = crc32sum;
	if (crc32_acc)
		*crc32_acc = _crc32_acc;
	return 0;
}

int copy_file(int fromfd, int tofd)
{
	int szr, szw;
	int buf[4096];

	if (lseek(fromfd, SEEK_SET, 0) < 0) {
		fprintf(stderr, "lseek: %s\n", strerror(errno));
		return -1;
	}
	while (true) {
		szr = read(fromfd, buf, sizeof(buf));
		if (szr > 0) {
			szw = write(tofd, buf, szr);
			if (szw != szr) {
				fprintf(stderr, "copy_file: error writing %d bytes: %s\n", szr, strerror(errno));
				return -1;
			}
		} else if (szr == 0) {
			break;
		} else {
			fprintf(stderr, "copy_file: error reading: %s\n", strerror(errno));
			return -1;
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	struct file_info kernel_fi;
	struct file_info rootfs_fi;
	struct file_info output_fi;
	struct imghdr_t ih;
	int fd, sz;
	int ret = -1;

	if(argc != 4) {
		printf("Usage:\n\t%s <kernel file> <rootfs file> <output file>\n", argv[0]);
		return 1;
	}

	kernel_fi.crc_sz = CRC_SZ_KERNEL;
	rootfs_fi.crc_sz = CRC_SZ_ROOTFS;

	kernel_fi.fd = -1;
	rootfs_fi.fd = -1;
	output_fi.fd = -1;

	kernel_fi.filename = argv[1];
	rootfs_fi.filename = argv[2];
	output_fi.filename = argv[3];

	kernel_fi.crc32sum = 0;
	rootfs_fi.crc32sum = 0;
	output_fi.crc32sum = 0xffffffff;

	if (get_file_info(&kernel_fi, &output_fi.crc32sum) < 0)
		goto done;
	if (get_file_info(&rootfs_fi, &output_fi.crc32sum) < 0)
		goto done;
	output_fi.crc32sum = ~output_fi.crc32sum;

	// print out stats
	fprintf(stderr, "%s: size %d (0x%x), crc32 = 0x%x\n",
		kernel_fi.filename, kernel_fi.filesize, kernel_fi.filesize, kernel_fi.crc32sum);
	fprintf(stderr, "%s: size %d (0x%x), crc32 = 0x%x\n",
		rootfs_fi.filename, rootfs_fi.filesize, rootfs_fi.filesize, rootfs_fi.crc32sum);
	if (kernel_fi.filesize > SZ_MAX_KERNEL) {
		fprintf(stderr, "%s: filesize exceeds 0x%x limit\n", SZ_MAX_KERNEL);
		goto done;
	}
	if (rootfs_fi.filesize > SZ_MAX_ROOTFS) {
		fprintf(stderr, "%s: filesize exceeds 0x%x limit\n", SZ_MAX_ROOTFS);
		goto done;
	}

	// now for the header ...
	memset(&ih, 0xff, sizeof(ih));
	strcpy(ih.magic, "GMTKRT400N");
	ih.checksum = htonl(output_fi.crc32sum);
	ih.kernel_length = htonl(kernel_fi.filesize);
	ih.rootfs_length = htonl(rootfs_fi.filesize);
	ih.kernel_upgrade_flag = htonl(0x1);
	ih.rootfs_upgrade_flag = htonl(0x1);
	ih.kernel_checksum = htonl(kernel_fi.crc32sum);
	ih.rootfs_checksum = htonl(rootfs_fi.crc32sum);
	ih.fw_totalsize = htonl(kernel_fi.filesize + rootfs_fi.filesize + sizeof(ih));

	output_fi.fd = open(output_fi.filename, O_WRONLY | O_CREAT | O_TRUNC,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	sz = write(output_fi.fd, &ih, sizeof(ih));
	if (sz != sizeof(ih)) {
		fprintf(stderr, "error writing imghdr to output: %s\n", strerror(errno));
		goto done;
	}
	if (copy_file(kernel_fi.fd, output_fi.fd)) {
		fprintf(stderr, "error copying %s to %s\n", kernel_fi.filename, output_fi.filename);
		goto done;
	}
	if (copy_file(rootfs_fi.fd, output_fi.fd)) {
		fprintf(stderr, "error copying %s to %s\n", rootfs_fi.filename, output_fi.filename);
		goto done;
	}
	// print some stats out
	fprintf(stderr, "crc = 0x%x, total size = %d (0x%x)\n",
			output_fi.crc32sum, ntohl(ih.fw_totalsize), ntohl(ih.fw_totalsize));
	ret = 0;

done:
#define CLOSE_FI_FD(fi)	do {	\
	if ((fi).fd >= 0) {			\
		close((fi).fd);			\
	}							\
} while (0)

	CLOSE_FI_FD(kernel_fi);
	CLOSE_FI_FD(rootfs_fi);
	CLOSE_FI_FD(output_fi);
	return ret;
}
