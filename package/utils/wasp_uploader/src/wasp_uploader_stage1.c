/*
 * Simple stage 1 firmware uploader for AVM WASP as found in the FRITZ!Box 3390
 *
 * The protocol was found by dumping MDIO traffic between the two SoCs,
 * so some things might be wrong or incomplete.
 *
 * MDIO read/write functions are taken from mdio-tool.c,
 * Copyright (C) 2013 Pieter Voorthuijsen
 *
 * (c) 2019-2020 Andreas Böhler
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <linux/mii.h>
#include <getopt.h>
#include <libgen.h>

#ifndef __GLIBC__
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#endif

#define CHUNK_SIZE	14

#define MDIO_ADDR			0x07
#define MDIO_TIMEOUT_COUNT	1000

#define WRITE_SLEEP_US 20000
#define POLL_SLEEP_US  100
#define BOOT_SLEEP_US  10000
// 10 second timeout with above sleep time

#define RESP_RETRY			0x0102
#define RESP_OK				0x0002
#define RESP_WAIT			0x0401
#define RESP_COMPLETED		0x0000
#define RESP_READY_TO_START	0x0202
#define RESP_STARTING       0x00c9

#define CMD_SET_PARAMS				0x0c01
#define CMD_SET_CHECKSUM_3390		0x0801
#define CMD_SET_CHECKSUM_X490		0x0401
#define CMD_SET_DATA				0x0e01
#define CMD_START_FIRMWARE_3390		0x0201
#define CMD_START_FIRMWARE_X490		0x0001
#define CMD_START_FIRMWARE2_X490	0x0101

static const uint32_t start_addr = 0xbd003000;
static const uint32_t exec_addr = 0xbd003000;

static uint16_t m_reg_zero = 0x0;
static uint16_t m_reg_status = 0x700;
static uint16_t m_reg_data1 = 0x702;
static uint16_t m_reg_data2 = 0x704;
static uint16_t m_reg_data3 = 0x706;
static uint16_t m_reg_data4 = 0x708;
static uint16_t m_reg_data5 = 0x70a;
static uint16_t m_reg_data6 = 0x70c;
static uint16_t m_reg_data7 = 0x70e;

static char *opt_filename;
static char *opt_iface;
static char *opt_model;
static char *progname;
static int opt_verbose = 0;

static int skfd = -1;		/* AF_INET socket for ioctl() calls. */
static struct ifreq ifr;

static const char mac_data[CHUNK_SIZE] = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x04, 0x20, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};

typedef enum {
	MODEL_3390,
	MODEL_X490,
	MODEL_UNKNOWN
} t_model;

static t_model m_model = MODEL_UNKNOWN;

off_t fsize(const char *filename) {
    struct stat st; 

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1; 
}


static int mdio_read(int location, int *value)
{
    struct mii_ioctl_data *mii = (struct mii_ioctl_data *)&ifr.ifr_data;
    mii->reg_num = location;

    if (ioctl(skfd, SIOCGMIIREG, &ifr) < 0) {
		fprintf(stderr, "SIOCGMIIREG on %s failed: %s\n", ifr.ifr_name,
		strerror(errno));
		return -1;
    }
	*value = mii->val_out;
	if(opt_verbose)
		printf("mdio_read: reg = 0x%x, val = 0x%x\n", location, *value);
    return 0;
}

static int mdio_write(int location, int value)
{
    struct mii_ioctl_data *mii = (struct mii_ioctl_data *)&ifr.ifr_data;
    mii->reg_num = location;
    mii->val_in = value;

	if (ioctl(skfd, SIOCSMIIREG, &ifr) < 0) {
		fprintf(stderr, "SIOCSMIIREG on %s failed: %s\n", ifr.ifr_name,
		strerror(errno));
		return -1;
    }
    if(opt_verbose)
    	printf("mdio_write: reg = 0x%x, val = 0x%x\n", location, value);
    return 0;
}

