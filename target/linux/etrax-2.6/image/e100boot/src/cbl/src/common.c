#include "hwregs.h"
#include "e100boot.h"

static void toggle_led(void);

static void read_load_info(void);
static void decode_load_info(void);

static void read_file(byte* addr, udword size);

#if USE_PRINT_DESCR
static void print_descr(dma_descr_T *d);
#endif

static int  memory_test(udword addr, udword size, udword *failed_address);
static void memory_dump(udword *from, udword *to);

extern byte _Stext[];
extern byte _Edata[];

#ifdef USE_BAUDRATE_CHANGING
byte change_baudrate;
udword new_baudrate;
#endif

void
level2_boot(void)
{
#if USE_LEDS
  REG_SET(R_PORT_PA_DATA, data_out, 0xaa);
#endif

#if 0
  io_buf_next    = (byte*)IO_BUF_START;
  io_buf_cur     = (byte*)IO_BUF_START;
#endif

#if 1
  send_string("\r\n\r\nDevice ID = ");
  send_hex(ntohl(tx_header.id), NL);
  send_string(e100boot_version);
#endif

#if 1
  {
    udword sum = 0;
    byte *b;

    for (b = (byte*)_Stext; b != (byte*)_Edata; b++) {
      sum += *b;
    }
    send_string("Checksum of bootloader is ");
    send_hex(sum, NL);
  }
#endif

  read_load_info();

  __asm__ volatile ("jump _start");
}

void 
toggle_led(void)
{
#if USE_LEDS
  REG_SET(R_PORT_PA_DATA, data_out, 0x55);

  while (1) {
    REG_SET(R_PORT_PA_DATA, data_out, ~REG_GET(R_PORT_PA_READ, data_in));
    {
      volatile udword i;
      
      for (i = 0; i != 2000000; i++)
        ;
    }
  }
#else
  while (1) {
  }
#endif
}

void
read_load_info(void)
{
#ifdef USE_BAUDRATE_CHANGING
  change_baudrate = 0;
#endif
  
  send_string("Waiting for load info.\r\n");

  send_ack();
  
  read_file((byte*)IO_BUF_START, IO_BUF_END - IO_BUF_START - CRC_LEN);
  send_string("Got load info.\r\n");
  decode_load_info();
  
#ifdef USE_BAUDRATE_CHANGING
  if (change_baudrate) {
    REG_WR(R_SERIAL0_BAUD, new_baudrate);
    {
      udword i = 0;
      
      while (i++ < 1000000)
        ;
    }
    send_ack();
  }
#endif

  toggle_led();
}

