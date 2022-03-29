#include <stdio.h>
#include <string.h>
#include <errno.h>  
#include <sys/socket.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_arp.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/select.h>
#include <linux/wireless.h>
#include "ate.h"
//#include "linux/autoconf.h"
//#include "nvram.h"

#ifdef DBG
#include <stdarg.h>
#endif // DBG //
#define VERSION_STR "1.2.2"
#define SIGNAL

#ifdef SIGNAL
#include <signal.h>             /* signal */


/******************************************************************************************************
*	Prototype
*******************************************************************************************************/
void init_signals(void);
void signup(int);
#endif // SIGNAL //

#if 0 /* Shift to header*/
#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif
#ifndef os_memcpy
#define os_memcpy(d, s, n) memcpy((d), (s), (n))
#endif

#ifndef os_memset
#define os_memset(s, c, n) memset(s, c, n)
#endif

#ifndef os_strlen
#define os_strlen(s) strlen(s)
#endif

#ifndef os_strncpy
#define os_strncpy(d, s, n) strncpy((d), (s), (n))
#endif

#ifndef os_strchr
#define os_strchr(s, c) strchr((s), (c))
#endif

#ifndef os_strcmp
#define os_strcmp(s1, s2) strcmp((s1), (s2))
#endif
#endif
static void RaCfg_Agent(void);
static int  OpenRaCfgSocket(void);
static void SanityCheck(u16 command_id, u16 sequence, u16 len);
static void SanityCheckATE(u16 command_id, u16 sequence, u16 len);
static void NetReceive(u8 *inpkt, int len);
static void SendRaCfgAckFrame(int len);

/* GetOpt - only used in main.c */
static int GetOpt(int argc, char *const argv[], const char *optstring);
static void Usage(void);

/* Debugging function - conditional printf and hex dump. Driver wrappers can
 * use these for debugging purposes. */

/* default : ate_debug_level == 2 */
int ate_debug_level = MSG_WARNING;

#ifdef DBG
static void ate_hexdump(int level, char *str, unsigned char *pSrcBufVA, unsigned long SrcBufLen);
//static void ate_printf(int level, char *fmt, ...); /* Shift to Header Koko*/

static void ate_hexdump(int level, char *str, unsigned char *pSrcBufVA, unsigned long SrcBufLen)
{
	unsigned char *pt;
	int x;

	if (level < ate_debug_level)
		return;
	
	pt = pSrcBufVA;
	printf("%s: %p, len = %lu\n",str,  pSrcBufVA, SrcBufLen);
	for (x=0; x<SrcBufLen; x++)
	{
		if (x % 16 == 0) 
		{
			printf("0x%04x : ", x);
		}
		printf("%02x ", ((unsigned char)pt[x]));
		if (x%16 == 15) printf("\n");
	}
	printf("\n");
}

/**
 * ate_printf - conditional printf
 * @level: priority level (MSG_*) of the message
 * @fmt: printf format string, followed by optional arguments
 *
 * This function is used to print conditional debugging and error messages.
 *
 * Note: New line '\n' is added to the end of the text when printing to stdout.
 */
void ate_printf(int level, char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	if (level >= ate_debug_level)
	{
		vprintf(fmt, ap);
		printf("\n");
	}
	va_end(ap);
}
#else // DBG //
#if 0 /* Shift to Header Koko*/
#define ate_printf(args...) do { } while (0)
#define ate_hexdump(l,t,b,le) do { } while (0)
#endif
#endif // DBG //