static int write_header(const uint32_t start_addr, const uint32_t len, const uint32_t exec_addr) {
	int regval;
	int timeout = MDIO_TIMEOUT_COUNT;
	mdio_write(m_reg_data1, ((start_addr & 0xffff0000) >> 16));
	mdio_write(m_reg_data2, (start_addr & 0x0000ffff));
	mdio_write(m_reg_data3, ((len & 0xffff0000) >> 16));
	mdio_write(m_reg_data4, (len & 0x0000ffff));
	mdio_write(m_reg_data5, ((exec_addr & 0xffff0000) >> 16));
	mdio_write(m_reg_data6, (exec_addr & 0x0000ffff));
	mdio_write(m_reg_status, CMD_SET_PARAMS);

	if(m_model == MODEL_3390) {
		do {
			usleep(POLL_SLEEP_US);
			mdio_read(m_reg_zero, &regval);
			timeout--;
		} while((regval != RESP_OK) && (timeout > 0));

		if(regval != RESP_OK) {
			printf("Error writing header! m_reg_zero = %d\n", regval);
			return -1;
		}
	}

	timeout = MDIO_TIMEOUT_COUNT;
	do {
		usleep(POLL_SLEEP_US);
		mdio_read(m_reg_status, &regval);
		timeout--;
	} while((regval != RESP_OK) && (timeout > 0));
	
	if(regval != RESP_OK) {
		printf("Error writing header! m_reg_status = 0x%x\n", regval);
		return -1;
	}
	return 0;
}

static int write_checksum(const uint32_t checksum) {
	int regval;
	int timeout = MDIO_TIMEOUT_COUNT;
	mdio_write(m_reg_data1, ((checksum & 0xffff0000) >> 16));
	mdio_write(m_reg_data2, (checksum & 0x0000ffff));
	if(m_model == MODEL_3390) {
		mdio_write(m_reg_data3, 0x0000);
		mdio_write(m_reg_data4, 0x0000);
		mdio_write(m_reg_status, CMD_SET_CHECKSUM_3390);
	} else if(m_model == MODEL_X490) {
		mdio_write(m_reg_status, CMD_SET_CHECKSUM_X490);
	}

	if(m_model == MODEL_3390) {
		do {
			usleep(POLL_SLEEP_US);
			mdio_read(m_reg_zero, &regval);
			timeout--;
		} while((regval != RESP_OK) && (timeout > 0));

		if(regval != RESP_OK) {
			printf("Error writing checksum! m_reg_zero = %d\n", regval);
			return -1;
		}
	}


	timeout = MDIO_TIMEOUT_COUNT;
	do {
		usleep(POLL_SLEEP_US);
		mdio_read(m_reg_status, &regval);
		timeout--;
	} while((regval != RESP_OK) && (timeout > 0));

	if(regval != RESP_OK) {
		printf("Error writing checksum! m_reg_status = %d\n", regval);
		return -1;
	}
	return 0;
}

static int write_chunk(const char *data, const int len) {
	int regval;
	int timeout = MDIO_TIMEOUT_COUNT;
	
	regval = (data[0] & 0xff);
	if(len > 1)
		regval = (regval << 8) | (data[1] & 0xff);
	mdio_write(m_reg_data1, regval);
	if(len > 2) {
		regval = (data[2] & 0xff);
		if(len > 3)
			regval = (regval << 8) | (data[3] & 0xff);
		mdio_write(m_reg_data2, regval);
	}
	if(len > 4) {
		regval = (data[4] & 0xff);
		if(len > 5)
			regval = (regval << 8) | (data[5] & 0xff);
		mdio_write(m_reg_data3, regval);
	}
	if(len > 6) {
		regval = (data[6] & 0xff);
		if(len > 7)
			regval = (regval << 8) | (data[7] & 0xff);
		mdio_write(m_reg_data4, regval);
	}
	if(len > 8) {
		regval = (data[8] & 0xff);
		if(len > 9)
			regval = (regval << 8) | (data[9] & 0xff);
		mdio_write(m_reg_data5, regval);
	}
	if(len > 10) {
		regval = (data[10] & 0xff);
		if(len > 11)
			regval = (regval << 8) | (data[11] & 0xff);
		mdio_write(m_reg_data6, regval);
	}
	if(len > 12) {
		regval = (data[12] & 0xff);
		if(len > 13)
			regval = (regval << 8) | (data[13] & 0xff);
		mdio_write(m_reg_data7, regval);
	}
	
	mdio_write(m_reg_status, CMD_SET_DATA);

	if(m_model == MODEL_3390) {
		do {
			usleep(POLL_SLEEP_US);
			mdio_read(m_reg_zero, &regval);
					timeout--;
		} while((regval != RESP_OK) && (timeout > 0));

		if((regval != RESP_OK) && (regval != RESP_COMPLETED) && (regval != RESP_WAIT)) {
			printf("Error writing chunk: m_reg_zero = 0x%x!\n", regval);
			return -1;
		}
	}


	timeout = MDIO_TIMEOUT_COUNT;
	do {
		usleep(POLL_SLEEP_US);
		mdio_read(m_reg_status, &regval);
		timeout--;
	} while((regval != RESP_OK) && (timeout > 0));

	if((regval != RESP_OK) && (regval != RESP_WAIT) && (regval != RESP_COMPLETED)) {
		printf("Error writing chunk: m_reg_status = 0x%x!\n", regval);
		return -1;
	}
	return 0;
}

