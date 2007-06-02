#define ser_init           init_interface
#define handle_serial_read handle_read
#define send_serial_ack    send_ack
#define send_serial_string send_string
#define send_serial_hex    send_hex

#include "hwregs.h"
#include "sv_addr_ag.h"
#include "e100boot.h"

void 
ser_init(void)
{
  REG_SET__R_SERIAL0_XOFF
    (
     tx_stop,   enable,
     auto_xoff, disable,
     xoff_char, 0
     );
  
  REG_SET__R_SERIAL0_BAUD
    (
     tr_baud,            c9600Hz,
     rec_baud,           c9600Hz
     );

  REG_SET__R_SERIAL0_REC_CTRL
    (
     dma_err,            stop,
     rec_enable,         enable,
     rts_,               inactive,
     sampling,           middle,
     rec_stick_par,      normal,
     rec_par,            even,
     rec_par_en,         disable,
     rec_bitnr,          rec_8bit
     );
  
  REG_SET__R_SERIAL0_TR_CTRL 
    (
     txd,                0,
     tr_enable,          enable,
     auto_cts,           disabled,
     stop_bits,          one_bit,
     tr_stick_par,       normal,
     tr_par,             even,
     tr_par_en,          disable,
     tr_bitnr,           tr_8bit
     );

  serial_up = TRUE;
}

#define SER_MASK (IO_MASK(R_SERIAL0_READ, data_avail) | IO_MASK(R_SERIAL0_READ, data_in))

int
handle_serial_read(void)
{
  udword status_and_data_in = *R_SERIAL0_READ & SER_MASK;

  if (status_and_data_in & IO_STATE(R_SERIAL0_READ, data_avail, yes)) {
    *(char*)(target_address + nbr_read++) = status_and_data_in & 0xff; /* ugly mask */
    last_timeout = REG_GET(R_TIMER0_DATA, count);
    return TRUE;
  }

  return FALSE;
}

void
send_serial_ack(void)
{
  while (!REG_EQL(R_SERIAL0_STATUS, tr_ready, ready))
    ;

  REG_ISET(0, R_SERIAL0_TR_DATA, data_out, '+');
}

void 
send_serial_string(char *str)
{
  int i;

  for (i = 0; str[i];) {
    if (REG_IEQL(0, R_SERIAL0_STATUS, tr_ready, ready)) {
      REG_ISET(0, R_SERIAL0_TR_DATA, data_out, str[i]); 
      i++;
    }
  }
}

void 
send_serial_hex(udword v, byte nl)
{
  int i;
  byte buf[13];
  byte nybble;

  buf[0] = '0';
  buf[1] = 'x';
  buf[10] = '\r';
  buf[11] = '\n';
  buf[12] = '\0';

  if (nl == FALSE) {
    buf[10] = '\0';
  }

  for (i = 0; i != 8; i++) {
    nybble = (v >> (i*4)) & 0xf;
    if (nybble > 9) {
      nybble += 7;
    }
    buf[7-i+2] = nybble + '0';
  }
  send_serial_string(buf);
}