/******************************************************************************************************
*	Private Data
*******************************************************************************************************/
static const char *ate_daemon_version =
"ate daemon v" VERSION_STR "\n";
static const char broadcast_addr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static unsigned char packet[1536];
static int sock = -1;
static unsigned char buffer[2048];
static char my_eth_addr[6];
static int if_index;
static int do_fork = 1;
static int signup_flag = 1;
static char bridge_ifname[IFNAMSIZ + 1];
static char driver_ifname[IFNAMSIZ + 1];
static int optind = 1;
static int optopt;
static char *optarg;
/* respond to QA by unicast frames if bUnicast == TRUE */
static boolean bUnicast = FALSE;
unsigned short ate_cmd_id_len_tbl[] =
{
	0, 0, 0, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 6, 6, 6, 2, 2, 2, 0,
};
 unsigned short cmd_id_len_tbl[] =
{
	18, 2, 4, 0, 0xffff, 4, 8, 6, 2, 3, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0, 0, 0, 0, 2,
};

/******************************************************************************************************
*	Functions
*******************************************************************************************************/
#ifdef SIGNAL

void signup(int dummy)
{
	int s;
	struct iwreq pwrq;
	unsigned short CmdId;

	ate_printf(MSG_DEBUG, "===>%s\n", __FUNCTION__);
	
	/* Send ATESTOP command to driver before I am killed by command line(not by GUI). */
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
	{
		ate_printf(MSG_ERROR, "Socket error in IOCTL\n");
		return;
	}
	bzero(&pwrq, sizeof(pwrq));
	CmdId = cpu2le16(RACFG_CMD_ATE_STOP);
	os_memcpy(&packet[20], &CmdId, 2);
	pwrq.u.data.pointer = (caddr_t) &packet[14];
	pwrq.u.data.length = 8;// 8 == Cmd Type(2) + Cmd ID(2) + Length(2) + Seq(2)
	os_strncpy(pwrq.ifr_name, driver_ifname, IFNAMSIZ);

	ioctl(s, RTPRIV_IOCTL_ATE, &pwrq);
	close(s);

	// It's time to terminate myself.
    signup_flag = 0;
}

void init_signals(void)
{
	struct sigaction sa;

	sa.sa_flags = 0;

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGHUP);
	sigaddset(&sa.sa_mask, SIGTERM);
	sigaddset(&sa.sa_mask, SIGABRT);
	
	sa.sa_handler = signup;
	sigaction(SIGHUP, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGABRT, &sa, NULL);
}

#endif // SIGNAL //

int main(int argc, char *argv[])
{
    pid_t pid;
	int c = 0;
	
#ifdef SIGNAL
	init_signals();
#endif

	/* initialize interface */
	os_memset(bridge_ifname, 0, IFNAMSIZ + 1);
	os_memset(driver_ifname, 0, IFNAMSIZ + 1);

	/* set default interface name */
	os_memcpy(bridge_ifname, "br0", 4);
	os_memcpy(driver_ifname, "ra0", 4);

	/* get interface name from arguments */
	for (;;)
	{
		c = GetOpt(argc, argv, "b:hui:vdq");
		if (c < 0)
			break;
		switch (c)
		{
			case 'b':
			{
				os_memset(bridge_ifname, 0, IFNAMSIZ + 1);
				os_memcpy(bridge_ifname, optarg, os_strlen(optarg));
				break;
			}
			case 'h':
				Usage();
				return -1;
			case 'u':
				bUnicast = TRUE;
				break;
			case 'i':
			{
				os_memset(driver_ifname, 0, IFNAMSIZ + 1);
				os_memcpy(driver_ifname, optarg, os_strlen(optarg));
				break;
			}
			case 'v':
				printf("%s\n", ate_daemon_version);
				break;
			case 'd':
#ifndef DBG
				printf("Debugging disabled without "
				       "-DDBG compile time "
				       "option.\n");
				return -1;
#else // !DBG //
				ate_debug_level--;
				break;
#endif // !DBG //
			case 'q':
#ifndef DBG
				printf("Debugging disabled without "
				       "-DDBG compile time "
				       "option.\n");
				return -1;
#else // !DBG //
				ate_debug_level++;
				break;
#endif // !DBG //
			default:
				/* error */
				Usage();
				return -1;
		}
	}

    /* background ourself */
    if (do_fork)
	{
        pid = fork();
    }
	else
	{
        pid = getpid();
    }

    switch (pid)
	{
	    case -1:
	        /* error */
	        perror("fork/getpid");
	        return -1;
	    case 0:
	        /* child, success */
	        break;
	    default:
	        /* parent, success */
	        if (do_fork)
	            return 0;
	        break;
    }

	RaCfg_Agent();
	return 0;
}

