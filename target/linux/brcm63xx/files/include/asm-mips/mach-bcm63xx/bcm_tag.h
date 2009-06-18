#ifndef __BCM63XX_TAG_H
#define __BCM63XX_TAG_H

#define IMAGE_LEN 10                   /* Length of Length Field */
#define ADDRESS_LEN 12                 /* Length of Address field */
#define TAGID_LEN  6                   /* Length of tag ID */
#define TAGINFO_LEN 20                 /* Length of vendor information field in tag */
#define TAGVER_LEN 4                   /* Length of Tag Version */
#define TAGLAYOUT_LEN 4                /* Length of FlashLayoutVer */

#define NUM_TAGID 5
#define IMAGETAG_CRC_START		0xFFFFFFFF

struct tagiddesc_t {
  char tagid[TAGID_LEN + 1];
  char tagiddesc[80];
};

 // bc221 is used by BT Voyager and should be right
 // bc310 should be right, and may apply to 3.08 code as well
#define TAGID_DEFINITIONS { \
  { "bccfe", "Broadcom CFE flash image" }, \
  { "bc300", "Broadcom code version 3.00-3.06 and all ftp/tftp flash" }, \
  { "ag306", "Alice Gate (Pirelli, based on Broadcom 3.06)" }, \
  { "bc221", "Broadcom code version 2.21" }, \
  { "bc310", "Broadcom code version 3.10-3.12" }, \
}

struct bcm_tag_bccfe {
	unsigned char tagVersion[TAGVER_LEN];           // 0-3: Version of the image tag
	unsigned char sig_1[20];                        // 4-23: Company Line 1
	unsigned char sig_2[14];                        // 24-37: Company Line 2
	unsigned char chipid[6];                        // 38-43: Chip this image is for
	unsigned char boardid[16];                      // 44-59: Board name
	unsigned char big_endian[2];                    // 60-61: Map endianness -- 1 BE 0 LE
	unsigned char totalLength[IMAGE_LEN];           // 62-71: Total length of image
	unsigned char cfeAddress[ADDRESS_LEN];          // 72-83: Address in memory of CFE
	unsigned char cfeLength[IMAGE_LEN];             // 84-93: Size of CFE
	unsigned char rootAddress[ADDRESS_LEN];         // 94-105: Address in memory of rootfs
	unsigned char rootLength[IMAGE_LEN];            // 106-115: Size of rootfs
	unsigned char kernelAddress[ADDRESS_LEN];       // 116-127: Address in memory of kernel
	unsigned char kernelLength[IMAGE_LEN];          // 128-137: Size of kernel
	unsigned char dualImage[2];                     // 138-139: Unused at present
	unsigned char inactiveFlag[2];                  // 140-141: Unused at present
        unsigned char information1[TAGINFO_LEN];        // 142-161: Unused at present
        unsigned char tagId[TAGID_LEN];                 // 162-167: Identifies which type of tag this is, currently two-letter company code, and then three digits for version of broadcom code in which this tag was first introduced
        unsigned char tagIdCRC[4];                      // 168-171: CRC32 of tagId
	unsigned char reserved1[44];                    // 172-215: Reserved area not in use
	unsigned char imageCRC[4];                      // 216-219: CRC32 of images
        unsigned char reserved2[16];                    // 220-235: Unused at present
        unsigned char headerCRC[4];                     // 236-239: CRC32 of header excluding tagVersion
        unsigned char reserved3[16];                    // 240-255: Unused at present
};

