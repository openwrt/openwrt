/****************************************************************************

                            Copyright (c) 2005
                         Infineon Technologies AG
               St. Martin Strasse 53; 81669 Munich, Germany

  Any use of this Software is subject to the conclusion of a respective
  License Agreement. Without such a License Agreement no rights to the
  Software are granted.

*****************************************************************************/
/** 
History:
1.02:
09/08/2006 Removed DEMO_ONLY define
1.03:
31/08/2006 Add IOCTL AUTOBOOT_ENABLE_SET
1.04:
14/09/2006 Add min snr margin check for in Annex B DMT mode
1.05:
18/09/2006 Add G.lite support
1.06: 611101:tc.chen
10/11/2006 change quiet mode and showtime lock implement mechanism (old: using cmv, new: using ioctl to kernel mode, and the mei driver keep the value) per the mail from Oliver Salomon on 08/11/2006
1.07: 
13/11/2006 TC Chen: Fix minimal snr issue for ADSL Annex B.
1.08:
08/12/2006 TC Chen: Fix loop diagnostic warning issue
1.09:
27/12/2006 TC Chen: Fix the issue that "The autoboot daemon will call "./translate" which is a problem when started through scripts."
1.10"
1/09/2007 Bing Tao: Fix AnnexJ issue
*/

#define _IFXMIPS_ADSL_APP
//#define IFXMIPS_PORT_RTEMS 1
#define __LINUX__

#if defined(IFXMIPS_PORT_RTEMS)
#include "ifxmips_mei_rtems.h"
#define KERNEL_SPACE_APPLICATION 1
#endif

#define u32	unsigned int
 
#define VERSION_MAJOR	1
#define VERSION_MINOR	10
 
#if defined(__LINUX__)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>

#include <asm/ifxmips/ifxmips_mei_app.h>
#include <asm/ifxmips/ifxmips_mei_app_ioctl.h>
#include <asm/ifxmips/ifxmips_mei_ioctl.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#define IFXMIPS_MEI_DEV 	"/dev/ifxmips/mei"
#endif

#if defined(KERNEL_SPACE_APPLICATION)
#include "ifxmips_adsl_fw.h"
#endif

#define IFXMIPS_GPIO_DEVICE	"/dev/ifxmips_port"
#undef DEMO_ONLY //todo: remove for normal release

#define SEGMENT_SIZE (64*1024)

#define MODE_ALL	0
#define MODE_992_1A	1
#define MODE_992_1B	2
#define MODE_992_3A	3
#define MODE_992_3B	4
#define MODE_992_5A	5
#define MODE_992_5B	6

#define MODE_992_3I 7
#define MODE_992_3J 8
#define MODE_992_3M 9
#define MODE_992_5I 10
#define MODE_992_5J 11
#define MODE_992_5M 12
#define MODE_M_ALL 13 
#define MODE_B_ALL 14 
#define MODE_M_B_ALL 15


#if defined(KERNEL_SPACE_APPLICATION)
extern int mei_ioctl(int ino, int * fil, unsigned int command, unsigned long lon);
#define ioctl(fd,cmd,arg) mei_ioctl(1,0,cmd,(unsigned long)arg)
extern void makeCMV(u8 opcode, u8 group, u16 address, u16 index, int size, u16 * data,u16 *CMVMSG);
#endif

static u16 Message[16]__attribute__ ((aligned(4))); 

#if !defined(KERNEL_SPACE_APPLICATION)
void makeCMV(u8 opcode, u8 group, u16 address, u16 index, int size, u16 * data, u16 *CMVMSG);
void makeCMV(u8 opcode, u8 group, u16 address, u16 index, int size, u16 * data, u16 *CMVMSG)
{
	memset(CMVMSG, 0, 16*2);
	CMVMSG[0]= (opcode<<4) + (size&0xf);
	if(opcode == H2D_DEBUG_WRITE_DM)
		CMVMSG[1]= (group&0x7f);
	else
		CMVMSG[1]= (((index==0)?0:1)<<7) + (group&0x7f);
	CMVMSG[2]= address;
	CMVMSG[3]= index;
	if((opcode == H2D_CMV_WRITE)||(opcode == H2D_DEBUG_WRITE_DM))
		memcpy(CMVMSG+4, data, size*2);
	return;
}
#endif

