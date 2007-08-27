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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/poll.h>

#include "mp3.h"

#define SOCKET_PORT 	369
#define BACKLOG 	10

typedef struct _MP3_TCP_SOCKET {
	fd_set master;
	fd_set clients;
	int max;
	int listener;
} MP3_TCP_SOCKET;

static MP3_TCP_SOCKET mp3_tcp_socket;

int mp3_tcp_socket_setup(void){
	struct sockaddr_in myaddr;
	int yes=1; 	
	FD_ZERO(&mp3_tcp_socket.master); 
	FD_ZERO(&mp3_tcp_socket.clients);
	
	if ((mp3_tcp_socket.listener = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return MP3_ERROR;
	}
	if (setsockopt(mp3_tcp_socket.listener, SOL_SOCKET, 
				SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		return MP3_ERROR;
	}
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(SOCKET_PORT);
	myaddr.sin_addr.s_addr = INADDR_ANY; 
    	memset(&(myaddr.sin_zero), '\0', 8);
	if (bind(mp3_tcp_socket.listener, (struct sockaddr *)&myaddr, sizeof(struct sockaddr)) == -1) {
		perror("bind");
		return MP3_ERROR;
	}
	if (listen(mp3_tcp_socket.listener, 3) == -1) {
		perror("listen");
		return MP3_ERROR;
	}
	FD_SET(mp3_tcp_socket.listener, &mp3_tcp_socket.master);
	mp3_tcp_socket.max = mp3_tcp_socket.listener;
	printf("Started tcp socket on port %d\n", SOCKET_PORT);
	return MP3_OK;
};

int mp3_tcp_socket_handle(void){
	struct sockaddr_in remoteaddr; 
	socklen_t addrlen;
	int i;
	int newfd; 
	char buf[1024]; 
	int nbytes;	
	char buf_out[1024];
	struct timeval tv;
	
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	mp3_tcp_socket.clients = mp3_tcp_socket.master;
	
	if (select(mp3_tcp_socket.max + 1, &mp3_tcp_socket.clients,
				NULL, NULL, &tv) == -1) {
		return MP3_ERROR;
	}
	for(i = 0; i <= mp3_tcp_socket.max; i++) {
		if (FD_ISSET(i, &mp3_tcp_socket.clients)) { 
			if (i == mp3_tcp_socket.listener) {
				addrlen = sizeof(remoteaddr);
				if ((newfd = accept(mp3_tcp_socket.listener, 
							(struct sockaddr *)&remoteaddr, 
							&addrlen)) == -1) {
					perror("error whilst accepting socket");
					return MP3_ERROR;
				} else {
					printf("New TCP connection from %s\n", 
							inet_ntoa(remoteaddr.sin_addr));
					FD_SET(newfd, &mp3_tcp_socket.master); 
					if (newfd > mp3_tcp_socket.max) { 
						mp3_tcp_socket.max = newfd;
					}
				}
			} else {
				if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
					if (nbytes == 0) {
						perror("selectserver: socket hung up\n");
					} else {
						perror("error whilst receiving socket");
					}
					close(i); 
					FD_CLR(i, &mp3_tcp_socket.master);
				} else {
					buf[nbytes] = '\0';
					printf("Got data : %s\n", buf);
					mp3_parser_incoming(buf,buf_out);
					if(*buf_out != '\0'){
						send(i, buf_out, strlen(buf_out), 0);
					}
					close(i); 
					FD_CLR(i, &mp3_tcp_socket.master);
				}
			}
		}	
	}
	return MP3_OK;
};

int mp3_tcp_socket_cleanup(void){
	return MP3_OK;
};

