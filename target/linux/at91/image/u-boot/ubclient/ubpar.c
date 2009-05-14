/*
 * ubparams.c
 * 
 * Generate a u-boot parameter block with correct crc
 *
 * (C) 1007 Guthrie Consulting
 * hamish@prodigi.ch
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif
#define __ASM_STUB_PROCESSOR_H__
#include <config.h>
#undef __ASSEMBLY__
#include <environment.h>

#define XMK_STR(x)  #x
#define MK_STR(x)  XMK_STR(x)

extern unsigned long crc32 (unsigned long, const unsigned char *, unsigned int);

#if !defined(ENV_CRC)
#define ENV_CRC ~0
#endif

#ifdef LAN_IP
	#warning LAN_IP
#else
	#warning LAN_IP NOT DEFINED
#endif
#ifdef LAN_SERVERIP
	#warning LAN_SERVERIP
#else
	#warning LAN_SERVERIP NOT DEFINED
#endif

static char *environment[] = {
	"bootdelay=3\0"
	"baudrate=115200\0"
	"stdin=serial\0"
	"stdout=serial\0"
	"stderr=serial\0"
	"fbargs=setenv bootargs root=/dev/mtdblock3 ro console=/dev/ttyS0,115200,mem=32M\0"
	"rdba=setenv bootargs root=/dev/ram rw initrd=0x21200000,6000000 ramdisk_size=20000 console=/dev/ttyS0,115200,mem=32M\0"
	"rdram=run rdba; tftp 21000000 uImage; tftp 21200000 root.squashfs; bootm 21000000\0"
	"flash=run fbargs; bootm 0xc0042000\0"
	"bootargs=setenv bootargs root=/dev/mtdblock3 ro console=/dev/ttyS0,115200,mem=32M\0"
	"bootcmd=bootm 0xc0042000\0"
	"ipaddr=" MK_STR(LAN_IP) "\0"
	"serverip=" MK_STR(LAN_SERVERIP) "\0"
	"\0"
	};

int main(int argc, char *argv[]) {
	env_t *envptr;
	char *src, *srcptr;
	char *dataptr;
	FILE *params;
	int argfail = 1;
	char newmac[30];
	char newser[30];
	int paramlen = 0;
	int progmac = 0;
	int progser = 0;

	if (argc < 3) {
		printf ("Invalid arguments\n");
		return 1;
		}

	switch (argc) {
		case 5:
			if (strcmp(argv[3], "--serial") == 0) {
				argfail = 0;
				sprintf(newser, "serial#=%s", argv[4]);
				progser = 1;
				}
		case 3:
			if (strcmp(argv[1], "--mac") == 0) {
				argfail = 0;
				sprintf(newmac, "ethaddr=%s", argv[2]);
				progmac = 1;
				}
			else
				argfail = 1;
		}

	if (argfail) {
		printf("Invalid arguments\n");
		return 1;
		}


	src = srcptr = *environment;
	envptr = (env_t *)malloc(CFG_ENV_SIZE);
	dataptr = (char *)envptr + ENV_HEADER_SIZE;

	while(*srcptr) {
		//printf("%d, %s\n", strlen(srcptr), srcptr);
		paramlen += strlen(srcptr) + 1;
		srcptr += strlen(srcptr) + 1;
		}

	printf("Make u-boot params\n");
	printf("Params size is %d\n", CFG_ENV_SIZE);

	memset(envptr, 0, CFG_ENV_SIZE);
	memcpy(dataptr, src, paramlen);
	dataptr += paramlen;

	if (progmac) {
		memcpy(dataptr, newmac, strlen(newmac));
		dataptr += strlen(newmac) + 1;
		}

	if (progser) {
		memcpy(dataptr, newser, strlen(newser));
		dataptr += strlen(newser) + 1;
		}

	envptr->crc = crc32(0, envptr->data, ENV_SIZE);

	params = fopen("/dev/mtd1", "w");
	fwrite(envptr, CFG_ENV_SIZE, 1, params);
	fclose(params);

	free(envptr);
	return 0;
}
