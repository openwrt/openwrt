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

#include "mp3.h"


#define MP3_PRE_BUFFER_COUNT	((128 * 1024) / MP3_CHUNK_SIZE)


typedef struct _MP3_FILE {
	unsigned char filename[2048];
	MP3_DATA mp3_data;
	FILE *fd; 	
	unsigned char file_end_found;
	MP3_FILE_ID3 *id3;
} MP3_FILE;

static MP3_FILE mp3_file;

void mp3_load_id3(FILE *fp){
	unsigned char *buf = malloc(1024);
	
	mp3_file.id3->album[0] = '\0';
	mp3_file.id3->artist[0] = '\0';
	mp3_file.id3->track[0] = '\0';
	
	
	fgets(buf, 1024, fp);
	if( (buf[0] == 'I') &&
		(buf[1] == 'D') &&
		(buf[2] == '3')){
		unsigned int id3_size;
		unsigned int i;
		unsigned int id3_version = buf[3];
		id3_version <<= 8;
		id3_version += buf[4];
		
		id3_size = 0;
		
		for(i = 0; i<4; i++){
			id3_size += buf[5 + i];
			id3_size <<= 7;
		};		
		if(id3_version>>8 == 3){
			unsigned int id3_pos = 10;
			unsigned int id3_tag_size;
			unsigned char tag_name[5];
			unsigned char tag_data[257];
			tag_name[4] = '\0';
			tag_data[256] = '\0';
			unsigned int count = 0;
			while(count < 10){
				strncpy(tag_name, &buf[id3_pos], 4);
				id3_tag_size = buf[id3_pos + 4];
				id3_tag_size <<= 8;
				id3_tag_size = buf[id3_pos + 5];
				id3_tag_size <<= 8;
				id3_tag_size = buf[id3_pos + 6];
				id3_tag_size <<= 8;
				id3_tag_size = buf[id3_pos + 7];
				if(id3_tag_size == 0){
					break;
				};
				if(id3_tag_size > 256){
					memcpy(&tag_data[0], &buf[id3_pos + 11] , 256);
				} else {
					memcpy(&tag_data[0], &buf[id3_pos + 11] , 
							id3_tag_size -1);
					tag_data[id3_tag_size-1] = '\0';
				};
				id3_pos += 10 + id3_tag_size;
				if(strcmp(tag_name, "TPE1") == 0){
					strncpy(mp3_file.id3->artist, tag_data, 255);
				};
				if(strcmp(tag_name, "TALB") == 0){
					strncpy(mp3_file.id3->album, tag_data, 255);
				};
				if(strcmp(tag_name, "TIT2") == 0){
					strncpy(mp3_file.id3->track, tag_data, 255);
				};
				if(id3_pos >= id3_size){
					break;
				};
				count ++;
			};
		};
		printf("ID3 tag found Version 2.%d.%d / size %d\n%s -- %s -- %s\n", 
				id3_version>>8, 
				id3_version&0xff, 
				id3_size,
				mp3_file.id3->artist, 
				mp3_file.id3->album,
				mp3_file.id3->track);
	} else {
		printf("No ID3 Tag was found\n");
	};
	free(buf);
};


int mp3_file_setup(unsigned char *filename, MP3_FILE_ID3 *id3){
	unsigned int i;
	mp3_file.id3 = id3;
	mp3_file.file_end_found = 0;
	strcpy(mp3_file.filename, filename);		
	mp3_file.fd = fopen(mp3_file.filename, "rb");
	if(!mp3_file.fd){
		mp3_file.fd = 0;
		printf("error opening file %s\n", mp3_file.filename);
		return MP3_ERROR;
	};
	printf("File %s opened Ok\n", mp3_file.filename);
	printf("Reading id3 tag\n");
	mp3_load_id3(mp3_file.fd);
	fseek(mp3_file.fd, 0, SEEK_SET);
	
	mp3_reset();
	
	printf("Buffering MP3 Data\n");
	mp3_file.mp3_data.state = MP3_BUFFERING;
	for(i = 0; i < MP3_PRE_BUFFER_COUNT - 1; i++){
		fread(mp3_file.mp3_data.mp3, MP3_CHUNK_SIZE, 1, mp3_file.fd);
		mp3_file.mp3_data.state = MP3_PLAYING;
		mp3_send_data_to_buffer(mp3_file.mp3_data);
	};

	printf("Starting to play file : %s\n", mp3_file.filename);
	return MP3_OK;	
};

int mp3_file_handle(void){
	unsigned char transmit_success = 1;
	if (!feof(mp3_file.fd)) {
		fread(mp3_file.mp3_data.mp3, MP3_CHUNK_SIZE, 1, mp3_file.fd);
		transmit_success = 0;
		while(!transmit_success){
			if(!mp3_send_data_to_buffer(mp3_file.mp3_data)){
				usleep(1);
				transmit_success = 0;
			} else {
				transmit_success = 1;
			};	
		};
		return MP3_OK;
	} else {
		if(!mp3_file.file_end_found){
			mp3_file.mp3_data.state = MP3_BUFFER_FINISHED;
			mp3_send_data_to_buffer(mp3_file.mp3_data);
			printf("File end reached. Wait till kernel buffer has cleared.\n");
			mp3_file.file_end_found = 1;
		};
		if(!mp3_buffer_finished()){
			return MP3_OK;
		} else {
			return MP3_END;
		};
	};
};
	
int mp3_file_cleanup(void){
	if(mp3_file.fd){
		fclose(mp3_file.fd);
	};
	return MP3_OK;
};
