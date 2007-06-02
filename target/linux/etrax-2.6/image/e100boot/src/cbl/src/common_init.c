/*****************************************************************************
*!
*! FILE NAME  : common_init.c
*!
*! DESCRIPTION: This piece of code is loaded at bootstrap and is put in the
*!              cache at 0x380000F0.  Depending of how R_BUS_STATUS<2:1> is
*!              set different kinds of bootstrap is performed.
*!
*!              00 - Normal boot. No bootstrap is performed and this code
*!                   is never loaded.
*!              01 - Serial boot. 784 bytes is loaded and execution starts
*!                   at 0x380000F0.
*!              11 - Parallel boot. 784 bytes is loaded and execution starts
*!                   at 0x380000F0.
*!              10 - Network boot. 1484 bytes is loaded and execution start
*!                   at 0x380000F4.
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! 980326       Ronny Ranerup      Initial version
*! Sep 20 1999  Jonas Dellenvall   Added port3 debug support
*! 20020206     ronny              Yeah, and I removed it again...
*!
*! ---------------------------------------------------------------------------
*! (C) Copyright 1998-2002, Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/

/*

   Misc notes:

   It is very important to keep this file short. This and the boot
   interface specific parts must fit into the first boot packet.

*/

/****************** INCLUDE FILES SECTION ***********************************/

#include "hwregs.h"
#include "e100boot.h"

/****************** CONSTANT AND MACRO SECTION ******************************/

/****************** TYPE DEFINITION SECTION *********************************/

/****************** LOCAL FUNCTION DECLARATION SECTION **********************/

static int timeout(void);

/****************** GLOBAL VARIABLE DECLARATION SECTION *********************/

udword	nbr_read;	/* How many bytes has been read from current file */
byte    interface;	/* Which I/O interface is the current one */
byte    set_dest;	/* Have we set the destination address in tx_header */
udword  last_timeout;

struct packet_header_T tx_header;
dma_descr_T tx_descr;	/* For packet header */
dma_descr_T tx_descr2;	/* packet data */

struct packet_header_T rx_header;
dma_descr_T rx_descr;	/* For packet header */
dma_descr_T rx_descr2;	/* packet data */

udword seq;		/* Sequence number of next wanted packet */
byte serial_up;

/****************** LOCAL VARIABLE DECLARATION SECTION **********************/

/****************** FUNCTION DEFINITION SECTION *****************************/

void 
crt1(void)
{
  /* Do this only once so we don't reset the timers and destroy the 32
     bit timer-register used as random number generator */

  REG_SET__R_TIMER_CTRL( 
			timerdiv1, 0,
			timerdiv0, 0,
			presc_timer1, normal, 
			i1,        clr,
			tm1,       run,
			clksel1,   cascade0,
			presc_ext, prescale,
			i0,        clr,
			tm0,       run,
			clksel0,   c9600Hz);
  
  REG_SET__R_TIMER_CTRL(
			timerdiv1, 0,
			timerdiv0, 0,
			presc_timer1, normal, 
			i1,        nop,
			tm1,       run,
			clksel1,   cascade0,
			presc_ext, prescale,
			i0,        nop,
			tm0,       run,
			clksel0,   c9600Hz);
  
  start();
}

void
start(void)
{
#if USE_LEDS
  REG_SET__R_PORT_PA_DIR( 
			 dir7, output,
			 dir6, output,
			 dir5, output,
			 dir4, output,
			 dir3, output,
			 dir2, output,
			 dir1, output,
			 dir0, input);	/* not for prodtest */
  
  REG_SET__R_PORT_PA_DATA(data_out, 0);

  REG_SET__R_PORT_PB_DIR( 
			 dir7, output,
			 dir6, output,
			 dir5, output,
			 dir4, output,
			 dir3, output,
			 dir2, output,
			 dir1, output,
			 dir0, output);
  
  REG_SET__R_PORT_PB_DATA(data_out, 0xff);
#endif

  /* We must initialize all (global) variables here, since the .data
     and .bss area are used before they are loaded. */

  //serial_up        = FALSE;
  nbr_read         = 0;

  /* Get a random value to use as id. */
  tx_header.id     = htonl(REG_RD(R_TIMER_DATA));
  
  /* timer01 is used as timer. */
  last_timeout     = REG_GET(R_TIMER01_DATA, count);
  
  interface        = REG_GET(R_BUS_STATUS, boot) - 1; /* 0,1,2 */
  rx_descr2.status = 0;	

  /* Initialize the boot interface */
  init_interface();
  send_ack();	/* Ack the first bootpacket, i.e. this code. seq 0. */

  while (1) {
    if (read_data()) {
      if (nbr_read >= (udword)bytes_to_read) {
	break;
      }
      else if (interface == NETWORK) {
	REG_SET(R_DMA_CH1_CMD, cmd, start);
      }
    }
  }

#if USE_LEDS
  REG_SET(R_PORT_PA_DATA, data_out, 0x55);
#endif

  level2_boot();
}

int
read_data(void)
{
  if (handle_read()) {
    return TRUE;
  }

  if (timeout()) {
    send_ack();
  }

  return FALSE;
}

int
timeout(void)
{
  volatile int now = REG_GET(R_TIMER01_DATA, count);
  int elapsed;
  int wait_time = 9600;

  elapsed = last_timeout - now;

  if (elapsed < 0) {
    elapsed = -elapsed;
  }

  if (elapsed > wait_time) {
    last_timeout = now;
    return TRUE;
  }

  return FALSE;
}

/****************** END OF FILE common_init.c *******************************/
