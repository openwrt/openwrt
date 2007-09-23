/*
* a.lp_mp3 - VS1011B driver for Fonera 
* Copyright (c) 2007 phrozen.org - John Crispin <john@phrozen.org>
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
* Feedback, Bugs...  john@phrozen.org 
*
*/


#include <linux/module.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/genhd.h>
#include <linux/device.h> 
#include <asm-mips/mach-atheros/reset.h>

// do we want debuging info ?
#if 0
#define DBG(x) x
#else
#define DBG(x) 
#endif

#define MP3_CHUNK_SIZE 			4096
#define MP3_BUFFERING			0
#define MP3_PLAYING			1
#define MP3_BUFFER_FINISHED		2
#define MP3_PLAY_FINISHED		3
typedef struct _MP3_DATA{
	unsigned char 	mp3[MP3_CHUNK_SIZE];
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

#define CRYSTAL12288	0x9800
#define CRYSTAL24576	0x0

#define DEV_NAME 			"mp3"
#define DEV_MAJOR 			196
#define MAX_MP3_COUNT 	1

typedef struct _mp3_inf{
	unsigned char is_open;
} mp3_inf;
static mp3_inf mp3_info[MAX_MP3_COUNT];

#define MP3_BUFFER_SIZE		(128 * 1024)
unsigned char mp3_buffer[MP3_BUFFER_SIZE];

static unsigned long int mp3_buffer_offset_write = 0;
static unsigned long int mp3_buffer_offset_read = 0;
static unsigned char mp3_buffering_status = MP3_BUFFERING;
static unsigned long int mp3_data_in_buffer = 0;
static int mp3_thread = 0;
unsigned int crystal_freq;

#include "vs10xx.c"

static wait_queue_head_t wq;
static DECLARE_COMPLETION(mp3_exit);

static int mp3_playback_thread(void *data){
	int j;
	unsigned long timeout;
	unsigned char empty = 0;
	printk("started kthread\n");
	daemonize("kmp3");
	while(mp3_buffering_status != MP3_PLAY_FINISHED){
		if((mp3_buffering_status == MP3_PLAYING) || (mp3_buffering_status == MP3_BUFFER_FINISHED)){
			while((VS1011_NEEDS_DATA) && (!empty)){
				if(mp3_buffer_offset_read == MP3_BUFFER_SIZE){
					mp3_buffer_offset_read = 0;
				}
				
				if(mp3_data_in_buffer == 0){
					if(mp3_buffering_status == MP3_BUFFER_FINISHED){
						printk("mp3_drv.ko : finished playing\n");
						mp3_buffering_status = MP3_PLAY_FINISHED;
					} else {
						empty = 1;
						printk("mp3_drv.ko : buffer empty ?\n");
						if(mp3_buffering_status != MP3_PLAY_FINISHED){
						}
					}
				} else {
					for(j = 0; j < 32; j++){
						VS1011_send_SDI(mp3_buffer[mp3_buffer_offset_read + j]);
					}
					mp3_buffer_offset_read += 32;
					mp3_data_in_buffer -= 32;
				}
			}
		}
		empty = 0;
		timeout = 1;	
       	timeout = wait_event_interruptible_timeout(wq, (timeout==0), timeout);	
	}
	complete_and_exit(&mp3_exit, 0); 
}

static ssize_t module_write(struct file * file, const char * buffer, size_t count, loff_t *offset){
	MP3_DATA mp3_data;
	
	copy_from_user((char*) &mp3_data, buffer, sizeof(MP3_DATA));
	
	if(mp3_data.state == MP3_BUFFER_FINISHED){
		mp3_buffering_status = MP3_BUFFER_FINISHED;
		DBG(printk("mp3_drv.ko : file end reached\n"));
		return 1;
	}
	
	if(mp3_data.state == MP3_PLAY_FINISHED){
		mp3_buffering_status = MP3_PLAY_FINISHED;
		mp3_data_in_buffer = 0;
		DBG(printk("mp3_drv.ko : stop playing\n"));
		return 1;
	}
	
	if(mp3_data_in_buffer + MP3_CHUNK_SIZE >= MP3_BUFFER_SIZE){
		DBG(printk("mp3_drv.ko : buffer is full? %ld\n", mp3_data_in_buffer);)
		return 0;
	}
	
	if(mp3_buffer_offset_write == MP3_BUFFER_SIZE){
		mp3_buffer_offset_write = 0;
	}
	
	memcpy(&mp3_buffer[mp3_buffer_offset_write], mp3_data.mp3, MP3_CHUNK_SIZE);
	mp3_buffer_offset_write += MP3_CHUNK_SIZE;
	mp3_buffering_status = mp3_data.state;
	mp3_data_in_buffer += MP3_CHUNK_SIZE;
	return 1;
}

static int module_ioctl(struct inode * inode, struct file * file, unsigned int cmd, unsigned long arg){
	unsigned int 	retval = 0;
	AUDIO_DATA   	audio_data;
	MP3_BEEP	mp3_beep;
	DBG(printk("mp3_drv.ko : Ioctl Called (cmd=%d)\n", cmd );)
	switch (cmd) {
		case IOCTL_MP3_INIT:
			crystal_freq = arg;
			VS1011_init(crystal_freq, 1);
			VS1011_print_registers();
			break;

		case IOCTL_MP3_RESET:
			DBG(printk("mp3_drv.ko : doing a sw reset\n");)
			VS1011_init(crystal_freq, 0);
			VS1011_print_registers();
			VS1011_send_zeros(0x20);
			break;

		case IOCTL_MP3_SETVOLUME:
			DBG(printk("mp3_drv.ko : setting volume to : %lu\n", arg&0xffff);)
			VS1011_set_volume(arg);
			break;

		case IOCTL_MP3_GETVOLUME:
			retval = VS1011_get_volume();
			DBG(printk("mp3_drv.ko : read volume : %d\n", retval);)
			break;

		case IOCTL_MP3_GETAUDIODATA:
			DBG(printk("mp3_drv.ko : read audio data\n");)
			VS1011_get_audio_data(&audio_data);
			copy_to_user((char*)arg, (char*)&audio_data, sizeof(AUDIO_DATA));
			break;

		case IOCTL_MP3_CLEARBUFFER:
			DBG(printk("mp3_drv.ko : clearing buffer\n");)
			mp3_buffer_offset_read = 0;
			mp3_buffer_offset_write = 0;
			mp3_buffering_status = MP3_PLAY_FINISHED;
			mp3_data_in_buffer = 0;
			break;

		case IOCTL_MP3_PLAY:
			mp3_thread = kernel_thread(mp3_playback_thread, NULL, CLONE_KERNEL);
			break;

		case IOCTL_MP3_BEEP:
			copy_from_user((char*)&mp3_beep, (char*)arg, sizeof(MP3_BEEP));
			VS1011_sine(1,mp3_beep.freq);
			msDelay(mp3_beep.ms);
			VS1011_sine(0,0);
			break;

		case IOCTL_MP3_END_REACHED:
			if(mp3_buffering_status == MP3_PLAY_FINISHED){
				retval = 1;
			}
			break;

		case IOCTL_MP3_BASS:
			VS1011_set_bass(arg);
			break;

		default:
			printk("mp3_drv.ko : unknown ioctl\n");
			break;

	}
	return retval;
}

static int module_open(struct inode *inode, struct file *file){
	unsigned int dev_minor = MINOR(inode->i_rdev);
	if(dev_minor !=  0){
		printk("mp3_drv.ko : trying to access unknown minor device -> %d\n", dev_minor);
		return -ENODEV;
	}
	if(mp3_info[dev_minor].is_open) {
		printk("mp3_drv.ko : Device with minor ID %d already in use\n", dev_minor);
		return -EBUSY;
	}
	mp3_info[dev_minor].is_open = 1;
	
	mp3_buffering_status = MP3_PLAY_FINISHED;
	printk("mp3_drv.ko : Minor %d has been opened\n", dev_minor);
	return 0;
}

static int module_close(struct inode * inode, struct file * file){
	unsigned int dev_minor = MINOR(inode->i_rdev);
	mp3_info[dev_minor].is_open = 0;
	printk("mp3_drv.ko : Minor %d has been closed\n", dev_minor);
	mp3_buffering_status = MP3_PLAY_FINISHED;
	return 0;
}

struct file_operations modulemp3_fops = {
        write:         module_write,
        ioctl:         module_ioctl,
        open:          module_open,
        release:       module_close
};

static struct class *mp3_class; 

static int __init mod_init(void){
	printk("mp3_drv.ko : VS1011b Driver\n");
	printk("mp3_drv.ko : Made by John '2B|!2B' Crispin (john@phrozen.org)\n");
	printk("mp3_drv.ko : Starting ...\n");
	
	printk("disabling atheros reset button irq\n");

	ar531x_disable_reset_button();

	if(register_chrdev(DEV_MAJOR, DEV_NAME, &modulemp3_fops)) {
		printk( "mp3_drv.ko : Error whilst opening %s (%d)\n", DEV_NAME, DEV_MAJOR);
		return( -ENODEV );
	}

	printk("mp3_drv.ko : using sysfs to create device nodes\n");
	mp3_class = class_create(THIS_MODULE, DEV_NAME); 
	class_device_create(mp3_class, NULL, 
		MKDEV(DEV_MAJOR, 0), 
		NULL, DEV_NAME); 

	mp3_info[0].is_open = 0;
	printk("mp3_drv.ko : Device %s registered for major ID %d\n", DEV_NAME, DEV_MAJOR);
	crystal_freq = CRYSTAL12288;
	VS1011_init(crystal_freq, 1);
	VS1011_print_registers();
	printk("end of init\n");
	init_waitqueue_head(&wq);
	printk("wait queue started\n");
	return 0;
}

static void __exit mod_exit(void){
	printk( "mp3_drv.ko : Cleanup\n" );
	unregister_chrdev(DEV_MAJOR, DEV_NAME);
}

module_init (mod_init);
module_exit (mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("K. John '2B|!2B' Crispin");
MODULE_DESCRIPTION("vs1011 Driver for Fox Board");



