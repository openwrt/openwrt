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
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SOCKET_PORT 369


void print_usage(void){
	printf("mp3_play_tcp IP COMANND PARAMETERS\n");
	printf("    Commands :\n");
	printf("  	PLAY filename\n");	
	printf("	STREAM url [URL OF STREAM]\n");	
	printf("	STREAM pls [URL PLS FILE]\n");	
	printf("	VOLUME [0-255]\n");	
	printf("	STOP\n");	
	printf("	STATE\n");
	printf("	BASS [0-255]\n");
}

void issue_command(unsigned char *str, unsigned char *ip){
	int s, t, len;
	struct sockaddr_in remote;
	struct hostent *he;

	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	
	printf("Connecting to FOXMP3 on IP/DNS : %s  ...\n", ip);
	if((he=gethostbyname(ip)) == NULL) { 
		herror("gethostbyname");
		exit(1);
	}
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	remote.sin_family = AF_INET;
	remote.sin_port = htons(SOCKET_PORT);
	remote.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(remote.sin_zero), '\0', 8);

	if (connect(s, (struct sockaddr *)&remote,
			sizeof(struct sockaddr)) == -1) {
		perror("connect");
		exit(1);
	}
	printf("Connected ...\n\nSending command -> \n%s\n\n", str);
	if (send(s, str, strlen(str), 0) == -1) {
		perror("send");
		exit(1);	
	}
	if ((t=recv(s, str, 2048, 0)) > 0) {
		str[t] = '\0';
		printf("The answer was -> \n%s\n", str);
	} else {
		if (t < 0){
			perror("recv");
		} else {
			printf("Server closed connection\n");
		};
		exit(1);
	}
	close(s);
}

int main(int argc, char **argv){
	unsigned char buffer[2048];
	buffer[0] = '\0';
	if(argc > 2){
		if(((!strcmp(argv[2], "STOP")) || (!strcmp(argv[2], "STATE"))) 
				&& (argc == 3)){
			sprintf(buffer, "%s", argv[2]);
		} else if(((!strcmp(argv[2], "PLAY")) || (!strcmp(argv[2], "VOLUME")) 
			|| (!strcmp(argv[2], "BASS"))) && (argc == 4)){
			sprintf(buffer, "%s %s", argv[2], argv[3]);	
		} else if((!strcmp(argv[2], "STREAM")) && (argc == 5)
				&& ((!strcmp(argv[3], "url")) || (!strcmp(argv[3], "pls")))){
			sprintf(buffer, "%s %s %s", argv[2], argv[3], 
					argv[4]);	
		}
	};
	if(buffer[0] != '\0'){
		issue_command(buffer, argv[1]);
	} else {
		print_usage();
	};
	return 0;
}
