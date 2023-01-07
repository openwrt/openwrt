/******************************************************************************
  @file    stream_download_protocol.c
  @brief   stream protocol.

  DESCRIPTION
  QFirehoe Tool for USB and PCIE of Quectel wireless cellular modules.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  ---------------------------------------------------------------------------
  Copyright (c) 2016 - 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "usb_linux.h"
#include "hostdl_packet.h"

#define true (1==1)
#define false (1!=1)

#define MAX_SEND_BUFFER_SIZE 1280
#define MAX_RECEIVE_BUFFER_SIZE 1280
unsigned char g_Transmit_Buffer[MAX_SEND_BUFFER_SIZE];    
int g_Transmit_Length;                            

unsigned char g_Receive_Buffer[MAX_RECEIVE_BUFFER_SIZE];     
int g_Receive_Bytes;                            

static void *stream_usb_handle;

static void dump_buffer(unsigned char * buff, int len)
{
    int i = 0;

	dbg_time("dump buffer: %d bytes\n", len);
	for(i = 0; i < len; i++)
	{
		dbg_time("%02x ", buff[i]);
	}
	dbg_time("\nend\n");
	
}

#define CRC_16_L_SEED           0xFFFF
#define CRC_TAB_SIZE    256             /* 2^CRC_TAB_BITS      */
#define CRC_16_L_POLYNOMIAL     0x8408

static const uint16_t crc_16_l_table[ CRC_TAB_SIZE ] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

unsigned short crc_16_l_calc(unsigned char *buf_ptr, int len) {
	int data, crc_16;
	for (crc_16 = CRC_16_L_SEED; len >= 8; len -= 8, buf_ptr++) {
		crc_16 = crc_16_l_table[(crc_16 ^ *buf_ptr) & 0x00ff] ^ (crc_16 >> 8);
	}
	if (len != 0) {

		data = ((int) (*buf_ptr)) << (16 - 8);

		while (len-- != 0) {
			if (((crc_16 ^ data) & 0x01) != 0) {

				crc_16 >>= 1;
				crc_16 ^= CRC_16_L_POLYNOMIAL;

			} else {

				crc_16 >>= 1;

			}

			data >>= 1;
		}
	}
	return (~crc_16);
}

void  compute_reply_crc ()
{
    unsigned short crc = crc_16_l_calc (g_Transmit_Buffer, g_Transmit_Length * 8);
    g_Transmit_Buffer[g_Transmit_Length] = crc & 0xFF;
    g_Transmit_Buffer[g_Transmit_Length + 1] = crc >> 8;
    g_Transmit_Length += 2;
}

static void compose_packet(unsigned char cmd, unsigned char *parameter, uint32_t parameter_len, unsigned char *data, uint32_t data_len) {
    uint32_t i;

    g_Transmit_Buffer[0] = cmd;
    if (parameter == NULL) parameter_len = 0;
    if (data == NULL) data_len = 0;
    for (i = 0; i < parameter_len; i++) {
        g_Transmit_Buffer[1 + i] = parameter[i];
    }
    for (i = 0; i < data_len; i++) {
        g_Transmit_Buffer[1 + parameter_len + i] = data[i];
    }
    g_Transmit_Length = 1 + parameter_len + data_len;
    g_Transmit_Buffer[g_Transmit_Length] = 0;
}

static unsigned char stream_tx_buf[1280];
#define CHECK_FOR_DATA() do {} while(0)
#define TRANSMIT_BYTE(_byte)  do { stream_tx_buf[j++] = _byte; } while(0)

