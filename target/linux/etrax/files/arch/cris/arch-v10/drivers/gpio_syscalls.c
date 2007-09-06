#include <linux/autoconf.h>

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

#include <asm/uaccess.h>
#include <linux/gpio_syscalls.h>

#include <asm/etraxgpio.h>
#include <asm/arch/svinto.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/irq.h>
#include <asm/arch/io_interface_mux.h>

#include <asm/unistd.h>


extern int errno;


asmlinkage void sys_gpiosetbits(unsigned char port, unsigned int bits){
	switch(port){
	case 'G':
	case 'g':
		*R_PORT_G_DATA = port_g_data_shadow |= bits;
		break;
		
	case 'A':
	case 'a':
		*R_PORT_PA_DATA = port_pa_data_shadow |= bits;
		break;

	case 'B':
	case 'b':
		*R_PORT_PB_DATA = port_pb_data_shadow |= bits;
		break;
		
	};
};


asmlinkage void sys_gpioclearbits(unsigned char port, unsigned int bits){
	switch(port){
	case 'G':
	case 'g':
		*R_PORT_G_DATA = port_g_data_shadow &= ~bits;
		break;
		
	case 'A':
	case 'a':
		*R_PORT_PA_DATA = port_pa_data_shadow &= ~bits;
		break;

	case 'B':
	case 'b':
		*R_PORT_PB_DATA = port_pb_data_shadow &= ~bits;
		break;
		
	};
};

asmlinkage void sys_gpiosetdir(unsigned char port, unsigned char dir, unsigned int bits){
	if((dir=='I' )||(dir=='i')){
		switch(port){
		case 'G':
		case 'g':
			if(bits & (1<<0)){
				genconfig_shadow &= ~IO_MASK(R_GEN_CONFIG, g0dir);
			};
			if((bits & 0x0000FF00)==0x0000FF00){
				genconfig_shadow &= ~IO_MASK(R_GEN_CONFIG, g8_15dir);
			};			
			if((bits & 0x00FF0000)==0x00FF0000){
				genconfig_shadow &= ~IO_MASK(R_GEN_CONFIG, g16_23dir);
			};			
			if(bits & (1<<24)){
				genconfig_shadow &= ~IO_MASK(R_GEN_CONFIG, g24dir);
			};			
			*R_GEN_CONFIG = genconfig_shadow;
			break;
		
		case 'A':
		case 'a':
			*R_PORT_PA_DIR = port_pa_dir_shadow &= ~(bits & 0xff);
			break;

		case 'B':
		case 'b':
			*R_PORT_PB_DIR = port_pb_dir_shadow &= ~(bits & 0xff);
			break;
		};
	} else if((dir=='O' )||(dir=='o')){
		switch(port){
		case 'G':
		case 'g':
			if(bits & (1<<0)){
				genconfig_shadow |= IO_MASK(R_GEN_CONFIG, g0dir);
			};
			if((bits & 0x0000FF00)==0x0000FF00){
				genconfig_shadow |= IO_MASK(R_GEN_CONFIG, g8_15dir);
			};			
			if((bits & 0x00FF0000)==0x00FF0000){
				genconfig_shadow |= IO_MASK(R_GEN_CONFIG, g8_15dir);
			};			
			if(bits & (1<<24)){
				genconfig_shadow |= IO_MASK(R_GEN_CONFIG, g24dir);
			};			
			*R_GEN_CONFIG = genconfig_shadow;
			break;
		
		case 'A':
		case 'a':
			*R_PORT_PA_DIR = port_pa_dir_shadow |= (bits & 0xff);
			break;

		case 'B':
		case 'b':
			*R_PORT_PB_DIR = port_pb_dir_shadow |= (bits & 0xff);
			break;
		};
	};
};


asmlinkage void sys_gpiotogglebit(unsigned char port, unsigned int bits){
	switch(port){
	case 'G':
	case 'g':
		if(port_g_data_shadow & bits){
			*R_PORT_G_DATA = port_g_data_shadow &= ~bits;
		} else {
			*R_PORT_G_DATA = port_g_data_shadow |= bits;
		};
		break;
		
	case 'A':
	case 'a':
		if(*R_PORT_PA_DATA & bits){
			*R_PORT_PA_DATA = port_pa_data_shadow &= ~(bits & 0xff);
		} else {
			*R_PORT_PA_DATA = port_pa_data_shadow |= (bits & 0xff);	
		};
		break;

	case 'B':
	case 'b':
		if(*R_PORT_PB_DATA & bits){
			*R_PORT_PB_DATA = port_pb_data_shadow &= ~(bits & 0xff);
		} else {
			*R_PORT_PB_DATA = port_pb_data_shadow |= (bits & 0xff);	
		};
		break;
		
	};
};


asmlinkage unsigned int sys_gpiogetbits(unsigned char port, unsigned int bits){
	unsigned int data = 0;
	switch(port){
	case 'G':
	case 'g':
		data = *R_PORT_G_DATA;
		break;
		
	case 'A':
	case 'a':
		data = *R_PORT_PA_DATA;
		break;

	case 'B':
	case 'b':
		data = *R_PORT_PB_DATA;
		break;
		
	};
	data &= bits;
	return data;
};


