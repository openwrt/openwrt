/******************************************************************************
  @file    sahara_protocol.c
  @brief   sahara protocol.

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
#include "sahara_protocol.h"

static uint32_t le_uint32(uint32_t v32) {
    const int is_bigendian = 1;
    uint32_t tmp = v32;
    if ((*(char*)&is_bigendian) == 0) {
        unsigned char *s = (unsigned char *)(&v32);
        unsigned char *d = (unsigned char *)(&tmp);
        d[0] = s[3];
        d[1] = s[2];
        d[2] = s[1];
        d[3] = s[0];
    }
    return tmp;
}

#define dbg( log_level, fmt, arg... ) do {dbg_time(fmt "\n", ## arg);} while (0)

static int sahara_tx_data (void *usb_handle, void *tx_buffer, size_t bytes_to_send) {
    int need_zlp = 0; //zlp is not mandatory
    return qusb_noblock_write(usb_handle, tx_buffer, bytes_to_send, bytes_to_send, 3000, need_zlp);
}

int qusb_use_usbfs_interface(const void *handle);
static int sahara_rx_data(void *usb_handle, void *rx_buffer, size_t bytes_to_read) {
    sahara_packet_header* command_packet_header = NULL;
    size_t bytes_read = 0;

    const char *boot_sahara_cmd_id_str[SAHARA_LAST_CMD_ID] = {
        "SAHARA_NO_CMD_ID", //          = 0x00,
        " SAHARA_HELLO_ID", //            = 0x01, // sent from target to host
        "SAHARA_HELLO_RESP_ID", //       = 0x02, // sent from host to target
        "SAHARA_READ_DATA_ID", //        = 0x03, // sent from target to host
        "SAHARA_END_IMAGE_TX_ID", //     = 0x04, // sent from target to host
        "SAHARA_DONE_ID", //             = 0x05, // sent from host to target
        "SAHARA_DONE_RESP_ID", //        = 0x06, // sent from target to host
        "SAHARA_RESET_ID", //            = 0x07, // sent from host to target
        "SAHARA_RESET_RESP_ID", //       = 0x08, // sent from target to host
        "SAHARA_MEMORY_DEBUG_ID", //     = 0x09, // sent from target to host
        "SAHARA_MEMORY_READ_ID", //      = 0x0A, // sent from host to target
        "SAHARA_CMD_READY_ID", //        = 0x0B, // sent from target to host
        "SAHARA_CMD_SWITCH_MODE_ID", //  = 0x0C, // sent from host to target
        "SAHARA_CMD_EXEC_ID", //         = 0x0D, // sent from host to target
        "SAHARA_CMD_EXEC_RESP_ID", //    = 0x0E, // sent from target to host
        "SAHARA_CMD_EXEC_DATA_ID", //    = 0x0F, // sent from host to target
        "SAHARA_64_BITS_MEMORY_DEBUG_ID", // 	= 0x10, // sent from target to host
        "SAHARA_64_BITS_MEMORY_READ_ID", // 		= 0x11, // sent from host to target
        "SAHARA_64_BITS_READ_DATA_ID", // 		= 0x12,
    };

    if (0 == bytes_to_read) {
        if (qusb_use_usbfs_interface(usb_handle)) {
        bytes_read = qusb_noblock_read(usb_handle, rx_buffer, SAHARA_RAW_BUFFER_SIZE, 0, 5000);
             if (bytes_read < sizeof(sahara_packet_header)) 
            return 0;
        }
        else {
            bytes_read = qusb_noblock_read(usb_handle, rx_buffer, sizeof(sahara_packet_header), 0, 5000);
            if (bytes_read != sizeof(sahara_packet_header)) 
                return 0;
        }
        
        command_packet_header = (sahara_packet_header *) rx_buffer;
        if (le_uint32(command_packet_header->command) < SAHARA_LAST_CMD_ID) {
            dbg(LOG_EVENT, "RECEIVED <-- %s", boot_sahara_cmd_id_str[le_uint32(command_packet_header->command)]);

            if (!qusb_use_usbfs_interface(usb_handle)) {
                bytes_read += qusb_noblock_read(usb_handle, (uint8_t *)rx_buffer + sizeof(sahara_packet_header),
                    le_uint32(command_packet_header->length) - sizeof(sahara_packet_header), 0, 5000);
            }
        
            if (bytes_read != (le_uint32(command_packet_header->length))) {
                dbg(LOG_INFO, "Read %zd bytes, Header indicates command %d and packet length %d bytes",
                    bytes_read, le_uint32(command_packet_header->command), le_uint32(command_packet_header->length));            
                return 0;
            }
        } else {
            dbg(LOG_EVENT, "RECEIVED <-- SAHARA_CMD_UNKONOW_%d", le_uint32(command_packet_header->command));
            return 0;
        }
    } 
    else {
        bytes_read = qusb_noblock_read(usb_handle, rx_buffer, bytes_to_read, bytes_to_read, 5000);
    }
    
    return 1;
}

static int send_reset_command (void *usb_handle, void *tx_buffer) {
#if 0
    unsigned char edl_cmd[] = {0x4b, 0x65, 0x01, 0x00, 0x54, 0x0f, 0x7e};
    sahara_tx_data(usb_handle, edl_cmd, sizeof(edl_cmd));
    return 1;
#else
    sahara_packet_reset *sahara_reset = (sahara_packet_reset *)tx_buffer;
    sahara_reset->header.command = le_uint32(SAHARA_RESET_ID);
    sahara_reset->header.length = le_uint32(sizeof(sahara_packet_reset));

    /* Send the Reset Request */
    dbg(LOG_EVENT, "SENDING --> SAHARA_RESET");
    if (0 == sahara_tx_data (usb_handle, tx_buffer, sizeof(sahara_packet_reset))) {
        dbg(LOG_ERROR, "Sending RESET packet failed");
        return 0;
    }

    return 1;
