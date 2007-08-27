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

#define MAX_PACKET_SIZE 		1500	
#define MAX_BUFFER_SIZE 		(64*2048)	
#define MAX_BUFFER_ERROR		128

#define STREAM_URL			0
#define STREAM_PLS			1
int mp3_stream_setup(unsigned char *url, unsigned int type, unsigned char *ip, 
		unsigned char *path, unsigned int *port);
int mp3_stream_handle(void);
int mp3_stream_cleanup(void);
int mp3_stream_parse_url(unsigned char *url, unsigned char *ip, 
	unsigned char *path, unsigned int *port);
