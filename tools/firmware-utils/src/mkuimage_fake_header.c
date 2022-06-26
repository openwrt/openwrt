#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <zlib.h>
#include <unistd.h>
#include <stdint.h>


/*
 *  Uboot image header format
 *  (ripped from mkimage.c/image.h)
 */
#define IH_MAGIC    0x27051956
#define IH_NMLEN    32
#define UIMAGE_FAKE_HEADER "UIMAGE fake header"

typedef struct image_header {
    uint32_t    ih_magic;   /* Image Header Magic Number    */
    uint32_t    ih_hcrc;    /* Image Header CRC Checksum    */
    uint32_t    ih_time;    /* Image Creation Timestamp */
    uint32_t    ih_size;    /* Image Data Size      */
    uint32_t    ih_load;    /* Data  Load  Address      */
    uint32_t    ih_ep;      /* Entry Point Address      */
    uint32_t    ih_dcrc;    /* Image Data CRC Checksum  */
    uint8_t     ih_os;      /* Operating System     */
    uint8_t     ih_arch;    /* CPU architecture     */
    uint8_t     ih_type;    /* Image Type           */
    uint8_t     ih_comp;    /* Compression Type     */
    uint8_t     ih_name[IH_NMLEN];  /* Image Name       */
} image_header_t;

int main (int argc, char *argv[])
{
	char *filename = argv[3];
	uint32_t partsize = atoi(argv[1]);
	uint32_t erasesize = atoi(argv[2]);
	struct stat st;
	size_t size;
	unsigned long checksum;
	FILE *f;
	u_char *firm, *fullfirm;

	image_header_t uimage_fake_header;
	image_header_t uimage_real_header;	
	
	stat(filename, &st);
	size = st.st_size;
	firm = malloc(size-sizeof(image_header_t));
	fullfirm = malloc(partsize);
	memset(fullfirm, 0, partsize);

	f = fopen(filename, "rb");
	if(f) {
		fread(&uimage_real_header, sizeof(image_header_t), 1, f);
		fread(firm, size-sizeof(image_header_t), 1, f);
		fclose(f);
	}

	memcpy(&uimage_fake_header, &uimage_real_header, sizeof(image_header_t));

	memcpy(fullfirm+sizeof(image_header_t), firm, size-sizeof(image_header_t));
	memcpy(fullfirm+(partsize-erasesize), &uimage_real_header, sizeof(image_header_t));

	// Change uboot header for full firmware size
	checksum = crc32 (0, fullfirm+sizeof(image_header_t), partsize-sizeof(image_header_t));
	uimage_fake_header.ih_dcrc = htonl(checksum);
	uimage_fake_header.ih_size = htonl(partsize-sizeof(image_header_t));
	memset(uimage_fake_header.ih_name, 0, IH_NMLEN);
	memcpy(uimage_fake_header.ih_name, UIMAGE_FAKE_HEADER, sizeof(UIMAGE_FAKE_HEADER));

	uimage_fake_header.ih_hcrc = htonl(0);
	checksum = crc32 (0, (u_char*)&uimage_fake_header, sizeof(image_header_t));
	uimage_fake_header.ih_hcrc = htonl(checksum);

	memcpy(fullfirm, &uimage_fake_header, sizeof(image_header_t));

	f = fopen(filename, "wb");
	if(f) {
		fwrite(fullfirm, partsize, 1, f);
		fclose(f);
	}

	free(firm);
	free(fullfirm);

	return 0;
}
