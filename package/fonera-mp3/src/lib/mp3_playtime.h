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


typedef struct _MP3_PLAYTIME {
	unsigned int playtime;
	unsigned int playtime_secs;
	unsigned int playtime_mins;
} MP3_PLAYTIME;

void mp3_playtime_init(MP3_PLAYTIME *playtime);
void mp3_playtime_start(void);
void mp3_playtime_stop(void);

