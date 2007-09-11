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
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/poll.h>

#include "mp3.h"

typedef struct _MP3_STREAM {
	unsigned char buf[MAX_PACKET_SIZE + 1];
	int sockfd;
	unsigned char mp3_buffer[MAX_BUFFER_SIZE];
	unsigned long int mp3_buffer_write_pos;
	unsigned long int mp3_buffer_read_pos;
	unsigned long int mp3_data_in_buffer;
	unsigned char transmit_success;
	unsigned int buffer_error;
	MP3_DATA mp3_data;
	unsigned int numbytes;
	unsigned int metainterval;
} MP3_STREAM;

static MP3_STREAM mp3_stream;

int connect_timeout (int sfd, struct sockaddr *addr, int addrlen, 
			struct timeval *timeout) {
	struct timeval sv;
	int svlen = sizeof sv;
	int ret;

	if (!timeout) {
		return connect (sfd, addr, addrlen);
	};
	if (getsockopt (sfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&sv, &svlen) < 0) {
		return -1;
	};
	if (setsockopt (sfd, SOL_SOCKET, SO_SNDTIMEO, timeout,sizeof *timeout) < 0) {
		return -1;
	};
	ret = connect (sfd, addr, addrlen);
	setsockopt (sfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&sv, sizeof sv);
	    
	return ret;
}

int mp3_stream_parse_url(unsigned char *url, unsigned char *ip, 
	unsigned char *path, unsigned int *port){
	int len = strlen(url) - 1;
	while(((url[len] == '\n')||(url[len] == ' ')) && (len > 0)){
		url[len] = '\0';
		len--;
	};
	ip[0] = '\0';
	printf("Parsing stream url : %s\n", url);
	unsigned char *http = strstr(url, "http://");
	*port = 80;
	if(http){
		url = http + 7; 
		unsigned char *p = strstr(url, ":");
		if(p){
			*p = '\0';
			p ++;
			strcpy(ip, url);
			*port = atoi(p);
		}
		unsigned char *p2 = strstr((p)?(p):(url), "/");
		if(p2){
			strcpy(path, p2);
			*p2 = '\0';
			if(!p){
				strcpy(ip, url);
			}

		} else {
			strcpy(path, "/");
		};
		printf("ip   -> %s\nport -> %d\npath -> %s\n", ip, *port, path);
		return MP3_OK;
	};
	return MP3_ERROR;
};

int mp3_stream_get_url(unsigned char *url, unsigned int type, 
		unsigned char *ip, unsigned int *port, unsigned char *path){
	if(type == STREAM_PLS){
		if(mp3_pls_get_info(url, ip, path, port) == MP3_OK){
			return MP3_OK;
		};
	} else if(type == STREAM_URL){
		if(mp3_stream_parse_url(url, ip, path, port) == MP3_OK){
			return MP3_OK;
		}; 
	};
	return MP3_ERROR;
};

