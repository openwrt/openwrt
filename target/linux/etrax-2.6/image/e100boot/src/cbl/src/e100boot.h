/* $Id: e100boot.h,v 1.9 2003/12/16 09:04:07 magnusmn Exp $ */

#include "compiler.h"

#define DMA_DESCR__out_priority__BITNR  5
#define DMA_DESCR__out_priority__WIDTH  1
#define DMA_DESCR__out_priority__normal 0
#define DMA_DESCR__out_priority__high   1

#define DMA_DESCR__ecp_cmd__BITNR  4
#define DMA_DESCR__ecp_cmd__WIDTH  1
#define DMA_DESCR__ecp_cmd__normal 0
#define DMA_DESCR__ecp_cmd__high   1

#define DMA_DESCR__tx_err__BITNR 4
#define DMA_DESCR__tx_err__WIDTH 1
#define DMA_DESCR__tx_err__enable 1
#define DMA_DESCR__tx_err__disable 0

#define DMA_DESCR__intr__BITNR  3
#define DMA_DESCR__intr__WIDTH  1
#define DMA_DESCR__intr__enable 1
#define DMA_DESCR__intr__disable 0

#define DMA_DESCR__wait__BITNR 2
#define DMA_DESCR__wait__WIDTH 1
#define DMA_DESCR__wait__enable 1
#define DMA_DESCR__wait__disable 0

#define DMA_DESCR__eop__BITNR 1
#define DMA_DESCR__eop__WIDTH 1
#define DMA_DESCR__eop__enable 1
#define DMA_DESCR__eop__disable 0

#define DMA_DESCR__eol__BITNR 0
#define DMA_DESCR__eol__WIDTH 1
#define DMA_DESCR__eol__enable 1
#define DMA_DESCR__eol__disable 0

#define DMA_DESCR__sw_len__BITNR 0
#define DMA_DESCR__sw_len__WIDTH 16

#define DMA_DESCR__next__BITNR 0
#define DMA_DESCR__next__WIDTH 32

#define DMA_DESCR__buf__BITNR 0
#define DMA_DESCR__buf__WIDTH 32

#define DMA_DESCR__fifo_len__BITNR 8
#define DMA_DESCR__fifo_len__WIDTH 7

#define DMA_DESCR__crc_err__BITNR 7
#define DMA_DESCR__crc_err__WIDTH 1
#define DMA_DESCR__crc_err__enable 1
#define DMA_DESCR__crc_err__disable 0

#define DMA_DESCR__align_err__BITNR 6
#define DMA_DESCR__align_err__WIDTH 1
#define DMA_DESCR__align_err__enable 1
#define DMA_DESCR__align_err__disable 0

#define DMA_DESCR__in_priority__BITNR 5
#define DMA_DESCR__in_priority__WIDTH 1
#define DMA_DESCR__in_priority__high 1
#define DMA_DESCR__in_priority__normal 0

#define DMA_DESCR__stop__BITNR 4
#define DMA_DESCR__stop__WIDTH 1

#define DMA_DESCR__rd_eop__BITNR 1
#define DMA_DESCR__rd_eop__WIDTH 1

#define DMA_DESCR__hw_len__BITNR 0
#define DMA_DESCR__hw_len__WIDTH 16

#define SET_ETHER_ADDR(a0_0,a0_1,a0_2,a0_3,a0_4,a0_5,a1_0,a1_1,a1_2,a1_3,a1_4,a1_5) \
  *R_NETWORK_SA_0 = a0_0 | (a0_1 << 8) | (a0_2 << 16) | (a0_3 << 24); \
  *R_NETWORK_SA_1 = a0_4 | (a0_5 << 8) | (a1_0 << 16) | (a1_1 << 24); \
  *R_NETWORK_SA_2 = a1_2 | (a1_3 << 8) | (a1_4 << 16) | (a1_5 << 24);

#define DWORD_ALIGN(x) ((x) & 0xfffffffc)

#define CRC_LEN 4

#define TRUE  1
#define FALSE 0

#define NL 1
#define NO_NL 0

#define SERIAL   0
#define NETWORK  1
#define PARALLEL 2

#define STRING      0
#define INT         1
#define ACK         2
#define BOOT_PACKET 3
#define BOOT_CMDS   4
#define NET_INT     5
#define NET_INT_NL  6

#define JUMP           1
#define MEM_TEST       2
#define PACKET_INFO    3
#define SET_REGISTER   4
#define GET_REGISTER   5
#define MEM_DUMP       6
#define MEM_CLEAR      7
#define MEM_VERIFY     8
#define FLASH          9
#define PAUSE_LOOP     10
#define LOOP           11
#define BAUDRATE       12

#define ERR_FLASH_OK        0
#define ERR_FLASH_NONE      1
#define ERR_FLASH_TOO_SMALL 2
#define ERR_FLASH_VERIFY    3
#define ERR_FLASH_ERASE     4

#define TIMEOUT_LIMIT ( ((6250 * 1000) / 0xffff) / 2)

#define TX_CTRL_EOP \
(IO_STATE(DMA_DESCR, intr, disable) |\
 IO_STATE(DMA_DESCR, wait, enable)  |\
 IO_STATE(DMA_DESCR, eop,  enable)  |\
 IO_STATE(DMA_DESCR, eol,  enable))
     
#define TX_CTRL \
     (IO_STATE(DMA_DESCR, intr, disable) |\
      IO_STATE(DMA_DESCR, wait, disable) |\
      IO_STATE(DMA_DESCR, eop,  disable) |\
      IO_STATE(DMA_DESCR, eol,  disable))
    