static uint32_t calc_checksum(const char *filename) {
	uint32_t checksum = 0xffffffff;
	uint32_t cs;
	uint8_t data[4];
	size_t read = 0;
	int count = -1;

	FILE *fp = fopen(filename, "rb");
	while(!feof(fp)) {
		read = fread(data, 1, 4, fp);
		if(read == 0) {
			break;
		}
		cs = (data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3]);
		checksum = checksum - cs;
		count++;
	}
	fclose(fp);

	checksum = checksum - count;
	return checksum;
}

static int check_options(void) {
	if(!opt_filename) {
		fprintf(stderr, "No input filename specified.\n");
		return -1;
	}

	if(!opt_iface) {
		fprintf(stderr, "No interface specified.\n");
		return -1;
	}

	if(!opt_model) {
		fprintf(stderr, "No model specified.\n");
		return -1;
	}

	if(strcmp(opt_model, "3390") == 0) {
		m_model = MODEL_3390;
		m_reg_zero = 0x0;
		m_reg_status = 0x700;
		m_reg_data1 = 0x702;
		m_reg_data2 = 0x704;
		m_reg_data3 = 0x706;
		m_reg_data4 = 0x708;
		m_reg_data5 = 0x70a;
		m_reg_data6 = 0x70c;
		m_reg_data7 = 0x70e;
	} else if(strcmp(opt_model, "x490") == 0 ) {
		m_model = MODEL_X490;
		m_reg_zero = 0x0;
		m_reg_status = 0x0;
		m_reg_data1 = 0x2;
		m_reg_data2 = 0x4;
		m_reg_data3 = 0x6;
		m_reg_data4 = 0x8;
		m_reg_data5 = 0xa;
		m_reg_data6 = 0xc;
		m_reg_data7 = 0xe;
	} else {
		fprintf(stderr, "Invalid model specified.\n");
		return -1;
	}

	return 0;
}

static void usage(int status)
{
	fprintf(stderr, "Usage: %s [OPTIONS...]\n", progname);
	fprintf(stderr,
"\n"
"Options:\n"
"  -m <model>      use the specified FRITZ!Box Model (3390, x490 for models 3490/5490/7490)\n"
"  -i <interface>  use the specified Ethernet interface\n"
"  -f <file>       upload the specified firmware file\n"
"  -v              verbose output\n"
"  -h              show this screen\n"
	);

	exit(status);
}