static int send_packet(int flag) {
    int i;
    int ch;
    int j;

    j = 0;


    CHECK_FOR_DATA ();

    /* Since we don't know how long it's been. */
    if (!!flag) { 
        TRANSMIT_BYTE (0x7E);
    }

  for (i = 0; i < g_Transmit_Length; i++)
  {
    /* we only need to check once every 31 characters, since RX and TX
     * run at about the same speed, and our RX FIFO is 64 characters
     */
    if ((i & 31) == 31)
      CHECK_FOR_DATA ();

    ch = g_Transmit_Buffer[i];

    if (ch == 0x7E || ch == 0x7D)
    {
      TRANSMIT_BYTE (0x7D);
      TRANSMIT_BYTE (0x20 ^ ch);        /*lint !e734 */
    }
    else
    {
      TRANSMIT_BYTE (ch);       /*lint !e734 */
    }
  }

    CHECK_FOR_DATA ();
    TRANSMIT_BYTE (0x7E);

#if 0
  /* Hack for USB protocol.  If we have an exact multiple of the USB frame
   * size, then the last frame will not be sent out.  The USB standard says
   * that a "short packet" needs to be sent to flush the data.  Two flag
   * characters can serve as the short packet.  Doing it this way, we only
   * perform this test once on every entire packet from the target, so the
   * over head is not too much.
   */
    if ((j%512) == 0)
    {
        TRANSMIT_BYTE (0x7E);
        TRANSMIT_BYTE (0x7E);
    }
 #endif

    return (qusb_noblock_write(stream_usb_handle, stream_tx_buf, j, j, 3000, 1) == j) ? 0 : -1;
}

static int remove_escape_hdlc_flag(unsigned char* buffer, int len)
{
	int i = 0; 
	int index = 0;
	int escape = 0;
	//dump_buffer(buffer, len);
	if(len == 0) return 0;
	//ignore the first HDLC FLAG bytes
	while(buffer[i] == 0x7e)
	{
		i++;
	}
	//all bytes is HDLC FLAG
	if(i == len) 
		return 0;
	for(; i < len; i++)
	{
		if(buffer[i] == 0x7D)
		{
			escape = 1;
			continue;
		}
		if(escape == 1)
		{
			escape = 0;
			buffer[i] ^= 0x20;
		}
		buffer[index++] = buffer[i];
	}
	buffer[index] = 0;
	//dump_buffer(buffer, index);
	return index;
}

static int receive_packet(void)
{
	int bytesread = 0;
	unsigned char *buff = g_Receive_Buffer;
	int idx = 0;
	do
	{
		bytesread = qusb_noblock_read(stream_usb_handle, &buff[idx], MAX_RECEIVE_BUFFER_SIZE, 0, 3000);
		if(bytesread == 0)
		{
			//timeout may be error
			dbg_time("%s timeout\n", __FUNCTION__);
			return 0;
		}
		//dump_buffer(&buff[idx], bytesread);
		idx += bytesread;			
		if(buff[idx - 1] == 0x7e)
		{
			//check the packet whether valid.
			g_Receive_Bytes = remove_escape_hdlc_flag(buff, idx);
			if(g_Receive_Bytes == 0)
			{
				continue;
			}else{
				return 1;
			}
		}
	}while(1);
	
	return 0;
}

static int handle_hello(void)
{
    static const char host_header[] = "QCOM fast download protocol host";
    //static const char target_header[] = "QCOM fast download protocol targ";
    //char string1[64];
    //int size;
    int err;
    dbg_time("%s\n", __func__);

    memset(&g_Transmit_Buffer[0],0,sizeof(g_Transmit_Buffer));
    g_Transmit_Buffer[HELLO_CMD_OFFSET] = HELLO_REQ;        
    memcpy(&g_Transmit_Buffer[HELLO_MAGIC_NUM_OFFSET],host_header,32);
    g_Transmit_Buffer[HELLO_MAX_VER_OFFSET] = STREAM_DLOAD_MAX_VER;
    g_Transmit_Buffer[HELLO_MIN_VER_OFFSET] = STREAM_DLOAD_MIN_VER;
    g_Transmit_Buffer[HELLO_MAX_DATA_SZ_1_OFFSET] = 0;    
    g_Transmit_Length = 36;
    
    compute_reply_crc();    
    send_packet(1);
    
    int timeout = 5;
    do{
        err = receive_packet();
        if(err == 1){
            switch(g_Receive_Buffer[0])
            {
            case 0x02:
                return 1;
            case 0x0d:
                continue;
            default:
            	//dump_buffer(g_Receive_Buffer, 64);
                return 0;
            }
        }
        else if(err == -1){
        	dbg_time("error = %d, strerr = %s\n", errno, strerror(errno));
            return 0;
        }
        timeout--;
    }while(timeout);
    
    return 0;
}

