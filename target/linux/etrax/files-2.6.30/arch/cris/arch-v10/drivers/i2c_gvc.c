/*!***************************************************************************
*!
*! FILE NAME  : i2c.c
*!
*!
*! ---------------------------------------------------------------------------
*!
*! ( C ) Copyright 1999-2002 Axis Communications AB, LUND, SWEDEN
*!
*!***************************************************************************/

/******************** INCLUDE FILES SECTION ****************************/

#include <linux/module.h>
#include <linux/fs.h>

#ifdef CONFIG_ETRAX_I2C_DYN_ALLOC
#include <linux/types.h> /* for dev_t */
#include <linux/cdev.h>  /* for struct cdev */
#endif

#include <linux/device.h>

#include "etraxi2c.h"

#include "i2c_errno.h"

#include <asm/io.h>
#include <asm/delay.h>
#include <asm/arch/io_interface_mux.h>
#include <asm/uaccess.h>

#include "i2c_gvc.h"

MODULE_DESCRIPTION( "I2C Device Driver - 2.3" );

/*!*********************************************************************
 *!History I2C driver Geert Vancompernolle
 *!---------------------------------------
 *!
 *! - v1.0:
 *!     First official version.
 *!
 *! - v1.1:
 *!     Changes to remove unwanted spikes at ACK/NACK time.
 *!
 *!*********************************************************************/

MODULE_LICENSE( "GPL" );

/******************            MACRO's            **********************/

#define D( x )

#ifndef CONFIG_ETRAX_I2C_DYN_ALLOC
#define I2C_MAJOR 123  				/* LOCAL/EXPERIMENTAL */
#endif

#define WAITONEUS                 1
/* Following are abbreviations taken from Philips I2C standard */
/* Values are representing time in us and are rounded to next whole number, if relevant */
#define THDSTA                    4     /* Hold delay time for (repeated) START condition */
#define TLOW                      5     /* LOW period of the SCL clock */
#define THDDAT	                  1     /* Hold delay time for DATA: value of 0 is allowed but 1 taken to be sure */
#define TSUDAT                    1     /* Set-up time for DATA */
#define THIGH                     4     /* HIGH period of the SCL clock */
#define TSUSTA                    5     /* Set-up time for a repeated START condition */
#define TSUSTO                    4     /* Set-up time for STOP condition */
#define TBUF                      5     /* Bus-free time between STOP and START condition */

#ifdef CONFIG_ETRAX_I2C_SLAVE_DELAY
#define MAXSCLRETRIES		  100
#endif

#define MAXBUSFREERETRIES         5
#define MAXRETRIES                3
#define WRITEADDRESS_MASK         ( 0xFE )
#define READADDRESS_MASK          ( 0x01 )

#define SCL_HIGH                  1
#define SCL_LOW                   0
#define SDA_HIGH                  1
#define SDA_LOW                   0

#ifdef CONFIG_ETRAX_I2C_USES_PB_NOT_PB_I2C
/* Use PB and not PB_I2C */
#ifndef CONFIG_ETRAX_I2C_DATA_PORT
#define CONFIG_ETRAX_I2C_DATA_PORT 0
#endif
#ifndef CONFIG_ETRAX_I2C_CLK_PORT
#define CONFIG_ETRAX_I2C_CLK_PORT 1
#endif

#define SDABIT CONFIG_ETRAX_I2C_DATA_PORT
#define SCLBIT CONFIG_ETRAX_I2C_CLK_PORT
#define i2c_enable()
#define i2c_disable()

/* enable or disable output-enable, to select output or input on the i2c bus */
#define i2c_sda_dir_out() \
  REG_SHADOW_SET( R_PORT_PB_DIR, port_pb_dir_shadow, SDABIT, 1 )
#define i2c_sda_dir_in()  \
  REG_SHADOW_SET( R_PORT_PB_DIR, port_pb_dir_shadow, SDABIT, 0 )

#ifdef CONFIG_ETRAX_I2C_SLAVE_DELAY
#define i2c_scl_dir_out() \
  REG_SHADOW_SET( R_PORT_PB_DIR, port_pb_dir_shadow, SCLBIT, 1 )
#define i2c_scl_dir_in()  \
  REG_SHADOW_SET( R_PORT_PB_DIR, port_pb_dir_shadow, SCLBIT, 0 )
#endif

/* control the i2c clock and data signals */
#define i2c_set_scl( x ) \
  REG_SHADOW_SET( R_PORT_PB_DATA, port_pb_data_shadow, SCLBIT, x )
#define i2c_set_sda( x ) \
  REG_SHADOW_SET( R_PORT_PB_DATA, port_pb_data_shadow, SDABIT, x )

/* read status of SDA bit from the i2c interface */
#define i2c_sda_is_high() ( ( ( *R_PORT_PB_READ & ( 1 << SDABIT ) ) ) >> SDABIT )

/* read status of SCL bit from the i2c interface */
#define i2c_scl_is_high() ( ( ( *R_PORT_PB_READ & ( 1 << SCLBIT ) ) ) >> SCLBIT )

#else
/* enable or disable the i2c interface */
#define i2c_enable() *R_PORT_PB_I2C = ( port_pb_i2c_shadow |= IO_MASK( R_PORT_PB_I2C, i2c_en ) )
#define i2c_disable() *R_PORT_PB_I2C = ( port_pb_i2c_shadow &= ~IO_MASK( R_PORT_PB_I2C, i2c_en ) )

