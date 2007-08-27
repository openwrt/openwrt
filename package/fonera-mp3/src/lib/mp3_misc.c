/*
* FOXMP3 
* Copyright (c) 2006 acmesystems.it - john@acmesystems.it
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307USA
*
* Feedback, Bugs...  info@acmesystems.it
*
*/ 


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mp3.h"

#define TMP_PLS_NAME "/var/tmp.pls"

char* mp3_shell_run(char *filename, char **args, char *buffer, int length){
  	int fd1[2], fd2[2], l;
	if((pipe(fd1) !=0)  || (pipe(fd2)!=0)){
		return NULL;
	}
	if (fork() == 0){
		close(fd1[1]);
		close(fd2[0]);
		if ((dup2(fd1[0], STDIN_FILENO) == -1) 
			|| (dup2(fd2[1], STDOUT_FILENO) == -1)){
			exit(0);
		}
		close(fd1[0]);
		close(fd2[1]);
		execvp(filename, args);
        	printf("ERROR running : %s\n", filename);
		exit(0);
	}
	memset(buffer,'\0',length);
	close(fd1[0]);
	close(fd2[1]);
	close(fd1[1]);
	wait(NULL);
	if((l = read(fd2[0], buffer, length -1)) == -1){
		printf("read failed");
		return NULL;
	}
	buffer[l] = '\0';
	close (fd2[2]);
	return buffer;
}

int mp3_pls_get_info(unsigned char *pls_url, unsigned char *url,
		unsigned char *path, unsigned int *port){
	int ret = MP3_ERROR;
	char *exec_args[6];
	int i;
	remove(TMP_PLS_NAME);
	for (i = 0; i < 5; i++){
		exec_args[i] = malloc(2048);
	}
	exec_args[0][0] = '\0';
	strcpy(exec_args[1], "wget");
	strcpy(exec_args[2], pls_url);
	strcpy(exec_args[3], "-O");
	strcpy(exec_args[4], TMP_PLS_NAME);
	exec_args[5] = NULL;
	printf("Getting pls file --> %s \n", exec_args[2]);  
	if(mp3_shell_run("wget", &exec_args[1], 
				exec_args[0], 2048)){
		struct stat s;
		stat(TMP_PLS_NAME, &s);
		if(s.st_size > 0){
			FILE *fp = fopen(TMP_PLS_NAME, "r");
			if(fp > 0){
				unsigned char *data = malloc(2048);
				*url = '\0';
				while((!*url) && (!feof(fp))){
					if(fgets(data, 2048, fp) != NULL){
						if(strstr(data, "File")){
							unsigned char *t = strstr(data, "=");
							if(t){
								t++;
								if(mp3_stream_parse_url(t, url, 
									path, port) != MP3_OK){
									*url = '\0';
								}
							}
						}
					}
				}
				fclose(fp);
				free(data);
				if(*url){
					ret = MP3_OK;
				}
			}
		}
	} else {
		printf("WGET error\n");
	}
	for (i = 0; i < 5; i++){
		free(exec_args[i]);
	}
	if(ret == MP3_OK){
		printf("Found file valid file in pls\n");
	} else {
		printf("Error whilst parsing pls\n");	
	}
	return ret;
}



