#include "precomp.h"
/* Functions */
int init_if_ioctl(struct DRI_IF *fd,char *driver_ifname)
{
	struct ifreq ifr;
	fd->send = &send_ioctl;
	fd->close = &close_ioctl;
	fd->sock_ioctl = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd->sock_ioctl < 0) {
		ate_printf(MSG_ERROR,"Socket error in IOCTL\n");
		return 0;
	}

	os_memset(fd->ifname, 0, IFNAMSIZ+1);
	os_memcpy(fd->ifname, driver_ifname, os_strlen(driver_ifname));
	
	/* Get MAC Address*/
	ifr.ifr_addr.sa_family = AF_INET;
	os_memcpy(ifr.ifr_name , driver_ifname , os_strlen(driver_ifname));
	ioctl(fd->sock_ioctl, SIOCGIFHWADDR, &ifr);
	os_memcpy(fd->mac, ifr.ifr_hwaddr.sa_data, 6);	
	return fd->sock_ioctl;
}

int send_ioctl(struct DRI_IF *fd,unsigned char *pkt, int size)
{	
	struct iwreq pwrq;
	bzero(&pwrq, sizeof(pwrq));
	pwrq.u.data.pointer = (caddr_t) pkt;
	pwrq.u.data.length = size + RA_CFG_HLEN;
	os_strncpy(pwrq.ifr_name, fd->ifname, IFNAMSIZ);
	return ioctl(fd->sock_ioctl, RTPRIV_IOCTL_ATE, &pwrq);
}

int close_ioctl(struct DRI_IF *fd)
{
	int ret;

	ate_printf(MSG_INFO,"Close IOCTL Socket\n");
	ret = close(fd->sock_ioctl);

	return ret;
}

