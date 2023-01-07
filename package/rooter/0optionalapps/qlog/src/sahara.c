#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <termios.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <malloc.h>
#include <stdbool.h>
#include <pthread.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/un.h>
unsigned int inet_addr(const char *cp);

/* modify macro MIN
* usually we difine it as: (a) < (b) ? (a) : (b)
* but it will cause some problems, here is a case:
* MIN(i++, j++), when calling the macro above, i++ will be run two times, which is wrong.
* so we can modify it as following.
*  (void)(&_a == &_b); is use to check wether the type of 'a' and 'b' is same or not.
* (void) is used to eliminated warnning.
*/
#define MIN(a, b) ({ \
            typeof(a) _a = a; \
            typeof(b) _b = b; \
            (void)(&_a == &_b); \
            _a < _b ? _a : _b; \
        })

#include "sahara_protocol.h"
#include "qlog.h"

sahara_data_t sahara_data = {
    NULL,              // rx_buffer
    NULL,              // tx_buffer
    NULL,              // misc_buffer
    SAHARA_WAIT_HELLO, // state
    0,                 // timed_data_size
    -1,                // fd
    -1,                // ram_dump_image
    5,                 // max_ram_dump_retries
    SAHARA_RAW_BUFFER_SIZE,           // max_ram_dump_read
    SAHARA_MODE_LAST,  // mode
    SAHARA_MODE_LAST,  // prev_mode
    0,                 // command
	false              // ram_dump_64bit
};

typedef struct  {
    char *port_name;
    int port_fd;
    int rx_timeout;
    size_t MAX_TO_READ;
    size_t MAX_TO_WRITE;
} com_port_t;

static com_port_t com_port = {
    "/dev/ttyUSB0",                   // port_name
    -1, // port_fd
    5,                    // rx_timeout
    1024 * 64,
    1024 * 64,
};

typedef struct {
    const char *path_to_save_files;
    int verbose;
    int do_reset;
} kickstart_options_t;

static kickstart_options_t kickstart_options = {
    NULL,   // path_to_save_files
    1,     // verbose
    1,
};

enum LOG_LEVEL {
LOG_DEBUG = 1,
LOG_EVENT,
LOG_INFO,
LOG_STATUS,
LOG_WARN,
LOG_ERROR
};

extern unsigned qlog_msecs(void);
#define dbg( log_level, fmt, arg... ) do {if (kickstart_options.verbose || LOG_ERROR == log_level) { unsigned msec = qlog_msecs();  printf("[%03d.%03d]" fmt "\n",  msec/1000, msec%1000, ## arg);}} while (0)

static bool port_tx_data (void *buffer, size_t bytes_to_send) {
    int temp_bytes_sent;
    size_t bytes_sent = 0;

    while (bytes_sent < bytes_to_send) {
		do {
			temp_bytes_sent = write (com_port.port_fd, buffer + bytes_sent, MIN(bytes_to_send - bytes_sent, com_port.MAX_TO_WRITE));
			if (-1 == temp_bytes_sent && (errno == EINTR || errno == EAGAIN)) {
				sleep(1);
			} else {
				break;
			}
		} while(1);
        
        if (temp_bytes_sent <= 0) {
            dbg(LOG_ERROR, "Write returned failure %d, errno %d, System error code: %s", temp_bytes_sent, errno, strerror (errno));
            return false;
        }
        else {
            bytes_sent += temp_bytes_sent;
        }
    }

    return true;
}

static bool port_rx_data(void *buffer, size_t bytes_to_read, size_t *bytes_read) {
    fd_set rfds;
    struct timeval tv;
    int retval;

    // Init read file descriptor
    FD_ZERO (&rfds);
    FD_SET (com_port.port_fd, &rfds);

    // time out initializtion.
    tv.tv_sec  = com_port.rx_timeout >= 0 ? com_port.rx_timeout : 0;
    tv.tv_usec = 0;

    retval = select (com_port.port_fd + 1, &rfds, NULL, NULL, ((com_port.rx_timeout >= 0) ? (&tv) : (NULL)));    
    if (retval <= 0) {
        dbg(LOG_ERROR, "select returned error: %s", strerror (errno));
        return false;
    }

    retval = read (com_port.port_fd, buffer, MIN(bytes_to_read, com_port.MAX_TO_READ));
    if (retval <= 0) {
        dbg(LOG_ERROR, "Read/Write File descriptor returned error: %s, error code %d", strerror (errno), retval);
        return false;
    }

    if (NULL != bytes_read)
        *bytes_read = retval;
    
    return true;
}