static int handle_security_mode(unsigned char trusted) {
    dbg_time("%s trusted = %d\n", __func__, trusted);
    compose_packet(0x17, &trusted, 1, NULL, 0);
    compute_reply_crc();
    send_packet(1);
	int timeout = 5;
    do{
        if(receive_packet() == 1){
            switch(g_Receive_Buffer[0])
            {
            case 0x18:
                return 1;
            default:
                return 0;
            }
        }else
		{
			timeout--;
			if(timeout==0)
			{
				dbg_time("%s timeout\n", __FUNCTION__);
				return 0;
			}
		}
    }while(1);
    return 0;
}
/*
set download flag in module, quectel custom command, 
if flag : reboot, module will enter DM
if not flag: reboot normal
*/
static int handle_quectel_download_flag(unsigned char mode) {
	//byte mode = 1;
	compose_packet(0x60, &mode, 1, NULL, 0);
	compute_reply_crc();
	send_packet(1);
	int timeout = 5;
	do{
		if(receive_packet() == 1)
		{
			switch(g_Receive_Buffer[0])
			{
				case 0x61:
					switch( g_Receive_Buffer[1] )
						{
							case 0x00:
								return 1;
							default:
								return 0;
						}
					break;
				case 0x0E:
					dbg_time("Invalid command");
					return 2;
				default:
					dump_buffer(g_Receive_Buffer, 64);
					return 0;
			}
		}
		else
		{
			timeout--;
			if(timeout==0)
			{
				dbg_time("%s timeout\n", __FUNCTION__);
				return 0;
			}
		}
	}while(1);
}

static const char *stream_firehose_dir;
static int stread_fread(const char *filename, void **pp_filebuf) {
    int filesize = 0;
    FILE *fp;
    char fullpath[MAX_PATH*2];

    snprintf(fullpath, sizeof(fullpath), "%.240s/../%.240s", stream_firehose_dir, filename);
    fp = fopen(fullpath, "rb");
    if (fp == NULL) {
        dbg_time("fail to fope %s, errno: %d (%s)\n", fullpath, errno, strerror(errno));
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);

    *pp_filebuf = malloc(filesize);
    fseek(fp, 0, SEEK_SET);
    filesize = fread(*pp_filebuf, 1, filesize, fp);
    fclose(fp);

    dbg_time("%s filename=%s, filesize=%d\n", __func__, filename, filesize);
    return filesize;
}

static int handle_parti_tbl(unsigned char override) {
    int timeout = 5;
    int filesize;
    void *filebuf;
    const char *partition_path= "partition.mbn";
    dbg_time("%s override = %d\n", __func__, override);

    filesize = stread_fread(partition_path, &filebuf);
    if (filesize <= 0)
        return 0;
   
    compose_packet(0x19, &override, 1, filebuf, filesize);
    compute_reply_crc();
    send_packet(1);
    free(filebuf);
   
    do{
        if(receive_packet() == 1){
            dbg_time("handle_parti_tbl command = %02x, status = %02x\n", g_Receive_Buffer[0], g_Receive_Buffer[1]);
            switch(g_Receive_Buffer[0])
            {
            case 0x1a:
                switch( g_Receive_Buffer[1] ){
                    case 0x00: 
                        return 1;  
                    case 0x01: //0x1 this means that the original partition is different from the current partition,try to send partition
                        return 0; 
                    case 0x02: //0x2 Partition table format not recognized, does not accept override
                        return 0;
                    case 0x03: //0x3  Erase operation failed
                        return 0;
                    break;
            default:
                return 0;
            }
            default:
                return 0;
            }
        }else
		{
			timeout--;
			if(timeout == 0)
			{
				dbg_time("%s timeout\n", __FUNCTION__);
				return 0;
			}
		}
    }while(1);
}

