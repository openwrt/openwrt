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
 * Feedback, Bugs.... mail john@phrozen.org
 *
 */ 

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/poll.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/system.h>
#include <asm/irq.h>
#include "ar531xlnx.h"

#define AR5315_DSLBASE          0xB1000000
#define AR5315_GPIO_DI          (AR5315_DSLBASE + 0x0088)
#define AR5315_GPIO_DO          (AR5315_DSLBASE + 0x0090)
#define AR5315_GPIO_CR          (AR5315_DSLBASE + 0x0098)
#define AR5315_GPIO_INT         (AR5315_DSLBASE + 0x00a0)

#define GPIO_0			1<<0
#define GPIO_1			1<<1
#define GPIO_2			1<<2
#define GPIO_3			1<<3
#define GPIO_4			1<<4
#define GPIO_6			1<<6
#define GPIO_7			1<<7

#define DREQ				((unsigned int)GPIO_7)
#define SCK				((unsigned int)GPIO_1)
#define SI				((unsigned int)GPIO_4)
#define BSYNC				((unsigned int)GPIO_3)
#define CS				((unsigned int)GPIO_0)
#define SO				((unsigned int)GPIO_6)
#define RES				((unsigned int)GPIO_2)

#define REG_MODE			0x0
#define REG_STATUS			0x1
#define REG_BASS			0x2
#define REG_CLOCKF			0x3
#define REG_DECODETIME			0x4
#define REG_AUDATA			0x5
#define REG_WRAM			0x6
#define REG_WRAMADDR			0x7
#define REG_HDAT0			0x8
#define REG_HDAT1			0x9
#define REG_A1ADDR			0xa
#define REG_VOL				0xb
#define REG_A1CTRL0			0xc
#define REG_A1CTRL1			0xd
#define REG_A1CTRL2			0xe

#define VS1011_NEEDS_DATA  		spi_get_bit(DREQ)
#define VS1011_NEEDS_NO_DATA  		(spi_get_bit(DREQ)== 0x00)
#define VS1011_WHILE_NEEDS_NO_DATA  	while(spi_get_bit(DREQ)== 0x00){}

#define VS_CS_LO			spi_clear_bit(CS)
#define VS_CS_HI			spi_set_bit(CS)

#define VS_BSYNC_LO			spi_clear_bit(BSYNC)
#define VS_BSYNC_HI			spi_set_bit(BSYNC)

#define VS_RESET_LO			spi_clear_bit(RES)
#define VS_RESET_HI			spi_set_bit(RES)

#define VS1011_READ 			SPI_io_vs1011b(0x03)
#define VS1011_WRITE 			SPI_io_vs1011b(0x02)

void msDelay(int ms) {
	int i,a;
	int delayvar=10;

	for (a=0;a<ms;a++) {
		for (i=0;i<33084;i++) {
			delayvar*=2;        
			delayvar/=2;
		} 
	}
}   

int spi_get_bit(unsigned int pin){
	return ((sysRegRead(AR5315_GPIO_DI)&pin)?(1):(0));
}

void spi_set_bit(unsigned int pin){
	sysRegWrite(AR5315_GPIO_DO, (sysRegRead(AR5315_GPIO_DO) | pin));
}

void spi_clear_bit(unsigned int pin){
	sysRegWrite(AR5315_GPIO_DO, (sysRegRead(AR5315_GPIO_DO) & ~pin));
}

void SPI_clock_vs1011b(void){
	spi_clear_bit(SCK);
	spi_set_bit(SCK);
}

unsigned char SPI_io_vs1011b(unsigned char byte){
	int i;
	unsigned char this_bit;
	unsigned char byte_out = 0;
	for(i = 7; i>=0; i--){
		if(byte & (1<<i)){
			this_bit = 1;
		} else {
			this_bit = 0;
		}
		if(this_bit){
			spi_set_bit(SI);
		} else {
			spi_clear_bit(SI);
		}
		SPI_clock_vs1011b();
		byte_out += spi_get_bit(SO)<<i;
	}
	return byte_out;
}

void SPI_init_vs1011(void){
	sysRegWrite(AR5315_GPIO_CR, (sysRegRead(AR5315_GPIO_CR) | SI | SCK | CS | BSYNC | RES) & ~(SO|DREQ));
	spi_clear_bit(SCK);
	spi_clear_bit(SI);
	VS_CS_HI;
	VS_BSYNC_HI;
}

void VS1011_send_SCI(unsigned char reg, unsigned int data){
	VS_CS_LO;
	VS1011_WRITE;
	SPI_io_vs1011b(reg);
	SPI_io_vs1011b((data>>8)&0xff);
	SPI_io_vs1011b(data&0xff);
	VS_CS_HI;
}

unsigned int VS1011_read_SCI(unsigned char reg){
	unsigned int data;	
	VS_CS_LO;
	VS1011_READ;
	SPI_io_vs1011b(reg);
	data = 0;
	data = SPI_io_vs1011b(0x00);
	data <<= 8;
	data += SPI_io_vs1011b(0x00);
	VS_CS_HI;
	return data;
}