static void RaCfg_Agent()
{
	int n, count;
	struct timeval tv;
	fd_set readfds;
	unsigned short rcv_protocol;
//	struct iwreq pwrq;
//	unsigned short CmdId;

	if (OpenRaCfgSocket() != 0)
	{
		return;
	}

	/* QA will send ATESTART to driver via me when it starts */
#if 0
	/* Stop AP first */
	/* pass command to driver */
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
	{
		ate_printf(MSG_ERROR, "Socket error in IOCTL\n");
		return;
	}
	bzero(&pwrq, sizeof(pwrq));
	CmdId = cpu2le16(RACFG_CMD_ATE_START);
	os_memcpy(&packet[20], &CmdId, 2);
	pwrq.u.data.pointer = (caddr_t) &packet[14];
	pwrq.u.data.length = 8;
	//os_strncpy(pwrq.ifr_name, "ra0", IFNAMSIZ);
	os_memcpy(pwrq.ifr_name, driver_ifname, IFNAMSIZ);
	ioctl(s, RTPRIV_IOCTL_ATE, &pwrq);
	close(s);
#endif

	/*
	 * Loop to recv cmd from host 
	 * to start up RT28xx
	 */

	ate_printf(MSG_DEBUG, "28xx ATE agent program start\n");

	tv.tv_sec=1;
	tv.tv_usec=0;
	
	while (signup_flag) 
	{
		FD_ZERO(&readfds);
		FD_SET(sock,&readfds);
		
		//count = select(sock+1,&readfds,NULL,NULL,&tv);
		count = select(sock+1,&readfds,NULL,NULL,NULL);
	
		if (count < 0)
		{
			ate_printf(MSG_ERROR, "socket select error\n");
			perror("select failed():");
			continue;
		}
		else if (count == 0)
		{
			continue;
			//usleep(1000);
		}
		else
		{
			/* Data is available now. */
			if (FD_ISSET(sock, &readfds))
			{
				if ((n = recvfrom(sock, buffer, 2048, 0, NULL, NULL)) > 0)
				{
					os_memcpy(&rcv_protocol, buffer+12, 2);
				
					/* recv the protocol we are waiting */
					if (rcv_protocol == be2cpu16/*ntohs*/(ETH_P_RACFG))
					{
						ate_printf(MSG_DEBUG, "NetReceive\n");
						NetReceive(buffer, n);
					}
				}			
			}
		}
	}

	/* QA will send ATESTOP to driver via me when it is closed. */
#if 0
	/* Start AP */
	/* pass command to driver */
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
	{
		ate_printf(MSG_ERROR, "Socket error in IOCTL\n");
		return;
	}
	bzero(&pwrq, sizeof(pwrq));
	CmdId = cpu2le16(RACFG_CMD_ATE_STOP);
	os_memcpy(&packet[20], &CmdId, 2);
	pwrq.u.data.pointer = (caddr_t) &packet[14];
	pwrq.u.data.length = 8;
	os_strncpy(pwrq.ifr_name, driver_ifname, IFNAMSIZ);

	ioctl(s, RTPRIV_IOCTL_ATE, &pwrq);
	close(s);
#endif
	ate_printf(MSG_DEBUG, "28xx ATE agent is closed.\n");
	close(sock);
}

