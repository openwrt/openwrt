#ifndef __QFIREHOSE_USB_LINUX_H__
#define __QFIREHOSE_USB_LINUX_H__
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

#define MAX_PATH 256
#define MIN(a,b)	 ((a) < (b)? (a) : (b))

typedef struct module_sys_info {
/*
MAJOR=189
MINOR=1
DEVNAME=bus/usb/001/002
DEVTYPE=usb_device
DRIVER=usb
PRODUCT=2c7c/415/318
TYPE=239/2/1
BUSNUM=001
*/
    //char sys_path[MAX_PATH];
    int MAJOR;
    int MINOR;
    char DEVNAME[64];
    char DEVTYPE[64];
    char PRODUCT[64];
}MODULE_SYS_INFO;

void * qusb_noblock_open(const char *module_sys_path, int *idVendor, int *idProduct, int *interfaceNum);
int qusb_noblock_close(void *handle);
int qusb_noblock_write(const void *handle, void *pbuf, int max_size, int min_size, int timeout_msec, int need_zlp);
int qusb_noblock_read(const void *handle, void *pbuf, int max_size, int min_size, int timeout_msec);
int qfile_find_file(const char *dir, const char *prefix, const char *suffix, char** filename);
#define errno_nodev() (errno == ENOENT || errno == ENODEV)
// void dbg_time (const char *fmt, ...);
const char * firehose_get_time(void);
extern FILE *loghandler;
#ifdef FH_DEBUG
#define dbg_time(fmt, args...)                                                                           \
    do                                                                                                   \
    {                                                                                                    \
        fprintf(stdout, "[%15s-%04d]%s: " fmt, __FILE__, __LINE__, firehose_get_time(), ##args);         \
        fflush(stdout);                                                                                  \
        if (loghandler)                                                                                  \
            fprintf(loghandler, "[%15s-%04d]%s: " fmt, __FILE__, __LINE__, firehose_get_time(), ##args); \
    } while (0);
#else
#define dbg_time(fmt, args...)                                            \
    do                                                                    \
    {                                                                     \
        fprintf(stdout, "%s: " fmt, firehose_get_time(), ##args);         \
        fflush(stdout);                                                   \
        if (loghandler)                                                   \
            fprintf(loghandler, "%s: " fmt, firehose_get_time(), ##args); \
    } while (0);
#endif
double get_now();
void get_duration(double start);
int update_transfer_bytes(long long bytes_cur);
void set_transfer_allbytes(long long bytes);
int auto_find_quectel_modules(char *module_sys_path, unsigned size);
void quectel_get_syspath_name_by_ttyport(const char *module_port_name, char *module_sys_path, unsigned size);
void quectel_get_ttyport_by_syspath(const char *module_sys_path, char *module_port_name, unsigned size);
#define error_return()  do {dbg_time("%s %s %d fail\n", __FILE__, __func__, __LINE__); return __LINE__; } while(0)

extern int edl_pcie_mhifd;

#define IOCTL_BHI_GETDEVINFO 0x8BE0 + 1
#define IOCTL_BHI_WRITEIMAGE 0x8BE0 + 2

typedef unsigned int ULONG;

typedef struct _bhi_info_type
{
   ULONG bhi_ver_minor;
   ULONG bhi_ver_major;
   ULONG bhi_image_address_low;
   ULONG bhi_image_address_high;
   ULONG bhi_image_size;
   ULONG bhi_rsvd1;
   ULONG bhi_imgtxdb;
   ULONG bhi_rsvd2;
   ULONG bhi_msivec;
   ULONG bhi_rsvd3;
   ULONG bhi_ee;
   ULONG bhi_status;
   ULONG bhi_errorcode;
   ULONG bhi_errdbg1;
   ULONG bhi_errdbg2;
   ULONG bhi_errdbg3;
   ULONG bhi_sernum;
   ULONG bhi_sblantirollbackver;
   ULONG bhi_numsegs;
   ULONG bhi_msmhwid[6];
   ULONG bhi_oempkhash[48];
   ULONG bhi_rsvd5;
}BHI_INFO_TYPE, *PBHI_INFO_TYPE;

enum MHI_EE {
   MHI_EE_PBL  = 0x0,            /* Primary Boot Loader                                                */
   MHI_EE_SBL  = 0x1,            /* Secondary Boot Loader                                              */
   MHI_EE_AMSS = 0x2,            /* AMSS Firmware                                                      */
   MHI_EE_RDDM = 0x3,            /* WIFI Ram Dump Debug Module                                         */
   MHI_EE_WFW  = 0x4,            /* WIFI (WLAN) Firmware                                               */
   MHI_EE_PT   = 0x5,            /* PassThrough, Non PCIe BOOT (PCIe is BIOS locked, not used for boot */
   MHI_EE_EDL  = 0x6,            /* PCIe enabled in PBL for emergency download (Non PCIe BOOT)         */
   MHI_EE_FP   = 0x7,            /* FlashProg, Flash Programmer Environment                            */
   MHI_EE_BHIE = MHI_EE_FP,
   MHI_EE_UEFI = 0x8,            /* UEFI                                                               */

   MHI_EE_DISABLE_TRANSITION = 0x9,
   MHI_EE_MAX
};
int qpcie_open(const char *firehose_dir, const char *firehose_mbn);

#define Q_USB2TCP_VERSION 0x12345678
typedef struct {
    int tag;
    int length;
    int value[];
} TLV;

typedef struct {
    int tag;
    int length;
    int idVendor;
    int idProduct;
    int interfaceNum;
} TLV_USB;
#endif
