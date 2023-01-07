/******************************************************************************
  @file    usb_linux.c
  @brief   read and write usb devices.

  DESCRIPTION
  QFirehoe Tool for USB and PCIE of Quectel wireless cellular modules.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  ---------------------------------------------------------------------------
  Copyright (c) 2016 - 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <termios.h>
#include <dirent.h>
#include <pthread.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <netinet/in.h>
#include <linux/un.h>
#include <linux/usbdevice_fs.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 20)
#include <linux/usb/ch9.h>
#else
#include <linux/usb_ch9.h>
#endif
#include <sys/time.h>
#include <stdarg.h>
#include <sys/sysmacros.h>
#include "usb_linux.h"

int edl_pcie_mhifd = -1;
int switch_to_edl_mode(void *usb_handle);

extern uint32_t      inet_addr(const char *);

#define MAX_USBFS_BULK_IN_SIZE (4 * 1024)
#define EC20_MAX_INF 4
#define MKDEV(__ma, __mi) (((__ma & 0xfff) << 8) | (__mi & 0xff) | ((__mi & 0xfff00) << 12))

struct quectel_usb_device {
    char devname[64];
    int desc;
    int ttyfd;
    int idVendor;
    int idProduct;
    uint8_t bNumInterfaces;
    uint8_t intr_ep[EC20_MAX_INF];
    uint8_t bulk_ep_in[EC20_MAX_INF];
    uint8_t bulk_ep_out[EC20_MAX_INF];
    int wMaxPacketSize[EC20_MAX_INF];
    int control[EC20_MAX_INF][2];
};

static struct quectel_usb_device quectel_9x07;
static int tcp_socket_fd = -1;
static int usb_dm_interface = 0;

static int strStartsWith(const char *line, const char *prefix)
{
    if (!prefix || prefix[0] == '\0')
        return 1;

    for ( ; *line != '\0' && *prefix != '\0' ; line++, prefix++) {
        if (*line != *prefix) {
            return 0;
        }
    }

    return *prefix == '\0';
}

static int strEndsWith(const char *line, const char *suffix)
{
    size_t a, b;

    if (!suffix || suffix[0] == '\0')
        return 1;

    a = strlen(line);
    b = strlen(suffix);
    return (a >= b) && (strcmp(line + (a -b), suffix) == 0);
}

static int quectel_get_sysinfo_by_uevent(const char *uevent, MODULE_SYS_INFO *pSysInfo) {
    FILE *fp;
    char line[MAX_PATH];

    memset(pSysInfo, 0x00, sizeof(MODULE_SYS_INFO));

    fp = fopen(uevent, "r");
    if (fp == NULL) {
        dbg_time("fail to fopen %s, errno: %d (%s)\n", uevent, errno, strerror(errno));
        return 0;
    }

    //dbg_time("%s\n", uevent);
    while (fgets(line, sizeof(line), fp)) {
        if (line[strlen(line) - 1] == '\n' || line[strlen(line) - 1] == '\r') {
            line[strlen(line) - 1] = '\0';
        }

        //dbg_time("%s\n", line);
        if (strStartsWith(line, "MAJOR=")) {
            	pSysInfo->MAJOR = atoi(&line[strlen("MAJOR=")]);
        }
        else if (strStartsWith(line, "MINOR=")) {
            	pSysInfo->MINOR = atoi(&line[strlen("MINOR=")]);
        }
        else if (strStartsWith(line, "DEVNAME=")) {
            if(pSysInfo)
            	strncpy(pSysInfo->DEVNAME, &line[strlen("DEVNAME=")], sizeof(pSysInfo->DEVNAME));
        }
        else if (strStartsWith(line, "DEVTYPE=")) {
            strncpy(pSysInfo->DEVTYPE, &line[strlen("DEVTYPE=")], sizeof(pSysInfo->DEVTYPE));
        }
        else if (strStartsWith(line, "PRODUCT=")) {
            strncpy(pSysInfo->PRODUCT, &line[strlen("PRODUCT=")], sizeof(pSysInfo->PRODUCT));
        }
    }

    fclose(fp);

    return 1;
}

// the return value is the number of quectel modules
int auto_find_quectel_modules(char *module_sys_path, unsigned size)
{
    const char *base = "/sys/bus/usb/devices";
    DIR *busdir = NULL;
    struct dirent *de = NULL;
    int count = 0;

    busdir = opendir(base);
    if (busdir == NULL)
        return -1;

    while ((de = readdir(busdir))) {
        static char uevent[MAX_PATH];
        static MODULE_SYS_INFO sysinfo;

        if (!isdigit(de->d_name[0])) continue;

        snprintf(uevent, sizeof(uevent), "%.24s/%.16s/uevent", base, de->d_name);
        if (!quectel_get_sysinfo_by_uevent(uevent, &sysinfo))
            continue;

        if (sysinfo.MAJOR != 189)
            continue;

        //dbg_time("MAJOR=%d, MINOR=%d, DEVNAME=%s, DEVTYPE=%s, PRODUCT=%s\n",
        //    sysinfo.MAJOR, sysinfo.MINOR, sysinfo.DEVNAME, sysinfo.DEVTYPE, sysinfo.PRODUCT);

        if (sysinfo.DEVTYPE[0] == '\0' || strStartsWith(sysinfo.DEVTYPE, "usb_device") == 0)
            continue;

        if (sysinfo.PRODUCT[0] == '\0') {
            continue;
        }

        if (!(strStartsWith(sysinfo.PRODUCT, "2c7c/")
            || strStartsWith(sysinfo.PRODUCT, "5c6/9008")
            || strStartsWith(sysinfo.PRODUCT, "5c6/901f")
            || strStartsWith(sysinfo.PRODUCT, "5c6/9091")
            || strStartsWith(sysinfo.PRODUCT, "3763/3c93/318"))) {
            continue;
        }

        if ((strStartsWith(sysinfo.PRODUCT, "2c7c/6") || strStartsWith(sysinfo.PRODUCT, "2c7c/8"))
            && (sysinfo.PRODUCT[strlen("2c7c/6000")] == '/')) //skip ASR&HISI modules
            continue;

        snprintf(module_sys_path, size, "%.24s/%s", base, de->d_name);
        count++;
        dbg_time("[%d] %s %s\n", count, module_sys_path, sysinfo.PRODUCT);
    }

    closedir(busdir);

    return count;
}

void quectel_get_ttyport_by_syspath(const char *module_sys_path, char *module_port_name, unsigned size) {
    char infname[256];
    DIR *infdir = NULL;
    struct dirent *de = NULL;

    module_port_name[0] = '\0';

    sprintf(infname, "%s:1.%d", module_sys_path, usb_dm_interface);
    infdir = opendir(infname);
    if (infdir == NULL)
        return;

    while ((de = readdir(infdir))) {
        if (strStartsWith(de->d_name, "ttyUSB")) {
            snprintf(module_port_name, size, "/dev/%s", de->d_name);
            break;
        }
        else if (!strncmp(de->d_name, "tty", strlen("tty"))) {
            sprintf(infname, "%s:1.%d/tty", module_sys_path, usb_dm_interface);
            closedir(infdir);
            infdir = opendir(infname);
            if (infdir == NULL)
                break;
        }
    }

    if (infdir) closedir(infdir);
}

static void quectel_fixup_sysport(const char *module_port_name, char *sysport, unsigned size) {
    char syspath[MAX_PATH+16];
    const char *sys_base = "/sys/class/tty";
    DIR *sys_dir = NULL;
    struct dirent *dev = NULL;

    sysport[0] = '\0';
    sys_dir = opendir(sys_base);
    if (!sys_dir) {
        dbg_time("fail to opendir('%s'), errno: %d (%s)\n", sys_base, errno, strerror(errno));
        return;
    }

    while (NULL != (dev = readdir(sys_dir)))
    {
        if (!strncasecmp("ttyUSB", dev->d_name, strlen("ttyUSB"))) {
            MODULE_SYS_INFO sysinfo;

            snprintf(syspath, sizeof(syspath), "%.24s/%.16s/uevent", sys_base, dev->d_name);
            if (quectel_get_sysinfo_by_uevent(syspath, &sysinfo)) {
                struct stat buf;
                dev_t devt;

                devt = makedev(sysinfo.MAJOR, sysinfo.MINOR);
                if(!stat(module_port_name, &buf) && buf.st_rdev == devt) {
                	snprintf(sysport, size, "/sys/class/tty/%.16s", dev->d_name);
            		break;
                }
            }
        }
    }
    closedir(sys_dir);
}


void quectel_get_syspath_name_by_ttyport(const char *module_port_name, char *module_sys_path, unsigned size) {
    char syspath[MAX_PATH];
    char sysport[64];
    int count;
    char *pchar = NULL;
    char dm_tty[24];

    snprintf(dm_tty, sizeof(dm_tty), ":1.%d/tty", usb_dm_interface);
    module_sys_path[0] = '\0';

    snprintf(sysport, sizeof(sysport), "/sys/class/tty/%.48s", &module_port_name[strlen("/dev/")]);
    if(access(sysport, F_OK) && errno == ENOENT) {
		quectel_fixup_sysport(module_port_name, sysport, sizeof(sysport));//query real name
    }
    if(access(sysport, F_OK) && errno == ENOENT)
    	return;
    count = readlink(sysport, syspath, sizeof(syspath) - 1);
    if (count < (int)strlen(dm_tty))
        return;

//ttyUSB0 -> ../../devices/soc0/soc/2100000.aips-bus/2184200.usb/ci_hdrc.1/usb1/1-1/1-1:1.0/ttyUSB0/tty/ttyUSB0
    pchar = strstr(syspath, dm_tty);
    if (pchar == NULL)
        return;

    *pchar = '\0';
    while (*pchar != '/')
        pchar--;

    snprintf(module_sys_path, size, "/sys/bus/usb/devices/%.232s", pchar + 1);
}

static void quectel_get_usb_device_info(const char *module_sys_path, struct quectel_usb_device *udev) {
    static unsigned char devdesc[1024];
    size_t desclength, len;
    char devname[MAX_PATH];
    int desc_fd;
    __u8 bInterfaceNumber = 0;
    int dev_mknod_and_delete_after_use = 0;

    MODULE_SYS_INFO sysinfo;
    snprintf(devname, sizeof(devname), "%.248s/%s", module_sys_path, "uevent");
    if (!quectel_get_sysinfo_by_uevent(devname, &sysinfo))
        return;

    snprintf(devname, sizeof(devname), "/dev/%s", sysinfo.DEVNAME);
    if (access(devname, R_OK) && errno_nodev()) {
        //maybe Linux have create /sys/ device, but not ready to create /dev/ device.
        usleep(100*1000);
    }

    if (access(devname, R_OK) && errno_nodev())
    {
        char *p = strstr(devname+strlen("/dev/"), "/");

        while (p) {
            p[0] = '_';
            p = strstr(p, "/");
        }

        if (mknod(devname, S_IFCHR|0666, MKDEV(sysinfo.MAJOR, sysinfo.MINOR))) {
            devname[1] = 't';
            devname[2] = 'm';
            devname[3] = 'p';

            if (mknod(devname, S_IFCHR|0666, MKDEV(sysinfo.MAJOR, sysinfo.MINOR))) {
                dbg_time("Fail to mknod %s, errno : %d (%s)\n", devname, errno, strerror(errno));
                return;
            }
        }

        dev_mknod_and_delete_after_use = 1;
    }

    desc_fd = open(devname, O_RDWR | O_NOCTTY);

    if (dev_mknod_and_delete_after_use) {
        remove(devname);
    }

    if (desc_fd <= 0) {
        dbg_time("fail to open %s, errno: %d (%s)\n", devname, errno, strerror(errno));
        return;
    }

    desclength = read(desc_fd, devdesc, sizeof(devdesc));
    len = 0;
    while (len < desclength) {
        struct usb_descriptor_header *h = (struct usb_descriptor_header *)(&devdesc[len]);

        if (h->bLength == sizeof(struct usb_device_descriptor) && h->bDescriptorType == USB_DT_DEVICE) {
            struct usb_device_descriptor *device = (struct usb_device_descriptor *)h;

            udev->idVendor = device->idVendor;
            udev->idProduct = device->idProduct;
            dbg_time("P: %s idVendor=%04x idProduct=%04x\n", devname, device->idVendor, device->idProduct);
        }
        else if (h->bLength == sizeof(struct usb_config_descriptor) && h->bDescriptorType == USB_DT_CONFIG) {
            struct usb_config_descriptor *config = (struct usb_config_descriptor *)h;

            dbg_time("C: %s bNumInterfaces: %d\n", devname, config->bNumInterfaces);
            udev->bNumInterfaces = config->bNumInterfaces;
        }
        else if (h->bLength == sizeof(struct usb_interface_descriptor) && h->bDescriptorType == USB_DT_INTERFACE) {
            struct usb_interface_descriptor *interface = (struct usb_interface_descriptor *)h;

            dbg_time("I: If#= %d Alt= %d #EPs= %d Cls=%02x Sub=%02x Prot=%02x\n",
                interface->bInterfaceNumber, interface->bAlternateSetting, interface->bNumEndpoints,
                interface->bInterfaceClass, interface->bInterfaceSubClass, interface->bInterfaceProtocol);
            bInterfaceNumber = interface->bInterfaceNumber;
        }
        else if (h->bLength == USB_DT_ENDPOINT_SIZE && h->bDescriptorType == USB_DT_ENDPOINT) {
            if (bInterfaceNumber < EC20_MAX_INF) {
                struct usb_endpoint_descriptor *endpoint = (struct usb_endpoint_descriptor *)h;

                dbg_time("E: Ad=%02x Atr=%02x MxPS= %d Ivl=%dms\n",
                    endpoint->bEndpointAddress, endpoint->bmAttributes, endpoint->wMaxPacketSize, endpoint->bInterval);

                if ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK) {
                    if (endpoint->bEndpointAddress & USB_ENDPOINT_DIR_MASK)
                        udev->bulk_ep_in[bInterfaceNumber] = endpoint->bEndpointAddress;
                    else
                        udev->bulk_ep_out[bInterfaceNumber] = endpoint->bEndpointAddress;
                    udev->wMaxPacketSize[bInterfaceNumber] = endpoint->wMaxPacketSize;
                } else if ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_INT){
                    udev->intr_ep[bInterfaceNumber] = endpoint->bEndpointAddress;
                }
            }
        } else {
        }

        len += h->bLength;
    }

    if (len == desclength) {
        strcpy(udev->devname, devname);
        udev->desc = desc_fd;
    }

    usb_dm_interface = 0;
    if ((udev->bulk_ep_in[usb_dm_interface] == 0 && udev->bulk_ep_in[usb_dm_interface] == 0)
        || (udev->idVendor == 0x2c7c && udev->idProduct == 0x0127)    //EM05CEFC-LNV  Laptop
        || (udev->idVendor == 0x2c7c && udev->idProduct == 0x0514)    //EG060K-EA
        || (udev->idVendor == 0x2c7c && udev->idProduct == 0x0310)    //EM05-CN       Laptop
        || (udev->idVendor == 0x2c7c && udev->idProduct == 0x030a)    //EM05-G        Laptop
        || (udev->idVendor == 0x2c7c && udev->idProduct == 0x0309)    //EM05E-EDU     Laptop
        || (udev->idVendor == 0x2c7c && udev->idProduct == 0x030d))   //EM05G-FCCL    Laptop
    {

        if ((udev->idVendor == 0x2c7c && udev->idProduct == 0x0127)       //EM05CEFC-LNV  Laptop
            || (udev->idVendor == 0x2c7c && udev->idProduct == 0x0310)    //EM05-CN       Laptop
            || (udev->idVendor == 0x2c7c && udev->idProduct == 0x030a)    //EM05-G        Laptop
            || (udev->idVendor == 0x2c7c && udev->idProduct == 0x0309)    //EM05E-EDU     Laptop
            || (udev->idVendor == 0x2c7c && udev->idProduct == 0x030d))   //EM05G-FCCL    Laptop
            usb_dm_interface = 3;

        if (udev->idVendor == 0x2c7c && udev->idProduct == 0x0514)   //EG060K-EA
            usb_dm_interface = 2;
    }
}

static int usbfs_bulk_write(struct quectel_usb_device *udev, const void *data, int len, int timeout_msec, int need_zlp) {
    struct usbdevfs_urb bulk;
    struct usbdevfs_urb *urb = &bulk;
    int n = -1;
    int bInterfaceNumber = usb_dm_interface;

    (void)timeout_msec;
    //if (urb->type == 0)
    {
        memset(urb, 0, sizeof(struct usbdevfs_urb));
        urb->type = USBDEVFS_URB_TYPE_BULK;
        urb->endpoint = udev->bulk_ep_out[bInterfaceNumber];
    }

    urb->status = -1;
    urb->buffer = (void *)data;
    urb->buffer_length = len;
    urb->usercontext = urb;

    if (need_zlp && (len%udev->wMaxPacketSize[bInterfaceNumber]) == 0) {
        //dbg_time("USBDEVFS_URB_ZERO_PACKET\n");
#ifndef USBDEVFS_URB_ZERO_PACKET
#define USBDEVFS_URB_ZERO_PACKET    0x40
#endif
        urb->flags = USBDEVFS_URB_ZERO_PACKET;
    } else {
        urb->flags = 0;
    }

    do {
        n = ioctl(udev->desc, USBDEVFS_SUBMITURB, urb);
    } while((n < 0) && (errno == EINTR));

    if (n != 0) {
        dbg_time("inf[%d] USBDEVFS_SUBMITURB %d/%d, errno = %d (%s)\n", bInterfaceNumber, n, urb->buffer_length, errno, strerror(errno));
        return -1;
    }

    do {
        urb = NULL;
        n = ioctl(udev->desc, USBDEVFS_REAPURB, &urb);
    } while((n < 0) && (errno == EINTR));

    if (n != 0) {
        dbg_time("inf[%d] ep_out %d/%d, errno = %d (%s)\n", bInterfaceNumber, n, urb->buffer_length, errno, strerror(errno));
    }

    //dbg_time("[ urb @%p status = %d, actual = %d ]\n", urb, urb->status, urb->actual_length);

    if (urb && urb->status == 0 && urb->actual_length)
        return urb->actual_length;

    return -1;
}

static int poll_wait(int poll_fd, short events, int timeout_msec) {
    struct pollfd pollfds[] = {{poll_fd, events, 0}};
    int ret;

    do {
        ret = poll(pollfds, 1, timeout_msec);
    } while(ret == -1 && errno == EINTR);

    if (ret == 1 && (pollfds[0].revents & (events)))
        return 0;
    else if (ret == 0) {//timeout
        dbg_time("poll_wait events=%s msec=%d timeout\n",
            (events & POLLIN) ? "POLLIN" : "POLLOUT", timeout_msec);
        return ETIMEDOUT;
    }

    return EIO;
}

static int usbfs_bulk_read(struct quectel_usb_device *udev, void *pbuf, int len, int timeout) {
    struct usbdevfs_bulktransfer bulk;
    int n = -1;
    int bInterfaceNumber = usb_dm_interface;

    if (len < 512) {
        dbg_time("%s len=%d is too short\n", __func__, len);
        return 0;
    }

    bulk.ep = udev->bulk_ep_in[bInterfaceNumber];
    bulk.len = (len > MAX_USBFS_BULK_IN_SIZE) ? MAX_USBFS_BULK_IN_SIZE : len;
    bulk.data = (void *)pbuf;
    bulk.timeout = timeout;

    n = ioctl(udev->desc, USBDEVFS_BULK, &bulk);
    if( n <= 0 ) {
        if (errno == ETIMEDOUT) {
            dbg_time("inf[%d] ep_in %d/%d, errno = %d (%s), timeout=%d\n", bInterfaceNumber, n, bulk.len, errno, strerror(errno), timeout);
            n = 0;
        }
        else
            dbg_time("inf[%d] ep_in %d/%d, errno = %d (%s)\n", bInterfaceNumber, n, bulk.len, errno, strerror(errno));
    }

    return n ;
}

static int qtcp_connect(const char *port_name, int *idVendor, int *idProduct, int *interfaceNum) {
    int fd = -1;
    char *tcp_host = strdup(port_name);
    char *tcp_port = strchr(tcp_host, ':');
    struct sockaddr_in sockaddr;
    TLV_USB tlv_usb;

    dbg_time("%s port_name = %s\n", __func__, port_name);

    if (tcp_port == NULL)
        return -1;

    *tcp_port++ = '\0';
    if (atoi(tcp_port) < 1 || atoi(tcp_port) > 0xFFFF)
        return -1;

     fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd <= 0) {
        dbg_time("Device could not be socket: Linux System Errno: %s\n", strerror (errno));
        return -1;
    }

    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(tcp_host);
    sockaddr.sin_port = htons(atoi(tcp_port));

    free(tcp_host);
    if (connect(fd, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) < 0) {
        close(fd);
        dbg_time("Device could not be connect: Linux System Errno: %s\n", strerror (errno));
        return -1;
    }

    //block read, untill usb2tcp tell me the usb device information
    memset(&tlv_usb, 0x00, sizeof(tlv_usb));
    if (read(fd, &tlv_usb, sizeof(tlv_usb)) == -1) { };
    *idVendor = tlv_usb.idVendor;
    *idProduct = tlv_usb.idProduct;
    *interfaceNum = tlv_usb.interfaceNum;

    dbg_time("idVendor=%04x, idProduct=%04x, interfaceNum=%d\n", *idVendor, *idProduct, *interfaceNum);

    return fd;
}

static int qtcp_read(int fd, void *pbuf, int size, int timeout_msec) {
    static TLV tlv = {Q_USB2TCP_VERSION, 0};
    int cur = 0;
    int len;

    if (tlv.length == 0) {
        len = read(fd, &tlv, sizeof(tlv));
        if (len != sizeof(tlv)) {
            dbg_time("%s read=%d, errno: %d (%s)\n", __func__, len, errno, strerror(errno));
            return 0;
        }

        if (tlv.tag != Q_USB2TCP_VERSION) {
            dbg_time("%s tlv->tag=0x%x\n", __func__, tlv.tag);
            return 0;
        }
    }

    if (size > tlv.length)
        size = tlv.length;
    tlv.length -= size;

    while (cur < size) {
        if (poll_wait(fd, POLLIN, timeout_msec))
            break;

        len = read(fd, (uint8_t *)pbuf+cur, size-cur);
        if (len > 0) {
            cur += len;
        }
        else {
            dbg_time("%s read=%d, errno: %d (%s)\n", __func__, len, errno, strerror(errno));
            break;
        }
    }

    if (cur != size) {
        dbg_time("%s cur=%d, size=%d\n", __func__, cur, size);
    }

    return cur;
}

static int qtcp_write(int fd, void*pbuf, int size, int timeout_msec) {
    TLV tlv = {Q_USB2TCP_VERSION, size};
    int cur = 0;
    int len;

    len = write(fd, &tlv, sizeof(tlv));
    if (len != sizeof(tlv)) {
        dbg_time("%s write=%d, errno: %d (%s)\n", __func__, len, errno, strerror(errno));
        return 0;
    }

    while (cur < size) {
        if (poll_wait(fd, POLLOUT, timeout_msec))
            break;

        len = write(fd, (uint8_t *)pbuf+cur, size-cur);
        if (len > 0) {
            cur += len;
        }
        else {
            dbg_time("%s write=%d, errno: %d (%s)\n", __func__, len, errno, strerror(errno));
            break;
        }
    }

    if (cur != size) {
        dbg_time("%s cur=%d, size=%d\n", __func__, cur, size);
    }

    return cur;
}

struct usbfs_getdriver
{
    unsigned int interface;
    char driver[255 + 1];
};

struct usbfs_ioctl
{
    int ifno;       /* interface 0..N ; negative numbers reserved */
    int ioctl_code; /* MUST encode size + direction of data so the
			 * macros in <asm/ioctl.h> give correct values */
    void *data;     /* param buffer (in, or out) */
};