#endif
}

static int send_done_packet (void *usb_handle, void *tx_buffer) {
    sahara_packet_done *sahara_done = (sahara_packet_done *)tx_buffer;

    sahara_done->header.command = le_uint32(SAHARA_DONE_ID);
    sahara_done->header.length = le_uint32(sizeof(sahara_packet_done));
    // Send the image data
    dbg(LOG_EVENT, "SENDING --> SAHARA_DONE");
    if (0 == sahara_tx_data (usb_handle, tx_buffer, sizeof(sahara_packet_done))) {
        dbg(LOG_ERROR, "Sending DONE packet failed");
        return 0;
    }
    return 1;
}

static int start_image_transfer(void *usb_handle, void *tx_buffer, const sahara_packet_read_data *sahara_read_data, FILE *file_handle)
{
    int retval = 0;
    uint32_t bytes_read = 0, bytes_to_read_next;
    uint32_t DataOffset = le_uint32(sahara_read_data->data_offset);
    uint32_t DataLength = le_uint32(sahara_read_data->data_length);

    dbg(LOG_INFO, "0x%08x 0x%08x 0x%08x", le_uint32(sahara_read_data->image_id), DataOffset, DataLength);

    if (fseek(file_handle, (long)DataOffset, SEEK_SET)) {
        dbg(LOG_INFO, "%d errno: %d (%s)", __LINE__, errno, strerror(errno));
        return 0;
    }

    while (bytes_read < DataLength) {
	bytes_to_read_next = MIN((uint32_t)DataLength - bytes_read, SAHARA_RAW_BUFFER_SIZE);
	retval = fread(tx_buffer, 1, bytes_to_read_next, file_handle);
    
        if (retval < 0) {
            dbg(LOG_ERROR, "file read failed: %s", strerror(errno));
            return 0;
        }
    
        if ((uint32_t) retval != bytes_to_read_next) {
            dbg(LOG_ERROR, "Read %d bytes, but was asked for 0x%08x bytes", retval, DataLength);
            return 0;
	}

	/*send the image data*/
	if (0 == sahara_tx_data (usb_handle, tx_buffer, bytes_to_read_next)) {
            dbg(LOG_ERROR, "Tx Sahara Image Failed");
            return 0;
	}
    
	bytes_read += bytes_to_read_next;
    }

    return 1;
}

static int start_image_transfer_64bit(void *usb_handle, void *tx_buffer, const sahara_packet_read_data_64bit *sahara_read_data_64bit, FILE *file_handle)
{
    sahara_packet_read_data sahara_read_data;

    sahara_read_data.header = sahara_read_data_64bit->header;
    sahara_read_data.image_id = sahara_read_data_64bit->image_id;
    sahara_read_data.data_offset = sahara_read_data_64bit->data_offset;
    sahara_read_data.data_length = sahara_read_data_64bit->data_length;

    return start_image_transfer(usb_handle, tx_buffer, &sahara_read_data, file_handle);
}

