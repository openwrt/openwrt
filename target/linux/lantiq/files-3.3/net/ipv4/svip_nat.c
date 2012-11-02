/******************************************************************************

                               Copyright (c) 2009
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 81726 Munich, Germany

  THE DELIVERY OF THIS SOFTWARE AS WELL AS THE HEREBY GRANTED NON-EXCLUSIVE,
  WORLDWIDE LICENSE TO USE, COPY, MODIFY, DISTRIBUTE AND SUBLICENSE THIS
  SOFTWARE IS FREE OF CHARGE.

  THE LICENSED SOFTWARE IS PROVIDED "AS IS" AND INFINEON EXPRESSLY DISCLAIMS
  ALL REPRESENTATIONS AND WARRANTIES, WHETHER EXPRESS OR IMPLIED, INCLUDING
  WITHOUT LIMITATION, WARRANTIES OR REPRESENTATIONS OF WORKMANSHIP,
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, DURABILITY, THAT THE
  OPERATING OF THE LICENSED SOFTWARE WILL BE ERROR FREE OR FREE OF ANY THIRD
  PARTY CLAIMS, INCLUDING WITHOUT LIMITATION CLAIMS OF THIRD PARTY INTELLECTUAL
  PROPERTY INFRINGEMENT.

  EXCEPT FOR ANY LIABILITY DUE TO WILFUL ACTS OR GROSS NEGLIGENCE AND EXCEPT
  FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR ANY CLAIM
  OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

 ****************************************************************************

Description : This file contains implementation of Custom NAT function
for Infineon's VINETIC-SVIP16
 *******************************************************************************/

#include <linux/module.h>
#include <linux/netfilter_ipv4.h>
#include <linux/if_ether.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/if_vlan.h>
#include <linux/udp.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/in6.h> /* just to shut up a warning */
#include <linux/miscdevice.h>
#include <asm/checksum.h>

#include <linux/svip_nat.h>

MODULE_AUTHOR("Lantiq Deutschland GmbH");
MODULE_DESCRIPTION("SVIP Network Address Translation module");
MODULE_LICENSE("GPL");

#define SVIP_NAT_INFO_STR "@(#)SVIP NAT, version "SVIP_NAT_VERSION

/** maximum voice packet channels possible on the SVIP LC system
  (equals maximum number of Codec channels possible) */
#define SVIP_SYS_CODEC_NUM    ((SVIP_SYS_NUM) * (SVIP_CODEC_NUM))

/** end UDP port number of the SVIP Linecard System */
#define SVIP_UDP_TO           ((SVIP_UDP_FROM) + (SVIP_SYS_CODEC_NUM) - 1)

/** end UDP port number of the Master SVIP in SVIP Linecard System */
#define SVIP_UDP_TO_VOFW0     ((SVIP_UDP_FROM) + (SVIP_CODEC_NUM) - 1)

#define SVIP_PORT_INRANGE(nPort) \
	((nPort) >= (SVIP_UDP_FROM) && (nPort) <= (SVIP_UDP_TO))

#define SVIP_PORT_INDEX(nPort)   (nPort - SVIP_UDP_FROM)

#define SVIP_NET_DEV_ETH0_IDX       0
#define SVIP_NET_DEV_VETH0_IDX      1
#define SVIP_NET_DEV_LO_IDX         2

#define SVIP_NET_DEV_ETH0_NAME      "eth0"
#define SVIP_NET_DEV_ETH1_NAME      "eth1"
#define SVIP_NET_DEV_VETH1_NAME     "veth0"
#define SVIP_NET_DEV_LO_NAME        "lo"

#define SVIP_NAT_STATS_LOC2REM   0
#define SVIP_NAT_STATS_REM2LOC   1
#define SVIP_NAT_STATS_TYPES     2

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
#define SVIP_NAT_FOR_EACH_NETDEV(d) for_each_netdev(&init_net, dev)
#define SVIP_NAT_IP_HDR(ethhdr) ip_hdr(ethhdr)
#else
#define SVIP_NAT_FOR_EACH_NETDEV(d) for(d=dev_base; dev; dev = dev->next)
#define SVIP_NAT_IP_HDR(ethhdr) (ethhdr)->nh.iph
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24) */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define SVIP_NAT_SKB_MAC_HEADER(ethhdr) (ethhdr)->mac.ethernet
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
#define SVIP_NAT_SKB_MAC_HEADER(ethhdr) (ethhdr)->mac.raw
#else
#define SVIP_NAT_SKB_MAC_HEADER(ethhdr) skb_mac_header(ethhdr)
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
#define VLAN_DEV_REAL_DEV(dev)      vlan_dev_real_dev(dev)
#define VLAN_DEV_VLAN_ID(dev)       vlan_dev_vlan_id(dev)
#else
#define VLAN_DEV_REAL_DEV(dev)      (VLAN_DEV_INFO(dev)->real_dev)
#define VLAN_DEV_VLAN_ID(dev)       (VLAN_DEV_INFO(dev)->vlan_id)
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24) */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0))
#define MOD_INC_USE_COUNT
#define MOD_DEC_USE_COUNT
#endif

#if ! ((LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)) && \
       (defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)))
#define VLAN_8021Q_UNUSED
#endif


extern spinlock_t vlan_group_lock;
extern struct net_device *__vlan_find_dev_deep(struct net_device *real_dev, unsigned short VID);

typedef struct SVIP_NAT_stats
{
	unsigned long        inPackets;
	unsigned long        outPackets;
	unsigned long        outErrors;
} SVIP_NAT_stats_t;

typedef struct SVIP_NAT_table_entry
{
	SVIP_NAT_IO_Rule_t   natRule;
	SVIP_NAT_stats_t     natStats[SVIP_NAT_STATS_TYPES];
} SVIP_NAT_table_entry_t;

/* pointer to the SVIP NAT table */
static SVIP_NAT_table_entry_t *pNatTable = NULL;

struct net_device *net_devs[3];
static u32 *paddr_eth0;
static u32 *paddr_eth0_0;
static u32 *paddr_veth0;
static u32 *pmask_veth0;

static struct semaphore *sem_nat_tbl_access;
static int proc_read_in_progress = 0;

static int nDeviceOpen = 0;

/* saves the NAT table index between subsequent invocation */
static int nProcReadIdx = 0;

static long SVIP_NAT_device_ioctl(struct file *,unsigned int ,unsigned long);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
static int  SVIP_NAT_device_release (struct inode *,struct file *);
#else
static void SVIP_NAT_device_release (struct inode *,struct file *);
#endif
static int  SVIP_NAT_device_open    (struct inode *,struct file *);