#define IOCTL_USBFS_DISCONNECT	_IO('U', 22)
#define IOCTL_USBFS_CONNECT	_IO('U', 23)

int usbfs_is_kernel_driver_alive(int fd, int ifnum)
{
    struct usbfs_getdriver getdrv;
    getdrv.interface = ifnum;
    if (ioctl(fd, USBDEVFS_GETDRIVER, &getdrv) < 0) {
        if (errno != ENODATA)
            dbg_time("%s ioctl USBDEVFS_GETDRIVER failed, errno: %d (%s)\n", __func__, errno, strerror(errno));
        return 0;
    }
    dbg_time("%s find interface %d has match the driver %s\n", __func__, ifnum, getdrv.driver);
    return 1;
}

void usbfs_detach_kernel_driver(int fd, int ifnum)
{
    struct usbfs_ioctl operate;
    operate.data = NULL;
    operate.ifno = ifnum;
    operate.ioctl_code = IOCTL_USBFS_DISCONNECT;
    if (ioctl(fd, USBDEVFS_IOCTL, &operate) < 0) {
        dbg_time("%s detach kernel driver failed\n", __func__);
    } else {
        dbg_time("%s detach kernel driver success\n", __func__);
    }
}

#define KVERSION(j,n,p)	((j)*1000000 + (n)*1000 + (p))
static struct utsname utsname;	/* for the kernel version */
static int ql_get_kernel_version(void)
{
    int osmaj, osmin, ospatch;
    int kernel_version;

    uname(&utsname);
    osmaj = osmin = ospatch = 0;
    sscanf(utsname.release, "%d.%d.%d", &osmaj, &osmin, &ospatch);
    kernel_version = KVERSION(osmaj, osmin, ospatch);

    return kernel_version;
}

