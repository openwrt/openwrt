#define _IFXMIPS_ADSL_APP
#define u32	unsigned int
#define IFXMIPS_MEI_DEV 	"/dev/ifxmips/mei"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <asm/ifxmips/ifxmips_mei_app.h>
#include <asm/ifxmips/ifxmips_mei_ioctl.h>
#include <asm/ifxmips/ifxmips_mei_app_ioctl.h>

#define SEGMENT_SIZE (64*1024)
#if 0
#define u8 unsigned char 
#define u16 unsigned short
#endif

int main(int argc, char **argv)
{
        int fd_image, fd_mei;
        char *buf;
	struct stat state;
	int i,j;
	char * filename;  
	int size,read_size=SEGMENT_SIZE;
	u16 temp=0;
	u16 Message[16]__attribute__ ((aligned(4))); 
	meidebug dbg_data;
	int rw_flag =0;
	  
	if(argc<4){
		printf("\nWrong Arguments! Usage: %s r/w address size/value\n",argv[0]);
		return -1;
	}

	fd_mei=open(IFXMIPS_MEI_DEV, O_RDWR);
	if(fd_mei<0)
		printf("\n open device fail");

	dbg_data.iAddress = strtoul(argv[2],NULL,0);
	printf("Address = %08X\n",dbg_data.iAddress);
	if(argv[1][0]=='w')
	{
		dbg_data.buffer[0]=strtoul(argv[3],NULL,0);

		dbg_data.iCount = 1;
		rw_flag = 1;
	}else
	{
		dbg_data.iCount =strtoul(argv[3],NULL,0);
	}
	if (rw_flag==1)
	{
		if(ioctl(fd_mei, IFXMIPS_MEI_WRITEDEBUG,&dbg_data)!=MEI_SUCCESS){
			printf("\n IFXMIPS_MEI_WRITEDEBUG() failed");
			close(fd_mei);
			return -1;
		}
		printf("Writing %08X with data %X!\n",dbg_data.iAddress,dbg_data.buffer[0]);
	}else
	{
		if(ioctl(fd_mei, IFXMIPS_MEI_READDEBUG,&dbg_data)!=MEI_SUCCESS){
			printf("\n IFXMIPS_MEI_READDEBUG() failed");
			close(fd_mei);
			return -1;
		}
		printf("Read %08X .\nResult:",dbg_data.iAddress);
		for (i=0;i<dbg_data.iCount;i++)
		{
			printf("%08X ",dbg_data.buffer[i]);
			if(i%4==3)
				printf("\n");
		}
		printf("\n");
	}
	close(fd_mei);
}
