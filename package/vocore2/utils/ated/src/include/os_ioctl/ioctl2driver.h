#include "ate.h"
#include <sys/socket.h>
#include <linux/wireless.h>
#include <sys/ioctl.h>

int init_if_ioctl(struct DRI_IF *fd,char *driver_ifname);
int send_ioctl(struct DRI_IF *fd,unsigned char *pkt, int size);
int close_ioctl(struct DRI_IF *fd);