static int sahara_start(void *usb_handle, void *tx_buffer, void *rx_buffer, FILE *file_handle) {
    uint32_t image_id = 0;
    sahara_packet_header* sahara_cmd = (sahara_packet_header *)rx_buffer;
    sahara_packet_hello *sahara_hello = (sahara_packet_hello *)rx_buffer;
    sahara_packet_read_data *sahara_read_data = (sahara_packet_read_data *)rx_buffer;
    sahara_packet_read_data_64bit *sahara_read_data_64bit = (sahara_packet_read_data_64bit *)rx_buffer;
    sahara_packet_done_resp *sahara_done_resp = (sahara_packet_done_resp *)rx_buffer;
    sahara_packet_end_image_tx *sahara_end_image_tx = (sahara_packet_end_image_tx *)rx_buffer;

    sahara_packet_hello_resp *sahara_hello_resp = (sahara_packet_hello_resp *)tx_buffer;   

    dbg(LOG_EVENT, "STATE <-- SAHARA_WAIT_HELLO");
    if (0 == sahara_rx_data(usb_handle, rx_buffer, 0)) {
#if 0
        unsigned char edl_cmd[] = {0x4b, 0x65, 0x01, 0x00, 0x54, 0x0f, 0x7e};
        sahara_tx_data(usb_handle, edl_cmd, sizeof(edl_cmd));
#else
        sahara_tx_data(usb_handle, tx_buffer, 1);
#endif
        if (0 == sahara_rx_data(usb_handle, rx_buffer, 0))
            return 0;
    }

    //Check if the received command is a hello command
    if (le_uint32(sahara_cmd->command) != SAHARA_HELLO_ID) {
        dbg(LOG_ERROR, "Received a different command: %x while waiting for hello packet", sahara_hello->header.command);
        send_reset_command(usb_handle, rx_buffer);
        return 0;
    }
    
    // Recieved hello, send the hello response
    //Create a Hello request
    sahara_hello_resp->header.command = le_uint32(SAHARA_HELLO_RESP_ID);
    sahara_hello_resp->header.length = le_uint32(sizeof(sahara_packet_hello_resp));
    sahara_hello_resp->version = sahara_hello->version; //SAHARA_VERSION;
    sahara_hello_resp->version_supported = sahara_hello->version_supported; //SAHARA_VERSION_SUPPORTED;
    sahara_hello_resp->status = le_uint32(SAHARA_STATUS_SUCCESS);
    sahara_hello_resp->mode = sahara_hello->mode;
    sahara_hello_resp->reserved0 = le_uint32(1);
    sahara_hello_resp->reserved1 = le_uint32(2);
    sahara_hello_resp->reserved2 = le_uint32(3);
    sahara_hello_resp->reserved3 = le_uint32(4);
    sahara_hello_resp->reserved4 = le_uint32(5);
    sahara_hello_resp->reserved5 = le_uint32(6);

    switch (le_uint32(sahara_hello->mode)) {
        case SAHARA_MODE_IMAGE_TX_PENDING:
            dbg(LOG_EVENT, "RECEIVED <-- SAHARA_MODE_IMAGE_TX_PENDING");
        break;
        case SAHARA_MODE_IMAGE_TX_COMPLETE:
            dbg(LOG_EVENT, "RECEIVED <-- SAHARA_MODE_IMAGE_TX_COMPLETE");
        break;
        case SAHARA_MODE_MEMORY_DEBUG:
            dbg(LOG_EVENT, "RECEIVED <-- SAHARA_MODE_MEMORY_DEBUG");
        break;
        case SAHARA_MODE_COMMAND:
            dbg(LOG_EVENT, "RECEIVED <-- SAHARA_MODE_COMMAND");
        break;
        default:
            dbg(LOG_EVENT, "RECEIVED <-- SAHARA_MODE_0x%x", le_uint32(sahara_hello->mode));
        break;
    }

    if (le_uint32(sahara_hello->mode) != SAHARA_MODE_IMAGE_TX_PENDING) {
        dbg(LOG_ERROR, "ERROR NOT SAHARA_MODE_IMAGE_TX_PENDING");
        sahara_hello_resp->mode = SAHARA_MODE_IMAGE_TX_PENDING;
    }

    /*Send the Hello  Resonse Request*/
    dbg(LOG_EVENT, "SENDING --> SAHARA_HELLO_RESPONSE");
    if (0 == sahara_tx_data (usb_handle, tx_buffer, sizeof(sahara_packet_hello_resp))) {
        dbg(LOG_ERROR, "Tx Sahara Data Failed ");
        return 0;
    }

    while (1) {
        dbg(LOG_INFO, "STATE <-- SAHARA_WAIT_COMMAND");
        if (0 == sahara_rx_data(usb_handle, rx_buffer, 0))
            return 0;

        if (le_uint32(sahara_cmd->command) == SAHARA_READ_DATA_ID) {
            start_image_transfer(usb_handle, tx_buffer, sahara_read_data, file_handle);
        }
        else if (le_uint32(sahara_cmd->command) == SAHARA_64_BITS_READ_DATA_ID) {
            start_image_transfer_64bit(usb_handle, tx_buffer, sahara_read_data_64bit, file_handle);
        }
        else if (le_uint32(sahara_cmd->command) == SAHARA_END_IMAGE_TX_ID) {
            dbg(LOG_EVENT, "image_id = %d, status = %d", le_uint32(sahara_end_image_tx->image_id), le_uint32(sahara_end_image_tx->status));
            if (le_uint32(sahara_end_image_tx->status) == SAHARA_STATUS_SUCCESS) {
                image_id = le_uint32(sahara_end_image_tx->image_id);
                send_done_packet (usb_handle, tx_buffer);
                break;
            } else {
                return 0;
            }
        }
        else if (le_uint32(sahara_cmd->command) == SAHARA_HELLO_ID) {
             continue;
        }
        else {
            dbg(LOG_ERROR, "Received an unknown command: %d ", le_uint32(sahara_cmd->command));
            send_reset_command (usb_handle, tx_buffer);
            return 0;
        }
    }

    dbg(LOG_EVENT, "STATE <-- SAHARA_WAIT_DONE_RESP");
    if (0 == sahara_rx_data(usb_handle, rx_buffer, 0))
        return 0;

    dbg(LOG_INFO, "image_tx_status = %d", le_uint32(sahara_done_resp->image_tx_status));

    if (SAHARA_MODE_IMAGE_TX_PENDING == le_uint32(sahara_done_resp->image_tx_status)) {
        if (image_id == 13) //prog_nand_firehose_9x07.mbn
            return 1;
        if (image_id == 7) //NPRG9x55.mbn
            return 1;
        if (image_id == 21) //sbl1.mbn, October 22 2020 2:12 PM, AG35CEVAR05A07T4G 
            return 1;
      }
      else if (SAHARA_MODE_IMAGE_TX_COMPLETE == le_uint32(sahara_done_resp->image_tx_status)) {
        dbg(LOG_EVENT,"Successfully uploaded all images");
        return 1;
    }
    else {
        dbg(LOG_ERROR, "Received unrecognized status %d at SAHARA_WAIT_DONE_RESP state",
            le_uint32(sahara_done_resp->image_tx_status));
        return 0;
    }

    return 0;
}