static int OpenRaCfgSocket()
{
//	struct ifreq ethreq;
	struct ifreq ifr;
	struct sockaddr_ll addr;
//	struct in_addr	own_ip_addr;
	
	if ((sock=socket(PF_PACKET, SOCK_RAW, cpu2be16/*htons*/(ETH_P_RACFG))) < 0)
	{
		perror("socket");
		return -1;
	}

	os_memset(&ifr, 0, sizeof(ifr));
#ifdef CONFIG_LAN_WAN_SUPPORT
    os_memcpy(ifr.ifr_name, "eth0.1" , 7);
#else
	os_memcpy(ifr.ifr_name, bridge_ifname , IFNAMSIZ);
#endif

	if (ioctl(sock, SIOCGIFINDEX, &ifr) != 0)
	{
		perror("ioctl(SIOCGIFINDEX)(eth_sock)");
		goto close;
	}

	os_memset(&addr, 0, sizeof(addr));
	addr.sll_family = AF_PACKET;
	addr.sll_ifindex = ifr.ifr_ifindex;
	if_index = ifr.ifr_ifindex;

	if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) 
	{
		perror("bind");
		goto close;
	}

	if (ioctl(sock, SIOCGIFHWADDR, &ifr) != 0)
	{
		perror("ioctl(SIOCGIFHWADDR)(eth_sock)");
		goto close;
	}

	os_memcpy(my_eth_addr, ifr.ifr_hwaddr.sa_data, 6);

	return 0;

close:
	close(sock);
	sock = -1;
	return (-1);
}


/*
 * The Data field of RaCfgAck Frame always is empty
 * during bootstrapping state
 */

static void SendRaCfgAckFrame(int len)
{
	struct ethhdr *p_ehead;
	struct sockaddr_ll socket_address;
	unsigned char *header, *data;
	int send_result = 0;
	
	header = &packet[0];
	data = &packet[14];
	p_ehead = (struct ethhdr *)&packet[0];

	socket_address.sll_family = PF_PACKET;
	socket_address.sll_protocol = cpu2be16/*htons*/(ETH_P_RACFG);
	socket_address.sll_ifindex = if_index;
	socket_address.sll_pkttype = (bUnicast == FALSE) ? PACKET_BROADCAST : PACKET_OTHERHOST;
	socket_address.sll_hatype = ARPHRD_ETHER;

	socket_address.sll_halen = ETH_ALEN;
	
	bzero(&socket_address.sll_addr[0], 8);

	if (bUnicast == FALSE)
	{
		/* respond to QA by broadcast frame */
		os_memcpy(&socket_address.sll_addr[0], broadcast_addr, 6);
	}
	else
	{
		/* respond to QA by unicast frame */
		os_memcpy(&socket_address.sll_addr[0], p_ehead->h_dest, 6);
	}

   	send_result = sendto(sock, &packet[0], len, 0, (struct sockaddr *)&socket_address, sizeof(socket_address));

	ate_printf(MSG_DEBUG, "response send bytes = %d\n", send_result);

}

