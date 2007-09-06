#ifndef AMAZON_WDT_H
#define AMAZON_WDT_H
#ifdef __KERNEL__
typedef struct wdt_dev{
      char name[16];
      int major;
      int minor;
      
      int full;
      char buff[10];
}wdt_dev;
#define AMAZON_WDT_REG32(addr) (*((volatile u32*)(addr))) 
#endif //__KERNEL__

//AMAZON_WDT_IOC_START: start the WDT timer (must provide a initial timeout value)
//AMAZON_WDT_IOC_STOP: stop the WDT
//AMAZON_WDT_IOC_PING: reload the timer to initial value (must happend after a AMAZON_WDT_IOC_START)
#define AMAZON_WDT_IOC_MAGIC	0xc0
#define AMAZON_WDT_IOC_START	_IOW( AMAZON_WDT_IOC_MAGIC,0, int)
#define AMAZON_WDT_IOC_STOP	_IO( AMAZON_WDT_IOC_MAGIC,1)
#define AMAZON_WDT_IOC_PING	_IO( AMAZON_WDT_IOC_MAGIC,2)

#endif //AMAZON_WDT_H
