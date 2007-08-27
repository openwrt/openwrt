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


#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include "mp3.h"


static MP3_PLAYTIME *mp3_playtime;

void sig_alarm_handler(int sig){
	alarm(1);
	mp3_playtime->playtime++;
	mp3_playtime->playtime_secs++;
	if(mp3_playtime->playtime_secs > 59){
		mp3_playtime->playtime_mins++;
		mp3_playtime->playtime_secs = 0;
	}
	state_generic_event(MP3_EVENT_GENERIC_PLAYTIME, 0, NULL);
};

void mp3_playtime_init(MP3_PLAYTIME *playtime){
	mp3_playtime = playtime;
	signal(SIGALRM, sig_alarm_handler);
};

void mp3_playtime_start(void){
	mp3_playtime->playtime = 0;
	mp3_playtime->playtime_secs = 0;
	mp3_playtime->playtime_mins = 0;
	alarm(1);
	state_generic_event(MP3_EVENT_GENERIC_PLAYTIME, 0, NULL);
};

void mp3_playtime_stop(void){
	alarm(0);
	mp3_playtime->playtime = 0;
	mp3_playtime->playtime_secs = 0;
	mp3_playtime->playtime_mins = 0;
};