void print_usage(char *program_name)
{
	printf("Usage: %s ADSL_Firmware ADSL_MODE \n",program_name);
	printf("\tADSL_Firmeare: Specify the ADSL firmware file to use.\n");
	printf("\tADSL_MODE:Specify the ADSL mode to use for training.\n");
	printf("\t\tSupported Mode: all,1a,3a,5a,1b,3b,5b,3i,3j,3m,5i,5j,5m,m_all,b_all,mb_all\n");

}

#if defined(KERNEL_SPACE_APPLICATION)

int Download_Firmware(char *filename, int fd_mei)
{
	extern ssize_t mei_write(char * filp, const char * buf, size_t size, loff_t * loff);
	extern unsigned long cgi_pFileData_modemfw_bin[]; 
	unsigned long offset=0;
	
	mei_write(0,(char *)cgi_pFileData_modemfw_bin,sizeof(cgi_pFileData_modemfw_bin),&offset);
	return 0;
}
#else
int Download_Firmware(char *filename, int fd_mei)
{
	int fd_image=0;
	char *buf=NULL;
	int size=0,read_size = SEGMENT_SIZE;
	struct stat file_stat;

        fd_image=open(filename, O_RDONLY);
	if (fd_image<=0)
	{
                printf("\n open %s fail.\n",filename);
                return -1;
	}
        if(lstat(filename, &file_stat)<0){
                printf("\n lstat error");
                return -1;
        }
        size=file_stat.st_size;
        buf=malloc(read_size);
        if(buf==NULL){
                printf("\n malloc failed in MEI main()");
                return -1;
        }

        lseek(fd_image, 0, SEEK_SET);
        lseek(fd_mei, 0, SEEK_SET);
        while(size>0)
        {
                static flag=1;
                if (size>SEGMENT_SIZE)
                        read_size=SEGMENT_SIZE;
                else
                        read_size=size;
                if(read(fd_image, buf, read_size)<=0){
                        printf("\n amazon_mei_image not present");
                        return -1;
                }
                if(write(fd_mei, buf, read_size)!=read_size){
                        printf("\n write to mei driver fail");
                        free(buf);
                        return -1;
                }
                size-=read_size;
        }
        free(buf);
        close(fd_image);
}
#endif

// 609141:tc.chen :read min margin from CO for Annex B in DMT mode
int read_min_snr_margin(int fd, s16 *rt_ne_ds_min_margin)
{
	makeCMV(H2D_CMV_READ,STAT, 1, 0, 1, NULL, Message);
        if (ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)!=0)
		return -1;
        if (Message[4] == 0x8) //992.1 AnnexB
        {
             	u16 min_margin;
             	makeCMV(H2D_CMV_READ,INFO, 33, 1, 1, NULL, Message);
            	if (ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)!=0)
		{
			return -1;
		}
		// INFO 33 is 6 bits sign value
                min_margin = (Message[4] & 0x3f);
                if (min_margin > 31)
                {
                	*rt_ne_ds_min_margin = min_margin - 64;
                }else
                {
                        *rt_ne_ds_min_margin = min_margin ;
                }
        }else
	{
		*rt_ne_ds_min_margin = 0;
	}
	return 0;
}
// 609141:tc.chen  end