/* This structure holds the interface functions supported by
   the SVIP NAT configuration device. */
struct file_operations SVIP_NAT_Fops = {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
owner:      THIS_MODULE,
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0) */
	    llseek:  NULL,                      /* seek */
	    read:    NULL,
	    write:   NULL,
	    readdir: NULL,                      /* readdir */
	    poll:    NULL,                      /* select */
	    unlocked_ioctl:   SVIP_NAT_device_ioctl,     /* ioctl */
	    mmap:    NULL,                      /* mmap */
	    open:    SVIP_NAT_device_open,      /* open, */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
	    flush:   NULL,                      /* flush */
#endif
	    release: SVIP_NAT_device_release    /* close */
};

/** Structure holding MISC module operations */
static struct miscdevice SVIP_NAT_miscdev =
{
minor:   MINOR_NUM_SVIP_NAT,
	 name:    SVIP_NAT_DEVICE_NAME,
	 fops:    &SVIP_NAT_Fops
};

#ifdef CONFIG_SVIP_FW_PKT_SNIFFER
int nSVIP_NAT_Sniffer;
unsigned char pSVIP_NAT_SnifferMAC[ETH_ALEN];
int nSVIP_NAT_SnifferMacSet;
#endif

/******************************************************************************/
/**
  Function to read /proc/net/svip_nat/nat proc entry

  \arguments
  page     - pointer to page buffer
  start    - pointer to start address pointer
  off      - offset
  count    - maximum data length to read
  eof      - end of file flag
  data     - proc read data (provided by the function
  pointed to by data)

  \return
  length of read data

  \remarks:
  Each call of this routine forces a copy_to_user of the data returned by
  'fn'. This routine will be called by the user until 'len = 0'.
 ****************************************************************************/
static int SVIP_NAT_ProcRead (char *page, char **start, off_t off,
			      int count, int *eof, void *data)
{
	unsigned long flags;
	int (*fn)(char *buf, int size);
	int len;

	/* If the NAT table index is negative, the reading has completed */
	if (nProcReadIdx < 0)
	{
		nProcReadIdx = 0;
		*eof = 1;
		proc_read_in_progress = 0;
		up(sem_nat_tbl_access);
		return 0;
	}

	local_irq_save(flags);
	if (!proc_read_in_progress)
	{
		proc_read_in_progress = 1;
		local_irq_restore(flags);
		/* we use this semaphore in order to ensure no other party(could be ioctl
		   FIO_SVIP_NAT_RULE_LIST), uses function SVIP_NAT_ProcReadNAT(), during
		   the time read of the proc file takes place */
		down(sem_nat_tbl_access);
	}
	else
	{
		local_irq_restore(flags);
	}

	if (data != NULL)
	{
		fn = data;
		len = fn (page, count);
		/* In this setup each read of the proc entries returns the read data by
		   'fn' to the user. The user keeps issuing read requests as long as the
		   returned value of 'len' is greater than zero. */
		*eof = 1;
		*start = page;
	}
	else
	{
		len = 0;
	}

	return len;
}

#ifdef CONFIG_SVIP_FW_PKT_SNIFFER
/**
  Function to read remaining proc entries
  */
static int SVIP_NAT_ProcReadGen (char *page, char **start, off_t off,
				 int count, int *eof, void *data)
{
	int (*fn)(char *buf, int size);
	int len = 0;

	MOD_INC_USE_COUNT;

	if (data == NULL)
	{
		MOD_DEC_USE_COUNT;
		return 0;
	}

	fn = data;
	len = fn (page, count);

	if (len <= off + count)
	{
		*eof = 1;
	}
	*start = page + off;
	len -= off;
	if (len > count)
	{
		len = count;
	}
	if (len < 0)
	{
		len = 0;
	}

	MOD_DEC_USE_COUNT;

	return len;
}
#endif

/******************************************************************************/
/**
  Function for setting up /proc/net/svip_nat read data

  \arguments
  buf      - pointer to read buffer
  count    - size of read buffer

  \return
  length of read data into buffer

  \remarks:
  The global variable 'nProcReadIdx' is used to save the table index where
  the reading of the NAT table stopped. Reading is stopped when the end of
  the read buffer is approached. On the next itteration the reading continues
  from the saved index.
 *******************************************************************************/
static int SVIP_NAT_ProcReadNAT(char *buf, int count)
{
	int i, j;
	int len = 0;
	SVIP_NAT_IO_Rule_t *pNatRule;

	if (nProcReadIdx == -1)
	{
		nProcReadIdx = 0;
		return 0;
	}

	if (nProcReadIdx == 0)
	{
		len = sprintf(buf+len,
			      "Remote host IP  "         /* 16 char */
			      "Remote host MAC    "      /* 19 char */
			      "Local host IP  "          /* 15 char */
			      "Local host MAC     "      /* 19 char */
			      "Local host UDP  "         /* 16 char */
			      "Loc->Rem(in/out/err)  "   /* 22 char */
			      "Rem->Loc(in/out/err)\n\r");
	}

	for (i = nProcReadIdx; i < SVIP_SYS_CODEC_NUM; i++)
	{
		int slen;

		pNatRule = &pNatTable[i].natRule;

		if (pNatRule->remIP != 0)
		{
			/* make sure not to overwrite the buffer */
			if (count < len+120)
				break;

			/* remIP */
			slen = sprintf(buf+len, "%d.%d.%d.%d",
				       (int)((pNatRule->remIP >> 24) & 0xff),
				       (int)((pNatRule->remIP >> 16) & 0xff),
				       (int)((pNatRule->remIP >> 8) & 0xff),
				       (int)((pNatRule->remIP >> 0) & 0xff));
			len += slen;
			for (j = 0; j < (16-slen); j++)
				len += sprintf(buf+len, " ");

			/* remMAC */
			slen = 0;
			for (j = 0; j < ETH_ALEN; j++)
			{
				slen += sprintf(buf+len+slen, "%02x%s",
						pNatRule->remMAC[j], j < ETH_ALEN-1 ? ":" : " ");
			}
			len += slen;
			for (j = 0; j < (19-slen); j++)
				len += sprintf(buf+len, " ");

			/* locIP */
			slen = sprintf(buf+len, "%d.%d.%d.%d",
				       (int)((pNatRule->locIP >> 24) & 0xff),
				       (int)((pNatRule->locIP >> 16) & 0xff),
				       (int)((pNatRule->locIP >> 8) & 0xff),
				       (int)((pNatRule->locIP >> 0) & 0xff));
			len += slen;
			for (j = 0; j < (15-slen); j++)
				len += sprintf(buf+len, " ");

			/* locMAC */
			slen = 0;
			for (j = 0; j < ETH_ALEN; j++)
			{
				slen += sprintf(buf+len+slen, "%02x%s",
						pNatRule->locMAC[j], j < ETH_ALEN-1 ? ":" : " ");
			}
			len += slen;
			for (j = 0; j < (19-slen); j++)
				len += sprintf(buf+len, " ");

			/* locUDP */
			slen = sprintf(buf+len, "%d", pNatRule->locUDP);
			len += slen;
			for (j = 0; j < (16-slen); j++)
				len += sprintf(buf+len, " ");

			/* NAT statistics, Local to Remote translation */
			slen = sprintf(buf+len, "(%ld/%ld/%ld)",
				       pNatTable[i].natStats[SVIP_NAT_STATS_LOC2REM].inPackets,
				       pNatTable[i].natStats[SVIP_NAT_STATS_LOC2REM].outPackets,
				       pNatTable[i].natStats[SVIP_NAT_STATS_LOC2REM].outErrors);
			len += slen;
			for (j = 0; j < (22-slen); j++)
				len += sprintf(buf+len, " ");

			/* NAT statistics, Remote to Local translation */
			len += sprintf(buf+len, "(%ld/%ld/%ld)\n\r",
				       pNatTable[i].natStats[SVIP_NAT_STATS_REM2LOC].inPackets,
				       pNatTable[i].natStats[SVIP_NAT_STATS_REM2LOC].outPackets,
				       pNatTable[i].natStats[SVIP_NAT_STATS_REM2LOC].outErrors);
		}
	}
	if (i == SVIP_SYS_CODEC_NUM)
		nProcReadIdx = -1;   /* reading completed */
	else
		nProcReadIdx = i;    /* reading still in process, buffer was full */

	return len;
}

