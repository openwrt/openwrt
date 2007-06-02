/*!***************************************************************************
*!
*! FILE NAME  : network.c
*!
*! DESCRIPTION: Network boot stuff for e100boot.
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! 1996         Ronny Raneup       Initial version
*! 2002 05 02   Ronny Ranerup      Moved it into this file
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, 2000, 2001, 2002 Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/

/**************************  Include files  ********************************/

#include <stdio.h>
#include <pcap.h>
#include <conf.h>		/* from configure  */

#include <e100boot.h>
#include <common.h>
#include <network.h>

/**************************  Constants and macros  *************************/

#define CRC_LEN 4

/**************************  Type definitions  *****************************/

/**************************  Global variables  *****************************/

udword highest_ack_received;
udword last_ack_received = -1;
int new_ack		= FALSE;
int got_new_packet	= FALSE;

unsigned char dst_addr_of_device[] = { 0x01, 0x40, 0x8c, 0x00, 0x01, 0x00 };
unsigned char src_addr_of_device[] = { 0x02, 0x40, 0x8c, 0x00, 0x01, 0x00 };
unsigned char eth_addr_local[]     = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

struct sockaddr sock_addr;
int             sock_fd;
int pPacket = 0;
pcap_t *pd;

pcap_handler handler;

struct packet_buf first_rec_packet;
struct packet_buf *last_rec_packet = &first_rec_packet;

int promisc = 1;		/* promiscuous mode */
int all_ids     	= TRUE;

int both_addresses = FALSE;
int p_packet_bpl = 8;
int printPacketType = CHAR;

unsigned int id;
int one_id_only 	= TRUE;

unsigned char buf[10000];	/* pcap buffer */

const struct pcap_pkthdr *hdrG;

char host1[MAX_STRING_LEN];	/* name of ethernet host */
char host2[MAX_STRING_LEN];	/* name of ethernet host */

/**************************  Function prototypes  **************************/

int 			timeout			(struct timeval *tvThen, int ms);
int  			InitSendSocket		(char *device_name);
int  			highest_seq_received	(void);
struct packet_buf* 	ack_on_seq		(int seq);
struct packet_buf* 	packet_with_seq		(int seq);
struct timeval 		timeval_subtract	(struct timeval *x, struct timeval *y);
void 			GetNextPacket		(void);
void 			PrintPacket		(const unsigned char *p, int size, int type);

/****************************************************************************
*#
*#  FUNCTION NAME: net_init
*#
*#  DESCRIPTION: 
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE         NAME     CHANGES
*#  ----         ----     -------
*#  2002 05 02   ronny    Initial version
*#
*#***************************************************************************/
void
net_init(void)
{
  static char *dev;			
  static char errBuf[1000];
  
  GetLocalEthAddr();
  first_rec_packet.data = malloc(PACKET_SIZE);
  
  if ((dev = pcap_lookupdev(errBuf)) == NULL) {
    printf("Error %s\n", errBuf);
    if (errno == EPERM)
    {
      printf("Make sure this program is executed with root privileges.\n");
    }

    exit(EXIT_FAILURE);
  }
  
  if ((pd = pcap_open_live(device, 200, promisc, 10, errBuf)) == NULL) {
    printf("Error %s\n", errBuf);
    if (errno == EPERM)
    {
      printf("Make sure this program is executed with root privileges.\n");
    }

    exit(EXIT_FAILURE);
  }
  
  handler = (pcap_handler) Handler;
  
  InitSendSocket(device);
}

/****************************************************************************
*#
*#  FUNCTION NAME: NetBoot
*#
*#  DESCRIPTION: The main network boot routine.
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE         NAME     CHANGES
*#  ----         ----     -------
*#  2000 01 07   ronny    Initial version
*#  2002 05 02   ronny    Fixed segv bug
*#
*#***************************************************************************/

void
NetBoot(void)
{
  struct packet_buf *p;
  struct timeval     tv;

  gettimeofday(&tv, NULL);

  p = create_packet(0);
  SendToDevice(p->data, p->size);

  while(1) {
    static int all_trans = FALSE;

    if (got_new_packet) {
      got_new_packet = FALSE;
      
      if (new_ack && first_packet) {
	if (db4) {
	  printf("* got ACK %d.\n", last_ack_received);
	  printf("* ACK wanted %d.\n", first_packet->seq);
	}
	if (last_ack_received == first_packet->seq) { 
	  if (!(p = create_packet(first_packet->seq+1))) {
	    //break;
	  }
	  first_packet = free_packet(first_packet);
	}
      }
    }
    
    if (new_ack || timeout(&tv, 500)) {
      if (p) {
	SendToDevice(p->data, p->size);
      }
      new_ack = FALSE;
      gettimeofday(&tv, NULL);
    }
    
    GetNextPacket();
  }
}