#if !defined(KERNEL_SPACE_APPLICATION)
int main(int argc, char **argv)
#else
int ifxmips_autoboot_main (int argc, char **argv)
#endif
{
        int fd;
	int i,j;
	char systemstr[50];
	u16 all_data = 0;
	u16 zero = 0x0;
	u16 one=0x1;
	u16 mw = 0x10;
	u16 link = 0x2;
	u16 temp=0;
	u16 rt_macrostate;
	meireg meiregister;
	int reboot_flag, rt_ne_los_flag;
	u16 rt_ne_failure_flags=0;
	s16 rt_ne_ds_margin=0;
	s16 rt_ne_ds_min_margin=0; //609141:tc.chen:
	u16 rt_buildcode_lsw=0, rt_buildcode_msw=0;
	u32 rt_buildcode = 0;
	int FarEndResponseReceived;
	time_t start_time;
	int timeout_limit;
	int     tmpShowTimeLockEnabled = 0; 
 	u16 nFECS_L_count=0;
  	u16 nES_L_count=0; 
 	u16 nSES_L_count=0;
	u16 nLOSS_L_count=0; 	
	u16 first_power_on=1;
	int mode = -1;
	int pre_loop_diagnostics_mode=0;
	int loop_diagnostics_mode=0;
	
	if ((argc < 2) || (argc > 5) )
	{	printf("\n Wrong Argument\n");
		print_usage(argv[0]);
		return -1;
	}

//KD
// translate is called with "./translate -ef", but if the autoboot daemon
// is started during system boot, the current directory is "/", which
// makes the "system" call fail.
// Thus we make the program's directory the working directory.
        {
         char *argv0, *prog_dir;
         int argv0_size;
         /* The dirname system call might change its argument,
            thus we need a local copy */
         argv0_size = strlen(argv[0]) + 1;
         argv0 = malloc(argv0_size);
         if (argv0 == 0) {
           printf("\n Insufficient memory\n");
           return -1;
         }
         /* make sure there is no buffer overflow and
            the string is null-terminated */
         strncpy(argv0, argv[0], argv0_size);
         argv0[argv0_size-1] = '\0';

         prog_dir = dirname(argv0);
         chdir(prog_dir);
         free(argv0);
         argv0 = 0;
        }

	//display version info or download firmware
	if(argc==2) 
	{
		if ((strncmp(argv[1], "ver", 3)==0) || (strncmp(argv[1], "VER", 3)==0)){
			printf("\n ifxmips_autoboot_daemon version: %d.%02d.00\n",VERSION_MAJOR,VERSION_MINOR);	
		}else
		{
#if !defined(KERNEL_SPACE_APPLICATION)		
			fd=open(IFXMIPS_MEI_DEV, O_RDWR);
			if(fd<0){
				printf("autoboot open %s fail\n",IFXMIPS_MEI_DEV);
				return -1;
			}
#endif
			// Notify mei driver that it is controlled by autoboot daemon
			i = 1;
			if (ioctl(fd, AUTOBOOT_ENABLE_SET, &i)<0){
				printf("\n\n mei ioctl AUTOBOOT_ENABLE_SET fail.\n");
			}

			Download_Firmware(argv[1],fd);
			if (ioctl(fd, IFXMIPS_MEI_START,NULL)<0){
				printf("\n\n mei start fail.\n");
#if !defined(KERNEL_SPACE_APPLICATION)			
				close(fd);
#endif			
				return -1;
			}
#if !defined(KERNEL_SPACE_APPLICATION)			
			close(fd);
#endif			
		}
		return 0;
	}

	if (strncmp(argv[2], "all", 3) == 0)
		mode = MODE_ALL;
	else if ((strncmp(argv[2], "1a", 2) == 0) || (strncmp(argv[2], "1A", 2) == 0))
		mode = MODE_992_1A;
	else if ((strncmp(argv[2], "1b", 2) == 0) || (strncmp(argv[2], "1B", 2) == 0))
		mode = MODE_992_1B;
	else if ((strncmp(argv[2], "3a", 2) == 0) || (strncmp(argv[2], "3A", 2) == 0))
		mode = MODE_992_3A;
	else if ((strncmp(argv[2], "3b", 2) == 0) || (strncmp(argv[2], "3B", 2) == 0))
		mode = MODE_992_3B;
	else if ((strncmp(argv[2], "5a", 2) == 0) || (strncmp(argv[2], "5A", 2) == 0))
		mode = MODE_992_5A;
	else if ((strncmp(argv[2], "5b", 2) == 0) || (strncmp(argv[2], "5B", 2) == 0))
		mode = MODE_992_5B;

	else if ((strncmp(argv[2], "3i",2) == 0) || (strncmp(argv[2], "3I",2) == 0))
		mode = MODE_992_3I;
	else if ((strncmp(argv[2], "3j",2) == 0) || (strncmp(argv[2], "3J",2) == 0))
		mode = MODE_992_3J;
	else if ((strncmp(argv[2], "3m",2) == 0) || (strncmp(argv[2], "3M",2) == 0))
		mode = MODE_992_3M;
	else if ((strncmp(argv[2], "5i",2) == 0) || (strncmp(argv[2], "5I",2) == 0))
		mode = MODE_992_5I;
	else if ((strncmp(argv[2], "5j",2) == 0) || (strncmp(argv[2], "5J",2) == 0))
		mode = MODE_992_5J;
	else if ((strncmp(argv[2], "5m",2) == 0) || (strncmp(argv[2], "5M",2) == 0))
		mode = MODE_992_5M;
        else if (strncmp(argv[2], "m_all",5) == 0) 
        	mode = MODE_M_ALL; 
        else if (strncmp(argv[2], "b_all",5) == 0) 
                mode = MODE_B_ALL; 
	else if (strncmp(argv[2], "mb_all",6) == 0) 
                mode = MODE_M_B_ALL; 

	if (mode == -1)
	{	printf("\n Wrong Argument\n");
		print_usage(argv[0]);
		return -1;
	}

#if !defined(KERNEL_SPACE_APPLICATION)
	fd=open(IFXMIPS_MEI_DEV, O_RDWR);
	if(fd<0){
		printf("autoboot open %s fail\n",IFXMIPS_MEI_DEV);
		return -1;
	}
#endif
fw_download:
	i = 1;
	if (ioctl(fd, AUTOBOOT_ENABLE_SET, &i)<0){
		printf("\n\n mei ioctl AUTOBOOT_ENABLE_SET fail.\n");
	}
	Download_Firmware(argv[1],fd);

	if (ioctl(fd, IFXMIPS_MEI_START,NULL)<0){
		printf("\n\n mei start fail.\n");
		ioctl(fd, IFXMIPS_MEI_REBOOT,NULL);
		goto fw_download;
	}

	// test Annex A or B
	makeCMV(H2D_CMV_READ, INFO, 54,1, 1, &temp, Message);
	if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
		printf("\n\n read INFO 54 1 fail");
		return -1;
	}

	if(((Message[4]>>8)&0x3f)==0x1){//Annex A
		all_data = 0x9105; 
		if ((mode == MODE_992_1B) || (mode == MODE_992_3B) || (mode == MODE_992_5B))
		{	printf("\n\nWrong Argument, Annex A is running!\n\n");
			return -1;
		}
	}
	else if(((Message[4]>>8)&0x3f)==0x2){// Annex B
		all_data=0x4208;
		if ( (mode == MODE_992_1A) || (mode == MODE_992_3A) || (mode == MODE_992_5A)\
// changed by xu bingtao 09/01/2007
/*
		|| (mode == MODE_992_3I) || (mode == MODE_992_3J) ||(mode == MODE_992_3M)\
		|| (mode == MODE_992_5I) || (mode == MODE_992_5J) || (mode == MODE_992_5M))
*/
		|| (mode == MODE_992_3I) ||(mode == MODE_992_3M)\
		|| (mode == MODE_992_5I) || (mode == MODE_992_5M))
// changed by xu bingtao 09/01/2007
		{	printf("\n\nWrong Argument, Annex B is running!\n\n");
			return -1;	
		}
	}
	else{
		printf("\n\n Firmware Neither Annex A nor B\n\n");
		return -1;
	}

	/* add by tc chen */
	system("echo \"0\" > /tmp/adsl_status");

	makeCMV(H2D_CMV_READ, INFO, 55, 0, 1, &temp, Message);
	if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
		printf("\n\n read INFO 55 0 fail");
		return -1;
	}

	rt_buildcode_lsw = Message[4];
	
	makeCMV(H2D_CMV_READ, INFO, 55, 1, 1, &temp, Message);
	if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
		printf("\n\n read INFO 55 1 fail");
		return -1;
	}
	rt_buildcode_msw = Message[4];

	rt_buildcode = rt_buildcode_lsw + (rt_buildcode_msw << 16);
	printf("date: %d, month: %d, hour: %d, minute: %d\n",
		(rt_buildcode >> 0) & 0xFF,
		(rt_buildcode >> 8) & 0xFF,
		(rt_buildcode >> 16) & 0xFF,
		(rt_buildcode >> 25) & 0xFF
	      );

	while(1){
		makeCMV(H2D_CMV_WRITE, OPTN, 0, 0, 1, &zero, Message);
		if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
			printf("\n\n write OPTN 0 0 fail");
			goto rt_reboot;
		}
		if (mode == MODE_992_1A)
		{	makeCMV(H2D_CMV_READ, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n read OPTN 0 0 fail");
				goto rt_reboot;
			}
			temp = Message[4];
			temp|=0x4;
			makeCMV(H2D_CMV_WRITE, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n write OPTN 0 0 fail");
				goto rt_reboot;
			}
		}
		else if (mode == MODE_992_3A)
		{	makeCMV(H2D_CMV_READ, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n read OPTN 0 0 fail");
				goto rt_reboot;
			}
			temp = Message[4];
			temp|=0x100;
			makeCMV(H2D_CMV_WRITE, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n write OPTN 0 0 fail");
				goto rt_reboot;
			}
		}
		else if (mode == MODE_992_5A)
		{	makeCMV(H2D_CMV_READ, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n read OPTN 0 0 fail");
				goto rt_reboot;
			}
			temp = Message[4];
			temp|=0x8100;
			makeCMV(H2D_CMV_WRITE, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n write OPTN 0 0 fail");
				goto rt_reboot;
			}
		}
		else if (mode == MODE_992_1B)
		{	makeCMV(H2D_CMV_READ, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n read OPTN 0 0 fail");
				goto rt_reboot;
			}
			temp = Message[4];
			temp|=0x8;
			makeCMV(H2D_CMV_WRITE, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n write OPTN 0 0 fail");
				goto rt_reboot;
			}
		}
		else if (mode == MODE_992_3B)
		{	makeCMV(H2D_CMV_READ, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n read OPTN 0 0 fail");
				goto rt_reboot;
			}
			temp = Message[4];
			temp|=0x200;
			makeCMV(H2D_CMV_WRITE, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n write OPTN 0 0 fail");
				goto rt_reboot;
			}
		}
		else if (mode == MODE_992_5B)
		{	makeCMV(H2D_CMV_READ, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n read OPTN 0 0 fail");
				goto rt_reboot;
			}
			temp = Message[4];
			temp|=0x4200;
			makeCMV(H2D_CMV_WRITE, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n write OPTN 0 0 fail");
				goto rt_reboot;
			}
		}

		else if (mode == MODE_992_3I)
		{	makeCMV(H2D_CMV_READ, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n read OPTN 0 0 fail");
				goto rt_reboot;
			}
			temp = Message[4];
			temp|=0x400;
			makeCMV(H2D_CMV_WRITE, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n write OPTN 0 0 fail");
				goto rt_reboot;
			}
		}
		else if (mode == MODE_992_3J)
		{	makeCMV(H2D_CMV_READ, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n read OPTN 0 0 fail");
				goto rt_reboot;
			}
			temp = Message[4];
			temp|=0x800;
			makeCMV(H2D_CMV_WRITE, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n write OPTN 0 0 fail");
				goto rt_reboot;
			}
		}
		else if (mode == MODE_992_3M)
		{	makeCMV(H2D_CMV_READ, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n read OPTN 0 0 fail");
				goto rt_reboot;
			}
			temp = Message[4];
			temp|=0x2000;
			makeCMV(H2D_CMV_WRITE, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n write OPTN 0 0 fail");
				goto rt_reboot;
			}
		}
		else if (mode == MODE_992_5I)
		{	makeCMV(H2D_CMV_READ, OPTN, 7, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n read OPTN 7 0 fail");
				goto rt_reboot;
			}
			temp = Message[4];
			temp|=0x1;
			makeCMV(H2D_CMV_WRITE, OPTN, 7, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n write OPTN 7 0 fail");
				goto rt_reboot;
			}
		}
		else if (mode == MODE_992_5J)
		{	makeCMV(H2D_CMV_READ, OPTN, 7, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n read OPTN 7 0 fail");
				goto rt_reboot;
			}
			temp = Message[4];
			temp|=0x2;
			makeCMV(H2D_CMV_WRITE, OPTN, 7, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n write OPTN 7 0 fail");
				goto rt_reboot;
			}
		}
		else if (mode == MODE_992_5M)
		{	makeCMV(H2D_CMV_READ, OPTN, 7, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n read OPTN 7 0 fail");
				goto rt_reboot;
			}
			temp = Message[4];
			temp|=0x4;
			makeCMV(H2D_CMV_WRITE, OPTN, 7, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n write OPTN 7 0 fail");
				goto rt_reboot;
			}
		}
                else if (mode == MODE_M_ALL){
			makeCMV(H2D_CMV_READ, OPTN, 0, 0, 1, &temp, Message);
                        if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
                                printf("\n\n read OPTN 0 0 fail");
                                goto rt_reboot;
                        }
                        temp = Message[4];
                        temp|=0x2000;
                        makeCMV(H2D_CMV_WRITE, OPTN, 0, 0, 1, &temp, Message);
                        if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
                                printf("\n\n write OPTN 0 0 fail");
                                goto rt_reboot;
                        }

                        makeCMV(H2D_CMV_READ, OPTN, 7, 0, 1, &temp, Message);
                        if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
                                printf("\n\n read OPTN 7 0 fail");
                                goto rt_reboot;
                        }
                        temp = Message[4];
                        temp|=0x4;
                        makeCMV(H2D_CMV_WRITE, OPTN, 7, 0, 1, &temp, Message);
                        if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
                                printf("\n\n write OPTN 7 0 fail");
                                goto rt_reboot;
                        }
                }
                else if (mode == MODE_B_ALL){
                        makeCMV(H2D_CMV_READ, OPTN, 0, 0, 1, &temp, Message);
                        if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
                                printf("\n\n read OPTN 0 0 fail");
                                goto rt_reboot;
                        }
                        temp = Message[4];
                        temp|=0x4208;
                        makeCMV(H2D_CMV_WRITE, OPTN, 0, 0, 1, &temp, Message);
                        if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
                                printf("\n\n write OPTN 0 0 fail");
                                goto rt_reboot;
                        }
                }else if (mode == MODE_M_B_ALL){
                        makeCMV(H2D_CMV_READ, OPTN, 0, 0, 1, &temp, Message);
                        if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
                                printf("\n\n read OPTN 0 0 fail");
                                goto rt_reboot;
                }
                        temp = Message[4];
                        temp|=0x6208;
                        makeCMV(H2D_CMV_WRITE, OPTN, 0, 0, 1, &temp, Message);
                        if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
                                printf("\n\n write OPTN 0 0 fail");
                                goto rt_reboot;
                        }
                }
		else if (mode == MODE_ALL)
		{	makeCMV(H2D_CMV_READ, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n read OPTN 0 0 fail");
				goto rt_reboot; 
			}
			temp = Message[4];
			temp|=all_data;
			temp|= 2; //609181:tc.chen support G.lite

			makeCMV(H2D_CMV_WRITE, OPTN, 0, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n write OPTN 0 0 fail");
				goto rt_reboot; 
			}
		}

		if(ioctl(fd, GET_ADSL_LOOP_DIAGNOSTICS_MODE, &j)<0){
			printf("\n\n ioctl GET_ADSL_LOOP_DIAGNOSTICS_MODE fail");
			goto rt_reboot;
		}
		if (j == 1) // LOOP DIAGNOSTICS enabled
		{
			makeCMV(H2D_CMV_READ, OPTN, 9, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n read OPTN 9 0 fail");
				goto rt_reboot; 
			}
			temp = Message[4];
			temp |= 1<<2;
			//temp = 1<<2;
			makeCMV(H2D_CMV_WRITE, OPTN, 9, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n write OPTN 9 0 fail");
				goto rt_reboot; 
			}
		}

		/************ remove in future ***********/		

		if(argc==4){	//execute script file
			strncpy(systemstr, "./translate -ef ",16);
			strcpy(systemstr+16, argv[3]);
			system(systemstr);
		}

