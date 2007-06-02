#define net_init            init_interface
#define handle_network_read handle_read
#define send_network_ack    send_ack
#define send_network_string send_string
#define send_network_hex    send_hex

#include "sv_addr_ag.h"
#include "e100boot.h"

static void start_network_trans(void);

void 
net_init(void)
{
  *R_NETWORK_GA_1 = *R_NETWORK_GA_0 = 0;
  
  SET_ETHER_ADDR(0x01,0x40,0x8c,0x00,0x01,0x00,
                 0x01,0x40,0x8c,0x00,0x01,0x00);

  *R_NETWORK_REC_CONFIG =  
    IO_STATE (R_NETWORK_REC_CONFIG, duplex,     half)    |
    IO_STATE (R_NETWORK_REC_CONFIG, bad_crc,    discard) |
    IO_STATE (R_NETWORK_REC_CONFIG, oversize,   discard) |
    IO_STATE (R_NETWORK_REC_CONFIG, undersize,  discard) |
    IO_STATE (R_NETWORK_REC_CONFIG, all_roots,  discard) |
    IO_STATE (R_NETWORK_REC_CONFIG, broadcast,  discard) |
    IO_STATE (R_NETWORK_REC_CONFIG, individual, discard) |
    IO_STATE (R_NETWORK_REC_CONFIG, ma1,        disable) |
    IO_STATE (R_NETWORK_REC_CONFIG, ma0,        enable);

  *R_NETWORK_MGM_CTRL =  
    IO_FIELD(R_NETWORK_MGM_CTRL, txd_pins, 0)       |
    IO_FIELD(R_NETWORK_MGM_CTRL, txer_pin, 0)       |
    IO_FIELD(R_NETWORK_MGM_CTRL, mdck,     0)       |
    IO_STATE(R_NETWORK_MGM_CTRL, mdoe,     disable) |
    IO_FIELD(R_NETWORK_MGM_CTRL, mdio,     0);
  
  *R_NETWORK_TR_CTRL = 
    (IO_STATE (R_NETWORK_TR_CTRL, clr_error, clr)     |
     IO_STATE (R_NETWORK_TR_CTRL, delay,     none)    |
     IO_STATE (R_NETWORK_TR_CTRL, cancel,    dont)    |
     IO_STATE (R_NETWORK_TR_CTRL, cd,        enable)  |
     IO_STATE (R_NETWORK_TR_CTRL, pad,       enable) |
     IO_STATE (R_NETWORK_TR_CTRL, crc,       enable)  |
     IO_STATE (R_NETWORK_TR_CTRL, retry,     enable));
     
  *R_NETWORK_GEN_CONFIG =  
    IO_STATE (R_NETWORK_GEN_CONFIG, loopback, off)   |
    IO_STATE (R_NETWORK_GEN_CONFIG, frame,    ether) |
    IO_STATE (R_NETWORK_GEN_CONFIG, vg,       off)   |
    IO_STATE (R_NETWORK_GEN_CONFIG, phy,      mii_clk)   |
    IO_STATE (R_NETWORK_GEN_CONFIG, enable,   on);

  *R_DMA_CH0_CMD = IO_STATE(R_DMA_CH0_CMD, cmd, reset);
  *R_DMA_CH1_CMD = IO_STATE(R_DMA_CH1_CMD, cmd, reset);

  rx_descr.sw_len = NETWORK_HEADER_LENGTH;
  rx_descr.ctrl   = TX_CTRL;
  rx_descr.next   = (udword)&rx_descr2;
  rx_descr.buf    = (udword)&rx_header;

  rx_descr2.sw_len = 1500;
  rx_descr2.ctrl   = TX_CTRL_EOP;
  rx_descr2.buf    = target_address;

  /* Endian dependent, but saves a few bytes... */
  *(udword*)&tx_header.src[0] = htonl(0x02408c00); 
  *(uword*)&tx_header.src[4]  = htons(0x0100); 
  tx_header.length = htons(64);
  tx_header.snap1  = htonl(0xaaaa0300);
  tx_header.snap2  = htonl(0x408c8856);
  tx_header.tag    = htonl(0xfffffffe);
  tx_header.seq    = 0;
  tx_header.type   = 0;

  tx_descr.sw_len = NETWORK_HEADER_LENGTH;
  tx_descr.ctrl   = TX_CTRL_EOP;
  tx_descr.buf    = (udword)&tx_header;
  
  set_dest        = FALSE;
  seq = 0;

  *R_DMA_CH1_FIRST = (udword)&rx_descr;
  *R_DMA_CH1_CMD   = IO_STATE(R_DMA_CH1_CMD, cmd, start);
}