#ifdef CONFIG_SVIP_FW_PKT_SNIFFER
/**
  Converts MAC address from ascii to hex respesentaion
  */
static int SVIP_NAT_MacAsciiToHex(const char *pMacStr, unsigned char *pMacHex)
{
	int i=0, c=0, b=0, n=0;

	memset(pMacHex, 0, ETH_ALEN);
	while (pMacStr[i] != '\0')
	{
		if (n >= 0)
		{
			unsigned char nToHex = 0;

			/* check for hex digit */
			if (pMacStr[i] >= '0' && pMacStr[i] <= '9')
				nToHex = 0x30;
			else if (pMacStr[i] >= 'a' && pMacStr[i] <= 'f')
				nToHex = 0x57;
			else if (pMacStr[i] >= 'A' && pMacStr[i] <= 'F')
				nToHex = 0x37;
			else
			{
				if (n != 0)
				{
					printk(KERN_ERR "SVIP NAT: invalid MAC address format[%s]\n", pMacStr);
					return -1;
				}
				i++;
				continue;
			}
			n^=1;
			pMacHex[b] |= ((pMacStr[i] - nToHex)&0xf) << (4*n);
			if (n == 0)
			{
				/* advance to next byte, check if complete */
				if (++b >= ETH_ALEN)
					return 0;
				/* byte completed, next we expect a colon... */
				c = 1;
				/* and, do not check for hex digit */
				n = -1;
			}
			i++;
			continue;
		}
		if (c == 1)
		{
			if (pMacStr[i] == ':')
			{
				/* next we expect hex digit, again */
				n = 0;
			}
			else
			{
				printk(KERN_ERR "SVIP NAT: invalid MAC address format[%s]\n", pMacStr);
				return -1;
			}
		}
		i++;
	}
	return 0;
}

/**
  Used to set the destination MAC address of a host where incoming
  SVIP VoFW packets are to be addressed. In case the address is set
  to 00:00:00:00:00:00 (the default case), the packets will written
  out to eth0 with its original MAC addess.

  \remark
usage: 'echo "00:03:19:00:15:D1" > cat /proc/net/svip_nat/snifferMAC'
*/
int SVIP_NAT_ProcWriteSnifferMAC (struct file *file, const char *buffer,
				  unsigned long count, void *data)
{
	/* at least strlen("xx:xx:xx:xx:xx:xx") characters, followed by '\0' */
	if (count >= 18)
	{
		int ret;

		ret = SVIP_NAT_MacAsciiToHex(buffer, pSVIP_NAT_SnifferMAC);

		if (ret != 0)
			return 0;

		if (!(pSVIP_NAT_SnifferMAC[0]==0 && pSVIP_NAT_SnifferMAC[1]==0 &&
		      pSVIP_NAT_SnifferMAC[2]==0 && pSVIP_NAT_SnifferMAC[3]==0 &&
		      pSVIP_NAT_SnifferMAC[4]==0 && pSVIP_NAT_SnifferMAC[5]==0))
		{
			nSVIP_NAT_SnifferMacSet = 1;
		}
	}
	return count;
}

/**
  Used to read the destination MAC address of a sniffer host
  */
int SVIP_NAT_ProcReadSnifferMAC (char *buf, int count)
{
	int len = 0;

	len = snprintf(buf, count, "%02x:%02x:%02x:%02x:%02x:%02x\n",
		       pSVIP_NAT_SnifferMAC[0], pSVIP_NAT_SnifferMAC[1],
		       pSVIP_NAT_SnifferMAC[2], pSVIP_NAT_SnifferMAC[3],
		       pSVIP_NAT_SnifferMAC[4], pSVIP_NAT_SnifferMAC[5]);

	if (len > count)
	{
		printk(KERN_ERR "SVIP NAT: Only part of the text could be put into the buffer\n");
		return count;
	}

	return len;
}

/**
  Used to switch VoFW message sniffer on/off

  \remark
usage: 'echo "1" > cat /proc/net/svip_nat/snifferOnOff'
*/
int SVIP_NAT_ProcWriteSnifferOnOff (struct file *file, const char *buffer,
				    unsigned long count, void *data)
{
	/* at least one digit expected, followed by '\0' */
	if (count >= 2)
	{
		int ret, nSnifferOnOff;

		ret = sscanf(buffer, "%d", &nSnifferOnOff);

		if (ret != 1)
			return count;

		if (nSnifferOnOff > 0)
			nSnifferOnOff = 1;

		nSVIP_NAT_Sniffer = nSnifferOnOff;
	}
	return count;
}