struct bcm_tag_bc300 {
	unsigned char tagVersion[TAGVER_LEN];           // 0-3: Version of the image tag
	unsigned char sig_1[20];                        // 4-23: Company Line 1
	unsigned char sig_2[14];                        // 24-37: Company Line 2
	unsigned char chipid[6];                        // 38-43: Chip this image is for
	unsigned char boardid[16];                      // 44-59: Board name
	unsigned char big_endian[2];                    // 60-61: Map endianness -- 1 BE 0 LE
	unsigned char totalLength[IMAGE_LEN];           // 62-71: Total length of image
	unsigned char cfeAddress[ADDRESS_LEN];          // 72-83: Address in memory of CFE
	unsigned char cfeLength[IMAGE_LEN];             // 84-93: Size of CFE
	unsigned char flashImageStart[ADDRESS_LEN];     // 94-105: Address in memory of kernel (start of image)
	unsigned char flashRootLength[IMAGE_LEN];       // 106-115: Size of rootfs + deadcode (web flash uses this + kernelLength to determine the size of the kernel+rootfs flash image)
 	unsigned char kernelAddress[ADDRESS_LEN];       // 116-127: Address in memory of kernel
	unsigned char kernelLength[IMAGE_LEN];          // 128-137: Size of kernel
	unsigned char dualImage[2];                     // 138-139: Unused at present
	unsigned char inactiveFlag[2];                  // 140-141: Unused at present
        unsigned char information1[TAGINFO_LEN];        // 142-161: Unused at present
        unsigned char tagId[TAGID_LEN];                 // 162-167: Identifies which type of tag this is, currently two-letter company code, and then three digits for version of broadcom code in which this tag was first introduced
        unsigned char tagIdCRC[4];                      // 168-173: CRC32 to ensure validity of tagId
        unsigned char rootAddress[ADDRESS_LEN];         // 174-183: Address in memory of rootfs partition
        unsigned char rootLength[IMAGE_LEN];            // 184-193: Size of rootfs partition
	unsigned char reserved1[22];                    // 194-215: Reserved area not in use
	unsigned char imageCRC[4];                      // 216-219: CRC32 of images
        unsigned char reserved2[16];                    // 220-235: Unused at present
        unsigned char headerCRC[4];                     // 236-239: CRC32 of header excluding tagVersion
        unsigned char reserved3[16];                    // 240-255: Unused at present
};

struct bcm_tag_ag306 {
	unsigned char tagVersion[TAGVER_LEN];           // 0-3: Version of the image tag
	unsigned char sig_1[20];                        // 4-23: Company Line 1
	unsigned char sig_2[14];                        // 24-37: Company Line 2
	unsigned char chipid[6];                        // 38-43: Chip this image is for
	unsigned char boardid[16];                      // 44-59: Board name
	unsigned char big_endian[2];                    // 60-61: Map endianness -- 1 BE 0 LE
	unsigned char totalLength[IMAGE_LEN];           // 62-71: Total length of image
	unsigned char cfeAddress[ADDRESS_LEN];          // 72-83: Address in memory of CFE
	unsigned char cfeLength[IMAGE_LEN];             // 84-93: Size of CFE
	unsigned char flashImageStart[ADDRESS_LEN];     // 94-105: Address in memory of kernel (start of image)
	unsigned char flashRootLength[IMAGE_LEN];       // 106-115: Size of rootfs + deadcode (web flash uses this + kernelLength to determine the size of the kernel+rootfs flash image)
 	unsigned char kernelAddress[ADDRESS_LEN];       // 116-127: Address in memory of kernel
	unsigned char kernelLength[IMAGE_LEN];          // 128-137: Size of kernel
	unsigned char dualImage[2];                     // 138-139: Unused at present
	unsigned char inactiveFlag[2];                  // 140-141: Unused at present
        unsigned char information1[TAGINFO_LEN];        // 142-161: Unused at present
	unsigned char information2[54];                 // 162-215: Compilation and related information (not generated/used by OpenWRT)
	unsigned char kernelCRC[4] ;                    // 216-219: CRC32 of images
        unsigned char rootAddress[ADDRESS_LEN];         // 220-231: Address in memory of rootfs partition
        unsigned char tagIdCRC[4];                      // 232-235: Checksum to ensure validity of tagId
        unsigned char headerCRC[4];                     // 236-239: CRC32 of header excluding tagVersion
	unsigned char rootLength[IMAGE_LEN];            // 240-249: Size of rootfs
        unsigned char tagId[TAGID_LEN];                 // 250-255: Identifies which type of tag this is, currently two-letter company code, and then three digits for version of broadcom code in which this tag was first introduced
};