int
handle_network_read(void)
{
  if (!(rx_descr2.status & d_eop)) {
    return FALSE;
  }

  /* Even if wasn't to us, we must do this... */
  rx_descr2.status = 0;
  *R_DMA_CH1_FIRST = (udword)&rx_descr;

  /* Was it to me? */
  if (ntohl(rx_header.seq) == (seq+1)) { 
    interface        = NETWORK;
    last_timeout     = *R_TIMER_DATA >> IO_BITNR(R_TIMER_DATA, timer0);
    nbr_read        += rx_descr2.hw_len - CRC_LEN;
    rx_descr2.buf   += rx_descr2.hw_len - CRC_LEN;
    rx_descr2.sw_len = bytes_to_read - nbr_read - CRC_LEN < 1500 ?
      bytes_to_read - nbr_read + CRC_LEN: 1500;  /* FIX!!!!! */
    rx_descr2.sw_len = rx_descr2.sw_len + NETWORK_HEADER_LENGTH < 64
      ? 64 - NETWORK_HEADER_LENGTH : rx_descr2.sw_len; /* Less than minimum eth packet? */

    seq++;
    send_network_ack();
  }

  return TRUE;
}

void
send_network_ack(void)
{
/*  send_serial_string(">send_network_ack.\r\n"); */
  
  if (!set_dest) {    /* Copy destination address from first received packet */
    set_dest = TRUE;
    /* If we have done a bootrom network boot, source address is in a different
       place... */
    if (((*R_BUS_STATUS & IO_MASK(R_BUS_STATUS, boot))
         >> IO_BITNR(R_BUS_STATUS, boot)) - 1 == NETWORK) {
      *(udword*)&tx_header.dest[0] = *(udword*)0x380000dc;
      *(uword*)&tx_header.dest[4]  = *(uword*)0x380000e0;
    }
    else {
      *(udword*)&tx_header.dest[0] = *(udword*)&rx_header.src[0];
      *(uword*)&tx_header.dest[4]  = *(uword*)&rx_header.src[4];
    }
  }

#if 1
  tx_header.seq   = htonl(seq);
  tx_header.type  = htonl(ACK);
#else
  tx_header.seq   = htonl(seq);
  tx_header.type  = htonl(bytes_to_read);
  tx_header.id    = htonl(target_address);
  /* tx_header.id already set in start(). */
#endif

  tx_descr.ctrl   = TX_CTRL_EOP;
  tx_descr.sw_len = NETWORK_HEADER_LENGTH;

  start_network_trans();
}

void
start_network_trans(void)
{
/*  send_serial_string(">start_network_trans\r\n"); */
  
  /* Clear possible underrun or excessive retry errors */
/*  *R_NETWORK_TR_CTRL = */
/*    (IO_STATE (R_NETWORK_TR_CTRL, clr_error, clr)     | */
/*     IO_STATE (R_NETWORK_TR_CTRL, delay,     none)    | */
/*     IO_STATE (R_NETWORK_TR_CTRL, cancel,    dont)    | */
/*     IO_STATE (R_NETWORK_TR_CTRL, cd,        enable)  | */
/*     IO_STATE (R_NETWORK_TR_CTRL, pad,       enable)  | */
/*     IO_STATE (R_NETWORK_TR_CTRL, crc,       enable)  | */
/*     IO_STATE (R_NETWORK_TR_CTRL, retry,     enable)); */
  
  *R_DMA_CH0_FIRST = (udword)&tx_descr;
  *R_DMA_CH0_CMD = IO_STATE(R_DMA_CH0_CMD, cmd, start);
  while(*R_DMA_CH0_FIRST)
    ;
}

void 
send_network_string(char *str)
{
  int i;

  tx_descr.next   = (udword)&tx_descr2;
  tx_descr.ctrl   = TX_CTRL;
  for (i = 0; str[i]; i++)      /* strlen */
    ;

  tx_header.type   = STRING;

  tx_descr2.ctrl   = TX_CTRL_EOP;
  tx_descr2.buf    = (udword)str;
  tx_descr2.sw_len = i+1;

  start_network_trans();
}

void 
send_network_hex(udword v, byte nl)
{
  tx_descr.next   = (udword)&tx_descr2;
  tx_descr.ctrl   = TX_CTRL;

  tx_header.type  = nl ? htonl(NET_INT_NL) : htonl(NET_INT);

  /* Pause so we don't saturate network. */
  /* Hehe, the asm will fool gcc to not remove the loop even though it
     probably should. If we volatile i the code will be a few bytes
     longer than this version. Well I really did it like this just
     because it is possible... */
  {
    udword i;
    
    for(i = 0; i != 16384; i++)
      __asm__ ("");             
  }

  v = htonl(v);
  tx_descr2.ctrl   = TX_CTRL_EOP;
  tx_descr2.buf    = (udword)&v;
  tx_descr2.sw_len = (uword)sizeof(udword);

  start_network_trans();
}