/**
  Used to read the VoFW message sniffer configuration (on/off)
  */
int SVIP_NAT_ProcReadSnifferOnOff (char *buf, int count)
{
	int len = 0;

	len = snprintf(buf, count, "%d\n", nSVIP_NAT_Sniffer);

	if (len > count)
	{
		printk(KERN_ERR "SVIP NAT: Only part of the text could be put into the buffer\n");
		return count;
	}

	return len;
}
#endif

/******************************************************************************/
/**
  Creates proc read/write entries

  \return
  0 on success, -1 on error
  */
/******************************************************************************/
static int SVIP_NAT_ProcInstall(void)
{
	struct proc_dir_entry *pProcParentDir, *pProcDir;
	struct proc_dir_entry *pProcNode;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
	pProcParentDir = proc_net;
#else
	pProcParentDir = init_net.proc_net;
#endif
	pProcDir = proc_mkdir(SVIP_NAT_DEVICE_NAME, pProcParentDir);
	if (pProcDir == NULL)
	{
		printk(KERN_ERR "SVIP NAT: cannot create proc dir %s/%s\n\r",
		       pProcParentDir->name, SVIP_NAT_DEVICE_NAME);
		return -1;
	}

	pProcNode = create_proc_read_entry("nat", S_IFREG|S_IRUGO, pProcDir,
					   SVIP_NAT_ProcRead, (void *)SVIP_NAT_ProcReadNAT);
	if (pProcNode == NULL)
	{
		printk(KERN_ERR "SVIP NAT: cannot create proc entry %s/%s",
		       pProcDir->name, "nat");
		return -1;
	}

#ifdef CONFIG_SVIP_FW_PKT_SNIFFER
	nSVIP_NAT_Sniffer = 0;
	/* creates proc entry for switching on/off sniffer to VoFW messages */
	pProcNode = create_proc_read_entry("snifferOnOff", S_IFREG|S_IRUGO|S_IWUGO,
					   pProcDir, SVIP_NAT_ProcReadGen, (void *)SVIP_NAT_ProcReadSnifferOnOff);
	if (pProcNode == NULL)
	{
		printk(KERN_ERR "SVIP NAT: cannot create proc entry %s/%s\n\r",
		       pProcDir->name, "snifferOnOff");
		return -1;
	}
	pProcNode->write_proc = SVIP_NAT_ProcWriteSnifferOnOff;

	memset (pSVIP_NAT_SnifferMAC, 0, ETH_ALEN);
	nSVIP_NAT_SnifferMacSet = 0;
	/* creates proc entry for setting MAC address of sniffer host to VoFW messages */
	pProcNode = create_proc_read_entry("snifferMAC", S_IFREG|S_IRUGO|S_IWUGO,
					   pProcDir, SVIP_NAT_ProcReadGen, (void *)SVIP_NAT_ProcReadSnifferMAC);
	if (pProcNode == NULL)
	{
		printk(KERN_ERR "SVIP NAT: cannot create proc entry %s/%s\n\r",
		       pProcDir->name, "snifferMAC");
		return -1;
	}
	pProcNode->write_proc = SVIP_NAT_ProcWriteSnifferMAC;
#endif

	return 0;
}

/******************************************************************************/
/**
  No actions done here, simply a check is performed if an open has already
  been performed. Currently only a single open is allowed as it is a sufficient
  to have hat a single process configuring the SVIP NAT at one time.

  \arguments
  inode       - pointer to disk file data
  file        - pointer to device file data

  \return
  0 on success, else -1
  */
/******************************************************************************/
static int SVIP_NAT_device_open(struct inode *inode, struct file *file)
{
	unsigned long flags;
	struct in_device *in_dev;
	struct in_ifaddr *ifa;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
	local_irq_save(flags);
#else
	local_save_flags(flags);
#endif

	if (nDeviceOpen)
	{
		MOD_INC_USE_COUNT;
		local_irq_restore(flags);
		nDeviceOpen++;
		return 0;
	}

	/* find pointer to IP address of eth0 */
	if ((in_dev=in_dev_get(net_devs[SVIP_NET_DEV_ETH0_IDX])) != NULL)
	{
		for (ifa = in_dev->ifa_list; ifa != NULL; ifa = ifa->ifa_next)
		{
			if (!paddr_eth0 && ifa->ifa_address != 0)
			{
				paddr_eth0 = &ifa->ifa_address;
				continue;
			}
			if (paddr_eth0 && ifa->ifa_address != 0)
			{
				paddr_eth0_0 = &ifa->ifa_address;
				break;
			}
		}
		in_dev_put(in_dev);
	}
	if (paddr_eth0 == NULL || paddr_eth0_0 == NULL)
	{
		local_irq_restore(flags);
		return -ENODATA;
	}

	/* find pointer to IP address of veth0 */
	if ((in_dev=in_dev_get(net_devs[SVIP_NET_DEV_VETH0_IDX])) != NULL)
	{
		for (ifa = in_dev->ifa_list; ifa != NULL; ifa = ifa->ifa_next)
		{
			if (ifa->ifa_address != 0)
			{
				paddr_veth0 = &ifa->ifa_address;
				pmask_veth0 = &ifa->ifa_mask;
				break;
			}
		}
		in_dev_put(in_dev);
	}
	if (paddr_veth0 == NULL)
	{
		local_irq_restore(flags);
		return -ENODATA;
	}

	MOD_INC_USE_COUNT;
	nDeviceOpen++;
	local_irq_restore(flags);

	return 0;
}


/******************************************************************************/
/**
  This function is called when a process closes the SVIP NAT device file

  \arguments
  inode       - pointer to disk file data
  file        - pointer to device file data

  \return
  0 on success, else -1

*/
/******************************************************************************/
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
static int SVIP_NAT_device_release(struct inode *inode,
				   struct file *file)
#else
static void SVIP_NAT_device_release(struct inode *inode,
				    struct file *file)
#endif
{
	unsigned long flags;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
	save_flags(flags);
	cli();
#else
	local_save_flags(flags);
#endif

	/* The device can now be openned by the next caller */
	nDeviceOpen--;

	MOD_DEC_USE_COUNT;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
	restore_flags(flags);
#else
	local_irq_restore(flags);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
	return 0;
#endif
}