/* enable or disable output-enable, to select output or input on the i2c bus */
#define i2c_sda_dir_out() \
	*R_PORT_PB_I2C = ( port_pb_i2c_shadow &= ~IO_MASK( R_PORT_PB_I2C, i2c_oe_ ) ); \
	REG_SHADOW_SET( R_PORT_PB_DIR, port_pb_dir_shadow, 0, 1 );
#define i2c_sda_dir_in() \
	*R_PORT_PB_I2C = ( port_pb_i2c_shadow |= IO_MASK( R_PORT_PB_I2C, i2c_oe_ ) ); \
	REG_SHADOW_SET( R_PORT_PB_DIR, port_pb_dir_shadow, 0, 0 );

/* control the i2c clock and data signals */
#define i2c_set_scl( x ) \
	*R_PORT_PB_I2C = ( port_pb_i2c_shadow = ( port_pb_i2c_shadow & \
       ~IO_MASK( R_PORT_PB_I2C, i2c_set_scl ) ) | IO_FIELD( R_PORT_PB_I2C, i2c_set_scl, ( x ) ) ); \
       REG_SHADOW_SET( R_PORT_PB_DATA, port_pb_data_shadow, 1, x );

#define i2c_set_sda( x ) \
	*R_PORT_PB_I2C = ( port_pb_i2c_shadow = ( port_pb_i2c_shadow & \
	   ~IO_MASK( R_PORT_PB_I2C, i2c_d ) ) | IO_FIELD( R_PORT_PB_I2C, i2c_d, ( x ) ) ); \
	REG_SHADOW_SET( R_PORT_PB_DATA, port_pb_data_shadow, 0, x );

/* read a bit from the i2c interface */
#define i2c_sda_is_high() ( *R_PORT_PB_READ & 0x1 )
#endif

/* use the kernels delay routine */
#define i2c_delay( usecs ) udelay( usecs )


/******************           TYPEDEF's           **********************/


/****************** STATIC (file scope) VARIABLES **********************/
static DEFINE_SPINLOCK( i2c_lock ); /* Protect directions etc */
static const char i2c_name[] = "i2c";



/******************     PROTOTYPING SECTION     *************************/
static int  i2c_open( struct inode *inode, struct file *filp );
static int  i2c_release( struct inode *inode, struct file *filp );
static int  i2c_command( unsigned char  slave
                       , unsigned char* wbuf
                       , unsigned char  wlen
                       , unsigned char* rbuf
                       , unsigned char  rlen
                       );
static int  i2c_bus_free_check( unsigned char maxretries );
static void i2c_finalise( const char* text, unsigned long irqflags );


/************************************************************************/
/******************         AUXILIARIES         *************************/
/************************************************************************/

/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_open
 *#
 *# DESCRIPTION  : opens an I2C device
 *#
 *# PARAMETERS   : *inode: reference to inode
 *#                *filp : reference to file pointer
 *#
 *#---------------------------------------------------------------------------
 */
static int i2c_open( struct inode *inode, struct file *filp )
{
    return 0;
}   /* i2c_open */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_release
 *#
 *# DESCRIPTION  : Releases the I2C device
 *#
 *# PARAMETERS   : *inode: reference to inode
 *#                *filp : reference to file pointer
 *#
 *#---------------------------------------------------------------------------
 */
static int i2c_release( struct inode *inode, struct file *filp )
{
    return 0;
}   /* i2c_release */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_ioctl
 *#
 *# DESCRIPTION  : Main device API: ioctl's to write/read
 *#                to/from i2c registers
 *#
 *# PARAMETERS   : *inode: reference to inode
 *#                *filp : reference to file pointer
 *#                cmd   : command to be executed during the ioctl call
 *#                arg   : pointer to a structure with the data???
 *#
 *# RETURN       : result of the ioctl call
 *#
 *#---------------------------------------------------------------------------
 */
