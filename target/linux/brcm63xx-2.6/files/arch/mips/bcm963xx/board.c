/*
<:copyright-gpl 
 Copyright 2002 Broadcom Corp. All Rights Reserved. 
 
 This program is free software; you can distribute it and/or modify it 
 under the terms of the GNU General Public License (Version 2) as 
 published by the Free Software Foundation. 
 
 This program is distributed in the hope it will be useful, but WITHOUT 
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
 for more details. 
 
 You should have received a copy of the GNU General Public License along 
 with this program; if not, write to the Free Software Foundation, Inc., 
 59 Temple Place - Suite 330, Boston MA 02111-1307, USA. 
:>
*/

/* Includes. */
#include <linux/version.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/capability.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/pagemap.h>
#include <asm/uaccess.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/if.h>
#include <linux/spinlock.h>

#include <bcm_map_part.h>
#include <board.h>
#include <bcmTag.h>
#include "boardparms.h"
#include "bcm_intr.h"
#include "board.h"
#include "bcm_map_part.h"

static DEFINE_SPINLOCK(board_lock);

/* Typedefs. */
#if defined (NON_CONSECUTIVE_MAC)
// used to be the last octet. Now changed to the first 5 bits of the the forth octet
// to reduced the duplicated MAC addresses.
#define CHANGED_OCTET   3
#define SHIFT_BITS      3
#else
#define CHANGED_OCTET   1
#define SHIFT_BITS      0
#endif

typedef struct
{
    unsigned long ulId;
    char chInUse;
    char chReserved[3];
} MAC_ADDR_INFO, *PMAC_ADDR_INFO;

typedef struct
{
    unsigned long ulSdramSize;
    unsigned long ulPsiSize;
    unsigned long ulNumMacAddrs;
    unsigned long ucaBaseMacAddr[NVRAM_MAC_ADDRESS_LEN];
    MAC_ADDR_INFO MacAddrs[1];
} NVRAM_INFO, *PNVRAM_INFO;

typedef struct
{
    unsigned long eventmask;    
} BOARD_IOC, *PBOARD_IOC;


/*Dyinggasp callback*/
typedef void (*cb_dgasp_t)(void *arg);
typedef struct _CB_DGASP__LIST
{
    struct list_head list;
    char name[IFNAMSIZ];
    cb_dgasp_t cb_dgasp_fn;
    void *context;
}CB_DGASP_LIST , *PCB_DGASP_LIST;


static LED_MAP_PAIR LedMapping[] =
{   // led name     Initial state       physical pin (ledMask)
    {kLedEnd,       kLedStateOff,       0, 0, 0, 0},
    {kLedEnd,       kLedStateOff,       0, 0, 0, 0},
    {kLedEnd,       kLedStateOff,       0, 0, 0, 0},
    {kLedEnd,       kLedStateOff,       0, 0, 0, 0},
    {kLedEnd,       kLedStateOff,       0, 0, 0, 0},
    {kLedEnd,       kLedStateOff,       0, 0, 0, 0}, 
    {kLedEnd,       kLedStateOff,       0, 0, 0, 0}, 
    {kLedEnd,       kLedStateOff,       0, 0, 0, 0},     
    {kLedEnd,       kLedStateOff,       0, 0, 0, 0} // NOTE: kLedEnd has to be at the end.
};

/* Externs. */
extern struct file fastcall *fget_light(unsigned int fd, int *fput_needed);
extern unsigned int nr_free_pages (void);
extern const char *get_system_type(void);
extern void kerSysFlashInit(void);
extern unsigned long get_nvram_start_addr(void);
extern unsigned long get_scratch_pad_start_addr(void);
extern unsigned long getMemorySize(void);
extern void __init boardLedInit(PLED_MAP_PAIR);
extern void boardLedCtrl(BOARD_LED_NAME, BOARD_LED_STATE);
extern void kerSysLedRegisterHandler( BOARD_LED_NAME ledName,
    HANDLE_LED_FUNC ledHwFunc, int ledFailType );

/* Prototypes. */
void __init InitNvramInfo( void );

/* DyingGasp function prototype */
static void __init kerSysDyingGaspMapIntr(void);
static irqreturn_t kerSysDyingGaspIsr(int irq, void * dev_id);
static void __init kerSysInitDyingGaspHandler( void );
static void __exit kerSysDeinitDyingGaspHandler( void );
/* -DyingGasp function prototype - */

static PNVRAM_INFO g_pNvramInfo = NULL;
static int g_ledInitialized = 0;
static CB_DGASP_LIST *g_cb_dgasp_list_head = NULL;

