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
#include <unistd.h>
#include <sys/poll.h>
#include <signal.h>
#include "mp3.h"

typedef struct _MP3_STATE {
	struct {
		unsigned char volume;
		unsigned char bass;
		unsigned char treble;
	} decoder;
	struct {
		unsigned char name[2048];
		MP3_FILE_ID3 id3;
	} file;
	struct {
		unsigned char url[2048];
		unsigned char path[256];
		unsigned int port;
	} stream;
	MP3_PLAYTIME playtime;
	} MP3_STATE;

static MP3_STATE mp3_state;

void state_startup_enter(int state_last, int event, EVENT_PARAM *param){
	mp3_state.decoder.volume = 0x30;
	mp3_state.decoder.bass = 1;
	mp3_state.file.name[0] = '\0';
	mp3_state.stream.url[0] = '\0';
	mp3_state.stream.path[0] = '\0';
	mp3_state.stream.port = 0;
	mp3_playtime_init(&mp3_state.playtime);
	mp3_init();
	mp3_nix_socket_setup();
	mp3_tcp_socket_setup();
};

void state_idle_enter(int state_last, int event, EVENT_PARAM *param){
	mp3_stop();
	mp3_playtime_stop();
};

void state_file_startup_enter(int state_last, int event, EVENT_PARAM *param){
	if(mp3_file_setup(param->text, &mp3_state.file.id3) == MP3_OK){
		strcpy(mp3_state.file.name, param->text);
	} else {
		state_event(MP3_EVENT_ERROR, NULL);
	};
};

void state_file_startup_leave(int state_new, int event){
	if(state_new == MP3_STATE_FILE_HANDLE){
		mp3_play();
		mp3_playtime_start();
		mp3_nix_socket_write("START FILE\n");
	};
};

void state_file_handle_enter(int state_last, int event, EVENT_PARAM *param){
	int ret = mp3_file_handle();
	if(ret == MP3_ERROR){
		state_event(MP3_EVENT_ERROR, NULL);
	};
	if(ret == MP3_END){
		state_event(MP3_EVENT_END, NULL);
	};

};

void state_file_handle_leave(int state_new, int event){
	if(state_new != MP3_STATE_FILE_HANDLE){
		mp3_playtime_stop();
		mp3_file_cleanup();
		mp3_nix_socket_write("STOP FILE\n");
	};
};

void state_stream_startup_enter(int state_last, int event, EVENT_PARAM *param){
	if(mp3_stream_setup(param->text, param->numeric, mp3_state.stream.url, 
			mp3_state.stream.path, &mp3_state.stream.port) != MP3_OK){
		state_event(MP3_EVENT_ERROR, NULL);	
	};
};

void state_stream_startup_leave(int state_new, int event){
	if(state_new == MP3_STATE_STREAM_HANDLE){
		mp3_play();
		mp3_playtime_start();
		mp3_nix_socket_write("START STREAM\n");
	};
};

void state_stream_handle_enter(int state_last, int event, EVENT_PARAM *param){
	if(mp3_stream_handle() == MP3_ERROR){
		state_event(MP3_EVENT_ERROR, NULL);
	}
};

void state_stream_handle_leave(int state_new, int event){
	if(state_new != MP3_STATE_STREAM_HANDLE){
		mp3_stream_cleanup();
		mp3_playtime_stop();
		mp3_nix_socket_write("STOP STREAM\n");
	}
};

extern STATE states[MAX_STATE_COUNT];
void state_error_enter(int state_last, int event, EVENT_PARAM *param){
	if(param){
		printf("Error in state %s -> %s\n", states[state_last].name, param->text);
		mp3_nix_socket_write("ERROR Error in state %s -> %s\n", states[state_last].name, param->text);
	} else {
		printf("Unknown error in state %s\n", states[state_last].name);
	};
};

void state_shutdown_enter(int state_last, int event, EVENT_PARAM *param){
	printf("Entering state SHUTDOWN ...\n");
	printf("Shutting down player ...\n");
	mp3_nix_socket_cleanup();
	mp3_tcp_socket_cleanup();
	printf("Quitting statemachine ...\n");
	exit(0);
};
extern int state_current;
void state_generic_event(unsigned int event, unsigned char in_int, 
		unsigned char *out_uchar){
	switch(event){
	case MP3_EVENT_GENERIC_VOLUME:
		mp3_state.decoder.volume = in_int;
		mp3_set_volume(mp3_state.decoder.volume, 
				mp3_state.decoder.volume);
		mp3_nix_socket_write("VOLUME %d\n", mp3_state.decoder.volume);
		break;
	case MP3_EVENT_GENERIC_BASS:
		mp3_state.decoder.bass = in_int;
		mp3_bass(8, mp3_state.decoder.treble, 
				8, mp3_state.decoder.bass); 
		mp3_nix_socket_write("BASS %d\n", mp3_state.decoder.bass);
		break;
	case MP3_EVENT_GENERIC_STATE:
		out_uchar[0] = '\0';
		sprintf(out_uchar, "%sVOLUME %d\n", 
				out_uchar, mp3_state.decoder.volume);
		sprintf(out_uchar, "%sBASS %d\n", 
				out_uchar, mp3_state.decoder.bass);
		sprintf(out_uchar, "%sTREBLE %d\n", 
				out_uchar, mp3_state.decoder.treble);
		sprintf(out_uchar, "%sPLAYTIME %d\n",
				out_uchar,
				(mp3_state.playtime.playtime_mins * 60) + mp3_state.playtime.playtime_secs);
		switch(state_current){
		case MP3_STATE_IDLE:
			sprintf(out_uchar, "%sSTATE MP3_STATE_IDLE\n", out_uchar);
			break;
		case MP3_STATE_FILE_START:
		case MP3_STATE_FILE_HANDLE:
			sprintf(out_uchar, "%sFILE %s\n", out_uchar, 
					mp3_state.file.name);
			if(strlen(mp3_state.file.id3.artist)){
				sprintf(out_uchar, "%sID3_ARTIST %s\n", 
					out_uchar, 
					mp3_state.file.id3.artist);
			};
			if(strlen(mp3_state.file.id3.album)){
				sprintf(out_uchar, "%sID3_ALBUM %s\n", 
					out_uchar, 
					mp3_state.file.id3.album);
			};
			if(strlen(mp3_state.file.id3.album)){
				sprintf(out_uchar, "%sID3_TRACK %s\n", 
					out_uchar, 
					mp3_state.file.id3.track);
			};
			sprintf(out_uchar, "%sSTATE MP3_STATE_FILE\n", out_uchar);	
			break;
		case MP3_STATE_STREAM_START:
		case MP3_STATE_STREAM_HANDLE:
			sprintf(out_uchar, "%sSTREAM %s:%d%s\n", out_uchar, 
					mp3_state.stream.url, 
					mp3_state.stream.port,
					mp3_state.stream.path);
			sprintf(out_uchar, "%sSTATE MP3_STATE_STREAM\n", out_uchar);	
			break;
		default:
			sprintf(out_uchar, "STATE Unkonwn\n");
			break;
		};
		sprintf(out_uchar, "%sOK\n", out_uchar);
		break;
	case MP3_EVENT_GENERIC_PLAYTIME:
	//	printf("%02d:%02d\n", mp3_state.playtime.playtime_mins, 
	//			mp3_state.playtime.playtime_secs);
		break;
	default:
		break;
	};
};