struct bcm_tag_bc221 {
	unsigned char tagVersion[TAGVER_LEN];           // 0-3: Version of the image tag
	unsigned char sig_1[20];                        // 4-23: Company Line 1
	unsigned char sig_2[14];                        // 24-37: Company Line 2
	unsigned char chipid[6];                        // 38-43: Chip this image is for
	unsigned char boardid[16];                      // 44-59: Board name
	unsigned char big_endian[2];                    // 60-61: Map endianness -- 1 BE 0 LE
	unsigned char totalLength[IMAGE_LEN];           // 62-71: Total length of image
	unsigned char cfeAddress[ADDRESS_LEN];          // 72-83: Address in memory of CFE
	unsigned char cfeLength[IMAGE_LEN];             // 84-93: Size of CFE
	unsigned char flashImageStart[ADDRESS_LEN];     // 94-105: Address in memory of kernel (start of image)
	unsigned char flashRootLength[IMAGE_LEN];       // 106-115: Size of rootfs + deadcode (web flash uses this + kernelLength to determine the size of the kernel+rootfs flash image)
 	unsigned char kernelAddress[ADDRESS_LEN];       // 116-127: Address in memory of kernel
	unsigned char kernelLength[IMAGE_LEN];          // 128-137: Size of kernel
	unsigned char dualImage[2];                     // 138-139: Unused at present
	unsigned char inactiveFlag[2];                  // 140-141: Unused at present
        unsigned char rsa_signature[TAGINFO_LEN];       // 142-161: RSA Signature (unused at present; some vendors may use this)
        unsigned char reserved5[2];                     // 162-163: Unused at present
        unsigned char tagId[TAGID_LEN];                 // 164-169: Identifies which type of tag this is, currently two-letter company code, and then three digits for version of broadcom code in which this tag was first introduced
        unsigned char rootAddress[ADDRESS_LEN];         // 170-181: Address in memory of rootfs partition
        unsigned char rootLength[IMAGE_LEN];            // 182-191: Size of rootfs partition
        unsigned char flashLayoutVer[4];                // 192-195: Version flash layout
        unsigned char fskernelCRC[4];                   // 196-199: Guessed to be kernel CRC
        unsigned char reserved4[16];                    // 200-215: Reserved area; unused at present
	unsigned char imageCRC[4];                      // 216-219: CRC32 of images
        unsigned char reserved2[12];                    // 220-231: Unused at present
        unsigned char tagIdCRC[4];                      // 232-235: CRC32 to ensure validity of tagId
        unsigned char headerCRC[4];                     // 236-239: CRC32 of header excluding tagVersion
        unsigned char reserved3[16];                    // 240-255: Unused at present
};

struct bcm_tag_bc310 {
	unsigned char tagVersion[4];                    // 0-3: Version of the image tag
	unsigned char sig_1[20];                        // 4-23: Company Line 1
	unsigned char sig_2[14];                        // 24-37: Company Line 2
	unsigned char chipid[6];                        // 38-43: Chip this image is for
	unsigned char boardid[16];                      // 44-59: Board name
	unsigned char big_endian[2];                    // 60-61: Map endianness -- 1 BE 0 LE
	unsigned char totalLength[IMAGE_LEN];           // 62-71: Total length of image
	unsigned char cfeAddress[ADDRESS_LEN];          // 72-83: Address in memory of CFE
	unsigned char cfeLength[IMAGE_LEN];             // 84-93: Size of CFE
	unsigned char flashImageStart[ADDRESS_LEN];     // 94-105: Address in memory of kernel (start of image)
	unsigned char flashRootLength[IMAGE_LEN];       // 106-115: Size of rootfs + deadcode (web flash uses this + kernelLength to determine the size of the kernel+rootfs flash image)
 	unsigned char kernelAddress[ADDRESS_LEN];       // 116-127: Address in memory of kernel
	unsigned char kernelLength[IMAGE_LEN];          // 128-137: Size of kernel
	unsigned char dualImage[2];                     // 138-139: Unused at present
	unsigned char inactiveFlag[2];                  // 140-141: Unused at present
        unsigned char information1[TAGINFO_LEN];        // 142-161: Unused at present; Some vendors use this for optional information
        unsigned char tagId[6];                         // 162-167: Identifies which type of tag this is, currently two-letter company code, and then three digits for version of broadcom code in which this tag was first introduced
        unsigned char tagIdCRC[4];                      // 168-171: CRC32 to ensure validity of tagId
        unsigned char rootAddress[ADDRESS_LEN];         // 172-183: Address in memory of rootfs partition
        unsigned char rootLength[IMAGE_LEN];            // 184-193: Size of rootfs partition
	unsigned char reserved1[22];                    // 193-215: Reserved area not in use
	unsigned char imageCRC[4];                      // 216-219: CRC32 of images
        unsigned char rootfsCRC[4];                     // 220-227: CRC32 of rootfs partition
        unsigned char kernelCRC[4];                     // 224-227: CRC32 of kernel partition
        unsigned char reserved2[8];                     // 228-235: Unused at present
        unsigned char headerCRC[4];                     // 235-239: CRC32 of header excluding tagVersion
        unsigned char reserved3[16];                    // 240-255: Unused at present
};

union bcm_tag {
  struct bcm_tag_bccfe bccfe;
  struct bcm_tag_bc300 bc300;
  struct bcm_tag_ag306 ag306;
  struct bcm_tag_bc221 bc221;
  struct bcm_tag_bc310 bc310;
};

#endif /* __BCM63XX_TAG_H */