static int g_wakeup_monitor = 0;
static struct file *g_monitor_file = NULL;
static struct task_struct *g_monitor_task = NULL;
static unsigned int (*g_orig_fop_poll)
    (struct file *, struct poll_table_struct *) = NULL;

void kerSysMipsSoftReset(void)
{
    if (PERF->RevID == 0x634800A1) {
        typedef void (*FNPTR) (void);
        FNPTR bootaddr = (FNPTR) FLASH_BASE;
        int i;

        /* Disable interrupts. */
        //cli();
	spin_lock_irq(&board_lock);
	
        /* Reset all blocks. */
        PERF->BlockSoftReset &= ~BSR_ALL_BLOCKS;
        for( i = 0; i < 1000000; i++ )
            ;
        PERF->BlockSoftReset |= BSR_ALL_BLOCKS;
        /* Jump to the power on address. */
        (*bootaddr) ();
    }
    else
        PERF->pll_control |= SOFT_RESET;    // soft reset mips
}


int kerSysGetMacAddress( unsigned char *pucaMacAddr, unsigned long ulId )
{
    int nRet = 0;
    PMAC_ADDR_INFO pMai = NULL;
    PMAC_ADDR_INFO pMaiFreeNoId = NULL;
    PMAC_ADDR_INFO pMaiFreeId = NULL;
    unsigned long i = 0, ulIdxNoId = 0, ulIdxId = 0, shiftedIdx = 0;

    /* CMO -- Fix le problème avec les adresses mac que l'on n'arrive pas
     *  * à relire plusieurs fois */
    /* inv_xde */
#if 0
    if (boot_loader_type == BOOT_CFE)
      memcpy( pucaMacAddr, g_pNvramInfo->ucaBaseMacAddr,
              NVRAM_MAC_ADDRESS_LEN );
    else {
#endif
      pucaMacAddr[0] = 0x00;
      pucaMacAddr[1] = 0x07;
      pucaMacAddr[2] = 0x3A;
      pucaMacAddr[3] = 0xFF;
      pucaMacAddr[4] = 0xFF;
      pucaMacAddr[5] = 0xFF;
#if 0
    }
#endif

    return nRet;
} /* kerSysGetMacAddr */

int kerSysReleaseMacAddress( unsigned char *pucaMacAddr )
{
    int nRet = -EINVAL;
    unsigned long ulIdx = 0;
    int idx = (pucaMacAddr[NVRAM_MAC_ADDRESS_LEN - CHANGED_OCTET] -
        g_pNvramInfo->ucaBaseMacAddr[NVRAM_MAC_ADDRESS_LEN - CHANGED_OCTET]);

    // if overflow 255 (negitive), add 256 to have the correct index
    if (idx < 0)
        idx += 256;
    ulIdx = (unsigned long) (idx >> SHIFT_BITS);

    if( ulIdx < g_pNvramInfo->ulNumMacAddrs )
    {
        PMAC_ADDR_INFO pMai = &g_pNvramInfo->MacAddrs[ulIdx];
        if( pMai->chInUse == 1 )
        {
            pMai->chInUse = 0;
            nRet = 0;
        }
    }

    return( nRet );
} /* kerSysReleaseMacAddr */

int kerSysGetSdramSize( void )
{
  if (boot_loader_type == BOOT_CFE) {
    return( (int) g_pNvramInfo->ulSdramSize );
  }
  else {
    printk("kerSysGetSdramSize : 0x%08X\n", (int)getMemorySize() + 0x00040000);
    return((int)getMemorySize() + 0x00040000);
  }
} /* kerSysGetSdramSize */


void kerSysLedCtrl(BOARD_LED_NAME ledName, BOARD_LED_STATE ledState)
{
    if (g_ledInitialized)
      boardLedCtrl(ledName, ledState);
}

unsigned int kerSysMonitorPollHook( struct file *f, struct poll_table_struct *t)
{
    int mask = (*g_orig_fop_poll) (f, t);

    if( g_wakeup_monitor == 1 && g_monitor_file == f )
    {
        /* If g_wakeup_monitor is non-0, the user mode application needs to
         * return from a blocking select function.  Return POLLPRI which will
         * cause the select to return with the exception descriptor set.
         */
        mask |= POLLPRI;
        g_wakeup_monitor = 0;
    }

    return( mask );
}

/* Put the user mode application that monitors link state on a run queue. */
void kerSysWakeupMonitorTask( void )
{
    g_wakeup_monitor = 1;
    if( g_monitor_task )
        wake_up_process( g_monitor_task );
}

