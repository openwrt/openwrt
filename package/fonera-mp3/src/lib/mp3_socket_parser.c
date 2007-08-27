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
#include <stdlib.h>

#include "mp3.h"

#define TOKEN_MAX 		16
#define TOKEN_SIZE 		256

static unsigned char mp3_parser_tokens[TOKEN_MAX][TOKEN_SIZE];

int mp3_parser_tokenize(unsigned char *in){
	int i = 0;
	char *token = in;
	char *tmp;
	do {
		tmp = strstr(token, " ");
		if(tmp){
			*tmp = '\0';
			strcpy(mp3_parser_tokens[i], token);
			tmp++;
			token = tmp;				
		} else {
			strcpy(mp3_parser_tokens[i], token);
		};
		i++;
	}while((i < TOKEN_MAX) && (tmp));
	return i;
};

extern int state_current;
void mp3_parser_incoming(unsigned char *in, unsigned char *out){
	int c =	mp3_parser_tokenize(in);
	int ret = 0;
	int t1;
	if(c){
		printf("Parsing command from frontend app -> %s ---  %d tokens\n", in, c);
		if((!strcmp(mp3_parser_tokens[0], "PLAY")) && (c == 2)){
			state_event(MP3_EVENT_FILE, state_new_event(mp3_parser_tokens[1], 0));
			ret = 1;
		} else if((!strcmp(mp3_parser_tokens[0], "STREAM")) 
				&& (c == 3)){
			if(!strcmp(mp3_parser_tokens[1], "pls")){
				state_event(MP3_EVENT_STREAM, state_new_event(mp3_parser_tokens[2], STREAM_PLS));
				ret = 1;
			} else if(!strcmp(mp3_parser_tokens[1], "url")){
				state_event(MP3_EVENT_STREAM, state_new_event(mp3_parser_tokens[2], STREAM_URL));
				ret = 1;
			}	
		} else if((!strcmp(mp3_parser_tokens[0], "VOLUME")) 
				&& (c == 2)){
			t1 = atoi(mp3_parser_tokens[1]);
			state_generic_event(MP3_EVENT_GENERIC_VOLUME, t1, NULL);
			ret = 1;
		} else if((!strcmp(mp3_parser_tokens[0], "STOP")) 
				&& (c == 1)){
			state_event(MP3_EVENT_STOP, NULL);
			ret = 1;
		} else if((!strcmp(mp3_parser_tokens[0], "STATE"))
				&& (c == 1)){
			state_generic_event(MP3_EVENT_GENERIC_STATE, 0, out);
			return;
		} else if((!strcmp(mp3_parser_tokens[0], "BASS")) 
				&& (c == 2)){
			t1 = atoi(mp3_parser_tokens[1]);
			state_generic_event(MP3_EVENT_GENERIC_BASS, t1, NULL);
			ret = 1;
		} 	
		if(ret){
			sprintf(out, "OK\n");
			printf("Command parsed ok.\n");
		} else {
			
			sprintf(out, "ERROR\n");
			printf("Command parsed with error.\n");
		};
	} else {
		printf("Got command from frontend with 0 tokens.\n");
	};
};


