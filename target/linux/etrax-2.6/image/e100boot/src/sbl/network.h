
extern unsigned char dst_addr_of_device[];
extern unsigned char src_addr_of_device[];
extern unsigned char eth_addr_local[];

extern int pPacket;

extern int promisc;

extern char host1[MAX_STRING_LEN];
extern char host2[MAX_STRING_LEN];

extern int both_addresses;
extern int printPacketType;
extern int p_packet_bpl;


#define UDEC  0
#define CHAR  1
#define ASCII 2
#define HEX   3

#define SIZE_OF_HEADER (sizeof(struct packet_header_T))
#define DATA_SIZE 1460
#define SNAP1      0xaaaa0300
#define SNAP2      0x408c8856
#define SERVER_TAG 0xffffffff
#define CLIENT_TAG 0xfffffffe

#define SIZE_OF_FIRST_HEADER (SIZE_OF_HEADER-8)
#define PACKET_SIZE (DATA_SIZE + SIZE_OF_HEADER)
#define SIZE_OF_BOOT_CMDS ((IO_BUF_END-IO_BUF_START)-CRC_LEN)

#define BOOT_ADDRESS 0x380000f4


void 			NetBoot			(void);
void 			SendToDevice		(unsigned char *data, int data_len);
void 			Handler			(unsigned char *buf, const struct pcap_pkthdr *hdr, const unsigned char *p);
void 			GetLocalEthAddr		(void);
void 			net_init		(void);
void			DecodeSvintoBoot	(const unsigned char *p);