static bool sahara_tx_data (size_t bytes_to_send) {
    return port_tx_data(sahara_data.tx_buffer, bytes_to_send);
}

static bool sahara_rx_data(size_t bytes_to_read) {
    sahara_packet_header* command_packet_header = NULL;
    size_t temp_bytes_read = 0, bytes_read = 0;

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
        command_packet_header = (sahara_packet_header *) sahara_data.rx_buffer;
        memset(command_packet_header, 0x00, sizeof(sahara_packet_header));
        
        if (false == port_rx_data(sahara_data.rx_buffer, sizeof(sahara_packet_header), &temp_bytes_read)) 
            return false;

        dbg(LOG_INFO, "Read %zd bytes, command %d and packet length %d bytes", temp_bytes_read, qlog_le32(command_packet_header->command), qlog_le32(command_packet_header->length));
        if (temp_bytes_read != sizeof(sahara_packet_header))
            return false;

        if (qlog_le32(command_packet_header->command) < SAHARA_LAST_CMD_ID) {
            dbg(LOG_EVENT, "RECEIVED <-- %s", boot_sahara_cmd_id_str[qlog_le32(command_packet_header->command)]);
            if (false == port_rx_data(sahara_data.rx_buffer + sizeof(sahara_packet_header), qlog_le32(command_packet_header->length) - sizeof(sahara_packet_header), &temp_bytes_read)) 
                return false;
            if (temp_bytes_read != (qlog_le32(command_packet_header->length) - sizeof(sahara_packet_header))) {
                dbg(LOG_INFO, "Read %zd bytes", temp_bytes_read + sizeof(sahara_packet_header));
                return false;
             }
        } else {
            dbg(LOG_EVENT, "RECEIVED <-- SAHARA_CMD_UNKONOW_%d", qlog_le32(command_packet_header->command));
            return false;
        }
    } 
    else {
        while (bytes_read < bytes_to_read) {
            if (false == port_rx_data(sahara_data.rx_buffer + bytes_read, bytes_to_read - bytes_read, &temp_bytes_read)) {
                dbg(LOG_ERROR, "bytes_read = %zd, bytes_to_read = %zd", bytes_read, bytes_to_read);
                return false;
            } else
                bytes_read += temp_bytes_read;
        }
    }
    
    return true;
}

static int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y) {
    // Perform the carry for the later subtraction by updating y.
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    // Compute the time remaining to wait. tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    // Return 1 if result is negative.
    return x->tv_sec < y->tv_sec;
}

static void time_throughput_calculate(struct timeval *start_time, struct timeval *end_time, size_t size_bytes)
{
    struct timeval result;
	double TP = 0.0;

    if (size_bytes == 0) {
        dbg(LOG_INFO, "Cannot calculate throughput, size is 0");
        return;
    }
    timeval_subtract(&result, end_time, start_time);

	TP  = (double)result.tv_usec/1000000.0;
	TP += (double)result.tv_sec;

	if(TP>0.0)
	{
		TP = (double)((double)size_bytes/TP)/(1024.0*1024.0);
		dbg(LOG_STATUS, "%zd bytes transferred in %ld.%06ld seconds (%.4fMBps)", size_bytes, result.tv_sec, result.tv_usec,TP);
	}
	else
		dbg(LOG_STATUS, "%zd bytes transferred in %ld.%06ld seconds", size_bytes, result.tv_sec, result.tv_usec);
}

static bool send_reset_command ()
{
    sahara_packet_reset *sahara_reset = (sahara_packet_reset *)sahara_data.tx_buffer;
    sahara_reset->header.command = qlog_le32(SAHARA_RESET_ID);
    sahara_reset->header.length = qlog_le32(sizeof(sahara_packet_reset));

    /* Send the Reset Request */
    dbg(LOG_EVENT, "SENDING --> SAHARA_RESET");
    if (false == sahara_tx_data (sizeof(sahara_packet_reset))) {
        dbg(LOG_ERROR, "Sending RESET packet failed");
        return false;
    }

    return true;
}