int sahara_main(const char *firehose_dir, const char *firehose_mbn, void *usb_handle, int edl_mode_05c69008) {
    int retval = 0;
    char full_path[512];
    FILE *file_handle;
    void *tx_buffer;
    void *rx_buffer;

    if (edl_mode_05c69008) {
        snprintf(full_path, sizeof(full_path), "%.255s/%.240s", firehose_dir, firehose_mbn);
    }
    else {
        char *prog_nand_firehose_filename = NULL;

        snprintf(full_path, sizeof(full_path), "%.255s/..", firehose_dir);
        if (!qfile_find_file(full_path, "NPRG9x", ".mbn", &prog_nand_firehose_filename)
            && !qfile_find_file(full_path, "NPRG9x", ".mbn", &prog_nand_firehose_filename)) {
            dbg(LOG_ERROR, "retrieve NPRG MBN failed.");
            return ENOENT;
        }
        dbg(LOG_INFO, "prog_nand_firehose_filename = %s", prog_nand_firehose_filename);

        snprintf(full_path, sizeof(full_path), "%.255s/../%.240s", firehose_dir, prog_nand_firehose_filename);
        free(prog_nand_firehose_filename);
    }
    
    file_handle = fopen(full_path, "rb");
    if (file_handle == NULL) {
        dbg(LOG_INFO, "%s %d %s errno: %d (%s)", __func__, __LINE__, full_path, errno, strerror(errno));
        return ENOENT;
    }

    rx_buffer = malloc (SAHARA_RAW_BUFFER_SIZE);
    tx_buffer = malloc (SAHARA_RAW_BUFFER_SIZE);

    if (NULL == rx_buffer || NULL == tx_buffer) {
        dbg(LOG_ERROR, "Failed to allocate sahara buffers");
        return ENOMEM;
    }

    retval = sahara_start(usb_handle, tx_buffer, rx_buffer, file_handle);
    if (0 == retval) {
        dbg(LOG_ERROR, "Sahara protocol error");
    }
    else {
        dbg(LOG_STATUS, "Sahara protocol completed");
    }

    free(rx_buffer);
    free(tx_buffer);
    fclose(file_handle);

    if (retval)
        return 0;
    
    return __LINE__;
}