static int i2c_ioctl( struct inode *inode
                    , struct file *file
                    , unsigned int cmd
                    , unsigned long arg
                    )
{
    /* the acme ioctls */
    I2C_DATA i2cdata;
    int RetVal = EI2CNOERRORS;

    if ( _IOC_TYPE( cmd ) != ETRAXI2C_IOCTYPE )
    {
        return ( -EINVAL );
    }

    switch ( _IOC_NR( cmd ) )
    {
    case I2C_WRITEREG:
        /* write to an i2c slave */
        RetVal = i2c_writereg( I2C_ARGSLAVE( arg )
                             , I2C_ARGREG( arg )
                             , I2C_ARGVALUE( arg )
                             );
        break;

    case I2C_READREG:
        RetVal = i2c_readreg( I2C_ARGSLAVE( arg ), I2C_ARGREG( arg ) );
        break;

    /* New functions added by GVC */
    case I2C_READ:
        copy_from_user( (char*)&i2cdata, (char*)arg, sizeof( I2C_DATA ) );
        {
            int RetryCntr = MAXRETRIES;

            do
            {
                RetVal = i2c_command( i2cdata.slave
                                    , NULL
                                    , 0
                                    , i2cdata.rbuf
                                    , i2cdata.rlen
                                    );
             } while ( ( EI2CNOERRORS != RetVal )
                     &&( --RetryCntr )
                     );
        }
        copy_to_user( (char*)arg, (char*)&i2cdata, sizeof( I2C_DATA ) );
        break;

    case I2C_WRITE:
        copy_from_user( (char*)&i2cdata, (char*)arg, sizeof( I2C_DATA ) );
        {
            int RetryCntr = MAXRETRIES;

            do
            {
                RetVal = i2c_command( i2cdata.slave
                                    , i2cdata.wbuf
                                    , i2cdata.wlen
                                    , NULL
                                    , 0
                                    );
             } while ( ( EI2CNOERRORS != RetVal )
                     &&( --RetryCntr )
                     );
        }
        break;

    case I2C_WRITEREAD:
        copy_from_user( (char*)&i2cdata, (char*)arg, sizeof( I2C_DATA ) );
        {
            int RetryCntr = MAXRETRIES;

            do
            {
                RetVal = i2c_command( i2cdata.slave
                                    , i2cdata.wbuf
                                    , i2cdata.wlen
                                    , i2cdata.rbuf
                                    , i2cdata.rlen
                                    );
             } while ( ( EI2CNOERRORS != RetVal )
                     &&( --RetryCntr )
                     );
        }
        copy_to_user( (char*)arg, (char*)&i2cdata, sizeof( I2C_DATA ) );
        break;

    default:
        RetVal = -EINVAL;
    }

    return ( -RetVal );
}   /* i2c_ioctl */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_command
 *#
 *# DESCRIPTION  : general routine to read/write bytes from an I2C device
 *#
 *#                'i2c_command()' sends wlen bytes to the I2c bus and receives
 *#                rlen bytes from the I2c bus.
 *#                The data to be send must be placed in wbuf[ 0 ] upto wbuf[ wlen - 1 ).
 *#                The data to be received is assembled in rbuf[ 0 ] upto rbuf[ rlen - 1 ].
 *#
 *#                If no data is to be sent or received, put appropriate buffer parameter
 *#                to "NULL" and appropriate length parameter to "0".
 *#
 *# PARAMETERS   : slave = slave address of the I2C device
 *#                wbuf  = address of first element of write buffer (wbuf)
 *#                wlen  = number of bytes to be written to slave
 *#                rbuf  = address of first element of read buffer (rbuf)
 *#                rlen  = number of bytes to be read from slave
 *#
 *# RETURN       :
 *#    EI2CNOERRORS: I2C communication went fine
 *#    EI2CBUSNFREE: I2C bus is not free
 *#    EI2CWADDRESS: I2C write address failed
 *#    EI2CRADDRESS: I2C read address failed
 *#    EI2CSENDDATA: I2C send data failed
 *#    EI2CRECVDATA: I2C receive data failed
 *#    EI2CSTRTCOND: I2C start condition failed
 *#    EI2CRSTACOND: I2C repeated start condition failed
 *#    EI2CSTOPCOND: I2C stop condition failed
 *#    EI2CNOSNDBYT: I2C no bytes to be sent
 *#    EI2CNOSNDBUF: I2C no send buffer defined
 *#    EI2CNORCVBYT: I2C no bytes to be received
 *#    EI2CNORCVBUF: I2C no receive buffer defined
 *#    EI2CNOACKNLD: I2C no acknowledge received
 *#
 *# REMARK       :
 *#   First, the send part is completed.
 *#   In the send routine, there is no stop generated.  This is because maybe
 *#   a repeated start condition must be generated.
 *#   This happens when we want to receive some data from the I2c bus.  If not,
 *#   at the end of the general I2c loop the stopcondition is generated.
 *#   If, on the contrary, there are a number of bytes to be received, a new
 *#   startcondition is generated in the 'if' part of the main I2c routine,
 *#   which controls the receiving part.
 *#   Only when the receiving of data is finished, a final stopcondition is
 *#   generated.
 *#
 *#---------------------------------------------------------------------------
 */