/******************************************************************************/
/**
  This function is called when a process closes the SVIP NAT device file

  \arguments
  inode       - pointer to disk file data
  file        - pointer to device file data
  ioctl_num   - ioctl number requested
  ioctl_param - pointer to data related to the ioctl number

  \return
  0 on success, else -1

*/
/******************************************************************************/
long SVIP_NAT_device_ioctl (struct file *file,
			   unsigned int ioctl_num, unsigned long ioctl_param)
{
	int ret = 0;
	SVIP_NAT_IO_Rule_t *pNatRule, *pNatRuleIn;
	SVIP_UDP_PORT_t nPort;
	int nNatIdx;
	int bWrite = 0;
	int bRead = 0;
	unsigned char *pData = 0;
	int nSize;

	if (_IOC_DIR(ioctl_num) & _IOC_WRITE)
		bWrite = 1;
	if (_IOC_DIR(ioctl_num) & _IOC_READ)
		bRead = 1;
	nSize = _IOC_SIZE(ioctl_num);

	if (nSize > sizeof(int))
	{
		if (bRead || bWrite)
		{
			pData = kmalloc (nSize, GFP_KERNEL);
			if (bWrite)
			{
				if (copy_from_user ((void *)pData, (void *)ioctl_param, nSize) != 0)
				{
					printk(KERN_ERR "SVIP NAT: ioctl %x: copy_from_user() failed!\n", ioctl_num);
					ret = -1;
					goto error;
				}
			}
		}
	}

	switch (ioctl_num)
	{
	case FIO_SVIP_NAT_RULE_ADD:

		pNatRuleIn = (SVIP_NAT_IO_Rule_t *)pData;

		/* check if destination UDP port is within range */
		nPort = ntohs(pNatRuleIn->locUDP);

		if (!SVIP_PORT_INRANGE(nPort))
		{
			printk(KERN_ERR "SVIP NAT: Error, UDP port(%d) is out of range(%d..%d)\n",
			       nPort, SVIP_UDP_FROM, SVIP_UDP_TO);
			ret = -1;
			goto error;
		}
		nNatIdx = SVIP_PORT_INDEX(nPort);

		down(sem_nat_tbl_access);
		pNatRule = &pNatTable[nNatIdx].natRule;

		/* add rule to the NAT table */
		pNatRule->remIP  = pNatRuleIn->remIP;
		memcpy((char *)pNatRule->remMAC, (char *)pNatRuleIn->remMAC, ETH_ALEN);
		pNatRule->locIP  = pNatRuleIn->locIP;
		memcpy((char *)pNatRule->locMAC, (char *)pNatRuleIn->locMAC, ETH_ALEN);
		pNatRule->locUDP = pNatRuleIn->locUDP;

		memset(pNatTable[nNatIdx].natStats, 0,
		       sizeof(SVIP_NAT_stats_t)*SVIP_NAT_STATS_TYPES);
		up(sem_nat_tbl_access);
		break;

	case FIO_SVIP_NAT_RULE_REMOVE:

		pNatRuleIn = (SVIP_NAT_IO_Rule_t *)pData;

		/* check if destination UDP port is within range */
		nPort = ntohs(pNatRuleIn->locUDP);
		if (!SVIP_PORT_INRANGE(nPort))
		{
			printk(KERN_ERR "SVIP NAT: Error, UDP port(%d) is out of range(%d..%d)\n",
			       nPort, SVIP_UDP_FROM, SVIP_UDP_TO);
			ret = -1;
			goto error;
		}
		nNatIdx = SVIP_PORT_INDEX(nPort);
		down(sem_nat_tbl_access);
		/* remove rule from the NAT table */
		memset(&pNatTable[nNatIdx], 0, sizeof(SVIP_NAT_table_entry_t));
		up(sem_nat_tbl_access);
		break;

	case FIO_SVIP_NAT_RULE_LIST:
		{
			int len;
			char buf[256];

			down(sem_nat_tbl_access);
			while (nProcReadIdx != -1)
			{
				len = SVIP_NAT_ProcReadNAT(buf, 256);
				if (len > 0)
					printk("%s", buf);
			}
			nProcReadIdx = 0;
			up(sem_nat_tbl_access);
			break;
		}

	default:
		printk(KERN_ERR "SVIP NAT: unsupported ioctl (%x) command for device %s\n",
		       ioctl_num, PATH_SVIP_NAT_DEVICE_NAME);
		ret = -1;
		goto error;
	}

	if (nSize > sizeof(int))
	{
		if (bRead)
		{
			if (copy_to_user ((void *)ioctl_param, (void *)pData, nSize) != 0)
			{
				printk(KERN_ERR "SVIP NAT: ioctl %x: copy_to_user() failed!\n", ioctl_num);
				ret = -1;
				goto error;
			}
		}
	}

error:
	if (pData)
		kfree(pData);

	return ret;
}

#if 0
void dump_msg(unsigned char *pData, unsigned int nLen)
{
	int i;

	for (i=0; i<nLen; i++)
	{
		if (!i || !(i%16))
			printk("\n    ");
		else if (i && !(i%4))
			printk(" ");
		printk("%02x", pData[i]);
	}
	if (--i%16)
		printk("\n");
}
#endif

/******************************************************************************/
/**
  Used to recalculate IP/UDP checksum using the original IP/UDP checksum
  coming with the packet. The original source and destination IP addresses
  are accounted for, and, the checksum is updated using the new source and
  destination IP addresses.

  \arguments
  skb         - pointer to the receiving socket buffer
  csum_old    - original checksum
  saddr_old   - pointer to original source IP address
  saddr_new   - pointer to new source IP address
  daddr_old   - pointer to original destination IP address
  daddr_new   - pointer to new destination IP address

  \return
  recalculated IP/UDP checksum
  */
/******************************************************************************/
static inline u16 ip_udp_quick_csum(u16 csum_old, u16 *saddr_old, u16 *saddr_new,
				    u16 *daddr_old, u16 *daddr_new)
{
	u32 sum;

	sum = csum_old;

	/* convert back from one's complement */
	sum = ~sum & 0xffff;

	if (sum < saddr_old[0]) sum += 0xffff;
	sum -= saddr_old[0];
	if (sum < saddr_old[1]) sum += 0xffff;
	sum -= saddr_old[1];
	if (sum < daddr_old[0]) sum += 0xffff;
	sum -= daddr_old[0];
	if (sum < daddr_old[1]) sum += 0xffff;
	sum -= daddr_old[1];

	sum += saddr_new[0];
	sum += saddr_new[1];
	sum += daddr_new[0];
	sum += daddr_new[1];

	/* take only 16 bits out of the 32 bit sum and add up the carries */
	while (sum >> 16)
		sum = (sum & 0xffff)+((sum >> 16) & 0xffff);

	/* one's complement the result */
	sum = ~sum;

	return (u16)(sum & 0xffff);
}