static int detect_xhci_usb_zero_packet_bug_not_fix(const char *module_sys_path) {
    char buf[256];
    int tmp;
    char *driver;

    tmp = snprintf(buf, sizeof(buf), "/sys/bus/usb/devices/usb%c/../driver", module_sys_path[strlen("/sys/bus/usb/devices/")]);
    driver = buf + (++tmp);
    *driver = '\0';

    tmp = readlink(buf, driver, sizeof(buf) - tmp);
    dbg_time("tmp=%s, driver=%s\n", buf, driver);
    if (tmp <= 0)
        return 0;

    if (!strstr(driver, "xhci"))
        return 0;

    tmp = ql_get_kernel_version();
    if (tmp >= KVERSION(4,3,0))
        return 0;

    dbg_time("WARNNING ON File:%s Function:%s Line:%d\n", __FILE__, __func__, __LINE__);
    dbg_time("The module attach to XHCI controller, but your kernel verison less than V4.3.0\n");
    dbg_time("Please make sure your kernel had apply patch 'usb: xhci: Add support for URB_ZERO_PACKET to bulk/sg transfers'\n");
    dbg_time("https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/drivers/usb/host/xhci-ring.c?id=4758dcd19a7d9ba9610b38fecb93f65f56f86346\n");
    sleep(2); //sleep 2 seconds, make sure FAE/customers can notice this warnning.

    return 1;
}

