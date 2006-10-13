/*
 * NVRAM variable manipulation (Linux user mode half)
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <typedefs.h>
#include <bcmnvram.h>
#include <nvram_convert.h>
#include <shutils.h>
#include <utils.h>

#define PATH_DEV_NVRAM "/dev/nvram"

/* Globals */
static int nvram_fd = -1;
static char *nvram_buf = NULL;
int check_action(void);
int file_to_buf(char *path, char *buf, int len);

int
nvram_init(void *unused)
{
	if ((nvram_fd = open(PATH_DEV_NVRAM, O_RDWR)) < 0)
		goto err;

	/* Map kernel string buffer into user space */
	if ((nvram_buf = mmap(NULL, NVRAM_SPACE, PROT_READ, MAP_SHARED, nvram_fd, 0)) == MAP_FAILED) {
		close(nvram_fd);
		nvram_fd = -1;
		goto err;
	}

	return 0;

 err:
	perror(PATH_DEV_NVRAM);
	return errno;
}

char *
nvram_get(const char *name)
{
	size_t count = strlen(name) + 1;
	char tmp[100], *value;
	unsigned long *off = (unsigned long *) tmp;

	if (nvram_fd < 0)
		if (nvram_init(NULL))
			return NULL;

	if (count > sizeof(tmp)) {
		if (!(off = malloc(count)))
			return NULL;
	}

	/* Get offset into mmap() space */
	strcpy((char *) off, name);

	count = read(nvram_fd, off, count);

	if (count == sizeof(unsigned long))
		value = &nvram_buf[*off];
	else
		value = NULL;

	if (count < 0)
		perror(PATH_DEV_NVRAM);

	if (off != (unsigned long *) tmp)
		free(off);

	return value;
}

int
nvram_getall(char *buf, int count)
{
	int ret;

	if (nvram_fd < 0)
		if ((ret = nvram_init(NULL)))
			return ret;

	if (count == 0)
		return 0;

	/* Get all variables */
	*buf = '\0';

	ret = read(nvram_fd, buf, count);

	if (ret < 0)
		perror(PATH_DEV_NVRAM);

	return (ret == count) ? 0 : ret;
}

static int
_nvram_set(const char *name, const char *value)
{
	size_t count = strlen(name) + 1;
	char tmp[100], *buf = tmp;
	int ret;

	if (nvram_fd < 0)
		if ((ret = nvram_init(NULL)))
			return ret;

	/* Unset if value is NULL */
	if (value)
		count += strlen(value) + 1;

	if (count > sizeof(tmp)) {
		if (!(buf = malloc(count)))
			return -ENOMEM;
	}

	if (value)
		sprintf(buf, "%s=%s", name, value);
	else
		strcpy(buf, name);

	ret = write(nvram_fd, buf, count);

	if (ret < 0)
		perror(PATH_DEV_NVRAM);

	if (buf != tmp)
		free(buf);

	return (ret == count) ? 0 : ret;
}

int
nvram_set(const char *name, const char *value)
{
	 extern struct nvram_convert nvram_converts[];
         struct nvram_convert *v;
         int ret;

         ret = _nvram_set(name, value);

         for(v = nvram_converts ; v->name ; v++) {
                 if(!strcmp(v->name, name)){
                         if(strcmp(v->wl0_name,""))      _nvram_set(v->wl0_name, value);
                         if(strcmp(v->d11g_name,""))     _nvram_set(v->d11g_name, value);
                 }
         }

         return ret;
}

int
nvram_unset(const char *name)
{
	return _nvram_set(name, NULL);
}