static int i2c_command( unsigned char  slave
                      , unsigned char* wbuf
                      , unsigned char  wlen
                      , unsigned char* rbuf
                      , unsigned char  rlen
                      )
{
    /* Check arguments and report error if relevant... */
    if ( ( wlen > 0 ) && ( wbuf == NULL ) )
    {
        printk( KERN_DEBUG "I2C: EI2CNOSNDBUF\n" );
        return ( EI2CNOSNDBUF );
    }
    else if ( ( wlen == 0 ) && ( wbuf != NULL ) )
    {
        printk( KERN_DEBUG "I2C: EI2CNOSNDBYT\n" );
        return ( EI2CNOSNDBYT );
    }
    else if ( ( rlen > 0 ) && ( rbuf == NULL ) )
    {
        printk( KERN_DEBUG "I2C: EI2CNORCVBUF\n" );
        return ( EI2CNORCVBUF );
    }
    else if ( ( rlen == 0 ) && ( rbuf != NULL ) )
    {
        printk( KERN_DEBUG "I2C: EI2CNORCVBYT\n" );
        return ( EI2CNORCVBYT );
    }
    else if ( EI2CBUSNFREE == i2c_bus_free_check( MAXBUSFREERETRIES ) )
    {
        /* There's no need to try more, since we weren't even
         * able to start the I2C communication.
         * So, no IRQ flags are stored yet, no changes to any other
         * stuff like START, STOP, SENDBYTES...
         * Result, simply write down the error and return the correct error code.
         */
        printk( KERN_DEBUG "I2C: EI2CBUSNFREE\n" );
        return ( EI2CBUSNFREE );
    }
    else
    {
        /* Finally... We made it... */
        unsigned long irqflags = 0;

        /* we don't like to be interrupted */
        local_irq_save( irqflags );

        /* Check if there are bytes to be send,
         * or if you immediately want to receive data.
         */
        if ( 0 < wlen )
        {
            /* start I2C communication */
            if ( EI2CNOERRORS != i2c_start() )
            {
                return ( i2c_finalise( "I2C: EI2CSTRTCOND\n", irqflags  )
                       , EI2CSTRTCOND
                       );
            }

            /* send slave address: xxxxxxx0B (last bit must be zero) */
            if ( EI2CNOERRORS != i2c_outbyte( slave & WRITEADDRESS_MASK ) )
            {
                return ( i2c_finalise( "I2C: EI2CWADDRESS\n", irqflags  )
                       , EI2CWADDRESS
                       );
            }

            while ( wlen-- )
            {
                /* send register data */
                if ( EI2CNOERRORS != i2c_outbyte( *wbuf ) && wlen )
                {
                    return ( i2c_finalise( "I2C: EI2CSENDDATA\n", irqflags  )
                           , EI2CSENDDATA
                           );
                }

                wbuf++;
            };

            i2c_delay( TLOW );
        }

        /*
         * Receiving data from I2c_bus
         * If there are bytes to be received, a new start condition is
         * generated => Repeated Startcondition.
         * A final stopcondition is generated at the end of the main I2c
         * routine.
         */
        if ( 0 < rlen )
        {
            /*
             * Generate start condition if wlen == 0
             * or repeated start condition if wlen != 0...
             */
            if ( EI2CNOERRORS != i2c_start() )
            {
                return ( i2c_finalise( ( ( 0 < wlen )
                                       ? "I2C: EI2CRSTACOND\n"
                                       : "I2C: EI2CSTRTCOND\n"
                                       )
                                     , irqflags
                                     )
                       , ( ( 0 < wlen ) ? EI2CRSTACOND : EI2CSTRTCOND )
                       );
            }

            /* Send ReadAddress: xxxxxxx1B (last bit must be one) */
            if ( EI2CNOERRORS != i2c_outbyte( slave | READADDRESS_MASK ) )
            {
                return ( i2c_finalise( "I2C: EI2CRADDRESS\n", irqflags )
                       , EI2CRADDRESS
                       );
            }

            while ( rlen-- )
            {
                /* fetch register */
                *rbuf = i2c_inbyte();
                rbuf++;

                /* last received byte needs to be NACK-ed instead of ACK-ed */
                if ( rlen )
                {
                    i2c_sendack();
                }
                else
                {
                    i2c_sendnack();
                }
            };
        }

        /* Generate final stop condition */
        if ( EI2CNOERRORS != i2c_stop() )
        {
            return ( i2c_finalise( "I2C CMD: EI2CSTOPCOND\n", irqflags )
                   , EI2CSTOPCOND
                   );
        }

        /* enable interrupt again */
        local_irq_restore( irqflags );
    }

    return ( EI2CNOERRORS );
} /*  i2c_command */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_bus_free_check
 *#
 *# DESCRIPTION  : checks if the I2C bus is free before starting
 *#                an I2C communication
 *#
 *# PARAMETERS   : maxretries, the number of times we will try to release
 *#                the I2C bus
 *#
 *# RETURN       : I2cStatus_I2cBusNotFreeError in case the bus is not free,
 *#                I2cStatus_I2cNoError otherwise
 *#
 *#---------------------------------------------------------------------------
 */
static int i2c_bus_free_check( unsigned char maxretries )
{
    i2c_sda_dir_in();        /* Release SDA line */
    i2c_set_scl( SCL_HIGH ); /* put SCL line high */

    i2c_delay( WAITONEUS );

    while ( ( !i2c_sda_is_high() || !i2c_scl_is_high() )
          &&( maxretries-- )
          )
    {
        /* Try to release I2C bus by generating STOP conditions */
        i2c_stop();
    }

    if ( 0 == maxretries )
    {
        printk( KERN_DEBUG "I2C: EI2CBUSNFREE\n" );
        return ( EI2CBUSNFREE );
    }
    else
    {
        return ( EI2CNOERRORS );
    }
} /* i2c_bus_free_check */


static void i2c_finalise( const char* errortxt
                        , unsigned long irqflags
                        )
{
    printk( KERN_DEBUG "%s", errortxt );
    local_irq_restore( irqflags );
    /* The least we can do when things go terribly wrong,
     * is to try to release the bus.
     * If this fails, well, then I don't know
     * what I can do more for the moment...
     */
    (void)i2c_bus_free_check( MAXBUSFREERETRIES );
}   /* i2c_finalise */


static struct file_operations i2c_fops =
{
    .owner    = THIS_MODULE
,   .ioctl    = i2c_ioctl
,   .open     = i2c_open
,   .release  = i2c_release
};


/***********************************************************************/
/************* EXTERNAL FUNCTION DEFINITION SECTION ********************/
/***********************************************************************/

/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_init
 *#
 *# DESCRIPTION  : initialises the I2C device driver
 *#
 *# PARAMETERS   :
 *#
 *#---------------------------------------------------------------------------
 */