void *qusb_noblock_open(const char *module_sys_path, int *idVendor, int *idProduct, int *interfaceNum) {
    struct termios ios;
    int retval;
    int fd = -1;
    struct quectel_usb_device *udev = &quectel_9x07;

    *idVendor = *idProduct = *interfaceNum = 0;
    tcp_socket_fd = -1;

    if (module_sys_path && module_sys_path[0] == '/') {
        char port_name[64];

        memset(udev, 0, sizeof(struct quectel_usb_device));
        quectel_get_usb_device_info(module_sys_path, udev);
        if (udev->desc <= 0)
            return NULL;
        quectel_get_ttyport_by_syspath(module_sys_path, port_name, sizeof(port_name));
        detect_xhci_usb_zero_packet_bug_not_fix(module_sys_path);

        *idVendor = udev->idVendor;
        *idProduct = udev->idProduct;
        *interfaceNum = udev->bNumInterfaces;

        if (port_name[0] == '\0' || (port_name[0] != '\0' && access(port_name, R_OK)) || (udev->idVendor == 0x05c6 && udev->idProduct == 0x9008)) {
            int bInterfaceNumber = usb_dm_interface;

            if (usbfs_is_kernel_driver_alive(udev->desc, bInterfaceNumber)) {
                usbfs_detach_kernel_driver(udev->desc, bInterfaceNumber);
            }
            retval = ioctl(udev->desc, USBDEVFS_CLAIMINTERFACE, &bInterfaceNumber);
            if(retval != 0) {
                dbg_time("Fail to claim interface %d, errno: %d (%s)\n", bInterfaceNumber, errno, strerror(errno));
                if (udev->idVendor == 0x05c6) {
                    int n;
                    struct {
                        char infname[255 * 2];
                        char driver[255 * 2];
                    } *pl;
                    const char *driver = NULL;

                    pl = (typeof(pl)) malloc(sizeof(*pl));

                    snprintf(pl->infname, sizeof(pl->infname), "%.255s:1.%d/driver", module_sys_path, usb_dm_interface);
                    n = readlink(pl->infname, pl->driver, sizeof(pl->driver));
                    if (n > 0) {
                        pl->driver[n] = '\0';
                        while (pl->driver[n] != '/')
                            n--;
                        driver = (&pl->driver[n+1]);
                    }

                    dbg_time("Error: when module in 'Emergency download mode', should not register any usb driver\n");
                    if (driver)
                        dbg_time("Error: it register to usb driver ' %s ' now, should delete 05c6&9008 from the source file of this driver\n", driver);
                    if (driver && !strcmp(driver, "qcserial"))
                        dbg_time("Delete 05c6&9008 from 'drivers/usb/serial/qcserial.c' or disable qcserial from kernel config\n");
                    qusb_noblock_close(udev);
                    free(pl);
                }
                return NULL;
            }

            udev->ttyfd = -1;
            return udev;
        }
        else if (!access(port_name, R_OK)) {
            dbg_time("%s port_name = %s\n", __func__, port_name);

            fd = open (port_name, O_RDWR | O_SYNC);

            if (fd <= 0) {
                dbg_time("Device %s could not be open: Linux System Errno: %s", port_name, strerror (errno));
                return NULL;
            }

            retval = tcgetattr (fd, &ios);
            if (-1 == retval) {
                dbg_time("ermio settings could not be fetched Linux System Error:%s", strerror (errno));
                return NULL;
            }

            cfmakeraw (&ios);
            cfsetispeed(&ios, B115200);
            cfsetospeed(&ios, B115200);

            retval = tcsetattr (fd, TCSANOW, &ios);
            if (-1 == retval) {
                dbg_time("Device could not be configured: Linux System Errno: %s", strerror (errno));
            }
            udev->ttyfd = fd;
            fcntl(fd, F_SETFL, fcntl(fd,F_GETFL) | O_NONBLOCK);

            return udev;
        }
        else {
            dbg_time("fail to access %s errno: %d (%s)\n", port_name, errno, strerror(errno));
        }
    }
    else {
        fd = qtcp_connect(module_sys_path, idVendor, idProduct, interfaceNum);
        if (fd > 0) {
            tcp_socket_fd = fd;
            fcntl(fd, F_SETFL, fcntl(fd,F_GETFL) | O_NONBLOCK);
            return &tcp_socket_fd;
        }
    }

    return NULL;
}