#ifdef DEMO_ONLY
		if (mode == MODE_992_1A)
			temp=0x303;
		else
			temp=0x300;
		makeCMV(H2D_CMV_WRITE, OPTN, 1, 0, 1, &temp, Message);
		if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
			printf("\n\n write OPTN 1 0 fail");
			return -1;
		}
		temp=0x8f78;
		makeCMV(H2D_CMV_WRITE, OPTN, 2, 0, 1, &temp, Message);
		if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
			printf("\n\n write OPTN 2 0 fail");
			return -1;
		}
#endif

		makeCMV(H2D_CMV_WRITE, CNTL, 0, 0, 1, &link, Message);
		if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
			printf("\n\n set CNTL 0 0 fail");
			goto rt_reboot;
		}
		
		FarEndResponseReceived=0;
		time(&start_time);		
		timeout_limit=60;
			
		while(1){
			int	tmpQuietModeEnabled;
			tmpQuietModeEnabled = 0;
                        if(ioctl(fd, QUIET_MODE_GET, &tmpQuietModeEnabled)<0){
                                printf("\n\n ioctl QUIET_MODE_GET fail");
                                goto rt_reboot;
                        }
#if 0 // 611101:tc.chen : change to use ioctl QUIET_MODE_GET
                        makeCMV(H2D_CMV_READ, INFO, 94, 0, 1, &temp, Message);
                        if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
                                printf("\n\n INFO 94 0 read fail");
                                goto rt_reboot;
                        }
			if( Message[4] == 0x0 )
				tmpQuietModeEnabled = 1;