int __init i2c_init( void )
{
    static int res = 0;
    static int first = 1;

    if ( !first )
    {
        return res;
    }

    first = 0;

    /* Setup and enable the Port B I2C interface */

#ifndef CONFIG_ETRAX_I2C_USES_PB_NOT_PB_I2C
    /* here, we're using the dedicated I2C pins of FoxBoard */
    if ( ( res = cris_request_io_interface( if_i2c, "I2C" ) ) )
    {
        printk( KERN_CRIT "i2c_init: Failed to get IO interface\n" );
        return res;
    }

    *R_PORT_PB_I2C = port_pb_i2c_shadow |=
        IO_STATE( R_PORT_PB_I2C, i2c_en,  on ) |
        IO_FIELD( R_PORT_PB_I2C, i2c_d,   1 )  |
        IO_FIELD( R_PORT_PB_I2C, i2c_set_scl, 1 )  |
        IO_STATE( R_PORT_PB_I2C, i2c_oe_, enable );

    port_pb_dir_shadow &= ~IO_MASK( R_PORT_PB_DIR, dir0 );
    port_pb_dir_shadow &= ~IO_MASK( R_PORT_PB_DIR, dir1 );

    *R_PORT_PB_DIR = ( port_pb_dir_shadow |=
              IO_STATE( R_PORT_PB_DIR, dir0, input )  |
              IO_STATE( R_PORT_PB_DIR, dir1, output ) );
#else
        /* If everything goes fine, res = 0, meaning "if" fails =>
         * will do the "else" too and as such initialise the clock port...
         * Clever trick!
         */
        if ( ( res = cris_io_interface_allocate_pins( if_i2c
                                                    , 'b'
                                                    , CONFIG_ETRAX_I2C_DATA_PORT
                                                    , CONFIG_ETRAX_I2C_DATA_PORT
                                                    )
             )
           )
        {
            printk( KERN_WARNING "i2c_init: Failed to get IO pin for I2C data port\n" );
            return ( res );
        }
        /* Same here...*/
        else if ( ( res = cris_io_interface_allocate_pins( if_i2c
                                                         , 'b'
                                                         , CONFIG_ETRAX_I2C_CLK_PORT
                                                         , CONFIG_ETRAX_I2C_CLK_PORT
                                                         )
                  )
                )
        {
            cris_io_interface_free_pins( if_i2c
                                       , 'b'
                                       , CONFIG_ETRAX_I2C_DATA_PORT
                                       , CONFIG_ETRAX_I2C_DATA_PORT
                                       );
            printk( KERN_WARNING "i2c_init: Failed to get IO pin for I2C clk port\n" );
        }
#endif

    return ( res );
}   /* i2c_init */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_register
 *#
 *# DESCRIPTION  : this registers the i2c driver as a character device
 *#
 *# PARAMETERS   :
 *#
 *#---------------------------------------------------------------------------
 */

static struct class *i2c_class;

static int __init i2c_register( void )
{
    int res;
#ifdef CONFIG_ETRAX_I2C_DYN_ALLOC
    dev_t devt;
    struct cdev *my_i2cdev = NULL;
#endif

    res = i2c_init();

    if ( res < 0 )
    {
        return res;
    }

#ifdef CONFIG_ETRAX_I2C_DYN_ALLOC
    res = alloc_chrdev_region( &devt, 0, 1, i2c_name );

    if ( res < 0 )
    {
        printk( KERN_DEBUG "I2C: EI2CNOMNUMBR\n" );
        return ( res );
    }

    my_i2cdev = cdev_alloc();
    my_i2cdev->ops = &i2c_fops;
    my_i2cdev->owner = THIS_MODULE;

    /* make device "alive" */
    res = cdev_add( my_i2cdev, devt, 1 );

    if ( res < 0 )
    {
        printk( KERN_DEBUG "I2C: EI2CDADDFAIL\n" );
        return ( res );
    }

    int i2c_major = MAJOR( devt );
#else
    res = register_chrdev( I2C_MAJOR, i2c_name, &i2c_fops );

    if ( res < 0 )
    {
        printk( KERN_ERR "i2c: couldn't get a major number.\n" );
        return res;
    }
   
    int i2c_major = I2C_MAJOR;
#endif

    printk( KERN_INFO "I2C: driver v2.3, (c) 1999-2004 Axis Communications AB\n" );
    printk( KERN_INFO "I2C: Improvements by Geert Vancompernolle, Positive Going, BK srl\n" );

#ifdef CONFIG_ETRAX_I2C_SLAVE_DELAY
    printk( KERN_INFO "I2C: with master/slave delay patch\n" );
#endif

    i2c_class = class_create (THIS_MODULE, "i2c_etrax");
    device_create (i2c_class, NULL,
		   MKDEV(i2c_major,0), NULL, i2c_name);

    return ( 0 );
}   /* i2c_register */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_start
 *#
 *# DESCRIPTION  : generate i2c start condition
 *#
 *# PARAMETERS   : none
 *#
 *# RETURN       : EI2CNOERRORS if OK, EI2CSTRTCOND otherwise
 *#
 *#---------------------------------------------------------------------------
 */
