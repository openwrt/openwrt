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
// winhost script translation tool under Linux for Danube
//-----------------------------------------------------------------------
//Author:	Qi-Ming.Wu@infineon.com
//Created:	29-October-2004
//-----------------------------------------------------------------------
/* History
 * Last changed on:
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

#include <asm/ifxmips/ifxmips.h>
#include <asm/ifxmips/ifxmips_mei_app_ioctl.h>
#include <asm/ifxmips/ifxmips_mei_app.h>
#include <asm/ifxmips/ifxmips_mei_ioctl.h>
#include <asm/ifxmips/ifxmips_mei.h>


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
int c=0;
int input_flag=0;
int digit_optind=0;
FILE* script_file;
void (*func)()=NULL;
int fd;

UINT16 var16[8];
UINT32 var32[8];
UINT16 Message[16];

/*=============================================================*/




void display_version()
{
   printf("translate version1.0\nby Wu Qi Ming\nQi-Ming.Wu@infineon.com\n");
   return;
}


void translate_help()
{
    printf("Usage:translate [options] [parameter] ...\n");
    printf("options:\n");
    printf("-h --help            Display help information\n");       
    printf("-v --version         Display version information\n");
    printf("-f --file            Specify the script file\n");
    printf("-e --execute         Execute the specified script file\n");
    //printf("-c --check           Check the script file\n");
    return;
}

/*
print the content in buf excluding the command "echo" itself.
*/

void echo(char* buf)
{
       int i;
       char msg[128];
       for(i=0;i<128;i++)
       {
          if((buf[i]!=0)&&(buf[i]!=32))
	  {
	    sscanf(buf+i,"%s",&msg);
	    if(strcmp(msg,"echo")==0)
	    i+=4;
	    else break;
	  }
	  
       }
       for(i;i<128;i++)
       {
          if(buf[i]!=0&&(buf[i]!=10))
	  {
	    printf("%c",buf[i]);
          }
       }
       
       
    printf("\n");
    return;

}

static void makeCMV(u8 opcode, u8 group, u16 address, u16 index, int size, u16 * data)
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



UINT16 CMV_read(char* str_group,int address,int index)
{
        int group=0;
	if(strcmp(str_group,"optn")==0)
	group=OPTN;
	else if(strcmp(str_group,"cnfg")==0)
	group=CNFG;
	else if(strcmp(str_group,"cntl")==0)
	group=CNTL;
	else if(strcmp(str_group,"stat")==0)
	group=STAT;
	else if(strcmp(str_group,"rate")==0)
	group=RATE;
	else if(strcmp(str_group,"plam")==0)
	group=PLAM;
	else if(strcmp(str_group,"info")==0)
	group=INFO;
	else if(strcmp(str_group,"test")==0)
	group=TEST;
	else 
	 {
	   printf("wrong group type!\ncheck script file please!\n");
	   exit(0);
	 }
	
#ifndef DEBUG	
        makeCMV(H2D_CMV_READ, group, address, index, 1, NULL);
        if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
                printf("cr read %d %d %d fail",group,address,index);
                exit(0);
        }
#endif 
        
	return Message[4];

	
} 


int CMV_write(char* str_group,int address,int index,UINT16 value)
{
        int group=0;
	if(strcmp(str_group,"optn")==0)
	group=OPTN;
	else if(strcmp(str_group,"cnfg")==0)
	group=CNFG;
	else if(strcmp(str_group,"cntl")==0)
	group=CNTL;
	else if(strcmp(str_group,"stat")==0)
	group=STAT;
	else if(strcmp(str_group,"rate")==0)
	group=RATE;
	else if(strcmp(str_group,"plam")==0)
	group=PLAM;
	else if(strcmp(str_group,"info")==0)
	group=INFO;
	else if(strcmp(str_group,"test")==0)
	group=TEST;
	else 
	 {
	   printf("wrong group type!\ncheck script file please!\n");
	   exit(0);
	 }
	
#ifndef DEBUG	
        makeCMV(H2D_CMV_WRITE, group, address, index, 1, &value);
        if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
                printf("cw %d %d %d fail",group,address,index);
                return -1;
        }
