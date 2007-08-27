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

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <stdarg.h>
#include <fcntl.h>
#include "mp3.h"

#define SOCKET_PATH "/tmp/foxmp3"

typedef struct _MP3_NIX_SOCKET {
	fd_set master;
	fd_set clients;
	int max;
	int listener;
} MP3_NIX_SOCKET;

static MP3_NIX_SOCKET mp3_nix_socket;

int mp3_nix_socket_setup(void){
	struct sockaddr_un myaddr;
	int yes=1; 	
	int len;
	FD_ZERO(&mp3_nix_socket.master); 
	FD_ZERO(&mp3_nix_socket.clients);
	
	if ((mp3_nix_socket.listener = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return MP3_ERROR;
	}
	if (setsockopt(mp3_nix_socket.listener, SOL_SOCKET, 
				SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		return MP3_ERROR;
	}
	myaddr.sun_family = AF_UNIX;
	strcpy(myaddr.sun_path, SOCKET_PATH);
	unlink(myaddr.sun_path);
	len = strlen(myaddr.sun_path) + sizeof(myaddr.sun_family);
	if (bind(mp3_nix_socket.listener, (struct sockaddr *)&myaddr, len) == -1) {
		perror("bind");
		return MP3_ERROR;
	}
	if (listen(mp3_nix_socket.listener, 3) == -1) {
		perror("listen");
		return MP3_ERROR;
	}
	FD_SET(mp3_nix_socket.listener, &mp3_nix_socket.master);
	mp3_nix_socket.max = mp3_nix_socket.listener;

	return MP3_OK;
};

int mp3_nix_socket_handle(void){
	struct sockaddr_un remoteaddr; 
	socklen_t addrlen;
	int i;
	int newfd; 
	char buf[1024]; 
	int nbytes;	
	char buf_out[1024];
	struct timeval tv;
	
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	mp3_nix_socket.clients = mp3_nix_socket.master;
	
	if (select(mp3_nix_socket.max + 1, &mp3_nix_socket.clients,
				NULL, NULL, &tv) == -1) {
		// sometimes the select is interrupted, because of the alarm signal used by the playtime counter
		//perror("error whilst selecting socket");
		return MP3_ERROR;
	}
	for(i = 0; i <= mp3_nix_socket.max; i++) {
		if (FD_ISSET(i, &mp3_nix_socket.clients)) { 
			if (i == mp3_nix_socket.listener) {
				addrlen = sizeof(remoteaddr);
				if ((newfd = accept(mp3_nix_socket.listener, 
							(struct sockaddr *)&remoteaddr, 
							&addrlen)) == -1) {
					perror("error whilst accepting socket");
					return MP3_ERROR;
				} else {
					FD_SET(newfd, &mp3_nix_socket.master); 
					if (newfd > mp3_nix_socket.max) { 
						mp3_nix_socket.max = newfd;
					}
					fcntl(newfd, F_SETFL, O_NONBLOCK);
					printf("New socket client on %d\n", newfd);
				}
			} else {
				if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
					if (nbytes == 0) {
						printf("selectserver: socket hung up %d\n", i);
						close(i); 
						FD_CLR(i, &mp3_nix_socket.master);
					} else {
						printf("error whilst receiving socket %d\n", i);
					}
				} else {
					buf[nbytes] = '\0';
					printf("Got data : %s\n", buf);
					mp3_parser_incoming(buf,buf_out);
					if(*buf_out != '\0'){
						send(i, buf_out, strlen(buf_out), 0);
					}
				}
			}
		}	
	}
	return MP3_OK;
}

void mp3_nix_socket_write(unsigned char *data, ...){
	unsigned int i;
	unsigned char t[2048];	
	va_list ap;
	
	// clear possible dead sockets
	mp3_nix_socket_handle();
	memset(t, 0, 2048);	
	va_start(ap, data);
  	vsprintf(t, data, ap);
   	va_end(ap);
	printf("Sending data --> %s\n", t);	
	for(i = 0; i <= mp3_nix_socket.max; i++) {
		if (FD_ISSET(i, &mp3_nix_socket.master)) { 
			if (i != mp3_nix_socket.listener) {
				printf("Sending on socket %d\n", i);
				send(i, t, strlen(t), 0);
			}
		}	
	}
	printf("Finished sending\n");
}

int mp3_nix_socket_cleanup(void){
	return MP3_OK;
};