int i2c_start( void )
{
  /* Set SCL=1, SDA=1 */
  i2c_sda_dir_out();
  i2c_set_sda( SDA_HIGH );
  i2c_delay( WAITONEUS );
  i2c_set_scl( SCL_HIGH );
  i2c_delay( WAITONEUS );
  
  /* Set SCL=1, SDA=0 */
  i2c_set_sda( SDA_LOW );
  i2c_delay( THDSTA );
  
  /* Set SCL=0, SDA=0 */
  i2c_set_scl( SCL_LOW );
  /* We can take 1 us less than defined in spec (5 us), since the next action
   * will be to set the dataline high or low and this action is 1 us
   * before the clock is put high, so that makes our 5 us.
   */
  i2c_delay( TLOW - WAITONEUS );
  
  if ( i2c_sda_is_high() || i2c_scl_is_high() )
    {
      printk( KERN_DEBUG "I2C: EI2CSTRTCOND\n" );
      return ( EI2CSTRTCOND );
    }
  
  return ( EI2CNOERRORS );
}   /* i2c_start */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_stop
 *#
 *# DESCRIPTION  : generate i2c stop condition
 *#
 *# PARAMETERS   : none
 *#
 *# RETURN       : none
 *#
 *#---------------------------------------------------------------------------
 */
int i2c_stop( void )
{
#ifdef CONFIG_ETRAX_I2C_SLAVE_DELAY
    int n=MAXSCLRETRIES;
#endif
    i2c_sda_dir_out();
    
    /* Set SCL=0, SDA=0 */
    /* Don't change order, otherwise you might generate a start condition! */
    i2c_set_scl( SCL_LOW );
    i2c_delay( WAITONEUS );
    i2c_set_sda( SDA_LOW );
    i2c_delay( WAITONEUS );
    
    /* Set SCL=1, SDA=0 */
    
#ifdef CONFIG_ETRAX_I2C_SLAVE_DELAY
    i2c_set_scl( SCL_HIGH );
    i2c_scl_dir_in();
    for( ; n>0; n-- )
      {
	if( i2c_scl_is_high() )
	  break;
	i2c_delay( TSUSTO );
      }
    
    i2c_scl_dir_out();
#else
    i2c_set_scl( SCL_HIGH );
#endif
    i2c_delay( TSUSTO );
    
    /* Set SCL=1, SDA=1 */
    i2c_set_sda( SDA_HIGH );
    i2c_delay( TBUF );
    
    i2c_sda_dir_in();
    
    if ( !i2c_sda_is_high() || !i2c_scl_is_high() )
      {
	return ( EI2CSTOPCOND );
      }
    
    return ( EI2CNOERRORS );
}   /* i2c_stop */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_outbyte
 *#
 *# DESCRIPTION  : write a byte to the i2c interface
 *#
 *# PARAMETERS   : x: byte to be sent on the I2C bus
 *#
 *# RETURN       : none
 *#
 *#---------------------------------------------------------------------------
 */
int i2c_outbyte( unsigned char x )
{
    int i;

    i2c_sda_dir_out();

    for ( i = 0; i < 8; i++ )
    {
        if ( x & 0x80 )
        {
            i2c_set_sda( SDA_HIGH );
        }
        else
        {
            i2c_set_sda( SDA_LOW );
        }

        i2c_delay( TSUDAT );
        i2c_set_scl( SCL_HIGH );
        i2c_delay( THIGH );
        i2c_set_scl( SCL_LOW );
        i2c_delay( TSUDAT );
        i2c_set_sda( SDA_LOW );
        /* There should be only 5 us between falling edge and new rising
         * edge of clock pulse.
         * Since we spend already 1 us since clock edge was low, there are
         * only ( TLOW - TSUDAT ) us left.
         * Next to this, since the data line will be set up 1 us before the
         * clock line is set up, we can reduce the delay with another us.
         */
        i2c_delay( TLOW - TSUDAT - WAITONEUS );
        x <<= 1;
    }

    /* enable input */
    i2c_sda_dir_in();

    if ( !i2c_getack() )
      {
        return( EI2CNOACKNLD );
      }

    return ( EI2CNOERRORS );
}   /* i2c_outbyte */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_inbyte
 *#
 *# DESCRIPTION  : read a byte from the i2c interface
 *#
 *# PARAMETERS   : none
 *#
 *# RETURN       : returns the byte read from the I2C device
 *#
 *#---------------------------------------------------------------------------
 */
unsigned char i2c_inbyte( void )
{
#ifdef CONFIG_ETRAX_I2C_SLAVE_DELAY
    int n=MAXSCLRETRIES;
#endif
    unsigned char aBitByte = 0;
    unsigned char Mask     = 0x80;    /* !!! ATTENTION: do NOT use 'char', otherwise shifting is wrong!!! */
                                      /* Must be UNSIGNED, not SIGNED! */


    /* Switch off I2C to get bit */
    i2c_disable();
    i2c_sda_dir_in();

    while ( Mask != 0 )
    {
#ifdef CONFIG_ETRAX_I2C_SLAVE_DELAY
	i2c_scl_dir_in();
	for( ; n>0; n-- )
	{	
		if( i2c_scl_is_high() )
			break;
		i2c_delay( THIGH );
	}

        i2c_set_scl( SCL_HIGH );
	i2c_scl_dir_out();
#else
        i2c_set_scl( SCL_HIGH );
#endif
        i2c_delay( THIGH );

        if ( i2c_sda_is_high() )
        {
            aBitByte |= Mask;
        }

        i2c_set_scl( SCL_LOW );

        Mask >>= 1;

        i2c_delay( TLOW );
    }

    /*
     * we leave the clock low, getbyte is usually followed
     * by sendack/nack, they assume the clock to be low
     */
    return ( aBitByte );
}   /* i2c_inbyte */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_getack
 *#
 *# DESCRIPTION  : checks if ack was received from ic2
 *#
 *# PARAMETERS   : none
 *#
 *# RETURN       : returns the ack state of the I2C device
 *#
 *#---------------------------------------------------------------------------
 */