#endif
			// check if loop diagnostic mode is changed
			if(ioctl(fd, GET_ADSL_LOOP_DIAGNOSTICS_MODE, &loop_diagnostics_mode)<0)
			{
				printf("\n\n ioctl GET_ADSL_LOOP_DIAGNOSTICS_MODE fail");
				goto rt_reboot;
			}
			if (loop_diagnostics_mode != pre_loop_diagnostics_mode)
			{
				pre_loop_diagnostics_mode = loop_diagnostics_mode;
				goto rt_reboot;
			}

			makeCMV(H2D_CMV_READ, STAT, 0, 0, 1, &rt_macrostate, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n STAT read fail");
				goto rt_reboot;
			}
	
			if(Message[4]==2)
			{
				if( tmpQuietModeEnabled == 0 ) 
					break;
			} 
		
			if((FarEndResponseReceived==0) && ((Message[4]==5) || (Message[4]==6))){
				FarEndResponseReceived=1;
				timeout_limit=60;
				time(&start_time);
			}
			
			if(Message[4]==9){
				timeout_limit=120;
				if(FarEndResponseReceived==0){
					FarEndResponseReceived=1;
					time(&start_time);
				}
			}

			if(Message[4]==3){
				if( tmpQuietModeEnabled == 0 ){ 
					sleep(5);
					break;
				} 
			}

			if(Message[4]==7){
				break;
			}
			
			if(time((time_t *)0)-start_time > timeout_limit){
				if( tmpQuietModeEnabled == 0 ) 
					break;
			}
			usleep(100000);
		}
		
		if (Message[4] != 7)
		{
			if (Message[4] == 3) // Done with loop diagnostics
			{
				ioctl(fd,LOOP_DIAGNOSTIC_MODE_COMPLETE,NULL);
				while(1)
				{
					if(ioctl(fd, GET_ADSL_LOOP_DIAGNOSTICS_MODE, &loop_diagnostics_mode)<0)
					{
						printf("\n\n ioctl GET_ADSL_LOOP_DIAGNOSTICS_MODE fail");
						goto rt_reboot;
					}
					if (loop_diagnostics_mode == 0)
					{
						pre_loop_diagnostics_mode = loop_diagnostics_mode;
						goto rt_reboot;
					}
					sleep(1);
				}
			}else
			{
				printf("\n\n Keep trying to reboot!"); 
				goto rt_reboot;	
			}
		}

		j=0;
		reboot_flag=0;
		while(reboot_flag==0){
			if(ioctl(fd, GET_ADSL_LOOP_DIAGNOSTICS_MODE, &loop_diagnostics_mode)<0)
			{
				printf("\n\n ioctl GET_ADSL_LOOP_DIAGNOSTICS_MODE fail");
				goto rt_reboot;
			}
			if (loop_diagnostics_mode != pre_loop_diagnostics_mode)
			{
				pre_loop_diagnostics_mode = loop_diagnostics_mode;
				goto rt_reboot;
			}

			makeCMV(H2D_CMV_READ, PLAM, 0, 0, 1, &rt_ne_failure_flags, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n PLAM 0 0 read fail");
				goto rt_reboot;
			}

			rt_ne_failure_flags = Message[4];
			rt_ne_los_flag=rt_ne_failure_flags & 0x1;
			
#ifndef DEMO_ONLY
			// MIB count start
			ioctl(fd, IFXMIPS_MIB_LO_ATUC, (u32*)rt_ne_failure_flags);
			makeCMV(H2D_CMV_READ, PLAM, 1, 0, 1, &temp, Message);
			
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n PLAM 1 0 read fail");
				goto rt_reboot;
			}

			ioctl(fd, IFXMIPS_MIB_LO_ATUR, (u32*)Message[4]);
			//MIB count end