int main(int argc, char *argv[]) {
	uint32_t checksum;
	char data[CHUNK_SIZE];
	size_t read = 0;
	off_t size;
	int regval;
	int regval2;
	int count;
	int cont = 1;
	struct mii_ioctl_data *mii = (struct mii_ioctl_data *)&ifr.ifr_data;
	progname = basename(argv[0]);
	int ret = EXIT_FAILURE;
	
	while(1) {
		int c;

		c = getopt(argc, argv, "i:f:m:hv");
		if(c == -1)
			break;

		switch(c) {
		
		case 'i':
			opt_iface = optarg;
			break;

		case 'f':
			opt_filename = optarg;
			break;

		case 'v':
			opt_verbose = 1;
			break;
		case 'm':
			opt_model = optarg;
			break;

		case 'h':
			usage(EXIT_SUCCESS);
			break;

		default:
			usage(EXIT_FAILURE);
			break;
		}
	}
	
	ret = check_options();
	if(ret)
		return ret;
  
	printf("AVM WASP Stage 1 uploader.\n");
	
	printf("Using file     : %s\n", opt_filename);
	printf("Ethernet device: %s\n", opt_iface);
	
	size = fsize(opt_filename);
	if(size < 0) {
		fprintf(stderr, "Input file not found.\n");
		return 1;
	}
	
	if(size > 0xffff) {
		fprintf(stderr, "Error: Input file too big\n");
		return 1;
	}

	checksum = calc_checksum(opt_filename);

	printf("Checksum       : 0x%8x\n", checksum);

	/* Open a basic socket. */
	if ((skfd = socket(AF_INET, SOCK_DGRAM,0)) < 0) {
		perror("socket");
		return -1;
	}

	strncpy(ifr.ifr_name, opt_iface, IFNAMSIZ);
	mii->phy_id = MDIO_ADDR;

	mdio_read(m_reg_status, &regval);
	if(regval != RESP_OK) {
		printf("Error: WASP not ready (0x%x)\n", regval);
		return 1;
	}

	if(m_model == MODEL_3390) {
		mdio_read(m_reg_zero, &regval);
		if(regval != RESP_OK) {
			printf("Error: WASP not ready (0x%x)\n", regval);
			return 1;
		}
	}

	if(write_header(start_addr, size, exec_addr) < 0)
		return 1;

	if(write_checksum(checksum) < 0)
		return 1;

	FILE *fp = fopen(opt_filename, "rb");
	while(!feof(fp)) {
		read = fread(data, 1, CHUNK_SIZE, fp);
		if(write_chunk(data, read) < 0) {
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	
	printf("Done uploading firmware.\n");
	
	if(m_model == MODEL_X490) {
		mdio_write(m_reg_status, CMD_START_FIRMWARE_X490);
	} else if(m_model == MODEL_3390) {
		//usleep(15 * 100 * 1000); // 1.5 seconds
		mdio_write(m_reg_status, CMD_START_FIRMWARE_3390);
	}

	printf("Firmware start command sent.\n");
	//if(m_model == MODEL_3390) {
	//	usleep(WRITE_SLEEP_US);
	//}

	count = 0;
	mdio_read(m_reg_status, &regval);
	// Timeout: 10 seconds
	while((regval != RESP_READY_TO_START) && (count < MDIO_TIMEOUT_COUNT)) {
		mdio_read(m_reg_status, &regval);
		usleep(WRITE_SLEEP_US);
		count++;
	}
	if(count == MDIO_TIMEOUT_COUNT) {
		printf("Timed out waiting for response.\n");
		return 1;
	}

	if(m_model == MODEL_3390) {
		mdio_write(m_reg_status, CMD_START_FIRMWARE_3390);
	} else if(m_model == MODEL_X490) {
		mdio_write(m_reg_status, CMD_SET_CHECKSUM_X490);
	}

	printf("Firmware start command sent.\n");	
	usleep(WRITE_SLEEP_US);

	if(m_model == MODEL_X490) {
		cont = 1;
		while(cont) {
			count = 0;
			mdio_read(m_reg_status, &regval);
			while((regval != RESP_OK) && (count < MDIO_TIMEOUT_COUNT)) {
				mdio_read(m_reg_status, &regval);
				usleep(BOOT_SLEEP_US);
				count++;
			}
			if(count == MDIO_TIMEOUT_COUNT) {
				printf("Timed out waiting for response.\n");
				return 1;
			}
			mdio_read(m_reg_data1, &regval);
			mdio_read(m_reg_data2, &regval2);
			mdio_write(m_reg_status, CMD_SET_CHECKSUM_X490);
			if(regval == 0 && regval2 != 0)
				cont = regval2;
			else
				cont--;
		}

		count = 0;
		mdio_read(m_reg_status, &regval);
		while((regval != RESP_OK) && (count < MDIO_TIMEOUT_COUNT)) {
			mdio_read(m_reg_status, &regval);
			usleep(BOOT_SLEEP_US);
			count++;
		}
		if(count == MDIO_TIMEOUT_COUNT) {
			printf("Timed out waiting for response.\n");
			return 1;
		}
		
		mdio_write(m_reg_data1, 0x00);
		mdio_write(m_reg_status, CMD_START_FIRMWARE2_X490);
		
		mdio_read(m_reg_status, &regval);
		if(regval != RESP_OK) {
			printf("Error starting firmware: 0x%x\n", regval);
			return 1;
		}
	} else if(m_model == MODEL_3390) {
		count = 0;
		mdio_read(m_reg_status, &regval);
		while((regval != RESP_OK) && (count < MDIO_TIMEOUT_COUNT)) {
			mdio_read(m_reg_status, &regval);
			usleep(WRITE_SLEEP_US);
			count++;
		}
		if(count == MDIO_TIMEOUT_COUNT) {
			printf("Timed out waiting for response.\n");
			return 1;
		}
		if(write_chunk(mac_data, CHUNK_SIZE) < 0) {
			printf("Error sending MAC address!\n");
			return 1;
		}
	}
	
	printf("Firmware upload successful!\n");

	return 0;
}