void 
decode_load_info(void)
{
  udword         *type_p = (udword*)IO_BUF_START;
  udword          failed_address;
  udword          i;
  command_T      *cmd;

  while (type_p != (udword*)(IO_BUF_END - CRC_LEN)) { /* !!! */
//    send_hex(type_p, NL);
    *type_p = ntohl(*type_p);
//    send_hex(*type_p, NL);
    type_p++;
  }

//  memory_dump(IO_BUF_START, IO_BUF_END);

  cmd = (command_T*)IO_BUF_START;
  while (cmd->type) {
    switch (cmd->type) {
    case PACKET_INFO:
      send_string("PACKET_INFO\r\n");
      send_hex(cmd->args.packet_info.addr, NL);
      send_hex(cmd->args.packet_info.size, NL);

      seq--;
      send_ack();
      seq++;

      read_file((byte*)cmd->args.packet_info.addr, cmd->args.packet_info.size);

      (byte*)cmd += sizeof cmd->type + sizeof cmd->args.packet_info;
      break;

    case SET_REGISTER:
      send_string("SET_REGISTER\r\n");
      send_hex(cmd->args.set_register.addr, NL);
      send_hex(cmd->args.set_register.val, NL);

      *(udword*)cmd->args.set_register.addr = cmd->args.set_register.val;

      (byte*)cmd += sizeof cmd->type + sizeof cmd->args.set_register;
      break;

    case GET_REGISTER:
      send_string("GET_REGISTER\r\n");
      send_hex(cmd->args.get_register.addr, NL);
      send_hex(*(udword*)cmd->args.get_register.addr, NL);

      (byte*)cmd += sizeof cmd->type + sizeof cmd->args.get_register;
      break;

    case PAUSE_LOOP:
      send_string("PAUSE_LOOP\r\n");
      send_hex(cmd->args.pause_loop.pause, NL);

      for (i = cmd->args.pause_loop.pause; i; i--)
        ;

      (byte*)cmd += sizeof cmd->type + sizeof cmd->args.pause_loop;
      break;

    case MEM_VERIFY:
      send_string("MEM_VERIFY\r\n");
      send_hex(cmd->args.mem_verify.addr, NL);
      send_hex(cmd->args.mem_verify.val, NL);

      if (*(udword*)cmd->args.mem_verify.addr != cmd->args.mem_verify.val) {
        send_string("verify failed\r\n");
        goto decode_failed;
      }

      (byte*)cmd += sizeof cmd->type + sizeof cmd->args.mem_verify;
      break;

    case MEM_TEST:
      send_string("MEM_TEST\r\n");
      send_hex(cmd->args.mem_test.from, NL);
      send_hex(cmd->args.mem_test.to, NL);

      if (!memory_test(cmd->args.mem_test.from,
                       cmd->args.mem_test.to,
                       &failed_address)) {
        send_string("### Memory test failed at ");
        send_hex(failed_address, NL);
        memory_dump((udword*)DWORD_ALIGN(failed_address - 64),
                    (udword*)DWORD_ALIGN(failed_address + 64));
        goto decode_failed;
      }
      send_string("Passed memory test.\r\n");

      (byte*)cmd += sizeof cmd->type + sizeof cmd->args.mem_test;
      break;

    case MEM_DUMP:
      send_string("MEM_DUMP\r\n");
      send_hex(cmd->args.mem_dump.from_addr, NL);
      send_hex(cmd->args.mem_dump.to_addr, NL);

      memory_dump((udword*)cmd->args.mem_dump.from_addr,
                  (udword*)cmd->args.mem_dump.to_addr);

      (byte*)cmd += sizeof cmd->type + sizeof cmd->args.mem_dump;
      break;

    case MEM_CLEAR:
      send_string("MEM_CLEAR\r\n");
      send_hex(cmd->args.mem_clear.from_addr, NL);
      send_hex(cmd->args.mem_clear.to_addr, NL);

      for (i = cmd->args.mem_clear.from_addr;
           i <= cmd->args.mem_clear.to_addr;
           i++) {
        *(byte*)i = 0x00;
      }

      (byte*)cmd += sizeof cmd->type + sizeof cmd->args.mem_clear;
      break;

    case FLASH:
      send_string("FLASH\r\n");
      send_hex((udword)cmd->args.flash.source, NL);
      send_hex(cmd->args.flash.offset, NL);
      send_hex(cmd->args.flash.size, NL);

      if ((i = flash_write(cmd->args.flash.source,
                           cmd->args.flash.offset,
                           cmd->args.flash.size)) != ERR_FLASH_OK) {
        if (i == ERR_FLASH_VERIFY) {
          udword size =
            (cmd->args.flash.size < 65536 ? cmd->args.flash.size : 65536);

          /* Try to erase the first block(s) we tried to flash to prevent a
             unit which failed to flash correctly from booting */
          flash_write(NULL, cmd->args.flash.offset, size);
        }

        goto decode_failed;
      }

      (byte*)cmd += sizeof cmd->type + sizeof cmd->args.flash;
      break;

    case JUMP:
#if 1
      /* for the printf function in our libc */
      REG_WR(R_DMA_CH8_FIRST, *(udword*)&tx_header.dest[0]);
      REG_WR(R_DMA_CH9_FIRST, *(uword*)&tx_header.dest[4]);
//      REG_WR(R_NETWORK_SA_1, &tx_header.dest[4]);
//      REG_WR(R_NETWORK_SA_2, tx_header.id);
#endif
      send_string("JUMP\r\n");
      send_hex(cmd->args.jump.addr, NL);
      send_string("END\r\n");

      __asm__ volatile ("jump %0" :: "r" (cmd->args.jump.addr));    

      (byte*)cmd += sizeof cmd->type + sizeof cmd->args.jump;
      break;

    case LOOP:
      send_string("LOOP\r\n");
      send_hex(cmd->args.bne.addr, NL);
      send_hex(cmd->args.bne.target, NL);

      if (*(udword*)cmd->args.bne.addr) {
        (*(udword*)cmd->args.bne.addr)--;
        (byte*)cmd = cmd->args.bne.target;
      }
      else {
        (byte*)cmd += sizeof cmd->type + sizeof cmd->args.bne;
      }
      break;

#ifdef USE_BAUDRATE_CHANGING
    case BAUDRATE:
      send_string("BAUDRATE\r\n");
      send_hex(cmd->args.br.baudrate, NL);

      new_baudrate = cmd->args.br.baudrate;

      (byte*)cmd += sizeof cmd->type + sizeof cmd->args.br;
      break;
#endif

    default:
      send_string("### Unknown type: ");
      send_hex(cmd->type, NL);

      goto decode_failed;
      break;
    }
  }

decode_failed:
  send_string("END\r\n");
}