#endif
			
			rt_ne_ds_margin = 0;
			makeCMV(H2D_CMV_READ, PLAM, 46, 0, 1, &rt_ne_ds_margin, Message);
			
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n PLAM 46 0 read fail");
				goto rt_reboot;
			}

			rt_ne_ds_margin = (s16)Message[4];

			if(ioctl(fd, SHOWTIME_LOCK_GET, &tmpShowTimeLockEnabled)<0){
				printf("\n\n ioctl SHOWTIME_LOCK_GET fail!");
				goto rt_reboot;
			}
#if 0 //611101:tc.chen change to ioctl SHOWTIME_LOCK
			makeCMV(H2D_CMV_READ, TEST, 29, 0, 1, &temp, Message);
			if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
				printf("\n\n TEST 29 0 read fail");
				goto rt_reboot;
			}
			if( Message[4] == 0xffff )
				tmpShowTimeLockEnabled = 1;
#endif
			/* rt_ne_ds_margin is SNR *2  */	
			if((rt_ne_los_flag!=0)|| ( rt_ne_ds_margin<(rt_ne_ds_min_margin * 2))){
				if(tmpShowTimeLockEnabled == 1) {
// changed by xu bingtao 09/01/2007 
/*
					printf("NE_LOS or NE_DS_MARGIN detected but no rebooting because of the showtime lock\n");
*/
					printf("NE_LOS or NE_DS_MARGIN detected but no rebooting because of the showtime lock\n");
					printf("  Minimum Margin: %d/2 dB  -  Expected Margin: %d dB -  LOS : %d\n",rt_ne_ds_margin, rt_ne_ds_min_margin, rt_ne_los_flag);	
// changed by xu bingtao 09/01/2007 
				}
				else
					reboot_flag=1;
			}

			usleep(1500000);

			if(j<3){//wait for several seconds before setting showtime
				makeCMV(H2D_CMV_READ, STAT, 4, 0, 1, &temp, Message);
				if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
					printf("\n\n STAT 4 0 read fail");
					goto rt_reboot;
				}
				if((Message[4]&0x2)==0x2){
					printf("\n\n setting showtime to driver\n\n");
					system("/etc/rc.d/adsl_up &"); //000001:tc.chen
					system("echo \"7\" > /tmp/adsl_status");
					printf("ADSL SHOWTIME!!\n");
					sleep(1);			
//joelin 04/16/2005
#ifndef DEMO_ONLY
					if (!first_power_on){
		
						makeCMV(H2D_CMV_WRITE,PLAM, 6, 0, 1, &nFECS_L_count, Message); 
						ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message);
	 	
						makeCMV(H2D_CMV_WRITE,PLAM, 7, 0, 1, &nES_L_count, Message); 
						ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message);
	 
	 		 		
				 		makeCMV(H2D_CMV_WRITE,PLAM, 8, 0, 1, &nSES_L_count, Message); 
						ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message);
	
	 		
						makeCMV(H2D_CMV_WRITE,PLAM, 9, 0, 1, &nLOSS_L_count, Message); 
						ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message);
			  		}									