static int handle_reset(void) {
    dbg_time("%s\n", __func__);
    compose_packet(0x0b, NULL, 0, NULL, 0);
    compute_reply_crc();
    send_packet(1);
#if 1
	return 1;
#else
    int timeout = 5;
    do{
        if(receive_packet() == 1){
            switch(g_Receive_Buffer[0])
            {
            case 0x0c:
                return 1;
            case 0x0d:
                continue;
            default:
            	dump_buffer(g_Receive_Buffer, 64); 
                return 0;
            }
        }
        else
        {

            timeout--;
            if(timeout == 0)
            {
            	dbg_time("%s timeout\n", __FUNCTION__);
                return 0;
            }
        }
    }while(1);
#endif    
}

/******pkt_open_multi_image*******/

static void pkt_open_multi_image (unsigned char mode, unsigned char *data, uint32_t size) {
    compose_packet(0x1b, &mode, 1, data, size);
    compute_reply_crc();
}

static int handle_openmulti(uint32_t size,unsigned char* data)
{    
	int timeout = 5;
    unsigned char mode=0x0e;

    pkt_open_multi_image(mode, data, size);
    send_packet(1);
    do {
        if (receive_packet() == 1) {
            switch (g_Receive_Buffer[0]) {
            case 0x1c:
                return 1;
            case 0x0d:
                continue;
            default:
                return 0;
            }
        }else
        {

            timeout--;
            if(timeout == 0)
            {
            	dbg_time("%s timeout\n", __FUNCTION__);
                return 0;
            }
        }
    } while (1);
    return 1;    
}

/******pkt_write_multi_image*******/
static void pkt_write_multi_image(uint32_t addr, unsigned char*data, uint16_t size) {
    unsigned char parameter[4] = {(unsigned char)(addr)&0xff, (unsigned char)(addr>>8)&0xff, (unsigned char)(addr>>16)&0xff, (unsigned char)(addr>>24)&0xff};
    compose_packet(0x07, parameter, 4, data, size);
    compute_reply_crc();
}

static int handle_write(unsigned char *data, uint32_t size)
{    
    //uint32_t total_size;
    uint32_t addr = 0;
    uint32_t writesize;
    uint32_t buffer_size = 1024;
    //int loop = 1;
    int retry_cnt = 3;  //if send failed,send again
    int ret;

    //total_size = size;
    while(size)
    {
        writesize = (size < buffer_size) ? size : buffer_size;

        pkt_write_multi_image(addr, data, writesize);
start_send_packet:
        ret = send_packet(1);
        if(0 != ret)
        {
        	dbg_time("io read/write failed\n");
        	return 0;
        }
        if(receive_packet() == 1){
            switch(g_Receive_Buffer[0])
            {
            case 0x08:
                size -= writesize;
                addr += writesize;
                //retry_cnt=5;
                break;
            default:
                goto retry_send_packet;
                return 0;
            }
        }
        else
        {

retry_send_packet:
            retry_cnt--;
            if(retry_cnt > 0)
            {                                
                goto start_send_packet;
            }
            else
            {
                dbg_time( "value is [0x%02x]",g_Receive_Buffer[0]);
                return 0;
            }
        }
    
    }

    return 1;    
}
/******PARTITION*******/
static int handle_close(void) {
	int timeout = 5;
    compose_packet(0x15, NULL, 0, NULL, 0);
    compute_reply_crc();    
    send_packet(1);
    
    do{
        if(receive_packet() == 1){
            switch(g_Receive_Buffer[0])
            {
            case 0x16:
                return 1;
            default:
                return 0;
            }
        }else
        {

            timeout--;
            if(timeout == 0)
            {
            	dbg_time("%s timeout\n", __FUNCTION__);
                return 0;
            }
        }
    }while(1);
	return 0;
}

