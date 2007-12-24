#define _IFXMIPS_ADSL_APP
#define u32	unsigned int
#define IFXMIPS_MEI_DEV 	"/dev/ifxmips/mei"
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <asm/ifxmips/ifxmips_mei_app_ioctl.h>
#include <asm/ifxmips/ifxmips_mei_app.h>
#include <asm/ifxmips/ifxmips_mei_ioctl.h>

u16 Message[16]__attribute__ ((aligned(4))); 

void makeCMV(u8 opcode, u8 group, u16 address, u16 index, int size, u16 * data);
void makeCMV(u8 opcode, u8 group, u16 address, u16 index, int size, u16 * data)
{
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
	return;
}


int main(int argc, char **argv)
{
        int fd;
	u16 temp=0;
	u16 rt_buildcode_lsw=0, rt_buildcode_msw=0;
	u32 rt_buildcode = 0;
		
	fd=open(IFXMIPS_MEI_DEV, O_RDWR);
	if(fd<0){
		printf("\n\n autoboot open device fail");
		return -1;
	}


	makeCMV(H2D_CMV_READ, INFO, 55, 0, 1, &temp);
	if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
		printf("\n\n read INFO 55 0 fail");
		return -1;
	}
	rt_buildcode_lsw = Message[4];
	
	makeCMV(H2D_CMV_READ, INFO, 55, 1, 1, &temp);
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
	return 0;
}