int qusb_noblock_close(void *handle) {
    struct quectel_usb_device *udev = &quectel_9x07;

    if (handle == &tcp_socket_fd) {
         close(tcp_socket_fd);
         tcp_socket_fd = -1;
    } if (handle == udev && udev->ttyfd > 0) {
        close(udev->ttyfd);
        close(udev->desc);
    }
    else if (handle == udev && udev->desc > 0) {
        int bInterfaceNumber = usb_dm_interface;
        ioctl(udev->desc, USBDEVFS_RELEASEINTERFACE, &bInterfaceNumber);
        close(udev->desc);
    }
    else if (handle == &edl_pcie_mhifd && edl_pcie_mhifd > 0) {
        close(edl_pcie_mhifd); edl_pcie_mhifd = -1;
    }
    memset(udev, 0, sizeof(*udev));

    return 0;
}

int qusb_use_usbfs_interface(const void *handle) {
    struct quectel_usb_device *udev = &quectel_9x07;

    return (handle == udev && udev->ttyfd <= 0 && udev->desc > 0);
}

int qusb_noblock_read(const void *handle, void *pbuf, int max_size, int min_size, int timeout_msec) {
    struct quectel_usb_device *udev = &quectel_9x07;
    int cur = 0;
    int poll_ret = 0;

    if (min_size == 0)
        min_size = 1;
    if (timeout_msec == 0)
        timeout_msec = 3000;

#if 0 //depend on your worst net speed
    if (handle == &tcp_socket_fd) {
        if (timeout_msec > 1000) //before sahala&firebose, we allow read timeout occurs
            timeout_msec = 120*1000;
    }
#endif

    while (cur < min_size) {
        int len = 0;

        if (handle == &tcp_socket_fd) {
            if ((poll_ret = poll_wait(tcp_socket_fd, POLLIN, timeout_msec)))
                break;
            len = qtcp_read(tcp_socket_fd, (uint8_t *)pbuf+cur, max_size-cur, timeout_msec);
        }
        else if (handle == udev && udev->ttyfd > 0) {
            if ((poll_ret = poll_wait(udev->ttyfd, POLLIN, timeout_msec)))
                break;
            len = read(udev->ttyfd, (uint8_t *)pbuf+cur, max_size-cur);
        }
        else if (handle == udev && udev->desc > 0) {
            len = usbfs_bulk_read(udev, (uint8_t *)pbuf+cur, max_size-cur, timeout_msec);
        }
        else if (handle == &edl_pcie_mhifd && edl_pcie_mhifd > 0) {
            if ((poll_ret = poll_wait(edl_pcie_mhifd, POLLIN, timeout_msec)))
                break;
            len = read(edl_pcie_mhifd, (uint8_t *)pbuf+cur, max_size-cur);
        }
        else {
            break;
        }

        if (len > 0) {
            cur += len;
        } else {
            dbg_time("%s read=%d, errno: %d (%s)\n", __func__, len, errno, strerror(errno));
            break;
        }
    }

    if (poll_ret == EIO)
        return -1;
    else if (poll_ret == ETIMEDOUT)
        return cur;

    if (cur < min_size) {
        dbg_time("%s cur=%d, min_size=%d\n", __func__, cur, min_size);
    }

    return cur;
}

