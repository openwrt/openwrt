/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */
//-----------------------------------------------------------------------
//Description:	
// read the CMV register under Linux for Amazon
//-----------------------------------------------------------------------
//Author:	Joe.Lin@infineon.com
//Created:	31-December-2004
//-----------------------------------------------------------------------
/* History
 * Last changed on:
 *  507051:linmars 2005/07/5 fix makeCMV problem
 * Last changed by: 
 *
*/

#define _IFXMIPS_ADSL_APP
//#define DEBUG
#define u32 unsigned int
#define u16 unsigned short
#define u8  unsigned char
#define IFXMIPS_MEI_DEV  "/dev/ifxmips/mei"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <asm/ifxmips/ifxmips_mei_app_ioctl.h>
#include <asm/ifxmips/ifxmips_mei_app.h>
#include <asm/ifxmips/ifxmips_mei_ioctl.h>
#include <asm/ifxmips/ifxmips_mei.h>

#ifdef IFX_MULTILIB_UTIL
#define	main	cmvwrite_main
#define	display_version	cmvwrite_display_version
#endif

/*============================definitions======================*/
#define OPTN 5
#define CNFG 8
#define CNTL 1
#define STAT 2
#define RATE 6
#define PLAM 7
#define INFO 3
#define TEST 4

typedef unsigned short UINT16;
typedef unsigned long UINT32;

/*=============================================================*/


/*=============================global variables================*/
#ifdef IFX_MULTILIB_UTIL
extern int c;
extern int input_flag;
extern int digit_optind;
extern FILE* script_file;
extern void (*func)();
extern int fd;

UINT16 var16[8];
UINT32 var32[8];
UINT16 Message[16];
#else
int c=0;
int input_flag=0;
int digit_optind=0;
FILE* script_file;
void (*func)()=NULL;
int fd;

UINT16 var16[8];
UINT32 var32[8];
UINT16 Message[16];
#endif
/*=============================================================*/
/*165001:henryhsu 2005/9/7 Modify some error in cmvwrite utility*/
int ifx_makeCMV(unsigned char opcode, unsigned char group, unsigned short address, unsigned short index, int size, unsigned short * data, unsigned short *Message, int msg_len)
{
        if (msg_len < 16*2)
                return -1;
        memset(Message, 0, 16*2);
        Message[0]= (opcode<<4) + (size&0xf);
        if(opcode == H2D_DEBUG_WRITE_DM)
                Message[1]= (group&0x7f);
        else
                Message[1]= (((index==0)?0:1)<<7) + (group&0x7f);
        Message[2]= address;
        Message[3]= index;
        if((opcode == H2D_CMV_WRITE)||(opcode == H2D_DEBUG_WRITE_DM))
                memcpy(Message+4, data, size*2);

        return 0;
}


void display_version()
{
   printf("adsl cmv write version1.0\nby Joe Lin \nJoe.Lin@infineon.com\n");
   return;
}

//165001:henryhsu 2005/9/7 No need to check this any more
//#ifndef IFX_MULTILIB_UTIL

//void cmvreader_help()
void cmvwrite_help()
{
    printf("Usage:cmvwrite [options] [group name][address][index][data] ...\n");
    printf("options:\n");
    printf("	-h --help            Display help information\n");       
    printf("	-v --version         Display version information\n");
    printf("group name:			--group name of CMV to read\n");
    printf("	OPTN -- CMV Group 5 \n");
    printf("	CNFG -- CMV Group 8 \n");
    printf("	CNTL -- CMV Group 1 \n"); 
    printf("	STAT -- CMV Group 2 \n");  
    printf("	RATE -- CMV Group 6 \n");  
    printf("	PLAM -- CMV Group 7 \n");  
    printf("	INFO -- CMV Group 3 \n");  
    printf("	TEST -- CMV Group 4 \n");  
    printf("address --address value of CMV to write\n"); 
    printf("index --index value of CMV to write\n");     
    printf("data  --data to write in Hex.\n");        
 
    return;
}

int main (int argc, char** argv) {
     
  	char *endptr; 
  	unsigned short value;
 	if (argc < 2)
	{
	  cmvwrite_help();
	  return;
	}
 
 	if (strstr(argv[1], "-h") != NULL){
 	  cmvwrite_help();
	  return;
 	}
 
  	if (strstr(argv[1], "-v") != NULL){
 	  display_version();
	  return;
 	}
 
 
        fd=open(IFXMIPS_MEI_DEV, O_RDWR);
        if(fd<0){
                printf("\n\n autoboot open device fail\n");
                return -1;
        }
        
        
        
        int group=0,address,index,size;
	if((strcmp(argv[1],"optn")==0)||(strcmp(argv[1],"OPTN")==0))
	group=OPTN;
	else if((strcmp(argv[1],"cnfg")==0)||(strcmp(argv[1],"CNFG")==0))
	group=CNFG;
	else if((strcmp(argv[1],"cntl")==0)||(strcmp(argv[1],"CNTL")==0))
	group=CNTL;
	else if((strcmp(argv[1],"stat")==0)||(strcmp(argv[1],"STAT")==0))
	group=STAT;
	else if((strcmp(argv[1],"rate")==0)||(strcmp(argv[1],"RATE")==0))
	group=RATE;
	else if((strcmp(argv[1],"plam")==0)||(strcmp(argv[1],"PLAM")==0))
	group=PLAM;
	else if((strcmp(argv[1],"info")==0)||(strcmp(argv[1],"INFO")==0))
	group=INFO;
	else if((strcmp(argv[1],"test")==0)||(strcmp(argv[1],"TEST")==0))
	group=TEST;
	else 
	 {
	   printf("wrong group type!\nplease slect group:OPTN CNFG CNTL STAT RATE PLAM INFO TEST \n");
                close(fd);
                exit(0);
	 }
	 
	address = strtoul(argv[2], &endptr, 10);
	index = strtoul(argv[3], &endptr, 10);
 
	value = strtoul(argv[4],NULL,0);
        ifx_makeCMV(H2D_CMV_WRITE, group, address, index, 1, &value, Message, sizeof(Message));
        if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
                printf("cr read %d %d %d fail",group,address,index);
                close(fd);
                exit(0);
        }

	printf ("write %s %d %d 0x%X\n",argv[1],address,index,value);
	
	close(fd);
 	return 0;	     
}