int i2c_getack( void )
{
    int ack = 1;
#ifdef CONFIG_ETRAX_I2C_SLAVE_DELAY
    int n=MAXSCLRETRIES;
#endif

    /* generate ACK clock pulse */
    i2c_set_scl( SCL_HIGH );

    /* switch off I2C */
    i2c_disable();

#ifdef CONFIG_ETRAX_I2C_SLAVE_DELAY
    /* set clock low */
    i2c_set_scl( SCL_LOW );

    /* now wait for ack */
    i2c_delay( THIGH );

    /* set clock as input */
    i2c_scl_dir_in();

    /* wait for clock to rise (n=MAXSCLRETRIES) */
    for( ; n>0; n-- )
    {
	if( i2c_scl_is_high() )
            break;
	i2c_delay( THIGH );
    }

    i2c_set_scl( SCL_HIGH );

    i2c_scl_dir_out();

    i2c_delay( THIGH );
#else
    /* now wait for ack */
    i2c_delay( THIGH );
#endif

    /* check for ack: if SDA is high, then NACK, else ACK */
    if ( i2c_sda_is_high() )
    {
        ack = 0;
    }
    else
    {
        ack = 1;
    }

    /* end clock pulse */
    i2c_enable();
    i2c_set_scl( SCL_LOW );
    i2c_sda_dir_out();
    i2c_set_sda( SDA_LOW );

    /* Since we "lost" already THDDAT time, we can subtract it here... */
    i2c_delay( TLOW  - THDDAT );

    return ( ack );
}   /* i2c_getack */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_sendack
 *#
 *# DESCRIPTION  : sends ACK on received data
 *#
 *# PARAMETERS   : none
 *#
 *# RETURN       : none
 *#
 *#---------------------------------------------------------------------------
 */
void i2c_sendack( void )
{
#ifdef CONFIG_ETRAX_I2C_SLAVE_DELAY
    int n=MAXSCLRETRIES;
#endif

    /* enable output */
    /* Clock has been set to TLOW already at end of i2c_inbyte()
     * and i2c_outbyte(), so no need to do it again.
     */
    i2c_sda_dir_out();
    /* set ack pulse low */
    i2c_set_sda( SDA_LOW );
    /* generate clock pulse */
    i2c_delay( TSUDAT );

#ifdef CONFIG_ETRAX_I2C_SLAVE_DELAY
    i2c_scl_dir_in();
    /* wait for clock to rise (n=MAXSCLRETRIES) */
    for( ; n>0; n-- )
    {
	if( i2c_scl_is_high() )
            break;
	i2c_delay( THIGH );
    }

    i2c_set_scl( SCL_HIGH );
    i2c_scl_dir_out();
    i2c_delay( THIGH );
#else
    i2c_set_scl( SCL_HIGH );

    i2c_delay( THIGH );
#endif
    i2c_set_scl( SCL_LOW );
    i2c_delay( THDDAT );
    /* reset data out */
    i2c_set_sda( SDA_HIGH );
    /* Subtract time spend already when waited to put SDA high */
    i2c_delay( TLOW - THDDAT );

    /* release the SDA line */
    i2c_sda_dir_in();
}   /* i2c_sendack */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_sendnack
 *#
 *# DESCRIPTION  : sends NACK on received data
 *#
 *# PARAMETERS   : none
 *#
 *# RETURN       : none
 *#
 *#---------------------------------------------------------------------------
 */
void i2c_sendnack( void )
{
#ifdef CONFIG_ETRAX_I2C_SLAVE_DELAY
    int n=MAXSCLRETRIES;
#endif

    /* make sure the SDA line is set high prior to activation of the output.
     * this way, you avoid an unnecessary peak to ground when a NACK has to
     * be created.
     */
    /* set data high */
    i2c_set_sda( SDA_HIGH );
    /* enable output */
    i2c_sda_dir_out();

    /* generate clock pulse */
    i2c_delay( TSUDAT );

#ifdef CONFIG_ETRAX_I2C_SLAVE_DELAY
    i2c_scl_dir_in();
    /* wait for clock to rise (n=MAXSCLRETRIES) */
    for( ; n>0; n-- )
    {
	if( i2c_scl_is_high() )
            break;
	i2c_delay( THIGH );
    }

    i2c_set_scl( SCL_HIGH );
    i2c_scl_dir_out();
    i2c_delay( THIGH );
#else
    i2c_set_scl( SCL_HIGH );

    i2c_delay( THIGH );
#endif
    i2c_set_scl( SCL_LOW );
    i2c_delay( TSUDAT );
    i2c_set_sda( SDA_LOW );
    i2c_delay( TLOW - TSUDAT );

    /* There's no need to change the direction of SDA to "in" again,
     * since a NACK is always followed by a stop condition.
     * A STOP condition will put the direction of SDA back to "out"
     * resulting in a useless SDA "dip" on the line...
     */
    /* i2c_sda_dir_in(); */
}   /* i2c_sendnack */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_writereg
 *#
 *# DESCRIPTION  : writes a value to a register of an I2C device
 *#
 *# PARAMETERS   : theSlave = slave address of the I2C device
 *#                theReg   = register of the I2C device that needs to be written
 *#                theValue = value to be written to the register
 *#
 *# RETURN       : returns OR-ed result of the write action:
 *#                  0 = Ok
 *#                  1 = Slave_NoAck
 *#                  2 = Reg_NoAck
 *#                  4 = Val_NoAck
 *#
 *#---------------------------------------------------------------------------
 */
