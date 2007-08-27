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
#include "lib/mp3.h"

STATE states[MAX_STATE_COUNT];
static EVENT events[MAX_EVENT_COUNT];
static TRANSITION transitions[MAX_TRANSITION_COUNT];
static int transition_count = 0;
int state_current = MP3_STATE_STARTUP;
static int state_previous = MP3_STATE_NONE;
static int shutdown = 0;

void state_setup(void){
	EVENT_ADD(MP3_EVENT_TIMEOUT, "Timeout");
	EVENT_ADD(MP3_EVENT_FILE, "File");
	EVENT_ADD(MP3_EVENT_STREAM, "Stream"); 
	EVENT_ADD(MP3_EVENT_STOP, "Stop");
	EVENT_ADD(MP3_EVENT_ERROR, "Error");
	EVENT_ADD(MP3_EVENT_SHUTDOWN, "Shutdown");
	EVENT_ADD(MP3_EVENT_END, "End");

	STATE_ADD(MP3_STATE_STARTUP, "Startup", state_startup_enter, NULL);
	STATE_ADD(MP3_STATE_IDLE, "Idle", state_idle_enter, NULL);
	STATE_ADD(MP3_STATE_FILE_START, "File startup", state_file_startup_enter, state_file_startup_leave);
	STATE_ADD(MP3_STATE_FILE_HANDLE, "File handle", state_file_handle_enter, state_file_handle_leave);
	STATE_ADD(MP3_STATE_STREAM_START, "Stream start", state_stream_startup_enter, state_stream_startup_leave);
	STATE_ADD(MP3_STATE_STREAM_HANDLE, "Stream handle", state_stream_handle_enter, state_stream_handle_leave);
	STATE_ADD(MP3_STATE_ERROR, "Error", state_error_enter, NULL);
	STATE_ADD(MP3_STATE_SHUTDOWN, "Shutdown", state_shutdown_enter, NULL);

	TRANSITION_ADD(MP3_STATE_STARTUP, MP3_EVENT_TIMEOUT, MP3_STATE_IDLE);
	
	TRANSITION_ADD(MP3_STATE_IDLE, MP3_EVENT_TIMEOUT, MP3_STATE_IDLE);
	TRANSITION_ADD(MP3_STATE_IDLE, MP3_EVENT_FILE, MP3_STATE_FILE_START);
	TRANSITION_ADD(MP3_STATE_IDLE, MP3_EVENT_STREAM, MP3_STATE_STREAM_START);
	
	TRANSITION_ADD(MP3_STATE_FILE_START, MP3_EVENT_TIMEOUT, MP3_STATE_FILE_HANDLE);
	TRANSITION_ADD(MP3_STATE_FILE_START, MP3_EVENT_ERROR, MP3_STATE_ERROR);

	TRANSITION_ADD(MP3_STATE_FILE_HANDLE, MP3_EVENT_TIMEOUT, MP3_STATE_FILE_HANDLE);
	TRANSITION_ADD(MP3_STATE_FILE_HANDLE, MP3_EVENT_END, MP3_STATE_IDLE);
	TRANSITION_ADD(MP3_STATE_FILE_HANDLE, MP3_EVENT_ERROR, MP3_STATE_ERROR);
	TRANSITION_ADD(MP3_STATE_FILE_HANDLE, MP3_EVENT_FILE, MP3_STATE_FILE_START);
	TRANSITION_ADD(MP3_STATE_FILE_HANDLE, MP3_EVENT_STOP, MP3_STATE_IDLE);
	TRANSITION_ADD(MP3_STATE_FILE_HANDLE, MP3_EVENT_STREAM, MP3_STATE_STREAM_START);

	TRANSITION_ADD(MP3_STATE_STREAM_START, MP3_EVENT_TIMEOUT, MP3_STATE_STREAM_HANDLE);
	TRANSITION_ADD(MP3_STATE_STREAM_START, MP3_EVENT_ERROR, MP3_STATE_ERROR);

	TRANSITION_ADD(MP3_STATE_STREAM_HANDLE, MP3_EVENT_TIMEOUT, MP3_STATE_STREAM_HANDLE);
	TRANSITION_ADD(MP3_STATE_STREAM_HANDLE, MP3_EVENT_STOP, MP3_STATE_IDLE);
	TRANSITION_ADD(MP3_STATE_STREAM_HANDLE, MP3_EVENT_ERROR, MP3_STATE_ERROR);
	TRANSITION_ADD(MP3_STATE_STREAM_HANDLE, MP3_EVENT_FILE, MP3_STATE_FILE_START);
	TRANSITION_ADD(MP3_STATE_STREAM_HANDLE, MP3_EVENT_STREAM, MP3_STATE_STREAM_START);
	
	TRANSITION_ADD(MP3_STATE_ERROR, MP3_EVENT_TIMEOUT, MP3_STATE_IDLE);

	TRANSITION_ADD(MP3_STATE_DEFAULT, MP3_EVENT_ERROR, MP3_STATE_ERROR);
	TRANSITION_ADD(MP3_STATE_DEFAULT, MP3_EVENT_SHUTDOWN, MP3_STATE_SHUTDOWN);
	state_startup_enter(0, 0, NULL);
};