static bool send_memory_read_packet (uint64_t memory_table_address, uint64_t memory_table_length) {
    sahara_packet_memory_read *sahara_memory_read = (sahara_packet_memory_read *)sahara_data.tx_buffer;
    sahara_packet_memory_read_64bit *sahara_memory_read_64bit = (sahara_packet_memory_read_64bit *)sahara_data.tx_buffer;

    dbg(LOG_EVENT, "SENDING -->  SAHARA_MEMORY_READ, address 0x%08"PRIX64", length 0x%08"PRIX64, memory_table_address, memory_table_length);

    if (true == sahara_data.ram_dump_64bit) {
        sahara_memory_read_64bit->header.command = qlog_le32(SAHARA_64_BITS_MEMORY_READ_ID);
        sahara_memory_read_64bit->header.length = qlog_le32(sizeof(sahara_packet_memory_read_64bit));
        sahara_memory_read_64bit->memory_addr = qlog_le64(memory_table_address);
        sahara_memory_read_64bit->memory_length = qlog_le64(memory_table_length);

        /* Send the Memory Read packet */
        if (false == sahara_tx_data (sizeof(sahara_packet_memory_read_64bit))) {
            dbg(LOG_ERROR, "Sending MEMORY_READ packet failed");
            return false;
        }
    } else {
        sahara_memory_read->header.command	= qlog_le32(SAHARA_MEMORY_READ_ID);
        sahara_memory_read->header.length	= qlog_le32(sizeof(sahara_packet_memory_read));
        sahara_memory_read->memory_addr		= qlog_le32((uint32_t)memory_table_address);
        sahara_memory_read->memory_length	= qlog_le32((uint32_t)memory_table_length);

        /* Send the Memory Read packet */
        if (false == sahara_tx_data (sizeof(sahara_packet_memory_read))) {
            dbg(LOG_ERROR, "Sending MEMORY_READ packet failed");
            return false;
        }
    }
    
    return true;
}

static bool is_valid_memory_table(uint64_t memory_table_size)
{
	if (true == sahara_data.ram_dump_64bit && memory_table_size % sizeof(dload_debug_type_64bit) == 0) {
        return true;
    }
	else if (false == sahara_data.ram_dump_64bit && memory_table_size % sizeof(dload_debug_type) == 0) {
		return true;
	}
	else {
		return false;
	}
}

