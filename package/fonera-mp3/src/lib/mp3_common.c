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
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "mp3.h"

static int mp3_fd;
static int mp3_frequency_val = CRYSTAL12288;
unsigned int volume = 0x3030;

int mp3_open_port(unsigned char *port_name){
	int fd;
	if ((fd = open(port_name, O_RDWR)) < 0) {
		printf("Error whilst opening %s\n", port_name);
		return -1;
	}
	return fd;
};

void mp3_set_frequency(unsigned int crystal_frequency){
	mp3_frequency_val = crystal_frequency;
};

int mp3_init(void){
	mp3_fd = mp3_open_port("/dev/mp3");
	if(mp3_fd < 1){
		return 0;
	};
	ioctl(mp3_fd, IOCTL_MP3_INIT, mp3_frequency_val);
	return 1;
};

void mp3_shutdown(void){
	close(mp3_fd);
};

void mp3_bass(unsigned char t_freq, unsigned char t_amp, 
		unsigned char b_freq, unsigned char b_amp){
	unsigned int val;
	if(t_amp > 0xf){
		t_amp = 0xf;
	};
	if(b_amp > 0xf){
		b_amp = 0xf;
	};
	val = t_amp;
	val <<= 4;
	val += t_freq;
	val <<= 4;
	val += b_amp;
	val <<= 4;
	val += b_freq;
	ioctl(mp3_fd, IOCTL_MP3_BASS, val);
};

void mp3_reset(void){
	ioctl(mp3_fd, IOCTL_MP3_CLEARBUFFER, 0);
	ioctl(mp3_fd, IOCTL_MP3_RESET, mp3_frequency_val);
	ioctl(mp3_fd, IOCTL_MP3_SETVOLUME, volume);
};

unsigned char mp3_send_data_to_buffer(MP3_DATA mp3_data){
	return write(mp3_fd, (void*)&mp3_data, sizeof(MP3_DATA));
};

unsigned char mp3_play(void){
	return ioctl(mp3_fd, IOCTL_MP3_PLAY, 0);
};

void mp3_stop(void){
	ioctl(mp3_fd, IOCTL_MP3_CLEARBUFFER, 0);
};

void mp3_beep(unsigned char freq, unsigned int ms){
	MP3_BEEP mp3_beep_;
	mp3_beep_.freq = freq;
	mp3_beep_.ms = ms;
	ioctl(mp3_fd, IOCTL_MP3_BEEP, &mp3_beep_);
};

void mp3_set_volume(unsigned char left, unsigned char right){
	volume = left;
	volume <<= 8;
	volume += right;
	
	ioctl(mp3_fd, IOCTL_MP3_SETVOLUME, volume);
};

unsigned char mp3_buffer_finished(void){
	return ioctl(mp3_fd, IOCTL_MP3_END_REACHED, 0);
};

unsigned char mp3_get_audio_data(AUDIO_DATA *audio_data){
	return ioctl(mp3_fd, IOCTL_MP3_GETAUDIODATA, audio_data);
};


