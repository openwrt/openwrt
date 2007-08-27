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

#define MP3_CHUNK_SIZE 			4096	
#define MP3_BUFFERING			0
#define MP3_PLAYING			1
#define MP3_BUFFER_FINISHED		2
#define MP3_PLAY_FINISHED		3

typedef struct _MP3_DATA{
	unsigned char mp3[MP3_CHUNK_SIZE];
	unsigned char state;
} MP3_DATA;


#define IOCTL_MP3_INIT			0x01
#define IOCTL_MP3_RESET			0x02
#define IOCTL_MP3_SETVOLUME		0x03
#define IOCTL_MP3_GETVOLUME		0x04
typedef struct _AUDIO_DATA{
	unsigned int bitrate;
	unsigned int sample_rate;
	unsigned char is_stereo;
}AUDIO_DATA;
#define IOCTL_MP3_GETAUDIODATA		0x05

#define IOCTL_MP3_CLEARBUFFER		0x06
#define IOCTL_MP3_PLAY			0x07
typedef struct _MP3_BEEP{
	unsigned char 	freq;
	unsigned int	ms;
} MP3_BEEP;
#define IOCTL_MP3_BEEP			0x08			
#define IOCTL_MP3_END_REACHED		0x09

#define IOCTL_MP3_BASS			0x10

#define CRYSTAL12288			0x9800
#define CRYSTAL24576			0x0

#define MP3_OK				0
#define MP3_ERROR			1
#define MP3_END				2

void mp3_reset(void);
unsigned char mp3_send_data_to_buffer(MP3_DATA mp3_data);
unsigned char mp3_buffer_finished(void);
unsigned char mp3_play(void);
void mp3_stop(void);
int mp3_init(void);
void mp3_bass(unsigned char t_freq, unsigned char t_amp, 
		unsigned char b_freq, unsigned char b_amp);
void mp3_set_volume(unsigned char left, unsigned char right);