#endif 
        
	return 0;

	
} 

void execute_script()
{
   char buf[128];
   char str_command[20]={0};
   char str_group[20]={0};
   char str_value[128]={0};
   char op1[40],op2[40];
   int  address,index;
   int  n=0;
   UINT16 value=0;
   if(!script_file) return;
   
   while(fgets(buf,128,script_file))/*scan one line into buffer, 128 bytes at most*/
      {
      if(sscanf(buf,"%s",&str_command)==0) continue;
      /*if the line is empty, then go on to the next*/
      if(strcmp(str_command,"cr")==0)/*cr command*/
      {
        sscanf(buf,"%s %s %d %d %s",&str_command,\
                                   &str_group,\
				   &address,\
				   &index,\
				   &str_value);
       if(strncmp(str_value,"$",1)!=0) 
       {
          printf("error:cr parameter type mismatch!\n");
	  break;
       }
       n=strtoul(str_value+1,NULL,0);
       var16[n]=CMV_read(str_group,address,index);
       printf("read %s %d %d, value=%04x\n",str_group, address, index,var16[n]);
      }
      else if(strcmp(str_command,"cw")==0)/*cw command*/
      {
        sscanf(buf,"%s %s %d %d %s",&str_command,\
                                   &str_group,\
				   &address,\
				   &index,\
				   &str_value);
        n=strtoul(str_value+1,NULL,0);
	if(strncmp(str_value,"$",1)==0)
	 {
	   value=var16[n];   
	 }
	else if(strncmp(str_value,"#",1)==0)
	 {
	   value=strtoul(str_value+1,NULL,0);
	 }
	else
	 {
	    value=strtoul(str_value,NULL,0);
	 }  	
        if(CMV_write(str_group,address,index,value)<0)
	 {
	   break;
	 }
      }
      
      else if(strcmp(str_command,"mw")==0)/*mr command*/
      {
        
         sscanf(buf,"%s %s %s",&str_command,\
                               &op1,\
			       &str_value);
         
	 n=strtoul(str_value+1,NULL,0);
	 address=strtoul(op1,NULL,0);
	 if(strncmp(str_value,"$",1)==0)
	 {
	   value=var16[n];   
	 }
	 else if(strncmp(str_value,"#",1)==0)
	 {
	   value=strtoul(str_value+1,NULL,0);
	 }
	 else
	 {
	   value=strtoul(str_value,NULL,0);
	 
	 }
	 
	 makeCMV(H2D_DEBUG_WRITE_DM, 0x0, address>>16, (address)&(0xffff), 1, &value);
         if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
                        printf("mw %08x fail",address);
			break;
             
          }
	 
      }
      
      else if(strcmp(str_command,"mr")==0)/*mr command*/
      {
        
         sscanf(buf,"%s %s %s",&str_command,\
                               &op1,\
			       &str_value);
         address=strtoul(op1,NULL,0);
	 n=strtoul(str_value+1,NULL,0);
	 if(strncmp(str_value,"$",1)!=0) 
         {
          printf("error:mr parameter type mismatch!\n");
	  break;
         }
         makeCMV(H2D_DEBUG_READ_DM, 0x0, address>>16, (address)&(0xffff), 1, NULL);
         if(ioctl(fd, IFXMIPS_MEI_CMV_WINHOST, &Message)<0){
                        printf("mw %08x fail",address);
			break;
          }
	 value=Message[4];
         var16[n]=value;	 
      }
      
      
      
      else if(strcmp(str_command,"meiw")==0)/*meiw command*/
      {
         meireg regrdwr;
         sscanf(buf,"%s %s %s",&str_command,\
                               &op1,\
			       &op2);
         regrdwr.iAddress=strtoul(op1,NULL,0)+IFXMIPS_MEI_BASE_ADDR;
	 
	 
	 if(strncmp(op2,"#",1)==0) regrdwr.iData=strtoul(op1+1,NULL,0);
	 else if(strncmp(op2,"$",1)==0) regrdwr.iData=var16[strtoul(op2+1,NULL,0)];
	 else if(strncmp(op2,"@",1)==0) regrdwr.iData=var32[strtoul(op2+1,NULL,0)];
         else regrdwr.iData=strtoul(op1,NULL,0);
	 //printf("address=%08x\n", regrdwr.iAddress);   
	 if(ioctl(fd, IFXMIPS_MEI_CMV_WRITE, &regrdwr)<0){
                printf("meiw fail\n");
                break;;
        }
	 
      }
      
      else if(strcmp(str_command,"meir")==0)/*meir command*/
      {
         meireg regrdwr;
         sscanf(buf,"%s %s %s",&str_command,\
                               &op1,\
			       &op2);
         regrdwr.iAddress=strtoul(op1,NULL,0)+IFXMIPS_MEI_BASE_ADDR;
         if(ioctl(fd, IFXMIPS_MEI_CMV_READ, &regrdwr)<0){
                printf("meir fail\n");
                break;
         }
	 if(strncmp(op2,"$",1)==0) var16[strtoul(op2+1,NULL,0)]=regrdwr.iData;
	 else if(strncmp(op2,"@",1)==0) var32[strtoul(op2+1,NULL,0)]=regrdwr.iData;
         else
	   {
	     printf("meir grammar error!\n");
	     break; 
	   }
	 
      }
      
      
      else if(strcmp(str_command,"lst")==0)/*lst command*/
      {
         
         sscanf(buf,"%s %s",&str_command,\
                            &str_value);		    
         
	 if(strncmp(str_value,"$",1)==0)
	  {
	      n=strtoul(str_value+1,NULL,0);
	      printf("$%d=0x%04x\n",n,var16[n]);
	  
	  }
	 else if(strncmp(str_value,"@",1)==0)
	  {
	      n=strtoul(str_value+1,NULL,0);
	      printf("$%d=0x%08x\n",n,var32[n]);
	 
	  }
	 else
	  {
	    printf("lst grammar error!\n");
	    break;  
	  }
      }
      
      else if(strcmp(str_command,"echo")==0)/*echo command*/
      {
         echo(buf);
      } 
      else if(strcmp(str_command,"mov")==0)/*mov command*/
      {
          sscanf(buf,"%s %s %s",&str_command,\
                               &op1,\
			       &op2);
	  if(strncmp(op1,"$",1)==0)
          {
	    if(strncmp(op2,"$",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]=var16[strtoul(op2+1,NULL,0)];
	    }
	    else if(strncmp(op2,"@",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]= var32[strtoul(op2+1,NULL,0)];
	    
	    }
	    else if(strncmp(op2,"#",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]=strtoul(op2+1,NULL,0);
	    }
	    else
	    {
	       var16[strtoul(op1+1,NULL,0)]=strtoul(op2,NULL,0);
	    
	    }
	  }
	 else if(strncmp(op1,"@",1)==0)
	  {
	    if(strncmp(op2,"$",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]=var16[strtoul(op2+1,NULL,0)];
	    }
	    else if(strncmp(op2,"@",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]= var32[strtoul(op2+1,NULL,0)];
	    
	    }
	    else if(strncmp(op2,"#",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]=strtoul(op2+1,NULL,0);
	    }
	  }
	 else
	  {
	    printf("grammar error!\n");
	    break;  
	  }
      
      
      }
      
      
      else if(strcmp(str_command,"or")==0)/*or command*/
      {
         sscanf(buf,"%s %s %s",&str_command,\
                               &op1,\
			       &op2);
         if(strncmp(op1,"$",1)==0)
          {
	    if(strncmp(op2,"$",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]|=var16[strtoul(op2+1,NULL,0)];
	    }
	    else if(strncmp(op2,"@",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]|= var32[strtoul(op2+1,NULL,0)];
	    
	    }
	    else if(strncmp(op2,"#",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]|=strtoul(op2+1,NULL,0);
	    }
	  }
	 else if(strncmp(op1,"@",1)==0)
	  {
	    if(strncmp(op2,"$",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]|=var16[strtoul(op2+1,NULL,0)];
	    }
	    else if(strncmp(op2,"@",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]|= var32[strtoul(op2+1,NULL,0)];
	    
	    }
	    else if(strncmp(op2,"#",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]|=strtoul(op2+1,NULL,0);
	    }
	  }
	 else
	  {
	    printf("grammar error!\n");
	    break;  
	  }
      }
 
       else if(strcmp(str_command,"and")==0)/*and command*/
      {
         sscanf(buf,"%s %s %s",&str_command,\
                               &op1,\
			       &op2);
         if(strncmp(op1,"$",1)==0)
          {
	    if(strncmp(op2,"$",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]&=var16[strtoul(op2+1,NULL,0)];
	    }
	    else if(strncmp(op2,"@",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]&= var32[strtoul(op2+1,NULL,0)];
	    
	    }
	    else if(strncmp(op2,"#",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]&=strtoul(op2+1,NULL,0);
	    }
	  }
	 else if(strncmp(op1,"@",1)==0)
	  {
	    if(strncmp(op2,"$",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]&=var16[strtoul(op2+1,NULL,0)];
	    }
	    else if(strncmp(op2,"@",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]&= var32[strtoul(op2+1,NULL,0)];
	    
	    }
	    else if(strncmp(op2,"#",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]&=strtoul(op2+1,NULL,0);
	    }
	  }
	 else
	  {
	    printf("grammar error!\n");
	    break;  
	  }
         }
        else if(strcmp(str_command,"not")==0)/*not command*/
        {
         sscanf(buf,"%s %s",&str_command,\
                               &op1);
	 if(strncmp(op1,"$",1)==0)
	 { 
	   var16[strtoul(op1+1,NULL,0)]=~var16[strtoul(op1+1,NULL,0)];
	 }
	 else if(strncmp(op1,"@",1)==0)
	 { 
	   var32[strtoul(op1+1,NULL,0)]=~var32[strtoul(op1+1,NULL,0)];
	 }
	 else
	  {
	    printf("grammar error!\n");
	    break;  
	  }		       
        }
	 else if(strcmp(str_command,"shl")==0)/*shl command*/
      {
         sscanf(buf,"%s %s %s",&str_command,\
                               &op1,\
			       &op2);
         if(strncmp(op1,"$",1)==0)
          {
	    if(strncmp(op2,"$",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]<<=var16[strtoul(op2+1,NULL,0)];
	    }
	    else if(strncmp(op2,"@",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]<<= var32[strtoul(op2+1,NULL,0)];
	    
	    }
	    else if(strncmp(op2,"#",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]<<=strtoul(op2+1,NULL,0);
	    }
	  }
	 else if(strncmp(op1,"@",1)==0)
	  {
	    if(strncmp(op2,"$",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]<<=var16[strtoul(op2+1,NULL,0)];
	    }
	    else if(strncmp(op2,"@",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]<<= var32[strtoul(op2+1,NULL,0)];
	    
	    }
	    else if(strncmp(op2,"#",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]<<=strtoul(op2+1,NULL,0);
	    }
	  }
	 else
	  {
	    printf("grammar error!\n");
	    break;  
	  }
         }
	 
	else if(strcmp(str_command,"shr")==0)/*shr command*/
      {
         sscanf(buf,"%s %s %s",&str_command,\
                               &op1,\
			       &op2);
         if(strncmp(op1,"$",1)==0)
          {
	    if(strncmp(op2,"$",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]>>=var16[strtoul(op2+1,NULL,0)];
	    }
	    else if(strncmp(op2,"@",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]>>= var32[strtoul(op2+1,NULL,0)];
	    
	    }
	    else if(strncmp(op2,"#",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]>>=strtoul(op2+1,NULL,0);
	    }
	  }
	 else if(strncmp(op1,"@",1)==0)
	  {
	    if(strncmp(op2,"$",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]>>=var16[strtoul(op2+1,NULL,0)];
	    }
	    else if(strncmp(op2,"@",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]>>= var32[strtoul(op2+1,NULL,0)];
	    
	    }
	    else if(strncmp(op2,"#",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]>>=strtoul(op2+1,NULL,0);
	    }
	  }
	 else
	  {
	    printf("grammar error!\n");
	    break;  
	  }
         } 
	 
	else if(strcmp(str_command,"add")==0)/*add command*/
      {
         sscanf(buf,"%s %s %s",&str_command,\
                               &op1,\
			       &op2);
         if(strncmp(op1,"$",1)==0)
          {
	    if(strncmp(op2,"$",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]+=var16[strtoul(op2+1,NULL,0)];
	    }
	    else if(strncmp(op2,"@",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]+= var32[strtoul(op2+1,NULL,0)];
	    
	    }
	    else if(strncmp(op2,"#",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]+=strtoul(op2+1,NULL,0);
	    }
	  }
	 else if(strncmp(op1,"@",1)==0)
	  {
	    if(strncmp(op2,"$",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]+=var16[strtoul(op2+1,NULL,0)];
	    }
	    else if(strncmp(op2,"@",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]+= var32[strtoul(op2+1,NULL,0)];
	    
	    }
	    else if(strncmp(op2,"#",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]+=strtoul(op2+1,NULL,0);
	    }
	  }
	 else
	  {
	    printf("grammar error!\n");
	    break;  
	  }
         }  
	else if(strcmp(str_command,"sub")==0)/*sub command*/
      {
         sscanf(buf,"%s %s %s",&str_command,\
                               &op1,\
			       &op2);
         if(strncmp(op1,"$",1)==0)
          {
	    if(strncmp(op2,"$",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]-=var16[strtoul(op2+1,NULL,0)];
	    }
	    else if(strncmp(op2,"@",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]-= var32[strtoul(op2+1,NULL,0)];
	    
	    }
	    else if(strncmp(op2,"#",1)==0)
	    {
	       var16[strtoul(op1+1,NULL,0)]-=strtoul(op2+1,NULL,0);
	    }
	  }
	 else if(strncmp(op1,"@",1)==0)
	  {
	    if(strncmp(op2,"$",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]-=var16[strtoul(op2+1,NULL,0)];
	    }
	    else if(strncmp(op2,"@",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]-= var32[strtoul(op2+1,NULL,0)];
	    
	    }
	    else if(strncmp(op2,"#",1)==0)
	    {
	       var32[strtoul(op1+1,NULL,0)]-=strtoul(op2+1,NULL,0);
	    }
	  }
	 else
	  {
	    printf("grammar error!\n");
	    break;  
	  }
         }   
	
      memset(str_command,0,20);
      memset(buf,0,128);  
   }  
   return;
}