int qusb_noblock_write(const void *handle, void *pbuf, int max_size, int min_size, int timeout_msec, int need_zlp) {
    struct quectel_usb_device *udev = &quectel_9x07;
    int cur = 0;

    if (min_size == 0)
        min_size = 1;
    if (timeout_msec == 0)
        timeout_msec = 3000;

#if 0 //depend on your worst net speed
    if (handle == &tcp_socket_fd) {
        timeout_msec = 120*1000;
    }
#endif

    while (cur < min_size) {
        int len = 0;

        if (handle == &tcp_socket_fd) {
            if (poll_wait(tcp_socket_fd, POLLOUT, timeout_msec))
                break;
            len = qtcp_write(tcp_socket_fd, (uint8_t *)pbuf+cur, max_size-cur, timeout_msec);
        }
        else if (handle == udev && udev->ttyfd > 0) {
            if (poll_wait(udev->ttyfd, POLLOUT, timeout_msec))
                break;
            len = write(udev->ttyfd, (uint8_t *)pbuf+cur, max_size-cur);
        } else if (handle == udev && udev->desc > 0) {
            len = usbfs_bulk_write(udev, (uint8_t *)pbuf+cur, max_size-cur, timeout_msec, need_zlp);
        }
        else if (handle == &edl_pcie_mhifd && edl_pcie_mhifd > 0) {
            if (poll_wait(edl_pcie_mhifd, POLLOUT, timeout_msec))
                break;
            len = write(edl_pcie_mhifd, (uint8_t *)pbuf+cur, max_size-cur);
        }
        else {
            break;
        }

        if (len > 0) {
            cur += len;
        } else {
            dbg_time("%s write=%d, errno: %d (%s)\n", __func__, len, errno, strerror(errno));
            break;
        }
    }

    if (cur < min_size) {
        dbg_time("%s cur=%d, min_size=%d\n", __func__, cur, min_size);
    }

    return cur;
}