static void SanityCheck(	u16 Command_Id, u16 Sequence, u16 Len)
{
	/* Check for length and ID */
	/* Lengths of these two commands are not in cmd_id_len_tbl.*/
	if (((Command_Id == RACFG_CMD_ATE_START) || (Command_Id == RACFG_CMD_ATE_STOP)) && (Len == 0))
	{
		if (Command_Id == RACFG_CMD_ATE_START)
		{
			ate_printf(MSG_DEBUG, "Cmd:ATE Start\n");
		}
		else
		{
			ate_printf(MSG_DEBUG, "Cmd:ATE Stop\n");
		}
	}
	else if (((Command_Id == RACFG_CMD_ATE_START) || (Command_Id == RACFG_CMD_ATE_STOP)) && (Len != 0))
	{
			ate_printf(MSG_ERROR, "length field error, id = %x, Len = %d, len should be %d\n", Command_Id, Len, 0);
			return;
	}
	else if ((Command_Id <= SIZE_OF_CMD_ID_TABLE) && (cmd_id_len_tbl[Command_Id] != 0xffff))
	{
		if (Len != cmd_id_len_tbl[Command_Id])
		{
			ate_printf(MSG_ERROR, "length field or command id error, id = %x, Len = %d, len should be %d\n", Command_Id, Len, cmd_id_len_tbl[Command_Id]);
			return;
		}
	}
	// Len of RACFG_CMD_TX_START will be 0xffff or zero.
	else if ((Command_Id == RACFG_CMD_TX_START) && (Len != 0))/* (cmd_id_len_tbl[Command_Id] == 0xffff) */
	{
		if (Len < 40) // TXWI:20 Count:2 Hlen:2 Header:2+2+6+6
		{
			ate_printf(MSG_ERROR, "Cmd:TxStart, length is too short, len = %d\n", Len);
			return;
		}
	}
	else if ((Command_Id == RACFG_CMD_TX_START) && (Len == 0))/* (cmd_id_len_tbl[Command_Id] == 0x0000) */
	{
		ate_printf(MSG_DEBUG, "Cmd:TxStart, length is zero, either for Carrier test or for Carrier Suppression\n");
	}
	else if (Command_Id == RACFG_CMD_E2PROM_WRITE_ALL)/* (cmd_id_len_tbl[Command_Id] == 0xffff) */
	{
		;
	}
	else
	{
		/* message for debug */
		ate_printf(MSG_ERROR, "SanityCheck - command id = %x\n", Command_Id);
		return;
	}
}

static void SanityCheckATE(u16 Command_Id, u16 Sequence, u16 Len)
{
	u16 Command_Id_Offset = 0;

	Command_Id_Offset = Command_Id & (~(1 << 8));

	/* Check for length and ID */
	if ((Command_Id_Offset <= SIZE_OF_ATE_CMD_ID_TABLE) && (ate_cmd_id_len_tbl[Command_Id_Offset] != 0xffff))
	{
		if (Len != ate_cmd_id_len_tbl[Command_Id_Offset])
		{
			ate_printf(MSG_ERROR, "length field or command id error, id = %x, Len = %d, len should be %d\n", Command_Id, Len, ate_cmd_id_len_tbl[Command_Id_Offset]);
			return;
		}
	}
	else
	{
		/* message for debug */
		ate_printf(MSG_ERROR, "SanityCheckATE - command id = %x\n", Command_Id);
		return;
	}
	return;
}	