int main (int argc, char** argv) {
     
#ifndef DEBUG
        fd=open(IFXMIPS_MEI_DEV, O_RDWR);
        if(fd<0){
                printf("\n\n autoboot open device fail\n");
                return -1;
        }
#endif
     while(1) {
       int option_index=0;
       static struct option long_options[] ={
           {"help",0,0,0},
	   {"version",0,0,0},
	   {"file",1,0,0},
	   {"execute",0,0,0}   
       };
       
       c = getopt_long(argc,argv, "hvf:e",
                      long_options, &option_index);
       
       //printf("c=%d option_index=%d\n",c,option_index);	
       if(c==-1)
         {
	  if(input_flag==0)
	  {
	   printf("translate:please specify parameters\n");
	   func=&translate_help;
          }
	  if(func)
	   (*func)();
	  else
	   {
ERROR:      translate_help();
	   } 
	  break;
	 }
       input_flag=1;
       switch (c) {
            case 0:
	       if(option_index==0)
	        {
		 func=&translate_help;
		 break; 
	        } 
	       if(option_index==1) 
	        {
		    func=&display_version;
		    break;
                } 
	       if(option_index==2)
	        {
	            goto FILE;
		    break;
	        }
	       if(option_index==3)
	        {
	            goto EXECUTE;
		    break; 
	        }
	    case 'h':
	       func=&translate_help;
	       break;    
	    case 'v':
	       func=&display_version;
	       break;
	    case 'f':
FILE:	       if(!optarg) goto ERROR;
	       script_file=fopen((char*)optarg,"r");
               if(!script_file) perror("fopen");
	       break;
	    case 'e':
EXECUTE:       func=&execute_script;
               break;
	    }     
	
	    
     }
            if(script_file) fclose(script_file);
#ifndef DEBUG	    
	    close(fd);
#endif	     
}