/****************************************************************************
*#
*#  FUNCTION NAME: GetNextPacket
*#
*#  DESCRIPTION: 
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  961022   ronny    Initial version
*#
*#***************************************************************************/

void
GetNextPacket(void)
{
  int ret;

  if (db2) printf("> GetNextPacket\n");

    /*    got_new_packet = FALSE;*/
    if ((ret = pcap_dispatch(pd, 1, handler, buf)) == -1) {
      pcap_perror(pd, "Error in pcap_dispatch");
      exit(EXIT_FAILURE);
    }

  if (db2) printf("< GetNextPacket\n");
}

/****************************************************************************
*#
*#  FUNCTION NAME: Handler
*#
*#  PARAMETERS: 
*#
*#  DESCRIPTION: 
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  961022   ronny    Initial version
*#
*#***************************************************************************/

void
Handler(unsigned char *buf, const struct pcap_pkthdr *hdr,
	const unsigned char *p)
{
  const unsigned char *src = &p[6];
  const unsigned char *dst = &p[0];
  struct packet_header_T *h = (struct packet_header_T*)p;
  
  if (db2) printf("> Handler\n");
  got_new_packet = TRUE;
  if ((!memcmp(src, eth_addr_local, 6) && !memcmp(dst, dst_addr_of_device, 6))
      || (!memcmp(src, src_addr_of_device, 6) && !memcmp(dst, eth_addr_local, 6))) {
    if (db1) printf("#RX######################################################\n");
    if (db1) printf("Length: %u(0x%x)\n", (udword)hdr->len, (udword)hdr->len);
    if (pPacket) PrintPacket(p, hdr->caplen, printPacketType);
    DecodeSvintoBoot(p);
    /* should clean up this... */
    if ((ntohl(h->type) == ACK)) {
      if (all_ids || (ntohl(h->id) == id)) { 
        if (all_ids && one_id_only) {
	  if (ntohl(h->id) == 0) {
	    all_ids = FALSE;
	    id = ntohl(h->id);
	    printf("Booting device with random id %8.8x.\n", id);
	  }
        }
      }
      else {
        printf("Got ACK from a new id, %8.8lx. Ignoring.\n", 
               (unsigned long)ntohl(h->id));
        return;
      }
      new_ack = TRUE;
      last_ack_received = ntohl(h->seq);
      if (last_ack_received > highest_ack_received) {
        highest_ack_received = last_ack_received;
      }
    }

    if (db1) printf("#########################################################\n");
    
  }
  if (db2) printf("< Handler\n");
}

/****************************************************************************
*#
*#  FUNCTION NAME: ack_on_seq
*#
*#  PARAMETERS: 
*#
*#  DESCRIPTION: 
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  980817   ronny    Initial version
*#
*#***************************************************************************/

struct packet_buf *
packet_with_seq(int seq)
{
  static int last_seq = 0;
  struct packet_buf *p = first_packet;
  struct packet_header_T *h;
  
  if (seq < last_seq) {
    p = first_packet;
  }

  while(p) {
    h = (struct packet_header_T*)p->data;
    if (ntohl(h->seq) == seq) {
      return(p);
    }
    p = p->next;
  }
  return(NULL);
}

struct packet_buf *
ack_on_seq(int seq)
{
  struct packet_buf *p = &first_rec_packet;
  struct packet_header_T *h;

  if (db1) printf("***> ack_on_seq: %d.\n", seq);

  while (p) {
    /*    printf("\nPacket at %x.\n", p);*/
    /*    DecodeSvintoBoot(p->data);*/
    h = (struct packet_header_T*)p->data;
    if ( (ntohl(h->type) == ACK) && (ntohl(h->seq) == seq) ) {
      if (all_ids || ntohl(h->id) == id) {
	printf("***< ack_on_seq %d, ok.\n", seq);
	return(p);
      }
    }
    p = p->next;
  }
  if (db1) printf("***< ack_on_seq, no.\n");
  return(NULL);
}

int
highest_seq_received(void)
{
  struct packet_buf *p = &first_rec_packet;
  struct packet_header_T *h;
  int highest_seq = -1;

  if (db1) printf("***> highest_seq_received\n");

  while (p) {
    /*    printf("\nPacket at %x.\n", p);*/
    /*    DecodeSvintoBoot(p->data);*/
    h = (struct packet_header_T*)p->data;
    if ((ntohl(h->type) == ACK) && (all_ids || (ntohl(h->id) == id))) {
      if ((int)ntohl(h->seq) > highest_seq) {
        highest_seq = ntohl(h->seq);
        if (db4) printf("Highest seq: %d\n", highest_seq);
      }
    }
    p = p->next;
  }

  if (db1) printf("***< highest_seq_received: %d\n", highest_seq);
  return(highest_seq);
}