int qfile_find_file(const char *dir, const char *prefix, const char *suffix, char** filename) {
    DIR *pdir;
    struct dirent* ent = NULL;
    pdir = opendir(dir);

    *filename = NULL;
    if(pdir)
    {
        while((ent = readdir(pdir)) != NULL)
        {
            if (strStartsWith(ent->d_name, prefix) && strEndsWith(ent->d_name, suffix))
            {
                dbg_time("find '%s'\n", ent->d_name);
                *filename = strdup(ent->d_name);
                break;
            }
        }
    }

    closedir(pdir);
    return *filename != NULL;
}

const char * firehose_get_time(void) {
    static char time_buf[50];
    struct timeval  tv;
    static int s_start_msec = -1;
    int now_msec, cost_msec;

    gettimeofday (&tv, NULL);
    now_msec = tv.tv_sec * 1000;
    now_msec += (tv.tv_usec + 500) / 1000;

    if (s_start_msec == -1) {
        s_start_msec = now_msec;
    }

    cost_msec = now_msec - s_start_msec;

    sprintf(time_buf, "[%03d.%03d]", cost_msec/1000, cost_msec%1000);
    return time_buf;
}

// void dbg_time (const char *fmt, ...) {
//     va_list args;
//     va_start(args, fmt);
//     static char line[2048];
//     snprintf(line, sizeof(line), "%s ", firehose_get_time());
//     vsnprintf(line + strlen(line), sizeof(line) - strlen(line), fmt, args);
//     fprintf(stdout, "%s", line);
//     fflush(stdout);
// }

