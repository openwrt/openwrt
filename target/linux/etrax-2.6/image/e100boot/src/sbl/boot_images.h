/* boot_images.h - Collection of the different boot images
 * Johan Adolfsson Axis Communications AB 
 *  $Id: boot_images.h,v 1.2 2002/07/01 14:37:51 pkj Exp $ 
 */

#define INTERNAL_SER "INTERNAL_SER"
#define INTERNAL_NW  "INTERNAL_NW"
#define INTERNAL_NW_NOLEDS  "INTERNAL_NW_NOLEDS"
#define INTERNAL_SER_NOLEDS  "INTERNAL_SER_NOLEDS"

typedef struct boot_image_info_type
{
  const char *name;
  const char *ptr;
  unsigned long len;
  const char *info;
} boot_image_info_type;

extern struct  boot_image_info_type boot_image_info[];