//<<JUNHON, 2004/09/15, get reset button status , tim hou , 05/04/12
int kerSysGetResetHold(void)
{
	unsigned short gpio;

	if( BpGetPressAndHoldResetGpio( &gpio ) == BP_SUCCESS )
	{
    unsigned long gpio_mask = GPIO_NUM_TO_MASK(gpio);
    volatile unsigned long *gpio_reg = &GPIO->GPIOio;

    if( (gpio & ~BP_ACTIVE_MASK) >= 32 )
    {
        gpio_mask = GPIO_NUM_TO_MASK_HIGH(gpio);
        gpio_reg = &GPIO->GPIOio_high;
    }
	//printk("gpio=%04x,gpio_mask=%04x,gpio_reg=%04x\n",gpio,gpio_mask,*gpio_reg);
	if(*gpio_reg & gpio_mask)  //press down
		return RESET_BUTTON_UP;
	}
	return RESET_BUTTON_PRESSDOWN;
}
//<<JUNHON, 2004/09/15

/***************************************************************************
 * Dying gasp ISR and functions.
 ***************************************************************************/
#define KERSYS_DBG	printk

#if defined(CONFIG_BCM96345)
#define	CYCLE_PER_US	70
#elif defined(CONFIG_BCM96348) || defined(CONFIG_BCM96338)
/* The BCM6348 cycles per microsecond is really variable since the BCM6348
 * MIPS speed can vary depending on the PLL settings.  However, an appoximate
 * value of 120 will still work OK for the test being done.
 */
#define	CYCLE_PER_US	120
#endif
#define	DG_GLITCH_TO	(100*CYCLE_PER_US)
 
static void __init kerSysDyingGaspMapIntr()
{
    unsigned long ulIntr;
    	
#if defined(CONFIG_BCM96348) || defined(_BCM96348_) || defined(CONFIG_BCM96338) || defined(_BCM96338_)
    if( BpGetAdslDyingGaspExtIntr( &ulIntr ) == BP_SUCCESS ) {
		BcmHalMapInterrupt((FN_HANDLER)kerSysDyingGaspIsr, 0, INTERRUPT_ID_DG);
		BcmHalInterruptEnable( INTERRUPT_ID_DG );
    }
#elif defined(CONFIG_BCM96345) || defined(_BCM96345_)
    if( BpGetAdslDyingGaspExtIntr( &ulIntr ) == BP_SUCCESS ) {
        ulIntr += INTERRUPT_ID_EXTERNAL_0;
        BcmHalMapInterrupt((FN_HANDLER)kerSysDyingGaspIsr, 0, ulIntr);
        BcmHalInterruptEnable( ulIntr );
    }
#endif

} 

void kerSysSetWdTimer(ulong timeUs)
{
	TIMER->WatchDogDefCount = timeUs * (FPERIPH/1000000);
	TIMER->WatchDogCtl = 0xFF00;
	TIMER->WatchDogCtl = 0x00FF;
}

ulong kerSysGetCycleCount(void)
{
    ulong cnt; 
#ifdef _WIN32_WCE
    cnt = 0;
#else
    __asm volatile("mfc0 %0, $9":"=d"(cnt));
#endif
    return(cnt); 
}

static Bool kerSysDyingGaspCheckPowerLoss(void)
{
    ulong clk0;
    ulong ulIntr;

    ulIntr = 0;
    clk0 = kerSysGetCycleCount();

    UART->Data = 'D';
    UART->Data = '%';
    UART->Data = 'G';

#if defined(CONFIG_BCM96345)
    BpGetAdslDyingGaspExtIntr( &ulIntr );

    do {
        ulong clk1;
        
        clk1 = kerSysGetCycleCount();		/* time cleared */
	/* wait a little to get new reading */
        while ((kerSysGetCycleCount()-clk1) < CYCLE_PER_US*2)
            ;
    } while ((0 == (PERF->ExtIrqCfg & (1 << (ulIntr + EI_STATUS_SHFT)))) && ((kerSysGetCycleCount() - clk0) < DG_GLITCH_TO));

    if (PERF->ExtIrqCfg & (1 << (ulIntr + EI_STATUS_SHFT))) {	/* power glitch */
        BcmHalInterruptEnable( ulIntr + INTERRUPT_ID_EXTERNAL_0);
        KERSYS_DBG(" - Power glitch detected. Duration: %ld us\n", (kerSysGetCycleCount() - clk0)/CYCLE_PER_US);
        return 0;
    }
#elif (defined(CONFIG_BCM96348) || defined(CONFIG_BCM96338)) && !defined(VXWORKS)
    do {
        ulong clk1;
        
        clk1 = kerSysGetCycleCount();		/* time cleared */
	/* wait a little to get new reading */
        while ((kerSysGetCycleCount()-clk1) < CYCLE_PER_US*2)
            ;
     } while ((PERF->IrqStatus & (1 << (INTERRUPT_ID_DG - INTERNAL_ISR_TABLE_OFFSET))) && ((kerSysGetCycleCount() - clk0) < DG_GLITCH_TO));

    if (!(PERF->IrqStatus & (1 << (INTERRUPT_ID_DG - INTERNAL_ISR_TABLE_OFFSET)))) {
        BcmHalInterruptEnable( INTERRUPT_ID_DG );
        KERSYS_DBG(" - Power glitch detected. Duration: %ld us\n", (kerSysGetCycleCount() - clk0)/CYCLE_PER_US);
        return 0;
    }
#endif
    return 1;
}

