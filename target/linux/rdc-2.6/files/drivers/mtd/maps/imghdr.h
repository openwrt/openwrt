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
	u8		magic[4];		/* ASICII: GMTK */
	u32		checksum;		/* CRC32 */
	u32		version;		/* x.x.x.x */
	u32		kernelsz;		/* The size of the kernel image */
	u32		imagesz;		/* The length of this image file ( kernel + romfs + this header) */
	u32		pid;			/* Product ID */
	u32		fastcksum;		/* Partial CRC32 on (First(256), medium(256), last(512)) */
	u32		reserved;
}gt_imghdr_t;

#endif