int i2c_writereg( unsigned char theSlave
                , unsigned char theReg
                , unsigned char theValue
                )
{
    int error, cntr = 3;
    unsigned long flags;

    spin_lock( &i2c_lock );

    do
    {
        error = 0;
        /* we don't like to be interrupted */
        local_irq_save( flags );

        i2c_start();
        /* send slave address */
        if ( EI2CNOACKNLD == i2c_outbyte( theSlave & 0xfe ) )
        {
            error = 1;
        }

        /* now select register */
        if ( EI2CNOACKNLD == i2c_outbyte( theReg ) )
        {
            error |= 2;
        }

        /* send register register data */
        if ( EI2CNOACKNLD == i2c_outbyte( theValue ) )
        {
            error |= 4;
        }

        /* end byte stream */
        i2c_stop();
        /* enable interrupt again */
        local_irq_restore( flags );

    } while ( error && cntr-- );

    i2c_delay( TLOW );

    spin_unlock( &i2c_lock );

    return ( -error );
}   /* i2c_writereg */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_readreg
 *#
 *# DESCRIPTION  : reads the value from a certain register of an I2C device.
 *#                Function first writes the register that it wants to read
 *#                later on.
 *#
 *# PARAMETERS   : theSlave = slave address of the I2C device
 *#                theReg   = register of the I2C device that needs to be written
 *#
 *# RETURN       : returns OR-ed result of the write action:
 *#                  0 = Ok
 *#                  1 = Slave_NoAck
 *#                  2 = Reg_NoAck
 *#                  4 = Val_NoAck
 *#
 *#---------------------------------------------------------------------------
 */
unsigned char i2c_readreg( unsigned char theSlave
                         , unsigned char theReg
                         )
{
    unsigned char b = 0;
    int error, cntr = 3;
    unsigned long flags;

    spin_lock( &i2c_lock );

    do
    {
        error = 0;

        /* we don't like to be interrupted */
        local_irq_save( flags );

        /* generate start condition */
        i2c_start();

        /* send slave address */
        if ( EI2CNOACKNLD == i2c_outbyte( theSlave & 0xfe ) )
        {
            error = 1;
        }

        /* now select register */
        i2c_sda_dir_out();

        if ( EI2CNOACKNLD == i2c_outbyte( theReg ) )
        {
            error |= 2;
        }

        /* repeat start condition */
        i2c_delay( TLOW );
        i2c_start();

        /* send slave address */
        if ( EI2CNOACKNLD == i2c_outbyte( theSlave | 0x01 ) )
        {
            error |= 1;
        }

        /* fetch register */
        b = i2c_inbyte();
        /*
         * last received byte needs to be nacked
         * instead of acked
         */
        i2c_sendnack();

        /* end sequence */
        i2c_stop();

        /* enable interrupt again */
        local_irq_restore( flags );

    } while ( error && cntr-- );

    spin_unlock( &i2c_lock );

    return ( b );
}   /* i2c_readreg */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_read
 *#
 *# DESCRIPTION  :
 *#
 *# PARAMETERS   :
 *#
 *# RETURN       :
 *#
 *#---------------------------------------------------------------------------
 */
int i2c_read( unsigned char slave, unsigned char* rbuf, unsigned char rlen )
{
    return ( i2c_command( slave, NULL, 0, rbuf, rlen ) );
}   /* i2c_read */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_write
 *#
 *# DESCRIPTION  :
 *#
 *# PARAMETERS   :
 *#
 *# RETURN       :
 *#
 *#---------------------------------------------------------------------------
 */
int i2c_write( unsigned char slave, unsigned char* wbuf, unsigned char wlen )
{
    return ( i2c_command( slave, wbuf, wlen, NULL, 0 ) );
}   /* i2c_write */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: i2c_writeread
 *#
 *# DESCRIPTION  :
 *#
 *# PARAMETERS   :
 *#
 *# RETURN       :
 *#
 *#---------------------------------------------------------------------------
 */
int i2c_writeread( unsigned char  slave
                 , unsigned char* wbuf
                 , unsigned char  wlen
                 , unsigned char* rbuf
                 , unsigned char  rlen
                 )
{
    return ( i2c_command( slave, wbuf, wlen, rbuf, rlen ) );
}   /* i2c_writeread */


/*#---------------------------------------------------------------------------
 *#
 *# FUNCTION NAME: module_init
 *#
 *# DESCRIPTION  : this makes sure that i2c_register is called during boot
 *#
 *# PARAMETERS   :
 *#
 *#---------------------------------------------------------------------------
 */
module_init( i2c_register );

/****************** END OF FILE i2c.c ********************************/