static void kerSysDyingGaspShutdown( void )
{
    kerSysSetWdTimer(1000000);
#if defined(CONFIG_BCM96345)
    PERF->blkEnables &= ~(EMAC_CLK_EN | USB_CLK_EN | CPU_CLK_EN);
#elif defined(CONFIG_BCM96348)
    PERF->blkEnables &= ~(EMAC_CLK_EN | USBS_CLK_EN | USBH_CLK_EN | SAR_CLK_EN);
#endif
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0))
static irqreturn_t kerSysDyingGaspIsr(int irq, void * dev_id)
#else
static unsigned int kerSysDyingGaspIsr(void)
#endif
{	
    struct list_head *pos;
    CB_DGASP_LIST *tmp, *dsl = NULL;	

    if (kerSysDyingGaspCheckPowerLoss()) {        

        /* first to turn off everything other than dsl */        
        list_for_each(pos, &g_cb_dgasp_list_head->list) {    	
            tmp = list_entry(pos, CB_DGASP_LIST, list);
    	    if(strncmp(tmp->name, "dsl", 3)) {
    	        (tmp->cb_dgasp_fn)(tmp->context); 
    	    }else {
    		dsl = tmp;    		    	
    	    }       
        }  
        
        /* now send dgasp */
        if(dsl)
            (dsl->cb_dgasp_fn)(dsl->context); 

        /* reset and shutdown system */
        kerSysDyingGaspShutdown();
    }
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0))
return( IRQ_HANDLED );
#else
    return( 1 );
#endif
}

static void __init kerSysInitDyingGaspHandler( void )
{
    CB_DGASP_LIST *new_node;

    if( g_cb_dgasp_list_head != NULL) {
        printk("Error: kerSysInitDyingGaspHandler: list head is not null\n");
        return;	
    }
    new_node= (CB_DGASP_LIST *)kmalloc(sizeof(CB_DGASP_LIST), GFP_KERNEL);
    memset(new_node, 0x00, sizeof(CB_DGASP_LIST));
    INIT_LIST_HEAD(&new_node->list);    
    g_cb_dgasp_list_head = new_node; 
		
} /* kerSysInitDyingGaspHandler */

static void __exit kerSysDeinitDyingGaspHandler( void )
{
    struct list_head *pos;
    CB_DGASP_LIST *tmp; 
     	
    if(g_cb_dgasp_list_head == NULL)
        return;
        
    list_for_each(pos, &g_cb_dgasp_list_head->list) {    	
    	tmp = list_entry(pos, CB_DGASP_LIST, list);
        list_del(pos);
	kfree(tmp);
    }       

    kfree(g_cb_dgasp_list_head);	
    g_cb_dgasp_list_head = NULL;
    
} /* kerSysDeinitDyingGaspHandler */

void kerSysRegisterDyingGaspHandler(char *devname, void *cbfn, void *context)
{
    CB_DGASP_LIST *new_node;

    if( g_cb_dgasp_list_head == NULL) {
        printk("Error: kerSysRegisterDyingGaspHandler: list head is null\n");	
        return;    
    }
    
    if( devname == NULL || cbfn == NULL ) {
        printk("Error: kerSysRegisterDyingGaspHandler: register info not enough (%s,%x,%x)\n", devname, (unsigned int)cbfn, (unsigned int)context);	    	
        return;
    }
       
    new_node= (CB_DGASP_LIST *)kmalloc(sizeof(CB_DGASP_LIST), GFP_KERNEL);
    memset(new_node, 0x00, sizeof(CB_DGASP_LIST));    
    INIT_LIST_HEAD(&new_node->list);
    strncpy(new_node->name, devname, IFNAMSIZ);
    new_node->cb_dgasp_fn = (cb_dgasp_t)cbfn;
    new_node->context = context;
    list_add(&new_node->list, &g_cb_dgasp_list_head->list);
    
    printk("dgasp: kerSysRegisterDyingGaspHandler: %s registered \n", devname);
        	
} /* kerSysRegisterDyingGaspHandler */