void VS1011_send_SDI(unsigned char byte){
	int i;
	VS_BSYNC_LO;
	for(i = 7; i>=0; i--){
		if(byte & (1<<i)){
			spi_set_bit(SI);
			
		} else {
			spi_clear_bit(SI);
		}			
		spi_clear_bit(SCK);
		spi_set_bit(SCK);
	}
	VS_BSYNC_HI;
}

void VS1011_send_SDI_32(unsigned char* data){
	int i;
	VS1011_WHILE_NEEDS_NO_DATA;
	for(i=0; i<32; i++){
		VS1011_send_SDI(data[i]);
	}
}

void VS1011_send_zeros(unsigned char count){
	do{
		VS1011_send_SDI(0x0);
		count--;
	}while(count);
}

void VS1011_set_volume(unsigned int vol){
	VS1011_send_SCI(REG_VOL, vol);
}

void VS1011_SW_reset(unsigned int _crystal_freq){
	unsigned int regval = 0x0804;
	unsigned long int i = 0;
	msDelay(100);
	VS1011_send_zeros(32);
	VS1011_send_SCI(REG_MODE, regval);
	msDelay(10);
	while((VS1011_NEEDS_NO_DATA) && (i++<0xffff)){};
	VS1011_send_SCI(REG_CLOCKF, _crystal_freq);
	VS1011_send_zeros(16);
	VS1011_set_volume(0x00);
}

void VS1011_HW_reset(void){
	
	VS_RESET_LO;
	msDelay(1);
	VS_RESET_HI;
	msDelay(1);
}

void VS1011_init(unsigned int _crystal_freq, unsigned char hw){
	if(hw){
		SPI_init_vs1011();
	}
	printk("mp3_drv.ko : Init start\n");
	if(hw){
		VS1011_HW_reset();
	}
	VS1011_SW_reset(_crystal_freq);
	printk("mp3_drv.ko : init_ok\n");
}

void VS1011_sine(unsigned char state, unsigned char freq){
	VS1011_send_zeros(16);
	if(state == 0x01){
		VS1011_send_SDI(0x53);
		VS1011_send_SDI(0xEF);
		VS1011_send_SDI(0x6E);
		VS1011_send_SDI(freq);
		VS1011_send_zeros(0x04);
	} else {
		VS1011_send_SDI(0x45);
		VS1011_send_SDI(0x78);
		VS1011_send_SDI(0x69);
		VS1011_send_SDI(0x74);
		VS1011_send_zeros(0x04);		
	}
}

unsigned int VS1011_get_volume(void){
	return VS1011_read_SCI(REG_VOL);
}

unsigned int VS1011_get_decode_time(void){
	return VS1011_read_SCI(REG_DECODETIME);
}

const unsigned int sample_rate_values[]  = {0, 44100, 48000, 32000, 22050, 24000, 16000, 11025, 12000, 8000};

void VS1011_get_audio_data(AUDIO_DATA* audio){
	unsigned int audata = VS1011_read_SCI(REG_AUDATA);
	audio->sample_rate = sample_rate_values[(audata&0x1E00)>>9];
	audio->bitrate = audata&0x1FF;
	audio->is_stereo = (audata&0x8000)>>15;
}

void VS1011_print_registers(void){
	unsigned char i;
	for(i = 0; i< 14; i++){
		unsigned int regval = VS1011_read_SCI(i);
		printk("mp3_drv.ko : %d \n", regval);		
	}
}

void VS1011_volume(unsigned char left, unsigned char right){
	unsigned int regval = left;
	regval <<=8;
	regval += right;
	VS1011_send_SCI(REG_VOL, regval);
}

void VS1011_set_bass(unsigned int regval){
	VS1011_send_SCI(REG_BASS, regval);
}

void VS1011_set_reg(unsigned int reg, unsigned int regval){
	VS1011_send_SCI(reg, regval);
}

/*
int vs_test(void) {	
	SPI_init_vs1011();
	printk("%u\n", *R_GEN_CONFIG);
	VS1001_init(_crystal_freq);
	VS1001_print_registers();
	VS1001_volume(0x30, 0x30); 
	msDelay(1000);
	VS1001_sine(1, 0x30);
	msDelay(1000);
	VS1001_sine(0, 0);
	VS1001_send_zeros(0x20);
	msDelay(1000);
	VS1001_sine(1, 0x30);
	msDelay(1000);
	VS1001_sine(0, 0);
	VS1001_send_zeros(0x20);
	msDelay(1000);
	VS1001_sine(1, 0x30);
	msDelay(1000);
	VS1001_sine(0, 0);
	
	AUDIO_DATA a;
	VS1001_get_audio_data(&a);
	printk("mp3_drv.ko : rate : %d, bit : %d, stereo : %d \n", a.sample_rate, a.bitrate, a.is_stereo);
	VS1001_SW_reset(_crystal_freq);
	return 0;
}*/

