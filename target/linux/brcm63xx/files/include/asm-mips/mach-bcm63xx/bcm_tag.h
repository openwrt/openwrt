#ifndef __BCM63XX_TAG_H
#define __BCM63XX_TAG_H

#define IMAGE_LEN 10                   /* Length of Length Field */
#define ADDRESS_LEN 12                 /* Length of Address field */

struct bcm_tag {
	unsigned char tagVersion[4];                            // Version of the image tag
	unsigned char sig_1[20];                                // Company Line 1
	unsigned char sig_2[14];                                // Company Line 2
	unsigned char chipid[6];                                        // Chip this image is for
	unsigned char boardid[16];                              // Board name
	unsigned char big_endian[2];                             // Map endianness -- 1 BE 0 LE
	unsigned char totalLength[IMAGE_LEN];           //Total length of image
	unsigned char cfeAddress[ADDRESS_LEN];  // Address in memory of CFE
	unsigned char cfeLength[IMAGE_LEN];             // Size of CFE
	unsigned char cfeRootAddress[ADDRESS_LEN];         // Address in memory of rootfs
	unsigned char cfeRootLength[IMAGE_LEN];            // Size of rootfs
	unsigned char kernelAddress[ADDRESS_LEN];       // Address in memory of kernel
	unsigned char kernelLength[IMAGE_LEN];  // Size of kernel
	unsigned char dualImage[2];                             // Unused at present
	unsigned char inactiveFlag[2];                  // Unused at present
	unsigned char reserved1[74];                            // Reserved area not in use
	unsigned char imageCRC[4];                              // CRC32 of images
//      unsigned char reserved2[16];                    // Unused at present
        unsigned char reserved2[4];                     //Unused
        unsigned char rootAddress[ADDRESS_LEN];         // Address in memory of rootfs
        unsigned char headerCRC[4];                     // CRC32 of header excluding tagVersion
//      unsigned char reserved3[16];                    // Unused at present
        unsigned char reserved3[6];                     // Unused at present
        unsigned char rootLength[IMAGE_LEN];            // Size of rootfs
};

#endif /* __BCM63XX_TAG_H */
