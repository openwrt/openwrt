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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/foxmp3"


void print_usage(void){
	printf("mp3_play COMANND PARAMETERS\n");
	printf("    Commands :\n");
	printf("  	PLAY filename\n");	
	printf("	STREAM url [URL OF STREAM]\n");	
	printf("	STREAM pls [URL PLS FILE]\n");	
	printf("	VOLUME [0-255]\n");	
	printf("	STOP\n");	
	printf("	STATE\n");
	printf("	BASS [0-255]\n");
}

void issue_command(unsigned char *str){
	int s, t, len;
	struct sockaddr_un remote;

	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	printf("Connecting to mp3d ...\n");
	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, SOCKET_PATH);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	if (connect(s, (struct sockaddr *)&remote, len) == -1) {
		perror("connect");
		exit(1);
	}
	printf("Connected ...\n\nSending command -> \n%s\n\n", str);
	if (send(s, str, strlen(str), 0) == -1) {
		perror("send");
		exit(1);	
	}
	unsigned char loop = 1;
	while(loop){
		if ((t=recv(s, str, 2048, 0)) > 0) {
			str[t] = '\0';
			printf("The answer was -> \n%s\n", str);
			if((strstr(str, "OK")) || (strstr(str, "ERROR"))){
				loop = 0;
			}
		} else {
			if (t < 0){
				perror("recv");
			} else {
				printf("Server closed connection\n");
			};
		}
	}
	close(s);
}

int main(int argc, char **argv){
	unsigned char buffer[2048];
	buffer[0] = '\0';
	if(argc > 1){
		if(((!strcmp(argv[1], "STOP")) || (!strcmp(argv[1], "STATE"))) 
				&& (argc == 2)){
			sprintf(buffer, "%s", argv[1]);
		} else if(((!strcmp(argv[1], "PLAY")) || (!strcmp(argv[1], "VOLUME")) 
			|| (!strcmp(argv[1], "BASS"))) && (argc == 3)){
			sprintf(buffer, "%s %s", argv[1], argv[2]);	
		} else if((!strcmp(argv[1], "STREAM")) && (argc == 4)
				&& ((!strcmp(argv[2], "url")) || (!strcmp(argv[2], "pls")))){
			sprintf(buffer, "%s %s %s", argv[1], argv[2], 
					argv[3]);	
		}
	};
	if(buffer[0] != '\0'){
		issue_command(buffer);
	} else {
		print_usage();
	};
	return 0;
}
