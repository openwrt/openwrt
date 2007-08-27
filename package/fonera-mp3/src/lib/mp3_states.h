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

typedef struct _EVENT {
	unsigned char *name;
} EVENT;

typedef struct _EVENT_PARAM {
	int numeric;
	unsigned char *text;
} EVENT_PARAM;

typedef void (*STATE_ENTER)(int state_last, int event, EVENT_PARAM *param);
typedef void (*STATE_LEAVE)(int state_new, int event);

typedef struct _STATE {
	unsigned char *name;
	STATE_ENTER enter;
	STATE_LEAVE leave;
} STATE;

typedef struct _TRANSITION {
	int old_state;
	int event;
	int new_state;
} TRANSITION;

// a = id, b = name, c = enter, d = leave
#define STATE_ADD(a, b, c, d) { 			\
		if((a > 0) && (a < MAX_STATE_COUNT)){	\
			states[a].name = strdup(b); 	\
			states[a].enter = c; 		\
			states[a].leave = d; 		\
		} 					\
	}

// a = old, b = event, c = new 
#define TRANSITION_ADD(a, b, c) { 					\
		if((transition_count >= 0) && 				\
			(transition_count < MAX_TRANSITION_COUNT)){	\
			transitions[transition_count].old_state = a;	\
			transitions[transition_count].event = b; 	\
			transitions[transition_count].new_state = c; 	\
			transition_count++;				\
		} 							\
	}

// a = id, b = name
#define EVENT_ADD(a, b) { 				\
		if((a > 0) && (a < MAX_EVENT_COUNT)){	\
			events[a].name = strdup(b); 	\
		} 					\
	}
#define MAX_EVENT_COUNT 	20
#define MAX_STATE_COUNT 	20
#define MAX_TRANSITION_COUNT	60
	
#define MP3_STATE_NONE			0
#define MP3_STATE_STARTUP		1
#define MP3_STATE_IDLE			2
#define MP3_STATE_FILE_START		3
#define MP3_STATE_FILE_HANDLE		4	
#define MP3_STATE_STREAM_START		5
#define MP3_STATE_STREAM_HANDLE		6
#define MP3_STATE_ERROR			7
#define MP3_STATE_SHUTDOWN		8
#define MP3_STATE_DEFAULT		9

#define MP3_EVENT_TIMEOUT		1
#define MP3_EVENT_FILE			2
#define MP3_EVENT_STREAM		3
#define MP3_EVENT_STOP			4
#define MP3_EVENT_ERROR			5
#define MP3_EVENT_SHUTDOWN		6
#define MP3_EVENT_END			7

#define MP3_EVENT_GENERIC_VOLUME	0
#define MP3_EVENT_GENERIC_STATE		1
#define MP3_EVENT_GENERIC_BASS		2
#define MP3_EVENT_GENERIC_PLAYTIME	3

void state_startup_enter(int state_last, int event, EVENT_PARAM *param);
void state_idle_enter(int state_last, int event, EVENT_PARAM *param);
void state_file_startup_enter(int state_last, int event, EVENT_PARAM *param);
void state_file_startup_leave(int state_new, int event);
void state_file_handle_enter(int state_last, int event, EVENT_PARAM *param);
void state_file_handle_leave(int state_new, int event);
void state_stream_startup_enter(int state_last, int event, EVENT_PARAM *param);
void state_stream_startup_leave(int state_last, int event);
void state_stream_handle_enter(int state_last, int event, EVENT_PARAM *param);
void state_stream_handle_leave(int state_new, int event);
void state_error_enter(int state_last, int event, EVENT_PARAM *param);
void state_shutdown_enter(int state_last, int event, EVENT_PARAM *param);
void state_generic_event(unsigned int event, unsigned char in_int, 
		unsigned char *out_uchar);