#endif
					ioctl(fd, IFXMIPS_MEI_SHOWTIME,NULL); 	//set showtime=1 in driver			
					// 609141:tc.chen :read min margin from CO for Annex B in DMT mode
					// read adsl link mode
					read_min_snr_margin(fd, &rt_ne_ds_min_margin);
					j=1000;				
				}
				j++;
				if(j==3){//timeout, set showtimei
					printf("\n\n timeout, setting showtime to driver\n\n");
					//system("echo \"7\" > /tmp/adsl_status");
					printf("ADSL SHOWTIME!!\n");
					sleep(1);			
					ioctl(fd, IFXMIPS_MEI_SHOWTIME,NULL); 	//set showtime=1 in driver
					// 609141:tc.chen :read min margin from CO for Annex B in DMT mode
					// read adsl link mode
					read_min_snr_margin( fd, &rt_ne_ds_min_margin);
				}								
			}
		}
rt_reboot:
			        
#ifndef DEMO_ONLY
		makeCMV(H2D_CMV_READ,PLAM, 6, 0, 1, &temp, Message); 
		ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message);
 		nFECS_L_count=nFECS_L_count+Message[4];
 	
		makeCMV(H2D_CMV_READ,PLAM, 7, 0, 1, &temp, Message); 
		ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message);
 		nES_L_count=nES_L_count+Message[4]; 
 		 		
 		makeCMV(H2D_CMV_READ,PLAM, 8, 0, 1, &temp, Message); 
		ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message);
 		nSES_L_count=nSES_L_count+Message[4];
 		
		makeCMV(H2D_CMV_READ,PLAM, 9, 0, 1, &temp, Message); 
		ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message);
 		nLOSS_L_count=nLOSS_L_count+Message[4]; 
#endif

		/* add by tc chen */
		system("echo \"0\" > /tmp/adsl_status");
		printf("\n Rebooting ARC\n");
reboot_adsl:
		ioctl(fd, IFXMIPS_MEI_REBOOT,NULL);
		Download_Firmware(argv[1],fd);
		if (ioctl(fd, IFXMIPS_MEI_START,NULL)<0){
			printf("\n\n mei start fail.\n");
			goto reboot_adsl;
		}
	}

#if !defined(KERNEL_SPACE_APPLICATION)			
	close(fd);
#endif
        return 0;
}