void state_transition(int transition, int event, EVENT_PARAM *param){
	state_previous = state_current;
	state_current = transitions[transition].new_state;
	if(state_previous != state_current){
		printf("\n -- Transition %s -> %s\n", 
			states[state_previous].name, 
			states[state_current].name);
	};
	if(states[state_previous].leave != NULL)
			states[state_previous].leave(state_current, event);
	if(states[state_current].enter != NULL)
		states[state_current].enter(state_previous, event, param);
	if(param){
		if(param->text){
			free(param->text);
		};
		free(param);
	};
};

void state_event(int event, EVENT_PARAM *param){
	int i = 0;
	if(event != MP3_EVENT_TIMEOUT){
		printf("\n -- Got event %s (%d) in state %s (%d)\n", 
			events[event].name, 
			event, 
			states[state_current].name, 
			state_current);
	};
	for(i = 0; i < transition_count; i++){
		if((transitions[i].old_state == state_current) 
				&& (transitions[i].event == event)){
			state_transition(i, event, param);			
			return;
		};
	};
	for(i = 0; i < transition_count; i++){
		if((transitions[i].old_state == MP3_STATE_DEFAULT) 
				&& (transitions[i].event == event)){
			state_transition(i, event, param);			
			return;
		};
	};
	if(param){
		if(param->text){
			free(param->text);
		};
		free(param);
	};
	printf("Event %s not handled in state %s\n", events[event].name, states[state_current].name);
};

void state_mainloop(void){
	while(1){
		if(shutdown){
			state_event(MP3_EVENT_SHUTDOWN, NULL);
		};
		mp3_nix_socket_handle();
		mp3_tcp_socket_handle();
		if(state_current == MP3_STATE_IDLE){
			poll(0, 0, 200);
		} else {
			poll(0, 0, 1);
		};
		state_event(MP3_EVENT_TIMEOUT, NULL);
	};
};

EVENT_PARAM* state_new_event(unsigned char *text, int numeric){
	EVENT_PARAM *p = (EVENT_PARAM*)malloc(sizeof(EVENT_PARAM));
	if(text){
		p->text = strdup(text);
	} else {
		p->text = NULL;
	};
	p->numeric = numeric;
	return p;
};

void sig_handler(int sig){
	shutdown = 1;
};

int main(int argc, char **argv) {	
	unsigned char daemonize = 1;

	printf("\n\n");
	printf("ALPMP3 - MP3 Statemachine v1.0\n");

	if(argc > 1){
		if(!strcmp(argv[1], "--no-daemon")){
			daemonize = 0;
		} else {
			printf("I am now going to daemonize. If you want me to stay in ");
			printf("the foreground, add the parameter --no-daemon.\n");
		};
	};
	
	if(daemonize){
		daemon(0, 0);
		printf("----------------------------------------\n\n");
		printf("Made by openwrt.org (blogic@openwrt.org)\n");
		printf("\n");
	};

	signal(SIGINT, sig_handler); 
	
	state_setup();
	state_mainloop();

	return 0;
}