int
nvram_commit(void)
{
	int ret;
	
	cprintf("nvram_commit(): start\n");	
	
	if((check_action() == ACT_IDLE) || 
	   (check_action() == ACT_SW_RESTORE) || 
	   (check_action() == ACT_HW_RESTORE)){
		if (nvram_fd < 0)
			if ((ret = nvram_init(NULL)))
				return ret;

		ret = ioctl(nvram_fd, NVRAM_MAGIC, NULL);

		if (ret < 0)
			perror(PATH_DEV_NVRAM);
	
		cprintf("nvram_commit(): end\n");	
	}
	else
		cprintf("nvram_commit():  nothing to do...\n");

	return ret;
}

int file2nvram(char *filename, char *varname) {
   FILE *fp;
   int c,count;
   int i=0,j=0;
   char mem[10000],buf[30000];

   if ( !(fp=fopen(filename,"rb") ))
        return 0;

   count=fread(mem,1,sizeof(mem),fp);
   fclose(fp);
   for (j=0;j<count;j++) {
        if  (i > sizeof(buf)-3 )
                break;
        c=mem[j];
        if (c >= 32 && c <= 126 && c != '\\' && c != '~')  {
                buf[i++]=(unsigned char) c;
        } else if (c==0) {
		buf[i++]='~';
        } else {
                buf[i++]='\\';
                sprintf(buf+i,"%02X",c);
                i+=2;
        }
   }
   if (i==0) return 0;
   buf[i]=0;
   //fprintf(stderr,"================ > file2nvram %s = [%s] \n",varname,buf); 
   nvram_set(varname,buf);
   //nvram_commit(); //Barry adds for test
}

int nvram2file(char *varname, char *filename) {
   FILE *fp;
   int c,tmp;
   int i=0,j=0;
   char *buf;
   char mem[10000];
   
   if ( !(fp=fopen(filename,"wb") ))
        return 0;
        
   buf=strdup(nvram_safe_get(varname));
   //fprintf(stderr,"=================> nvram2file %s = [%s] \n",varname,buf);
   while (  buf[i] && j < sizeof(mem)-3 ) {
        if (buf[i] == '\\')  {
                i++;
                tmp=buf[i+2];
                buf[i+2]=0;
                sscanf(buf+i,"%02X",&c);
                buf[i+2]=tmp;
                i+=2;
                mem[j]=c;j++;
        } else if (buf[i] == '~') {
		mem[j]=0;j++;
		i++;
        } else {
                mem[j]=buf[i];j++;
                i++;
        }       
   }
   if (j<=0) return j;
   j=fwrite(mem,1,j,fp);
   fclose(fp);
   free(buf);
   return j;
}  

int
check_action(void)
{
	char buf[80] = "";
	
	if(file_to_buf(ACTION_FILE, buf, sizeof(buf))){
		if(!strcmp(buf, "ACT_TFTP_UPGRADE")){
			cprintf("Upgrading from tftp now, quiet exit....\n");
			return ACT_TFTP_UPGRADE;
		}
		else if(!strcmp(buf, "ACT_WEBS_UPGRADE")){
			cprintf("Upgrading from web (https) now, quiet exit....\n");
			return ACT_WEBS_UPGRADE;
		}
		else if(!strcmp(buf, "ACT_WEB_UPGRADE")){
			cprintf("Upgrading from web (http) now, quiet exit....\n");
			return ACT_WEB_UPGRADE;
		}
		else if(!strcmp(buf, "ACT_SW_RESTORE")){
			cprintf("Receive restore command from web, quiet exit....\n");
			return ACT_SW_RESTORE;
		}
		else if(!strcmp(buf, "ACT_HW_RESTORE")){
			cprintf("Receive restore commond from resetbutton, quiet exit....\n");
			return ACT_HW_RESTORE;
		}
	}
	//fprintf(stderr, "Waiting for upgrading....\n");
	return ACT_IDLE;
}

int
file_to_buf(char *path, char *buf, int len)
{
	FILE *fp;

	memset(buf, 0 , len);

	if ((fp = fopen(path, "r"))) {
		fgets(buf, len, fp);
		fclose(fp);
		return 1;
	}

	return 0;
}
