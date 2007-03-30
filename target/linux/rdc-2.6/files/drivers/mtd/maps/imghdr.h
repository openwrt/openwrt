#ifndef GT_IMGHDR_H
#define GT_IMGHDR_H

#define GTIMG_MAGIC		"GMTK"

/* Product ID */
#define PID_RTL_AIRGO		1
#define PID_RTL_RALINK		2
#define PID_RDC_AIRGO		3
#define PID_RDC_RALINK		5	/* White Lable */

/* Gemtek */
typedef struct
{
	UINT8		magic[4];		/* ASICII: GMTK */
	UINT32		checksum;		/* CRC32 */
	UINT32		version;		/* x.x.x.x */
	UINT32		kernelsz;		/* The size of the kernel image */
	UINT32		imagesz;		/* The length of this image file ( kernel + romfs + this header) */
	UINT32		pid;			/* Product ID */
	UINT32		fastcksum;		/* Partial CRC32 on (First(256), medium(256), last(512)) */
	UINT32		reserved;
}gt_imghdr_t;

#endif