/******************************************************************************/
/**
  Returns a pointer to an ipv4 address assigned to device dev. The ipv4
  instance checked is pointed to by ifa_start. The function is suited for
  itterative calls.

  \arguments
  dev         - pointer to network interface
  ifa_start   - pointer to ipv4 instance to return ipv4 address assigned
  to, NULL for the first one
  ppifa_addr   - output parameter

  \return
  pointer to the next ipv4 instance, which can be null if ifa_start was
  the last instance present
  */
/******************************************************************************/
static struct in_ifaddr *get_ifaddr(struct net_device *dev,
				    struct in_ifaddr *ifa_start, unsigned int **ppifa_addr)
{
	struct in_device *in_dev;
	struct in_ifaddr *ifa = NULL;

	if ((in_dev=in_dev_get(dev)) != NULL)
	{
		if (ifa_start == NULL)
			ifa = in_dev->ifa_list;
		else
			ifa = ifa_start;
		if (ifa)
		{
			*ppifa_addr = &ifa->ifa_address;
			ifa = ifa->ifa_next;
		}
		in_dev_put(in_dev);
		return ifa;
	}
	*ppifa_addr = NULL;
	return NULL;
}

/******************************************************************************/
/**
  This function performs IP NAT for received packets satisfying the
  following requirements:

  - packet is destined to local IP host
  - transport protocol type is UDP
  - destination UDP port is within range

  \arguments
  skb         - pointer to the receiving socket buffer

  \return
  returns 1 on performed SVIP NAT, else returns 0

  \remarks
  When function returns 0, it indicates the caller to pass the
  packet up the IP stack to make further decision about it
  */