static bool sahara_start(void) {
    int              retval = 0;
    int              num_debug_entries = -1;
    int              i = 0;
    uint64_t         memory_table_addr = 0;
    uint64_t           memory_table_length = 0;

    struct timeval time_start, time_end;

    sahara_packet_hello *sahara_hello = (sahara_packet_hello *)sahara_data.rx_buffer;
    sahara_packet_hello_resp *sahara_hello_resp = (sahara_packet_hello_resp *)sahara_data.tx_buffer;
    sahara_packet_memory_debug *sahara_memory_debug = (sahara_packet_memory_debug *)sahara_data.rx_buffer;
    sahara_packet_memory_debug_64bit *sahara_memory_debug_64bit = (sahara_packet_memory_debug_64bit *)sahara_data.rx_buffer;
    dload_debug_type *sahara_memory_table_rx = (dload_debug_type *)sahara_data.rx_buffer;
    dload_debug_type_64bit *sahara_memory_table = (dload_debug_type_64bit *)sahara_data.misc_buffer;
    sahara_packet_reset_resp *sahara_reset_resp = (sahara_packet_reset_resp *)sahara_data.rx_buffer;

    sahara_data.state = SAHARA_WAIT_HELLO;
    kickstart_options.verbose = 1;	

    while (1)
    {
        switch (sahara_data.state)
        {
        case SAHARA_WAIT_HELLO:
          dbg(LOG_EVENT, "STATE <-- SAHARA_WAIT_HELLO");
          if (false == sahara_rx_data(0))	// size 0 means we don't know what to expect. So we'll just try to read the 8 byte header 
            {
                sahara_tx_data(1);
                if (false == sahara_rx_data(0))
                    return false;
            }

          //Check if the received command is a hello command
          if (SAHARA_HELLO_ID != qlog_le32(sahara_hello->header.command)) {
                dbg(LOG_ERROR, "Received a different command: %x while waiting for hello packet", qlog_le32(sahara_hello->header.command));
                if (false == send_reset_command ()) {
                    return false;
                }
                // set the state to SAHARA_WAIT_RESET_RESP
                dbg(LOG_EVENT, "STATE <-- SAHARA_WAIT_RESET_RESP\n");
                sahara_data.state = SAHARA_WAIT_RESET_RESP;
            }
           else {
            // Recieved hello, send the hello response
            // Create a Hello request
            sahara_hello_resp->header.command = qlog_le32(SAHARA_HELLO_RESP_ID);
            sahara_hello_resp->header.length = qlog_le32(sizeof(sahara_packet_hello_resp));
            sahara_hello_resp->version = sahara_hello->version; //SAHARA_VERSION;
            sahara_hello_resp->version_supported = sahara_hello->version_supported; //SAHARA_VERSION_SUPPORTED;
            sahara_hello_resp->status = qlog_le32(SAHARA_STATUS_SUCCESS);
            sahara_hello_resp->mode = sahara_hello->mode;
            sahara_hello_resp->reserved0 = qlog_le32(1);
            sahara_hello_resp->reserved1 = qlog_le32(2);
            sahara_hello_resp->reserved2 = qlog_le32(3);
            sahara_hello_resp->reserved3 = qlog_le32(4);
            sahara_hello_resp->reserved4 = qlog_le32(5);
            sahara_hello_resp->reserved5 = qlog_le32(6);

              switch (qlog_le32(sahara_hello->mode)) {
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
                    dbg(LOG_EVENT, "RECEIVED <-- SAHARA_MODE_0x%x", qlog_le32(sahara_hello->mode));
                break;
              }

                if (qlog_le32(sahara_hello->mode) != sahara_data.mode) {
                    dbg(LOG_ERROR, "Not expect module state ");
                    return false;
                }

              /*Send the Hello  Resonse Request*/
              dbg(LOG_EVENT, "SENDING --> SAHARA_HELLO_RESPONSE");
              if (false == sahara_tx_data (sizeof(sahara_packet_hello_resp)))
              {
                dbg(LOG_ERROR, "Tx Sahara Data Failed ");
                return false;
              }
              sahara_data.state = SAHARA_WAIT_COMMAND;
             }
            break;

        case SAHARA_WAIT_COMMAND:
            dbg(LOG_INFO, "STATE <-- SAHARA_WAIT_COMMAND");
            if (false == sahara_rx_data(0))
               return false;

            // Check if it is  an end of image Tx
            if (SAHARA_MEMORY_DEBUG_ID == qlog_le32(((sahara_packet_header *)sahara_data.rx_buffer)->command)
				  || SAHARA_64_BITS_MEMORY_DEBUG_ID == qlog_le32(((sahara_packet_header *)sahara_data.rx_buffer)->command)) {
                dbg(LOG_EVENT, "RECEIVED <-- SAHARA_MEMORY_DEBUG");

				if (SAHARA_64_BITS_MEMORY_DEBUG_ID == qlog_le32(((sahara_packet_header *)sahara_data.rx_buffer)->command)) {
					sahara_data.ram_dump_64bit = true;
					dbg(LOG_EVENT, "Using 64 bit RAM dump mode");
					memory_table_addr = qlog_le64(sahara_memory_debug_64bit->memory_table_addr);
					memory_table_length = qlog_le64(sahara_memory_debug_64bit->memory_table_length);
				}
				else {
					sahara_data.ram_dump_64bit = false;
					memory_table_addr = qlog_le32(sahara_memory_debug->memory_table_addr);
					memory_table_length = qlog_le32(sahara_memory_debug->memory_table_length);
				}

                dbg(LOG_INFO, "Memory Table Address: 0x%08"PRIX64", Memory Table Length: 0x%08"PRIX64, memory_table_addr, memory_table_length);

                if (false == is_valid_memory_table(memory_table_length)) {
                    dbg(LOG_ERROR, "Invalid memory table received");
                    if (false == send_reset_command ()) {
                        return false;
                    }
                    sahara_data.state = SAHARA_WAIT_RESET_RESP;
                    break;
                }

                if (memory_table_length > 0) {
                    if (false == send_memory_read_packet(memory_table_addr, memory_table_length)) {
                        return false;
                    }

                    if (memory_table_length > SAHARA_RAW_BUFFER_SIZE) {
                        dbg(LOG_ERROR, "Memory table length is greater than size of intermediate buffer");
                        return false;
                    }
                }
                dbg(LOG_EVENT, "STATE <-- SAHARA_WAIT_MEMORY_TABLE");
                sahara_data.state = SAHARA_WAIT_MEMORY_TABLE;
            }
            else {
                dbg(LOG_ERROR, "Received an unknown command: %d ", qlog_le32(((sahara_packet_header *)sahara_data.rx_buffer)->command));
                if (SAHARA_HELLO_ID == qlog_le32(((sahara_packet_header *)sahara_data.rx_buffer)->command))
                    continue;
                if (false == send_reset_command ()) {
                    return false;
                }
                // set the state to SAHARA_WAIT_RESET_RESP
                dbg(LOG_EVENT, "STATE <-- SAHARA_WAIT_RESET_RESP");
                sahara_data.state = SAHARA_WAIT_RESET_RESP;
            }
            break;

        case SAHARA_WAIT_MEMORY_TABLE:
            dbg(LOG_INFO, "STATE <-- SAHARA_WAIT_MEMORY_TABLE");
            num_debug_entries = 0;
            if (memory_table_length > 0) {
                if (false == sahara_rx_data((size_t)memory_table_length)) {
                   return false;
                }
                dbg(LOG_INFO, "Memory Debug table received");

                if (true == sahara_data.ram_dump_64bit) {
                    memcpy (sahara_data.misc_buffer, sahara_data.rx_buffer, (size_t)memory_table_length);
                    num_debug_entries = (int)(memory_table_length/sizeof(dload_debug_type_64bit));
                }
                else {
                    num_debug_entries = (int)(memory_table_length/sizeof(dload_debug_type));
                    if (num_debug_entries * sizeof(dload_debug_type_64bit) > SAHARA_RAW_BUFFER_SIZE) {
                        dbg(LOG_ERROR, "Length of memory table converted to 64-bit entries is greater than size of intermediate buffer");
                        return false;
                    }
            
                    for (i = 0; i < num_debug_entries; ++i) {
                        sahara_memory_table[i].save_pref = (uint64_t) qlog_le32(sahara_memory_table_rx[i].save_pref);
                        sahara_memory_table[i].mem_base = (uint64_t) qlog_le32(sahara_memory_table_rx[i].mem_base);
                        sahara_memory_table[i].length = (uint64_t) qlog_le32(sahara_memory_table_rx[i].length);
                        strncpy(sahara_memory_table[i].filename, sahara_memory_table_rx[i].filename, DLOAD_DEBUG_STRLEN_BYTES);
                        strncpy(sahara_memory_table[i].desc, sahara_memory_table_rx[i].desc, DLOAD_DEBUG_STRLEN_BYTES);
                    } // end for (i = 0; i < num_debug_entries; ++i) 
                }
            }

            for(i = 0; i < num_debug_entries; i++) {
                dbg(LOG_EVENT, "Base 0x%08"PRIX64" Len 0x%08"PRIX64", '%s', '%s'", sahara_memory_table[i].mem_base, sahara_memory_table[i].length, sahara_memory_table[i].filename, sahara_memory_table[i].desc);
            }
            sahara_data.state = SAHARA_WAIT_MEMORY_REGION;
            break;

        case SAHARA_WAIT_MEMORY_REGION:
            dbg(LOG_INFO, "STATE <-- SAHARA_WAIT_MEMORY_REGION");
            for(i = 0; i < num_debug_entries; i++) {
                uint64_t cur = 0;
                int fd = -1;
                char full_filename[255] = {0};
                if (kickstart_options.path_to_save_files) {
                    strcpy(full_filename, kickstart_options.path_to_save_files);
                    strcat(full_filename, "/");
                }
                strcat(full_filename, sahara_memory_table[i].filename);
                    
                fd = open(full_filename, O_CREAT | O_WRONLY | O_TRUNC, 0444);
                if (fd==-1)  {
                    dbg(LOG_ERROR, "ERROR: Your file '%s' does not exist or cannot be created\n\n",sahara_memory_table[num_debug_entries].filename);
                    exit(0);
                }
                gettimeofday(&time_start, NULL);

                while (cur < sahara_memory_table[i].length) {
                    uint64_t len = MIN((uint32_t)(sahara_memory_table[i].length - cur), sahara_data.max_ram_dump_read);

                    if (len < sahara_data.max_ram_dump_read || cur == 0 || (cur%(16*1024*1024)) == 0)
                        kickstart_options.verbose = 1;
                    else
                        kickstart_options.verbose = 0;
                    
                    retval =  send_memory_read_packet(sahara_memory_table[i].mem_base + cur, len);
                    if (false == retval) {
                        return false;
                    }

                    retval = sahara_rx_data((size_t)len);
                    if (false == retval) {
						system("fuser /dev/ttyUSB0");
                        if ( sahara_data.max_ram_dump_read > (16*1024)) {
                            sahara_data.max_ram_dump_read = sahara_data.max_ram_dump_read / 2;
                            continue;
                        }
                        return false;
                    }

                    cur += len;

                    retval = write(fd, sahara_data.rx_buffer, (unsigned int)len);
                    if (retval < 0) {
                        dbg(LOG_ERROR, "file write failed: %s", strerror(errno));
                        return false;
                    }
                    if ((uint32_t) retval != len) {
                        dbg(LOG_WARN, "Wrote only %d of 0x%08"PRIX64" bytes", retval, memory_table_length);
                    }        
                }                    


                kickstart_options.verbose = 1;
                dbg(LOG_STATUS, "Received file '%s'", sahara_memory_table[i].filename);
                close(fd);
                gettimeofday(&time_end, NULL);
                time_throughput_calculate(&time_start, &time_end, sahara_memory_table[i].length);                                            
            }

            if ( kickstart_options.do_reset) {
                if (false == send_reset_command ()) {
                    return false;
                }
                sahara_data.state = SAHARA_WAIT_RESET_RESP;
            } else {
                return true;
            }
            break;

        case SAHARA_WAIT_DONE_RESP:
            dbg(LOG_EVENT, "STATE <-- SAHARA_WAIT_DONE_RESP");
            return false;
          break;

        case SAHARA_WAIT_RESET_RESP:
            dbg(LOG_EVENT, "STATE <-- SAHARA_WAIT_RESET_RESP");
            if (true == sahara_rx_data(0)) {
                if (SAHARA_RESET_RESP_ID != qlog_le32(sahara_reset_resp->header.command)) {
                    dbg(LOG_INFO,"Waiting for reset response code %i, received %i instead.", SAHARA_RESET_RESP_ID, qlog_le32(sahara_reset_resp->header.command));
                    continue;
                }
            } else {
                if (SAHARA_RESET_RESP_ID == qlog_le32(sahara_reset_resp->header.command)) {
                    dbg(LOG_INFO,"Get reset response code %i", sahara_reset_resp->header.command);
                    return true;
                } else {
                    dbg(LOG_ERROR, "read failed: Linux system error: %s", strerror(errno));
                    return false;
                }
            }

            return true;
        break;

        default:
          dbg(LOG_ERROR, "Unrecognized state %d",  sahara_data.state);
          return false;
        } /* end switch */
    } /* end while (1) */
}

 int sahara_catch_dump(int port_fd, const char *path_to_save_files, int do_reset) {
    int retval;

    sahara_data.mode = SAHARA_MODE_MEMORY_DEBUG;
    com_port.port_fd = port_fd;
    kickstart_options.path_to_save_files = path_to_save_files;
    kickstart_options.do_reset = do_reset;

    sahara_data.rx_buffer = malloc (SAHARA_RAW_BUFFER_SIZE);
    sahara_data.tx_buffer = malloc (2048);
    sahara_data.misc_buffer = malloc (2048);

    if (NULL == sahara_data.rx_buffer || NULL == sahara_data.tx_buffer || NULL == sahara_data.misc_buffer) {
        dbg(LOG_ERROR, "Failed to allocate sahara buffers");
        return false;
    }
	
    retval = sahara_start();
    if (false == retval) {
        dbg(LOG_ERROR, "Sahara protocol error");
    }
    else {
        dbg(LOG_ERROR, "Sahara protocol completed");
    }

    free(sahara_data.rx_buffer);
    free(sahara_data.tx_buffer);
    free(sahara_data.misc_buffer);

    sahara_data.rx_buffer = sahara_data.tx_buffer = sahara_data.misc_buffer = NULL;

    if (retval == false)
        dbg(LOG_INFO, "Catch DUMP using Sahara protocol failed\n\n");
    else
        dbg(LOG_INFO, "Catch DUMP using Sahara protocol successful\n\n");
            
    return retval;
}