#define LOAD_ADDRESS 0x38001000    
#define SIZEOF_BOOT_LEVEL_1 2048

/* This is where the commands are transfered to. */
#define IO_BUF_START	0x38001f00
#define IO_BUF_END	0x380020f0 /* bootcode start + cache size */

/* This should only be used in the cbl, but if we compile the sbl for 
 * elinux then __CRIS__ will be defined, and these are already defined
 * in uC-libc. Check that __linux__ is not defined as well!
 */

#if defined(__CRIS__) && !defined(__linux__)
#define NULL ((void*)0)

static inline udword
htonl(udword x)
{
  __asm__ ("swapwb %0" : "=r" (x) : "0" (x));
  
  return(x);
}
#ifndef ntohl     
#define ntohl(x) htonl(x)
#endif

static inline uword
htons(uword x)
{
  __asm__ ("swapb %0" : "=r" (x) : "0" (x));
  
  return(x);
}
#ifndef ntohs
#define ntohs(x) htons(x)
#endif
#endif

/*#define ntohs(x) \*/
/*((unsigned short)((((unsigned short)(x) & 0x00ffU) << 8) | \*/
/*		     (((unsigned short)(x) & 0xff00U) >>  8)))*/
/*     */

/*#define ntohl(x) \*/
/*((unsigned long int)((((unsigned long int)(x) & 0x000000ffU) << 24) | \*/
/*		     (((unsigned long int)(x) & 0x0000ff00U) <<  8) | \*/
/*		     (((unsigned long int)(x) & 0x00ff0000U) >>  8) | \*/
/*		     (((unsigned long int)(x) & 0xff000000U) >> 24)))*/

struct packet_header_T	/* Size = 38 */
{  
  byte      dest[6];
  byte      src[6];
  uword     length;
  udword    snap1;
  udword    snap2;
  udword    tag;		
  udword    seq;
  udword    type;
  udword    id;    
} __attribute__ ((packed));

typedef struct dma_descr_T {
  uword  sw_len;                /* 0-1 */
  uword  ctrl;                  /* 2-3 */
  udword next;                  /* 4-7 */
  udword buf;                   /* 8-11 */
  uword  hw_len;                /* 12-13 */
  uword  status;                /* 14-15 */
} dma_descr_T;

typedef struct packet_info_T {
  udword addr;
  udword size;
} packet_info_T;

typedef struct set_register_T {
  udword addr;
  udword val;
} set_register_T;

typedef struct get_register_T {
  udword addr;
} get_register_T;

typedef struct pause_loop_T {
  udword pause;
} pause_loop_T;

typedef struct mem_verify_T {
  udword addr;
  udword val;
} mem_verify_T;

typedef struct mem_test_T {
  udword from;
  udword to;
} mem_test_T;

typedef struct mem_dump_T {
  udword from_addr;
  udword to_addr;
} mem_dump_T;

typedef struct mem_clear_T {
  udword from_addr;
  udword to_addr;
} mem_clear_T;

typedef struct flash_T {
  unsigned char *source;
  udword offset;
  udword size;
} flash_T;

typedef struct jump_T {
  udword addr;
} jump_T;

typedef struct bne_T {
  udword addr;
  udword target;
} bne_T;

typedef struct br_T {
  udword baudrate;
} br_T;

typedef struct command_T {
  udword type;
  union {
    packet_info_T  packet_info;
    set_register_T set_register;
    get_register_T get_register;
    pause_loop_T   pause_loop;
    mem_verify_T   mem_verify;
    mem_test_T     mem_test;
    mem_dump_T     mem_dump;
    mem_clear_T    mem_clear;
    flash_T        flash;
    jump_T         jump;
    bne_T          bne;
    br_T           br;
  } args;
} command_T;

#define NETWORK_HEADER_LENGTH sizeof(struct packet_header_T)

void crt1(void);
void start(void);
void level2_boot(void);
int  read_data(void);
int  handle_network_read(void);
int  flash_write(const unsigned char *source, unsigned int offset, unsigned int size);

void init_interface(void);
int handle_read(void);
void send_ack(void);
void send_string(char *str);
void send_hex(udword v, byte nl);

extern char e100boot_version[];

extern volatile udword bytes_to_read;
extern volatile udword target_address;

extern udword  nbr_read;
extern byte    interface;
extern byte    set_dest;
extern udword  last_timeout;
extern byte   *io_buf_next;
extern byte   *io_buf_cur;

extern struct packet_header_T tx_header;
extern dma_descr_T tx_descr;
extern dma_descr_T tx_descr2;

extern struct packet_header_T rx_header;
extern dma_descr_T rx_descr;
extern dma_descr_T rx_descr2;

extern uword timeout_limit;  
extern udword seq;
extern byte serial_up;

enum {                    /* Available in:  */
  d_eol      = (1 << 0),  /* flags          */
  d_eop      = (1 << 1),  /* flags & status */
  d_wait     = (1 << 2),  /* flags          */
  d_int      = (1 << 3),  /* flags          */
  d_txerr    = (1 << 4),  /* flags          */
  d_stop     = (1 << 4),  /*         status */
  d_ecp      = (1 << 4),  /* flags & status */
  d_pri      = (1 << 5),  /* flags & status */
  d_alignerr = (1 << 6),  /*         status */
  d_crcerr   = (1 << 7)   /*         status */
};