static void NetReceive(u8 *inpkt, int len)
{
	struct ethhdr		*p_ehead;
	struct racfghdr 	*p_racfgh;
	u16 				Command_Type;
	u16					Command_Id;
	u16					Sequence;
	u16 				Len;	/*Data Length in Command*/
	struct 				iwreq pwrq;
	int    				s;
    pid_t               pid;
	
	unsigned char *p_tmp = &inpkt[ETH_HLEN];
	/* 
	 * Check packet len 
	 */
	if (len < (ETH_HLEN + 12/* sizeof(struct racfghdr) */)) 
	{
		ate_printf(MSG_ERROR, "packet len is too short!\n");
		return;
	}

	p_ehead = (struct ethhdr *) inpkt;
	p_racfgh = (struct racfghdr *) &inpkt[ETH_HLEN];
	
	//write_dump(log_sys, buf,recv_len); 
	/*
	 * 1. Check if dest mac is my mac or broadcast mac
	 * 2. Ethernet Protocol ID == ETH_P_RACFG
	 * 3. RaCfg Frame Magic Number
	 */
	if ((p_ehead->h_proto == cpu2be16/*htons*/(ETH_P_RACFG)) && 
		((strncmp(my_eth_addr, (char *)p_ehead->h_dest, 6) == 0) || (strncmp(broadcast_addr, (char *)p_ehead->h_dest, 6) == 0))&&
		(be2cpu32/*ntohl*/(p_racfgh->magic_no) == RACFG_MAGIC_NO)) 
	{
		/* Koko 2014.02.11 */
		p_tmp = p_tmp + OFFSET_OF(struct racfghdr, magic_no) + 4;
		os_memcpy(&Command_Type, p_tmp, sizeof(Command_Type));
		Command_Type = be2cpu16(Command_Type);
		p_tmp += sizeof(Command_Type);
		
		if ((Command_Type & RACFG_CMD_TYPE_PASSIVE_MASK) != RACFG_CMD_TYPE_ETHREQ)
		{
			int i = 0;
			ate_printf(MSG_ERROR, "Command_Type error, Packet Content Dump:\n");
			for(i = ETH_HLEN;i<len;i++){
				ate_printf(MSG_ERROR, "%02x", inpkt[i]);
			}
			ate_printf(MSG_ERROR, "\n");
			//ate_printf(MSG_ERROR, "Command_Type error = %04x\n", Command_Type);
			return;
		}
	} 
	else 
	{
		ate_printf(MSG_ERROR, "protocol or magic error\n");
		return;
	}
	
	/* Koko 2014.02.11 */
	os_memcpy(&Command_Id, p_tmp, sizeof(Command_Id));
	Command_Id = be2cpu16(Command_Id);
	p_tmp += sizeof(Command_Id);

	os_memcpy(&Len, p_tmp, sizeof(Len));
	Len	= be2cpu16(Len);
	p_tmp += sizeof(Len);
	
	os_memcpy(&Sequence, p_tmp, sizeof(Sequence));
	Sequence = be2cpu16(Sequence);
	p_tmp += sizeof(Sequence);
	
	#if 1
		ate_printf(MSG_ERROR, "MAGIC_NO :%08x\n",be2cpu32/*ntohl*/(p_racfgh->magic_no));
		ate_printf(MSG_ERROR, "Command_Type :%04x\n",Command_Type);
		ate_printf(MSG_ERROR, "Command_Id   :%04x\n",Command_Id);
		ate_printf(MSG_ERROR, "Sequence     :%04x\n",Sequence);
		ate_printf(MSG_ERROR, "Len          :%04x\n",Len);
	#endif
	ate_printf(MSG_MSGDUMP, "NetReceive : Command_Id == %x\n", Command_Id);
	ate_hexdump(MSG_MSGDUMP, "NetReceive : frame\n", p_ehead->h_dest, (PRE_PAYLOADLEN + Len));

	if (((Command_Id & (1 << 8)) == 0))
		{
		SanityCheck(Command_Id, Sequence, Len);

	}
	else
	{
		SanityCheckATE(Command_Id, Sequence, Len);
	}

	/* pass command to driver */
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
	{
		ate_printf(MSG_ERROR, "Socket error in IOCTL\n");
		return;
	}

	bzero(&pwrq, sizeof(pwrq));
	bzero(&packet[0], 1536);

	//
	// Tell my pid to ra0 with RACFG_CMD_ATE_STOP command.
	// It has 4-bytes content containing my pid.
	//
	if (Command_Id == RACFG_CMD_ATE_STOP)
	{
		pid = getpid();
		// stuff my pid into the content
		os_memcpy(&p_racfgh->data[0], (u8 *)&pid, sizeof(pid));
		// We have content now.
		Len += sizeof(pid);
		p_racfgh->length = cpu2be16(Len);
		os_memcpy(&packet[14], p_racfgh, Len + 12);
	}
	else
	{
		os_memcpy(&packet[14], p_racfgh, Len + 12);
	}
	pwrq.u.data.pointer = (caddr_t) &packet[14];
	pwrq.u.data.length = Len + 12;
	os_strncpy(pwrq.ifr_name, driver_ifname, IFNAMSIZ);

	ioctl(s, RTPRIV_IOCTL_ATE, &pwrq);
	close(s);
		
	/* add ack bit to command type */
	/* Koko 2014.02.11 */
	p_racfgh = (struct racfghdr *)&packet[14];
	p_tmp = &packet[ETH_HLEN] + OFFSET_OF(struct racfghdr, magic_no) + 4;
	Command_Type = Command_Type | (~RACFG_CMD_TYPE_PASSIVE_MASK);
	Command_Type = cpu2be16/*htons*/(Command_Type);
	os_memcpy(p_tmp, &Command_Type, sizeof(Command_Type));
	//p_racfgh->comand_type = p_racfgh->comand_type | cpu2be16/*htons*/(~RACFG_CMD_TYPE_PASSIVE_MASK);

	/* prepare ethernet header */
	if (bUnicast == FALSE)
	{
		/* respond to QA by broadcast frame */
		os_memcpy(&packet[0], broadcast_addr, 6);
	}
	else
	{
		/* respond to QA by unicast frame */
		os_memcpy(&packet[0], p_ehead->h_source, 6);
	}

	p_ehead = (struct ethhdr *)&packet[0];
	os_memcpy(p_ehead->h_source, my_eth_addr, 6);
	p_ehead->h_proto = cpu2be16/*htons*/(ETH_P_RACFG);
	
	// determine the length to send and send Ack
	{
		u32 length;
		
		length = be2cpu16/*ntohs*/(p_racfgh->length) + 14 + 12;
		if (length < 60) 
		{
			length = 60;
		}
		else if (length > 1514)
		{
			ate_printf(MSG_ERROR, "response ethernet length is too long\n");
			return;
		}
		SendRaCfgAckFrame(length);
	}
	
}