int qpcie_open(const char *firehose_dir, const char *firehose_mbn) {
    int bhifd, edlfd, diagfd;
    long ret;
    FILE *fp;
    BHI_INFO_TYPE *bhi_info = malloc(sizeof(BHI_INFO_TYPE));
    char prog_firehose_sdx24[256+32];
    size_t filesize;
    void *filebuf;

    snprintf(prog_firehose_sdx24, sizeof(prog_firehose_sdx24), "%.255s/%s", firehose_dir, firehose_mbn);
    fp = fopen(prog_firehose_sdx24, "rb");
    if (fp ==NULL) {
        dbg_time("fail to fopen %s, errno: %d (%s)\n", prog_firehose_sdx24, errno, strerror(errno));
        error_return();
    }

    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    filebuf = malloc(sizeof(filesize)+filesize);
    memcpy(filebuf, &filesize, sizeof(filesize));
    if (fread((uint8_t *)filebuf+sizeof(filesize), 1, filesize, fp) == (size_t)0) { };
    fclose(fp);

    diagfd = open("/dev/mhi_DIAG", O_RDWR | O_NOCTTY);
    if (diagfd > 0)
    {
       int edl_retry = 30; //SDX55 require long time by now 20190412
       void *usb_handle = &edl_pcie_mhifd;
       edl_pcie_mhifd = diagfd;

        while (access("/dev/mhi_DIAG", R_OK) == 0 && edl_retry-- > 0) {
            dbg_time("switch_to_edl_mode\n");
            switch_to_edl_mode(usb_handle);
            sleep(1);
        }

        close(diagfd);
        edl_pcie_mhifd = -1;
    }

    sleep(1); //see https://ticket.quectel.com/browse/FAE-39737
    bhifd = open("/dev/mhi_BHI", O_RDWR | O_NOCTTY);
    if (bhifd <= 0) {
        dbg_time("fail to open %s, errno: %d (%s)\n", "/dev/mhi_BHI", errno, strerror(errno));
        error_return();
    }

    ret = ioctl(bhifd, IOCTL_BHI_GETDEVINFO, bhi_info);
    if (ret) {
        dbg_time("fail to ioctl IOCTL_BHI_GETDEVINFO, errno: %d (%s)\n", errno, strerror(errno));
        error_return();
    }

    dbg_time("bhi_ee = %d\n", bhi_info->bhi_ee);
    if (bhi_info->bhi_ee != MHI_EE_EDL) {
        dbg_time("bhi_ee is not MHI_EE_EDL\n");
        close(bhifd);
        free(filebuf);
        error_return();
    }
    free(bhi_info);

    ret = ioctl(bhifd, IOCTL_BHI_WRITEIMAGE, filebuf);
    if (ret) {
        dbg_time("fail to ioctl IOCTL_BHI_GETDEVINFO, errno: %d (%s)\n", errno, strerror(errno));
        error_return();
    }

    close(bhifd);
    free(filebuf);

    sleep(1);
    edlfd = open("/dev/mhi_EDL", O_RDWR | O_NOCTTY);
    if (edlfd <= 0) {
        dbg_time("fail to access %s, errno: %d (%s)\n", "/dev/mhi_EDL", errno, strerror(errno));
        error_return();
    }

    edl_pcie_mhifd = edlfd;

    return 0;
}

int usbmon_fd = -1;
int usbmon_logfile_fd = -1;

void *catch_log(void *arg)
{
    int nreads = 0;
    char tbuff[256];
    size_t off = strlen("[999.999] ");

    (void)arg;
    tbuff[off - 1] = ' ';
    while(1) {
        nreads = read(usbmon_fd, tbuff + off, sizeof(tbuff) - off);
        if (nreads == -1 && errno == EINTR)
            continue;
        if (nreads <= 0)
            break;

        tbuff[off + nreads] = '\0';
        memcpy(tbuff, firehose_get_time(), off - 1);

        if (write(usbmon_logfile_fd, tbuff, strlen(tbuff)) == -1) { };
    }

    return NULL;
}

int ql_capture_usbmon_log(const char* usbmon_logfile)
{
    const char *usbmon_path = "/sys/kernel/debug/usb/usbmon/0u";
    pthread_t pt;
    pthread_attr_t attr;

    if (access("/sys/kernel/debug/usb", F_OK)) {
        dbg_time("debugfs is not mount, please execute \"mount -t debugfs none_debugs /sys/kernel/debug\"\n");
        return -1;
    }
    if (access("/sys/kernel/debug/usb/usbmon", F_OK)) {
        dbg_time("usbmon is not load, please execute \"modprobe usbmon\" or \"insmod usbmon.ko\"\n");
        return -1;
    }

    usbmon_fd = open(usbmon_path, O_RDONLY);
    if (usbmon_fd < 0) {
        dbg_time("open %s error(%d) (%s)\n", usbmon_path, errno, strerror(errno));
        return -1;
    }

    usbmon_logfile_fd = open(usbmon_logfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (usbmon_logfile_fd < 0) {
        dbg_time("open %s error(%d) (%s)\n", usbmon_logfile, errno, strerror(errno));
        close(usbmon_fd);
        return -1;
    }

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&pt, &attr, catch_log, NULL);

    return 0;
}

void ql_stop_usbmon_log()
{
    if (usbmon_logfile_fd > 0)
        close(usbmon_logfile_fd);
    if (usbmon_fd > 0)
        close(usbmon_fd);
}