/****************************************************************************
*#
*#  FUNCTION NAME: PrintPacket
*#
*#  PARAMETERS: 
*#
*#  DESCRIPTION: 
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  961022   ronny    Initial version
*#
*#***************************************************************************/

void
PrintPacket(const unsigned char *p, int size, int type)
{
  int i;

  /*  printf("size %d\n", size);*/
  for (i = 0; i != size; i++) {
    if (i % p_packet_bpl == 0)
      printf("\n%-4.4d: ", i);
    if (type == UDEC)
      printf("%-3d ", p[i]);
    else if (type == HEX)
      printf("%-2.2x ", p[i]);
    else if (type == CHAR) {
      if (isprint(p[i]))
	printf("%-3c ", p[i]);
      else
	printf("%-3d ", p[i]);
    }
    else if (type == ASCII) {
      if (isprint(p[i]))
	printf("%c", p[i]);
      else
	printf(".");
    }
  }
  printf("\n");
}

/****************************************************************************
*#
*#  FUNCTION NAME: DecodeSvintoBoot
*#
*#  PARAMETERS: 
*#
*#  DESCRIPTION: Decodes packets that follow the e100boot protocol.
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  961022   ronny    Initial version
*#
*#***************************************************************************/

void 
DecodeSvintoBoot(const unsigned char *p)
{
  char *str;
  volatile struct packet_header_T *ph =  (struct packet_header_T*)p;

  /*  printf("size %d \n", sizeof(struct packet_header_T));*/

  if (db4) printf("\n>DecodeSvintoBoot. Packet at 0x%x\n", (unsigned int)p);
  if (db1) {
    printf("%2.2x-%2.2x-%2.2x-%2.2x-%2.2x-%2.2x -> ", p[6],p[7],p[8],p[9],p[10], p[11]);
    printf("%2.2x-%2.2x-%2.2x-%2.2x-%2.2x-%2.2x\n", p[0],p[1],p[2],p[3],p[4],  p[5]);
    
    printf("length   : %4.4lx\n", (long)ntohs(ph->length));
    printf("snap1    : %8.8lx\n", (long)ntohl(ph->snap1));
    printf("snap2    : %8.8lx\n", (long)ntohl(ph->snap2));
  
    switch (ntohl(ph->tag)) {
    case 0xffffffff:
      str = "(host > ETRAX)";
      break;
    case 0xfffffffe:
      str = "(host < ETRAX)";
      break;
    default:
      str = "(unknown)";
      break;
    }
    printf("tag      : %8.8lx %s\n", (unsigned long)ntohl(ph->tag), str);
    printf("seq      : %8.8lx\n", (unsigned long)ntohl(ph->seq));
  }

  switch (ntohl(ph->type)) {

  case STRING:
    str = "(STRING)";
    fprintf(stderr, "%s", &p[sizeof(struct packet_header_T)]);
    find_string((char*)&p[sizeof(struct packet_header_T)]);
    break;

  case NET_INT: {
    char search_str[20];

    str = "(NET_INT)";
    sprintf(search_str, "0x%8.8x", ntohl(*(udword*)&p[sizeof(struct packet_header_T)]));
    fprintf(stderr, search_str);
    find_string(search_str);
    break;
  }

  case NET_INT_NL: {
    char search_str[20];
    
    str = "(NET_INT_NL)";
    sprintf(search_str, "0x%8.8x\n", ntohl(*(udword*)&p[sizeof(struct packet_header_T)]));
    fprintf(stderr, search_str);
    find_string(search_str);
    break;
  }

  case ACK:
    str = "(ACK)";
    break;

  case BOOT_PACKET:
    str = "(bootpacket)";
    break;

  case BOOT_CMDS:
    str = "(bootcmds)";
    break;

  default:
    str = "(unknown)";
    break;
  }

  if (db1) {
    printf("(type    : %8.8lx %s)\n", (unsigned long)ntohl(ph->type), str);
    printf("(id      : %8.8lx)\n", (unsigned long)ntohl(ph->id));
    id = ntohl(ph->id);
  }
}

/****************************************************************************
*#
*#  FUNCTION NAME: GetLocalEthAddr
*#
*#  PARAMETERS: None.
*#
*#  DESCRIPTION: 
*#
*#---------------------------------------------------------------------------
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  980818   ronny    Initial version
*#
*#***************************************************************************/