void kerSysDeregisterDyingGaspHandler(char *devname)
{
    struct list_head *pos;
    CB_DGASP_LIST *tmp;    
    
    if(g_cb_dgasp_list_head == NULL) {
        printk("Error: kerSysDeregisterDyingGaspHandler: list head is null\n");
        return;	
    }

    if(devname == NULL) {
        printk("Error: kerSysDeregisterDyingGaspHandler: devname is null\n");
        return;	
    }
    
    printk("kerSysDeregisterDyingGaspHandler: %s is deregistering\n", devname);

    list_for_each(pos, &g_cb_dgasp_list_head->list) {    	
    	tmp = list_entry(pos, CB_DGASP_LIST, list);
    	if(!strcmp(tmp->name, devname)) {
            list_del(pos);
	    kfree(tmp);
	    printk("kerSysDeregisterDyingGaspHandler: %s is deregistered\n", devname);
	    return;
	}
    }	
    printk("kerSysDeregisterDyingGaspHandler: %s not (de)registered\n", devname);
	
} /* kerSysDeregisterDyingGaspHandler */

//EXPORT_SYMBOL(kerSysNvRamGet);
EXPORT_SYMBOL(kerSysGetMacAddress);
EXPORT_SYMBOL(kerSysReleaseMacAddress);
EXPORT_SYMBOL(kerSysGetSdramSize);
EXPORT_SYMBOL(kerSysLedCtrl);
EXPORT_SYMBOL(kerSysGetResetHold);
EXPORT_SYMBOL(kerSysLedRegisterHwHandler);
EXPORT_SYMBOL(BpGetBoardIds);
EXPORT_SYMBOL(BpGetSdramSize);
EXPORT_SYMBOL(BpGetPsiSize);
EXPORT_SYMBOL(BpGetEthernetMacInfo);
EXPORT_SYMBOL(BpGetRj11InnerOuterPairGpios);
EXPORT_SYMBOL(BpGetPressAndHoldResetGpio);
EXPORT_SYMBOL(BpGetVoipResetGpio);
EXPORT_SYMBOL(BpGetVoipIntrGpio);
EXPORT_SYMBOL(BpGetPcmciaResetGpio);
EXPORT_SYMBOL(BpGetRtsCtsUartGpios);
EXPORT_SYMBOL(BpGetAdslLedGpio);
EXPORT_SYMBOL(BpGetAdslFailLedGpio);
EXPORT_SYMBOL(BpGetWirelessLedGpio);
EXPORT_SYMBOL(BpGetUsbLedGpio);
EXPORT_SYMBOL(BpGetHpnaLedGpio);
EXPORT_SYMBOL(BpGetWanDataLedGpio);
EXPORT_SYMBOL(BpGetPppLedGpio);
EXPORT_SYMBOL(BpGetPppFailLedGpio);
EXPORT_SYMBOL(BpGetVoipLedGpio);
EXPORT_SYMBOL(BpGetWirelessExtIntr);
EXPORT_SYMBOL(BpGetAdslDyingGaspExtIntr);
EXPORT_SYMBOL(BpGetVoipExtIntr);
EXPORT_SYMBOL(BpGetHpnaExtIntr);
EXPORT_SYMBOL(BpGetHpnaChipSelect);
EXPORT_SYMBOL(BpGetVoipChipSelect);
EXPORT_SYMBOL(BpGetWirelessSesBtnGpio);
EXPORT_SYMBOL(BpGetWirelessSesExtIntr);
EXPORT_SYMBOL(BpGetWirelessSesLedGpio);
EXPORT_SYMBOL(kerSysRegisterDyingGaspHandler);
EXPORT_SYMBOL(kerSysDeregisterDyingGaspHandler);
EXPORT_SYMBOL(kerSysGetCycleCount);
EXPORT_SYMBOL(kerSysSetWdTimer);
EXPORT_SYMBOL(kerSysWakeupMonitorTask);