static int GetOpt(int argc, char *const argv[], const char *optstring)
{
	static int optchr = 1;
	char *cp;

	if (optchr == 1)
	{
		if (optind >= argc)
		{
			/* all arguments processed */
			return EOF;
		}

		if (argv[optind][0] != '-' || argv[optind][1] == '\0') 
		{
			/* no option characters */
			return EOF;
		}
	}

	if (os_strcmp(argv[optind], "--") == 0) 
	{
		/* no more options */
		optind++;
		return EOF;
	}

	optopt = argv[optind][optchr];
	cp = os_strchr(optstring, optopt);
	if (cp == NULL || optopt == ':')
	{
		if (argv[optind][++optchr] == '\0') 
		{
			optchr = 1;
			optind++;
		}
		return '?';
	}

	if (cp[1] == ':')
	{
		/* Argument required */
		optchr = 1;
		if (argv[optind][optchr + 1])
		{
			/* No space between option and argument */
			optarg = &argv[optind++][optchr + 1];
		}
		else if (++optind >= argc)
		{
			/* option requires an argument */
			return '?';
		}
		else
		{
			/* Argument in the next argv */
			optarg = argv[optind++];
		}
	}
	else
	{
		/* No argument */
		if (argv[optind][++optchr] == '\0')
		{
			optchr = 1;
			optind++;
		}
		optarg = NULL;
	}
	return *cp;
}

static void Usage(void)
{
	printf("%s\n\n\n"
	       "usage:\n"
	       "  ated [-huvd]"
	       "[-b<br_ifname>] \\\n"
	       "[-i<driver_ifname>] \\\n"
	       "\n",
	       ate_daemon_version);

	printf("options:\n"
	       "  -b = bridge interface name\n"
		   "  -h = show this help text\n"
	       "  -u = respond to QA by unicast frame\n"
	       "  -i = driver interface name\n"
	       "  -v = show version\n"
	       "  -d = increase debugging verbosity (-dd even more)\n"
           "  -q = decrease debugging verbosity (-qq even less)\n");

	printf("example 1:\n"
	       "  ated -h\n");
	
	printf("example 2:\n"
	       "  ated -bbr1 -ira1 -v\n");

	printf("example 3:\n"
	       "  ated -u\n");

	printf("example 4:\n"
	       "  ated -d\n");
}