void 
read_file(byte* addr, udword size)
{
  udword nbr_read_last;
  udword sum;
  byte *b;
  byte *from;
  
/* send_string(">read_file\r\n"); */
  
  nbr_read = 0;
  nbr_read_last = 0;
  target_address = (udword)addr;
  
  if (interface == NETWORK) {
    rx_descr2.buf    = (udword)addr;
    bytes_to_read    = size;
    rx_descr2.sw_len = size + CRC_LEN > 1500 ? 1500 : size + CRC_LEN;
/*  rx_descr2.sw_len = 1500; */

    REG_SET(R_DMA_CH1_FIRST, first, (udword)&rx_descr);
    
    /* Restart receiver so descriptor is re-read. */
    REG_SET(R_DMA_CH1_CMD, cmd, reset);
    while (REG_EQL(R_DMA_CH1_CMD, cmd, reset)) {
    }
    
    REG_SET(R_DMA_CH1_CMD, cmd, start);

    while (1) {
/*    send_hex(rx_descr2.hw_len, NL); */
      from = (byte*)rx_descr2.buf;
      if (read_data()) {
        if (nbr_read < size) {
          REG_SET(R_DMA_CH1_CMD, cmd, start);
        }
        
#if USE_PRINT_DESCR
        print_descr(&rx_descr);
        print_descr(&rx_descr2);
#endif
        
#if 0
        send_string("Read ");
        send_hex(rx_descr2.hw_len - CRC_LEN, NO_NL);
        send_string(" bytes. ");
        send_hex((udword)from, NO_NL);
        send_string(" - ");
        send_hex(rx_descr2.buf-1, NO_NL);
        send_string(" (");
        send_hex(nbr_read, NO_NL);
        send_string("/");
        send_hex(size, NO_NL);
        send_string(")\r\n");
#endif

        nbr_read_last = nbr_read;
/*      from = (byte*)rx_descr2.buf; */

        if (nbr_read >= size) {
          break;
        }
      }
    }
  }
  else  {  /* interface != NETWORK */
    while (nbr_read < size) {
      read_data();
    }
  }
  
  sum = 0;
  for (b = addr; b != (byte*)(addr+size); b++) {
    sum += *b;
  }
  send_string("Checksum of file is ");
  send_hex(sum, NL);

/*  memory_dump((udword*)addr, (udword*)addr+size); */
/*  send_string("<read_file\r\n"); */
}

#if USE_PRINT_DESCR
void
print_descr(dma_descr_T *d)
{
  send_string("Descriptor at ");
  send_hex((udword)d, NL);
  
  send_string("ctrl   : ");
  send_hex(d->ctrl, NL);
  
  send_string("sw_len : ");
  send_hex(d->sw_len, NL);
  
  send_string("next   : ");
  send_hex(d->next, NL);
  
  send_string("buf    : ");
  send_hex(d->buf, NL);
  
  send_string("status : ");
  send_hex(d->status, NL);
  
  send_string("hw_len : ");
  send_hex(d->hw_len, NL);
}
#endif

int
memory_test(udword from, udword to, udword *failed_address)
{
  udword i;
  udword j;
  byte b;

  /* At each dword (but bytewise) write the inverse of the adress,
     check that it worked, then write the inverse of the last byte
     written. Exit on fail. The memory after a successfull test will
     be: 

     0xC0000000 : 0xC0000000 0xC0000004 0xC0000008 0xC000000C
     0xC0000010 : 0xC0000010 0xC0000014 0xC0000018 0xC000001C
     */

  for (i = from; i < to; i += 4) {
    for (j = 0; (j != sizeof(udword)) && (i+j < to); j++) {
      b = ((~i) >> (j*8)) & 0xff;
      *(volatile byte*)(i+j) = b;
      if (*(volatile byte*)(i+j) == b) {
        *(volatile byte*)(i+j) = ~b;
      }
      else {
        *failed_address = i+j;
        send_string("### Memory test 1 failed at ");
        send_hex(*failed_address, NL);
        return FALSE;
      }
    }
  }

  /* Run through entire region, check bytewise that the dwords contain
     the address to the dword. Exit on fail. */

  for (i = from; i < to; i += 4) {
    for (j = 0; (j != sizeof(udword)) && (i+j < to); j++) {
      b = (i >> (j*8)) & 0xff;
      if (*(volatile byte*)(i+j) != b) {
        *failed_address = i+j;
        send_string("### Memory test 2 failed at ");
        send_hex(*failed_address, NL);
        return FALSE;
      }
    }
  }

  return TRUE;
}

void
memory_dump(udword *from, udword *to)
{
  udword *i = from;
  int j;
  
  for (; i <= to; i += 4) {
    send_hex((udword)i, NO_NL);
    send_string(" :");
    for(j = 0; j != 4 && (i+j <= to); j++) {
      send_string(" ");
      send_hex(*(udword*)(i+j), NO_NL);
    }
    send_string("\r\n");
  }
}