int mp3_stream_setup(unsigned char *url, unsigned int type, unsigned char *ip, 
		unsigned char *path, unsigned int *port){
	struct hostent *he;
	struct sockaddr_in their_addr; 
	unsigned int error = 0;
	if(mp3_stream_get_url(url, type, ip, port, path) == MP3_ERROR){
		return MP3_ERROR;
	};
	
	mp3_stream.mp3_buffer_write_pos = 0;
	mp3_stream.mp3_buffer_read_pos = 0;
	mp3_stream.mp3_data_in_buffer = 0;
	mp3_stream.transmit_success = 1;
	mp3_stream.buffer_error = 0;
	mp3_stream.metainterval = 0;

	mp3_reset();
	
	if ((he=gethostbyname(ip)) == NULL) { 
		perror("Error in gethostbyname. Wrong url/ip ?");
		return MP3_ERROR;
	}
	if ((mp3_stream.sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Error opening stream socket");
		return MP3_ERROR;
	}
	
	their_addr.sin_family = AF_INET; 
	their_addr.sin_port = htons(*port); 
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(their_addr.sin_zero), '\0', 8);
	
	struct timeval tv;
	tv.tv_sec = 4;
	tv.tv_usec = 0;
	
	if (connect_timeout(mp3_stream.sockfd, (struct sockaddr *)&their_addr, 
				sizeof(struct sockaddr), &tv) == -1) {
		perror("connect");
		return MP3_ERROR;
	}
	
	unsigned char icy_request[1024];
	sprintf(icy_request, 
			"GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: A.LP-MP3\r\nAccept: */*\r\nicy-metadata:0\r\n\r\n", 
			path, 
			ip);
	printf("Sending request :\n%s\n", icy_request);
	send(mp3_stream.sockfd, icy_request, strlen(icy_request), 0);
	mp3_stream.numbytes = 0;
	while(mp3_stream.numbytes < MAX_PACKET_SIZE-1) {
		if ((mp3_stream.numbytes += recv(mp3_stream.sockfd, &mp3_stream.buf[mp3_stream.numbytes], MAX_PACKET_SIZE - 1 - mp3_stream.numbytes, 0)) == -1) {
			perror("recv");
			return MP3_ERROR;
		}
	}
	mp3_stream.buf[mp3_stream.numbytes] = '\0';
	printf("numbytes = %d\n", mp3_stream.numbytes);
	unsigned char *p = strstr(mp3_stream.buf, "\r\n\r\n");
	if(p) {
		*p = '\0';
		p += 4;
	} else {
		printf("funky p error in stream.c\n");
	}
	printf("Received: \n%s\n", mp3_stream.buf);
	if(((unsigned char*)strstr(mp3_stream.buf, "ICY 200 OK") != mp3_stream.buf) && 
		((unsigned char*)strstr(mp3_stream.buf, "HTTP/1.1 200 OK") != mp3_stream.buf) &&
		((unsigned char*)strstr(mp3_stream.buf, "HTTP/1.0 200 OK") != mp3_stream.buf)) {
		return MP3_ERROR;
	};
	int p_buf = p - mp3_stream.buf; 
	unsigned char *p2;
	p2 = strstr(mp3_stream.buf, "icy-metaint:");
	if(p2){
		p2 = strstr(p2, ":");
		p2++;
		unsigned char *p3 = strstr(p2, "\r");
		*p3 = '\0';
		mp3_stream.metainterval = atoi(p2);
		printf("META INT == %d\n", mp3_stream.metainterval);
	}
	
	printf("starting to buffer\n");
	memcpy(&mp3_stream.mp3_buffer[mp3_stream.mp3_buffer_write_pos], 
				p, p_buf);
	mp3_stream.mp3_buffer_write_pos += p_buf;
	mp3_stream.mp3_data_in_buffer += p_buf;

	while(mp3_stream.mp3_data_in_buffer + (unsigned long int)MAX_PACKET_SIZE 
			< (unsigned long int)MAX_BUFFER_SIZE){
		if ((mp3_stream.numbytes=recv(mp3_stream.sockfd, mp3_stream.buf, 
						MAX_PACKET_SIZE-1, 0)) == -1) {
			perror("disconnected");
			printf("disconntected\n");
			return MP3_ERROR;
		}

		if(mp3_stream.numbytes == 0){
			sleep(1);
			if(++error > 3){
				perror("disconnected");
				printf("disconntected\n");
				return MP3_ERROR;
			}			
		}

		memcpy(&mp3_stream.mp3_buffer[mp3_stream.mp3_buffer_write_pos], 
				mp3_stream.buf, mp3_stream.numbytes);
		mp3_stream.mp3_buffer_write_pos += mp3_stream.numbytes;
		mp3_stream.mp3_data_in_buffer += mp3_stream.numbytes;
		printf("%ld  ", mp3_stream.mp3_data_in_buffer);
		fflush(stdout);
		
	};
	printf("\n");
	mp3_stream.mp3_data.state = MP3_PLAYING;
	while(mp3_stream.mp3_data_in_buffer >= 2 * MP3_CHUNK_SIZE){
		memcpy(mp3_stream.mp3_data.mp3, 
				&mp3_stream.mp3_buffer[mp3_stream.mp3_buffer_read_pos], 
				MP3_CHUNK_SIZE);
		mp3_send_data_to_buffer(mp3_stream.mp3_data);
		mp3_stream.mp3_buffer_read_pos += MP3_CHUNK_SIZE;
		mp3_stream.mp3_data_in_buffer -= MP3_CHUNK_SIZE;
	};

	printf("Starting to play stream\n");
	return MP3_OK;
}
	