static int do_flash_mbn(const char *partion, const char *filepath) {
    int result = false;
    void *filebuf = NULL;
    int filesize = 0;
    
    dbg_time("%s %s\n", __func__, partion);

    if (filepath) {
        filesize = stread_fread(filepath, &filebuf);
        if (filesize <= 0)
            return 0;
    }
    else {
        filesize = 4*1024;
        filebuf = (unsigned char *)malloc(filesize);
        memset(filebuf, 0x00, filesize);
    }

    result = handle_openmulti(strlen(partion) + 1, (unsigned char *)partion);
    if (result == false) {
        dbg_time("%s open failed\n", partion);
        goto __fail;
    }

    dbg_time("sending '%s' (%dKB)\n", partion, (int)(filesize/1024));

    result = handle_write(filebuf, filesize);
    if (result == false) {
        dbg_time("%s download failed\n", partion);
        goto __fail;
    }

    result = handle_close();
    if (result == false) {
        dbg_time("%s close failed.\n", partion);
        goto __fail;
    }

    dbg_time("OKAY\n");
    
__fail:
    free(filebuf);

    return result;
}

int stream_download(const char *firehose_dir, void *usb_handle, unsigned qusb_zlp_mode)
{
    (void)qusb_zlp_mode;
    stream_usb_handle = usb_handle;
    stream_firehose_dir = firehose_dir;

    if (handle_hello() == false) {
        dbg_time("Send hello command fail\n");
        return false;
    } 
    
    /*
    hello packet will set dload flag in module, when upgrade interrup, restart module,module will enter dm(quectel sbl)
    */
    if (handle_security_mode(1) == false) {
        dbg_time("Send trust command fail\n");
        return false;
    }
        
    if (handle_parti_tbl(0) == false) {
        dbg_time("----------------------------------\n");
        dbg_time("Detect partition mismatch.\n");
        dbg_time("Download parition with override.\n");
        dbg_time("----------------------------------\n");

        if(handle_parti_tbl(1) == false) {
            dbg_time("override failed. \n");	
            return false;
        }
        
        /*
        partition is not match, the download flag will be clear, so set it again, reset will clear it
        */
        if(handle_quectel_download_flag(1) == false) {
            dbg_time("Set Quectel download flag failed\n");
        }
        else {
            dbg_time("Set Quectel download flag successfully\n");
        }
    }

#if 1    
    if (do_flash_mbn("0:SBL", "sbl1.mbn") == false) {
        return false;
    }
#endif

    if (handle_reset() == false) {
        dbg_time("Send reset command failed\n");
        return false;
    }

    dbg_time("%s successful\n", __func__);

    return true;
}

//hunter.lv add 
//retrieve module soft revision

typedef struct
{
	unsigned char cmd_code;
	unsigned char version;
	unsigned char reserved[2];
	unsigned char msm[4];
	unsigned char mobile_modle_number[4];
	unsigned char mobile_software_revision[1];
}__attribute__ ((packed))extended_build_id_response_t;

int retrieve_soft_revision(void *usb_handle, uint8_t *mobile_software_revision, unsigned length)
{
/* 
80-v1294-1_yyd_serial_interface_control_document_(icd)_for_cdma_dual-mode_subscriber_station_data  3.4.122 Extended Build ID
*/
    uint8_t req1[] = {0x7E,0x7C, 0x93,0x49, 0x7E};
    int ret;
    uint8_t *rx_buff = malloc(2048);

    memset(mobile_software_revision, 0x00, length);

    if (rx_buff == NULL)
        return 0;

    ret = qusb_noblock_write(usb_handle, req1, sizeof(req1), sizeof(req1), 1000, 0);
    if(ret > 0) {
        ret = qusb_noblock_read(usb_handle, rx_buff , 2048, 1, 3000);
        if (ret > 0) {
            if (rx_buff[0] == 0x7C && rx_buff[ret - 1] == 0x7E) {
                extended_build_id_response_t *rsp = (extended_build_id_response_t *)rx_buff;
                (void)length;
                memcpy(mobile_software_revision, rsp->mobile_software_revision, strlen((const char *)rsp->mobile_software_revision));
            }
        }
    }

    free(rx_buff);
    return (mobile_software_revision[0] !=  '\0');
}
