#ifndef __DANUBE_CGU_DEV_H__2005_07_20__14_26__
#define __DANUBE_CGU_DEV_H__2005_07_20__14_26__


/******************************************************************************
       Copyright (c) 2002, Infineon Technologies.  All rights reserved.

                               No Warranty
   Because the program is licensed free of charge, there is no warranty for
   the program, to the extent permitted by applicable law.  Except when
   otherwise stated in writing the copyright holders and/or other parties
   provide the program "as is" without warranty of any kind, either
   expressed or implied, including, but not limited to, the implied
   warranties of merchantability and fitness for a particular purpose. The
   entire risk as to the quality and performance of the program is with
   you.  should the program prove defective, you assume the cost of all
   necessary servicing, repair or correction.

   In no event unless required by applicable law or agreed to in writing
   will any copyright holder, or any other party who may modify and/or
   redistribute the program as permitted above, be liable to you for
   damages, including any general, special, incidental or consequential
   damages arising out of the use or inability to use the program
   (including but not limited to loss of data or data being rendered
   inaccurate or losses sustained by you or third parties or a failure of
   the program to operate with any other programs), even if such holder or
   other party has been advised of the possibility of such damages.
******************************************************************************/


/*
 * ####################################
 *              Definition
 * ####################################
 */

/*
 *  ioctl Command
 */
#define CGU_IOC_MAGIC                   'u'
#define CGU_GET_CLOCK_RATES             _IOW(CGU_IOC_MAGIC, 0, struct cgu_clock_rates)
#define CGU_IOC_MAXNR                   1


/*
 * ####################################
 *              Data Type
 * ####################################
 */

/*
 *  Data Type Used to Call ioctl(GET_CLOCK_RATES)
 */
struct cgu_clock_rates {
    u32     mips0;
    u32     mips1;
    u32     cpu;
    u32     io_region;
    u32     fpi_bus1;
    u32     fpi_bus2;
    u32     pp32;
    u32     pci;
    u32     ethernet;
    u32     usb;
    u32     clockout0;
    u32     clockout1;
    u32     clockout2;
    u32     clockout3;
};


/*
 * ####################################
 *             Declaration
 * ####################################
 */

#if defined(__KERNEL__)
    extern u32 cgu_get_mips_clock(int);
    extern u32 cgu_get_cpu_clock(void);
    extern u32 cgu_get_io_region_clock(void);
    extern u32 cgu_get_fpi_bus_clock(int);
    extern u32 cgu_get_pp32_clock(void);
    extern u32 cgu_get_pci_clock(void);
    extern u32 cgu_get_ethernet_clock(void);
    extern u32 cgu_get_usb_clock(void);
    extern u32 cgu_get_clockout(int);
#endif  //  defined(__KERNEL__)


#endif  //  __DANUBE_CGU_DEV_H__2005_07_20__14_26__
