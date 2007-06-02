/* boot_images.c - Collection of the different boot images
 * Johan Adolfsson Axis Communications AB 
 *  $Id: boot_images.c,v 1.1 2004/01/26 10:52:40 mikaelp Exp $ 
 */

#include <stdio.h>
#include "boot_images.h"

/* We include the C files here to the size etc. easy */
#include "net_ima.c"
#include "ser_ima.c"
#include "net_noleds_ima.c"
#include "ser_noleds_ima.c"

struct boot_image_info_type boot_image_info[] = 
{
  { INTERNAL_NW,  net_ima, sizeof net_ima, "Network boot (default)"},
  { INTERNAL_SER, ser_ima, sizeof ser_ima, "Serial boot"},
  { INTERNAL_SER_NOLEDS, ser_noleds_ima, sizeof ser_noleds_ima, "Serial boot, no leds"},
  { INTERNAL_NW_NOLEDS,  net_noleds_ima, sizeof net_noleds_ima, "Network boot, no leds"},

  { "DBGNONE", net_ima, sizeof net_ima, "Obsolete (use INTERNAL_NW instead)" },
  { "DBG0",    net_ima, sizeof net_ima, "Obsolete (use INTERNAL_NW instead)" },
  { "DBG1",    net_ima, sizeof net_ima, "Obsolete (use INTERNAL_NW instead)" },
  { "DBG2",    net_ima, sizeof net_ima, "Obsolete (use INTERNAL_NW instead)" },
  { "DBG3",    net_ima, sizeof net_ima, "Obsolete (use INTERNAL_NW instead)" },

  { "DBGNONE_NOLEDS", net_noleds_ima, sizeof net_noleds_ima, "Obsolete (use INTERNAL_NW_NOLEDS instead)" },
  { "DBG0_NOLEDS",    net_noleds_ima, sizeof net_noleds_ima, "Obsolete (use INTERNAL_NW_NOLEDS instead)" },
  { "DBG1_NOLEDS",    net_noleds_ima, sizeof net_noleds_ima, "Obsolete (use INTERNAL_NW_NOLEDS instead)" },
  { "DBG2_NOLEDS",    net_noleds_ima, sizeof net_noleds_ima, "Obsolete (use INTERNAL_NW_NOLEDS instead)" },
  { "DBG3_NOLEDS",    net_noleds_ima, sizeof net_noleds_ima, "Obsolete (use INTERNAL_NW_NOLEDS instead)" },

  { NULL, NULL, 0, NULL } /* End of array */
};