/******************************************************************************/
int do_SVIP_NAT (struct sk_buff *skb)
{
	struct net_device *real_dev;
	struct iphdr *iph;
	struct udphdr *udph;
	SVIP_NAT_IO_Rule_t *pNatRule;
	int nNatIdx, in_eth0, nDir;
#ifndef VLAN_8021Q_UNUSED
	int vlan;
	unsigned short vid;
#endif /* ! VLAN_8021Q_UNUSED */
	SVIP_UDP_PORT_t nPort;
	u32 orgSrcIp, orgDstIp, *pSrcIp, *pDstIp;
	struct ethhdr *ethh;

	/* do not consider if SVIP NAT device not open. */
	if (!nDeviceOpen)
	{
		return 0;
	}

	/* consider only UDP packets. */
	iph = SVIP_NAT_IP_HDR(skb);
	if (iph->protocol != IPPROTO_UDP)
	{
		return 0;
	}

	udph = (struct udphdr *)((u_int32_t *)iph + iph->ihl);
	/* consider only packets which UDP port numbers reside within
	   the predefined SVIP NAT UDP port range. */
	if ((!SVIP_PORT_INRANGE(ntohs(udph->dest))) &&
	    (!SVIP_PORT_INRANGE(ntohs(udph->source))))
	{
		return 0;
	}

#ifndef VLAN_8021Q_UNUSED
	/* check if packet delivered over VLAN. VLAN packets will be routed over
	   the VLAN interfaces of the respective real Ethernet interface, if one
	   exists(VIDs must match). Else, the packet will be send out as IEEE 802.3
	   Ethernet frame */
	if (skb->dev->priv_flags & IFF_802_1Q_VLAN)
	{
		vlan = 1;
		vid = VLAN_DEV_VLAN_ID(skb->dev);
		real_dev = VLAN_DEV_REAL_DEV(skb->dev);
	}
	else
	{
		vlan = 0;
		vid = 0;
		real_dev = skb->dev;
	}
#endif /* ! VLAN_8021Q_UNUSED */

#ifdef CONFIG_SVIP_FW_PKT_SNIFFER
	/** Debugging feature which can be enabled by writing,
	  'echo 1 > /proc/net/svip_nat/snifferOnOff'.
	  It copies all packets received on veth0 and, sends them out over eth0.
	  When a destination MAC address is specified through
	  /proc/net/svip_nat/snifferMAC, this MAC addess will substitute the
	  original MAC address of the packet.
	  It is recommended to specify a MAC address of some host where Wireshark
	  runs and sniffs for this traffic, else you may flood your LAN with
	  undeliverable traffic.

NOTE: In case of VLAN traffic the VLAN header information is lost. */
	if (nSVIP_NAT_Sniffer)
	{
		if (real_dev == net_devs[SVIP_NET_DEV_VETH0_IDX])
		{
			struct sk_buff *copied_skb;

			/* gain the Ethernet header from the skb */
			skb_push(skb, ETH_HLEN);

			copied_skb = skb_copy (skb, GFP_ATOMIC);

			if (nSVIP_NAT_SnifferMacSet == 1)
			{
				ethh = (struct ethhdr *)SVIP_NAT_SKB_MAC_HEADER(copied_skb);
				memcpy((char *)ethh->h_dest, (char *)pSVIP_NAT_SnifferMAC, ETH_ALEN);
			}
			copied_skb->dev = net_devs[SVIP_NET_DEV_ETH0_IDX];
			dev_queue_xmit(copied_skb);

			/* skip the ETH header again */
			skb_pull(skb, ETH_HLEN);
		}
	}
#endif


	/* check if packet arrived on eth0 */
	if (real_dev == net_devs[SVIP_NET_DEV_ETH0_IDX])
	{
		/* check if destination IP address equals the primary assigned IP address
		   of interface eth0. This is the case of packets originating from a
		   remote peer that are to be delivered to a channel residing on THIS
		   voice linecard system. This is typical SVIP NAT case, therefore this
		   rule is placed on top. */
		if (iph->daddr == *paddr_eth0)
		{
			nPort = ntohs(udph->dest);
			nDir = SVIP_NAT_STATS_REM2LOC;
		}
		/* check if destination IP address equals the secondary assigned IP address
		   of interface eth0. This is not a typical SVIP NAT case. It is basically
		   there, as someone might like for debugging purpose to use the LCC to route
		   Slave SVIP packets which are part of voice/fax streaming. */
		else if (iph->daddr == *paddr_eth0_0)
		{
			nPort = ntohs(udph->source);
			nDir = SVIP_NAT_STATS_LOC2REM;
		}
#ifndef VLAN_8021Q_UNUSED
		/* when the packet did not hit the top two rules, here we check if the packet
		   has addressed any of the IP addresses assigned to the VLAN interface attached
		   to eth0. This is not recommended approach because of the CPU cost incurred. */
		else if (vlan)
		{
			unsigned int *pifa_addr;
			struct in_ifaddr *ifa_start = NULL;
			int i = 0;

			do
			{
				ifa_start = get_ifaddr(skb->dev, ifa_start, &pifa_addr);
				if (!pifa_addr)
				{
					/* VLAN packet received on vlan interface attached to eth0,
					   however no IP address assigned to the interface.
					   The packet is ignored. */
					return 0;
				}
				if (iph->daddr == *pifa_addr)
				{
					/* packet destined to... */
					break;
				}
				if (!ifa_start)
				{
					return 0;
				}
				i++;
			} while (ifa_start);
			if (!i)
			{
				/* ...primary assigned IP address to the VLAN interface. */
				nPort = ntohs(udph->dest);
				nDir = SVIP_NAT_STATS_REM2LOC;
			}
			else
			{
				/* ...secondary assigned IP address to the VLAN interface. */
				nPort = ntohs(udph->source);
				nDir = SVIP_NAT_STATS_LOC2REM;
			}
		}
#endif /* ! VLAN_8021Q_UNUSED */
		else
		{
			return 0;
		}
		in_eth0 = 1;
	}
	/* check if packet arrived on veth0 */
	else if (real_dev == net_devs[SVIP_NET_DEV_VETH0_IDX])
	{
		nPort = ntohs(udph->source);
		nDir = SVIP_NAT_STATS_LOC2REM;
		in_eth0 = 0;
	}
	else
	{
		/* packet arrived neither on eth0, nor veth0 */
		return 0;
	}

	/* calculate the respective index of the NAT table */
	nNatIdx = SVIP_PORT_INDEX(nPort);
	/* process the packet if a respective NAT rule exists */
	pNatRule = &pNatTable[nNatIdx].natRule;

	ethh = (struct ethhdr *)SVIP_NAT_SKB_MAC_HEADER(skb);

	/* copy packet's original source and destination IP addresses to use
	   later on to perform efficient checksum recalculation */
	orgSrcIp = iph->saddr;
	orgDstIp = iph->daddr;

	if (in_eth0)
	{
		u8 *pDstMac;

		/* Process packet arrived on eth0 */

		if (nDir == SVIP_NAT_STATS_REM2LOC && iph->saddr == pNatRule->remIP)
		{
			pDstIp = &pNatRule->locIP;
			pDstMac = pNatRule->locMAC;
		}
		else if (nDir == SVIP_NAT_STATS_LOC2REM && iph->saddr == pNatRule->locIP)
		{
			pDstIp = &pNatRule->remIP;
			pDstMac = pNatRule->remMAC;
		}
		else
		{
			/* Rule check failed. The packet is passed up the layers,
			   it will be dropped by UDP */
			return 0;
		}

		if ((*pDstIp & *pmask_veth0) == (*paddr_veth0 & *pmask_veth0))
		{
#ifndef VLAN_8021Q_UNUSED
			if (vlan)
			{
				struct net_device *vlan_dev;

				spin_lock_bh(&vlan_group_lock);
				vlan_dev = __vlan_find_dev_deep(net_devs[SVIP_NET_DEV_VETH0_IDX], vid);
				spin_unlock_bh(&vlan_group_lock);
				if (vlan_dev)
				{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
					struct vlan_ethhdr *vethh;

					skb_push(skb, VLAN_ETH_HLEN);
					/* reconstruct the VLAN header.
NOTE: priority information is lost */
					vethh = (struct vlan_ethhdr *)skb->data;
					vethh->h_vlan_proto = htons(ETH_P_8021Q);
					vethh->h_vlan_TCI = htons(vid);
					vethh->h_vlan_encapsulated_proto = htons(ETH_P_IP);
					ethh = (struct ethhdr *)vethh;
#else
					skb_push(skb, ETH_HLEN);
#endif
					skb->dev = vlan_dev;
				}
				else
				{
					skb->dev = net_devs[SVIP_NET_DEV_VETH0_IDX];
					skb_push(skb, ETH_HLEN);
				}
			}
			else
#endif /* ! VLAN_8021Q_UNUSED */
			{
				skb->dev = net_devs[SVIP_NET_DEV_VETH0_IDX];
				skb_push(skb, ETH_HLEN);
			}
			pSrcIp = paddr_veth0;
		}
		else
		{
#ifndef VLAN_8021Q_UNUSED
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
			if (vlan)
			{
				struct vlan_ethhdr *vethh;

				/* reconstruct the VLAN header.
NOTE: priority information is lost */
				skb_push(skb, VLAN_ETH_HLEN);
				vethh = (struct vlan_ethhdr *)skb->data;
				vethh->h_vlan_proto = htons(ETH_P_8021Q);
				vethh->h_vlan_TCI = htons(vid);
				vethh->h_vlan_encapsulated_proto = htons(ETH_P_IP);
				ethh = (struct ethhdr *)vethh;
			}
			else
#endif
#endif /* ! VLAN_8021Q_UNUSED */
			{
				skb_push(skb, ETH_HLEN);
			}
			/* source IP address equals the destination IP address
			   of the incoming packet */
			pSrcIp = &iph->daddr;
		}
		iph->saddr = *pSrcIp;
		memcpy((char *)ethh->h_source, (char *)skb->dev->dev_addr, ETH_ALEN);
		iph->daddr = *pDstIp;
		memcpy((char *)ethh->h_dest, (char *)pDstMac, ETH_ALEN);
	}
	else
	{
		/* Process packet arrived on veth0 */

		if (iph->saddr != pNatRule->locIP)
		{
			/* Rule check failed. The packet is passed up the layers,
			   it will be dropped by UDP */
			return 0;
		}

		if (!((pNatRule->remIP & *pmask_veth0) == (*paddr_veth0 & *pmask_veth0)))
		{
#ifndef VLAN_8021Q_UNUSED
			if (vlan)
			{
				struct net_device *vlan_dev;

				spin_lock_bh(&vlan_group_lock);
				vlan_dev = __vlan_find_dev_deep(net_devs[SVIP_NET_DEV_ETH0_IDX], vid);
				spin_unlock_bh(&vlan_group_lock);
				if (vlan_dev)
				{
					unsigned int *pifa_addr;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
					struct vlan_ethhdr *vethh;

					skb_push(skb, VLAN_ETH_HLEN);
					/* construct the VLAN header, note priority information is lost */
					vethh = (struct vlan_ethhdr *)skb->data;
					vethh->h_vlan_proto = htons(ETH_P_8021Q);
					vethh->h_vlan_TCI = htons(vid);
					vethh->h_vlan_encapsulated_proto = htons(ETH_P_IP);
					ethh = (struct ethhdr *)vethh;
#else
					skb_push(skb, ETH_HLEN);
#endif
					skb->dev = vlan_dev;

					get_ifaddr(skb->dev, NULL, &pifa_addr);
					if (pifa_addr)
					{
						pSrcIp = pifa_addr;
					}
					else
					{
						pSrcIp = paddr_eth0;
					}
				}
				else
				{
					skb->dev = net_devs[SVIP_NET_DEV_ETH0_IDX];
					pSrcIp = paddr_eth0;
					skb_push(skb, ETH_HLEN);
				}
			}
			else
#endif /* ! VLAN_8021Q_UNUSED */
			{
				skb->dev = net_devs[SVIP_NET_DEV_ETH0_IDX];
				pSrcIp = paddr_eth0;
				skb_push(skb, ETH_HLEN);
			}
		}
		else
		{
			pSrcIp = paddr_veth0;
#ifndef VLAN_8021Q_UNUSED
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
			if (vlan)
			{
				struct vlan_ethhdr *vethh;

				skb_push(skb, VLAN_ETH_HLEN);
				/* reconstruct the VLAN header.
NOTE: priority information is lost. */
				vethh = (struct vlan_ethhdr *)skb->data;
				vethh->h_vlan_proto = htons(ETH_P_8021Q);
				vethh->h_vlan_TCI = htons(vid);
				vethh->h_vlan_encapsulated_proto = htons(ETH_P_IP);
				ethh = (struct ethhdr *)vethh;
			}
			else
#endif
#endif /* ! VLAN_8021Q_UNUSED */
			{
				skb_push(skb, ETH_HLEN);
			}
		}
		iph->saddr = *pSrcIp;
		memcpy((char *)ethh->h_source, (char *)skb->dev->dev_addr, ETH_ALEN);
		iph->daddr = pNatRule->remIP;
		memcpy((char *)ethh->h_dest, (char *)pNatRule->remMAC, ETH_ALEN);
	}
	pNatTable[nNatIdx].natStats[nDir].inPackets++;

	iph->check = ip_udp_quick_csum(iph->check, (u16 *)&orgSrcIp, (u16 *)&iph->saddr,
				       (u16 *)&orgDstIp, (u16 *)&iph->daddr);
	if (udph->check != 0)
	{
		udph->check = ip_udp_quick_csum(udph->check, (u16 *)&orgSrcIp, (u16 *)&iph->saddr,
						(u16 *)&orgDstIp, (u16 *)&iph->daddr);
	}

	/* write the packet out, directly to the network device */
	if (dev_queue_xmit(skb) < 0)
		pNatTable[nNatIdx].natStats[nDir].outErrors++;
	else
		pNatTable[nNatIdx].natStats[nDir].outPackets++;

	return 1;
}