void
GetLocalEthAddr(void)
{
  int fd;
  struct ifreq ifr;

  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    perror("socket (GetLocalEthAddr)");
    exit(EXIT_FAILURE);
  }

  strcpy(ifr.ifr_name, device);
  if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0) {
    perror("ioctl");
    exit(EXIT_FAILURE);
  }

  memcpy(eth_addr_local, ifr.ifr_hwaddr.sa_data, 6);
  if (db1) printf("Ethernet adress for device %s is %2.2x-%2.2x-%2.2x-%2.2x-%2.2x-%2.2x\n", 
		  device,
		  eth_addr_local[0],
		  eth_addr_local[1],
		  eth_addr_local[2],
		  eth_addr_local[3],
		  eth_addr_local[4],
		  eth_addr_local[5]);
  shutdown(fd, 2);
}

/****************************************************************************
*#
*#  FUNCTION NAME: SendToDevice
*#
*#  PARAMETERS: 
*#
*#  DESCRIPTION: 
*#
*#---------------------------------------------------------------------------
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  980818   ronny    Initial version
*#
*#***************************************************************************/

void
SendToDevice(unsigned char *data, int data_len)
{
  char fName[MAX_STRING_LEN];
  FILE *fd;
  struct packet_header_T *h = (struct packet_header_T*) data;
  
  if (db1) printf("***> SendToDevice\n");

  if (db2) printf("Sending %d bytes at 0x%x to %s.\n", data_len, (unsigned int)data, device);
  if (db1) printf("#TX######################################################\n");
  if (db1) DecodeSvintoBoot(data);

  if (db1) printf("#########################################################\n");
  if (toFiles || cmdsOnly) {
    if (cmdsOnly) {		/* use a simpler name */
      sprintf(fName, "e100boot.cmds");
    }
    else {
      sprintf(fName, "e100boot.seq%lu", (unsigned long)ntohl(h->seq));
    }
    if (db2) printf("Writing packet to file '%s'.\n", fName);
    if ((fd = fopen(fName, "w+")) == NULL) {
      printf("Cannot open/create '%s'. %s.\n", fName, strerror(errno));
      exit(EXIT_FAILURE);
    }
    fwrite(data, data_len, 1, fd);
    fclose(fd);
  }
  else if (sendto(sock_fd, data, data_len, 0, &sock_addr, sizeof(sock_addr)) < 0) {
    perror("Sendto failed:");
    exit(EXIT_FAILURE);
  }

  if (db1) printf("<*** SendToDevice\n");
}

/****************************************************************************
*#
*#  FUNCTION NAME: InitSendSocket
*#
*#  PARAMETERS: 
*#
*#  DESCRIPTION: 
*#
*#---------------------------------------------------------------------------
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  980818   ronny    Initial version
*#***************************************************************************/

int
InitSendSocket(char *device_name)
{
  if ((sock_fd = socket(AF_INET, SOCK_PACKET, htons(ETH_P_ALL))) < 0) {
    perror("Socket call failed:");
    exit(EXIT_FAILURE);
  }

  fcntl(sock_fd, F_SETFL, O_NDELAY);

  sock_addr.sa_family = AF_INET;
  strcpy(sock_addr.sa_data, device_name);

  return sock_fd;
}

/****************************************************************************
*#
*#  FUNCTION NAME: timeout
*#
*#  PARAMETERS: 
*#
*#  DESCRIPTION: 
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  980817   ronny    Initial version
*#
*#***************************************************************************/

int
timeout(struct timeval *tvThen, int ms)
{
  struct timeval tvNow;
  struct timeval tvDiff;

  (void) gettimeofday(&tvNow, NULL);
  tvDiff = timeval_subtract(&tvNow, tvThen);
  if (db4) printf("sec %d.%d\n", (int)tvDiff.tv_sec, (int)tvDiff.tv_usec);
  if (ms * 1000 < (tvDiff.tv_sec * 1000000 + tvDiff.tv_usec)) {
    if (db4) printf("TIMEOUT\n");
    return(TRUE);
  }
  
  return(FALSE);
}

/****************************************************************************
 *#
 *#  FUNCTION NAME: timeval_subtract
 *#
 *#  PARAMETERS: 
 *#
 *#  DESCRIPTION: Subtract x-y, and return result.
 *#
 *#  DATE     NAME     CHANGES
 *#  ----     ----     -------
 *#  970128   ronny    Initial version
 *#
 *#***************************************************************************/

struct timeval
timeval_subtract(struct timeval *x, struct timeval *y)
{
  struct timeval diff;

  diff.tv_sec  = x->tv_sec  - y->tv_sec;
  diff.tv_usec = x->tv_usec - y->tv_usec;

  if (diff.tv_usec < 0) {
    diff.tv_sec--;
    diff.tv_usec = 1000000 + diff.tv_usec;
  }

  return diff;
}