static int max_recv_errors = 10;
int mp3_stream_handle(void){
	if(MAX_BUFFER_SIZE >= mp3_stream.mp3_data_in_buffer + MAX_PACKET_SIZE){
		struct pollfd ufds;
		ufds.fd = mp3_stream.sockfd;
		ufds.events = POLLIN|POLLHUP;
		
		if(poll(&ufds, 1, 2000) > 0){
			max_recv_errors = 10;
			if ((mp3_stream.numbytes=recv(mp3_stream.sockfd, mp3_stream.buf, MAX_PACKET_SIZE-1, 0)) == -1) {
				perror("recv");
			}
			if((mp3_stream.numbytes != EAGAIN)&& (mp3_stream.numbytes != -1)){
				if(mp3_stream.mp3_buffer_write_pos + mp3_stream.numbytes <= MAX_BUFFER_SIZE){
					memcpy(&mp3_stream.mp3_buffer[mp3_stream.mp3_buffer_write_pos],
							mp3_stream.buf, mp3_stream.numbytes);
					mp3_stream.mp3_buffer_write_pos += mp3_stream.numbytes;
					mp3_stream.mp3_data_in_buffer += mp3_stream.numbytes;
					if(mp3_stream.mp3_buffer_write_pos == MAX_BUFFER_SIZE){
						mp3_stream.mp3_buffer_write_pos = 0;
					};
				} else {
					unsigned int buffer_offset = MAX_BUFFER_SIZE - mp3_stream.mp3_buffer_write_pos;
					memcpy(&mp3_stream.mp3_buffer[mp3_stream.mp3_buffer_write_pos], 
							mp3_stream.buf, buffer_offset);
					mp3_stream.mp3_buffer_write_pos = 
							mp3_stream.numbytes - buffer_offset;
					memcpy(&mp3_stream.mp3_buffer[0], &mp3_stream.buf[buffer_offset],
							mp3_stream.mp3_buffer_write_pos);
					mp3_stream.mp3_data_in_buffer += mp3_stream.numbytes;
				};
			};
		} else {
			max_recv_errors--;
			if(max_recv_errors == 0){
				printf("recv error\n");
				return MP3_ERROR;
			};
		};
	} 
	
	if(mp3_stream.mp3_data_in_buffer < MP3_CHUNK_SIZE){
		printf("radio_buffer is empty\n");
		mp3_stream.buffer_error ++;
		if(mp3_stream.buffer_error > MAX_BUFFER_ERROR){
			return MP3_ERROR;
		};			
	} else {
		mp3_stream.buffer_error = 0;
		do{
			if(mp3_stream.transmit_success){
				if(MAX_BUFFER_SIZE >= mp3_stream.mp3_buffer_read_pos + MP3_CHUNK_SIZE){
					memcpy(mp3_stream.mp3_data.mp3, 
						&mp3_stream.mp3_buffer[mp3_stream.mp3_buffer_read_pos], MP3_CHUNK_SIZE);
					mp3_stream.mp3_buffer_read_pos += MP3_CHUNK_SIZE;
					mp3_stream.mp3_data_in_buffer -= MP3_CHUNK_SIZE;
					if(mp3_stream.mp3_buffer_read_pos == MAX_BUFFER_SIZE){
						mp3_stream.mp3_buffer_read_pos = 0;
					};
					
				} else {
					unsigned int buffer_offset = MAX_BUFFER_SIZE - mp3_stream.mp3_buffer_read_pos;
					memcpy(mp3_stream.mp3_data.mp3, 
							&mp3_stream.mp3_buffer[mp3_stream.mp3_buffer_read_pos],
							buffer_offset);
					mp3_stream.mp3_buffer_read_pos = MP3_CHUNK_SIZE - buffer_offset;
					memcpy(&mp3_stream.mp3_data.mp3[buffer_offset], mp3_stream.mp3_buffer, 
							mp3_stream.mp3_buffer_read_pos);
				};
			} 
			if(!mp3_send_data_to_buffer(mp3_stream.mp3_data)){
				mp3_stream.transmit_success = 0;
			} else {
				mp3_stream.transmit_success = 1;
			};	
		} while((mp3_stream.transmit_success)&&(mp3_stream.mp3_data_in_buffer > MP3_CHUNK_SIZE));
	};
	return MP3_OK;
};
	
int mp3_stream_cleanup(void){
	close(mp3_stream.sockfd);
	return MP3_OK;
}