/******************************************************************************/
/**
  Function executed upon unloading of the SVIP NAT module. It unregisters the
  SVIP NAT configuration device and frees the memory used for the NAT table.

  \remarks:
  Currently the SVIP NAT module is statically linked into the Linux kernel
  therefore this routine cannot be executed.
 *******************************************************************************/
static int __init init(void)
{
	int ret = 0;
	struct net_device *dev;

	if (misc_register(&SVIP_NAT_miscdev) != 0)
	{
		printk(KERN_ERR "%s: cannot register SVIP NAT device node.\n",
		       SVIP_NAT_miscdev.name);
		return -EIO;
	}

	/* allocation of memory for NAT table */
	pNatTable = (SVIP_NAT_table_entry_t *)kmalloc(
						      sizeof(SVIP_NAT_table_entry_t) * SVIP_SYS_CODEC_NUM, GFP_ATOMIC);
	if (pNatTable == NULL)
	{
		printk (KERN_ERR "SVIP NAT: Error(%d), allocating memory for NAT table\n", ret);
		return -1;
	}

	/* clear the NAT table */
	memset((void *)pNatTable, 0, sizeof(SVIP_NAT_table_entry_t) * SVIP_SYS_CODEC_NUM);

	if ((sem_nat_tbl_access = kmalloc(sizeof(struct semaphore), GFP_KERNEL)))
	{
		sema_init(sem_nat_tbl_access, 1);
	}

	SVIP_NAT_ProcInstall();

	/* find pointers to 'struct net_device' of eth0 and veth0, respectevely */
	read_lock(&dev_base_lock);
	SVIP_NAT_FOR_EACH_NETDEV(dev)
	{
		if (!strcmp(dev->name, SVIP_NET_DEV_ETH0_NAME))
		{
			net_devs[SVIP_NET_DEV_ETH0_IDX] = dev;
		}
		if (!strcmp(dev->name, SVIP_NET_DEV_VETH1_NAME))
		{
			net_devs[SVIP_NET_DEV_VETH0_IDX] = dev;
		}
		else if (!strcmp(dev->name, SVIP_NET_DEV_ETH1_NAME))
		{
			net_devs[SVIP_NET_DEV_VETH0_IDX] = dev;
		}
	}
	read_unlock(&dev_base_lock);

	if (net_devs[SVIP_NET_DEV_ETH0_IDX] == NULL ||
	    net_devs[SVIP_NET_DEV_VETH0_IDX] == NULL)
	{
		printk (KERN_ERR "SVIP NAT: Error, unable to locate eth0 and veth0 interfaces\n");
		return -1;
	}

	printk ("%s, (c) 2009, Lantiq Deutschland GmbH\n", &SVIP_NAT_INFO_STR[4]);

	return ret;
}

/******************************************************************************/
/**
  Function executed upon unloading of the SVIP NAT module. It unregisters the
  SVIP NAT configuration device and frees the memory used for the NAT table.

  \remarks:
  Currently the SVIP NAT module is statically linked into the Linux kernel
  therefore this routine cannot be executed.
 *******************************************************************************/
static void __exit fini(void)
{
	MOD_DEC_USE_COUNT;

	/* unregister SVIP NAT configuration device */
	misc_deregister(&SVIP_NAT_miscdev);

	/* release memory of SVIP NAT table */
	if (pNatTable != NULL)
	{
		kfree (pNatTable);
	}
}

module_init(init);
module_exit(fini);
