/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright © 2003 Atheros Communications, Inc.,  All Rights Reserved.
 */

/*
 * Manage the atheros ethernet PHY.
 *
 * All definitions in this file are operating system independent!
 */

#include <config.h>
#include <linux/types.h>
#include <common.h>
#include <miiphy.h>
//#include "phy.h"
//#include "ar7100_soc.h"
#include "athrs26_phy.h"

#define phy_reg_read(base, addr, reg, datap)                    \
    miiphy_read("lq_cpe_eth", addr, reg, datap);
#define phy_reg_write(base, addr, reg, data)                   \
    miiphy_write("lq_cpe_eth", addr, reg, data);
            

/* PHY selections and access functions */

typedef enum {
    PHY_SRCPORT_INFO, 
    PHY_PORTINFO_SIZE,
} PHY_CAP_TYPE;

typedef enum {
    PHY_SRCPORT_NONE,
    PHY_SRCPORT_VLANTAG, 
    PHY_SRCPORT_TRAILER,
} PHY_SRCPORT_TYPE;

#ifdef DEBUG
#define DRV_DEBUG 1
#endif
//#define DRV_DEBUG 1

#define DRV_DEBUG_PHYERROR  0x00000001
#define DRV_DEBUG_PHYCHANGE 0x00000002
#define DRV_DEBUG_PHYSETUP  0x00000004

#if DRV_DEBUG
int athrPhyDebug = DRV_DEBUG_PHYERROR|DRV_DEBUG_PHYCHANGE|DRV_DEBUG_PHYSETUP;

#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)    \
{                                                   \
    if (athrPhyDebug & (FLG)) {                       \
        logMsg(X0, X1, X2, X3, X4, X5, X6);         \
    }                                               \
}

#define DRV_MSG(x,a,b,c,d,e,f)                      \
    logMsg(x,a,b,c,d,e,f)

#define DRV_PRINT(FLG, X)                           \
{                                                   \
    if (athrPhyDebug & (FLG)) {                       \
        printf X;                                   \
    }                                               \
}

#else /* !DRV_DEBUG */
#define DRV_LOG(DBG_SW, X0, X1, X2, X3, X4, X5, X6)
#define DRV_MSG(x,a,b,c,d,e,f)
#define DRV_PRINT(DBG_SW,X)
#endif

#define ATHR_LAN_PORT_VLAN          1
#define ATHR_WAN_PORT_VLAN          2

#define ENET_UNIT_LAN 0

#define TRUE    1
#define FALSE   0

#define ATHR_PHY0_ADDR   0x0
#define ATHR_PHY1_ADDR   0x1
#define ATHR_PHY2_ADDR   0x2
#define ATHR_PHY3_ADDR   0x3
#define ATHR_PHY4_ADDR   0x4

/*
 * Track per-PHY port information.
 */
typedef struct {
    BOOL   isEnetPort;       /* normal enet port */
    BOOL   isPhyAlive;       /* last known state of link */
    int    ethUnit;          /* MAC associated with this phy port */
    uint32_t phyBase;
    uint32_t phyAddr;          /* PHY registers associated with this phy port */
    uint32_t VLANTableSetting; /* Value to be written to VLAN table */
} athrPhyInfo_t;

/*
 * Per-PHY information, indexed by PHY unit number.
 */
static athrPhyInfo_t athrPhyInfo[] = {
    {TRUE,   /* phy port 0 -- LAN port 0 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY0_ADDR,
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* phy port 1 -- LAN port 1 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY1_ADDR,
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* phy port 2 -- LAN port 2 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY2_ADDR, 
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* phy port 3 -- LAN port 3 */
     FALSE,
     ENET_UNIT_LAN,
     0,
     ATHR_PHY3_ADDR, 
     ATHR_LAN_PORT_VLAN
    },

    {TRUE,   /* phy port 4 -- WAN port or LAN port 4 */
     FALSE,
     1,
     0,
     ATHR_PHY4_ADDR, 
     ATHR_LAN_PORT_VLAN   /* Send to all ports */
    },

    {FALSE,  /* phy port 5 -- CPU port (no RJ45 connector) */
     TRUE,
     ENET_UNIT_LAN,
     0,
     0x00, 
     ATHR_LAN_PORT_VLAN    /* Send to all ports */
    },
};

#ifdef CFG_ATHRHDR_EN
typedef struct {
    uint8_t data[ATHRHDR_MAX_DATA];
    uint8_t len;
    uint32_t seq;
} cmd_resp_t;

typedef struct {
 uint16_t reg_addr;
 uint16_t cmd_len;
 uint8_t *reg_data;
}cmd_write_t;

static cmd_write_t cmd_write,cmd_read;
static cmd_resp_t cmd_resp;
static struct eth_device *lan_mac;
//static atomic_t seqcnt = ATOMIC_INIT(0);
static int  seqcnt = 0;
static int cmd = 1;
//volatile uchar AthrHdrPkt[60];
#endif

#define ATHR_GLOBALREGBASE    0

//#define ATHR_PHY_MAX (sizeof(athrPhyInfo) / sizeof(athrPhyInfo[0]))
#define ATHR_PHY_MAX 5

/* Range of valid PHY IDs is [MIN..MAX] */
#define ATHR_ID_MIN 0
#define ATHR_ID_MAX (ATHR_PHY_MAX-1)

/* Convenience macros to access myPhyInfo */
#define ATHR_IS_ENET_PORT(phyUnit) (athrPhyInfo[phyUnit].isEnetPort)
#define ATHR_IS_PHY_ALIVE(phyUnit) (athrPhyInfo[phyUnit].isPhyAlive)
#define ATHR_ETHUNIT(phyUnit) (athrPhyInfo[phyUnit].ethUnit)
#define ATHR_PHYBASE(phyUnit) (athrPhyInfo[phyUnit].phyBase)
#define ATHR_PHYADDR(phyUnit) (athrPhyInfo[phyUnit].phyAddr)
#define ATHR_VLAN_TABLE_SETTING(phyUnit) (athrPhyInfo[phyUnit].VLANTableSetting)


#define ATHR_IS_ETHUNIT(phyUnit, ethUnit) \
            (ATHR_IS_ENET_PORT(phyUnit) &&        \
            ATHR_ETHUNIT(phyUnit) == (ethUnit))

#define ATHR_IS_WAN_PORT(phyUnit) (!(ATHR_ETHUNIT(phyUnit)==ENET_UNIT_LAN))
            
/* Forward references */
BOOL       athrs26_phy_is_link_alive(int phyUnit);
//static uint32_t athrs26_reg_read(uint16_t reg_addr);
static void athrs26_reg_write(uint16_t reg_addr, 
                              uint32_t reg_val);

/******************************************************************************
*
* athrs26_phy_is_link_alive - test to see if the specified link is alive
*
* RETURNS:
*    TRUE  --> link is alive
*    FALSE --> link is down
*/

void athrs26_reg_init(void)
{

    athrs26_reg_write(0x200, 0x200);
    athrs26_reg_write(0x300, 0x200);
    athrs26_reg_write(0x400, 0x200);
    athrs26_reg_write(0x500, 0x200);
    athrs26_reg_write(0x600, 0x7d);

#ifdef S26_VER_1_0
    phy_reg_write(0, 0, 29, 41);
    phy_reg_write(0, 0, 30, 0);
    phy_reg_write(0, 1, 29, 41);
    phy_reg_write(0, 1, 30, 0);
    phy_reg_write(0, 2, 29, 41);
    phy_reg_write(0, 2, 30, 0);
    phy_reg_write(0, 3, 29, 41);
    phy_reg_write(0, 3, 30, 0);
    phy_reg_write(0, 4, 29, 41);
    phy_reg_write(0, 4, 30, 0);
#endif

    athrs26_reg_write(0x38, 0xc000050e);

#ifdef CFG_ATHRHDR_EN
    athrs26_reg_write(0x104, 0x4804);
#else
    athrs26_reg_write(0x104, 0x4004);
#endif

    athrs26_reg_write(0x60, 0xffffffff);
    athrs26_reg_write(0x64, 0xaaaaaaaa);
    athrs26_reg_write(0x68, 0x55555555);
    athrs26_reg_write(0x6c, 0x0);

    athrs26_reg_write(0x70, 0x41af);
}

BOOL
athrs26_phy_is_link_alive(int phyUnit)
{
    uint16_t phyHwStatus;
    uint32_t phyBase;
    uint32_t phyAddr;

    phyBase = ATHR_PHYBASE(phyUnit);
    phyAddr = ATHR_PHYADDR(phyUnit);

    phy_reg_read(phyBase, phyAddr, ATHR_PHY_SPEC_STATUS, &phyHwStatus);

    if (phyHwStatus & ATHR_STATUS_LINK_PASS)
        return TRUE;

    return FALSE;
}


/******************************************************************************
*
* athrs26_phy_setup - reset and setup the PHY associated with
* the specified MAC unit number.
*
* Resets the associated PHY port.
*
* RETURNS:
*    TRUE  --> associated PHY is alive
*    FALSE --> no LINKs on this ethernet unit
*/

BOOL
athrs26_phy_setup(int ethUnit)
{
    int         phyUnit;
    uint16_t    phyHwStatus;
    uint16_t    timeout;
    int         liveLinks = 0;
    uint32_t    phyBase = 0;
    BOOL        foundPhy = FALSE;
    uint32_t  phyAddr = 0;
    uint32_t  regVal;
    

    /* See if there's any configuration data for this enet */
    /* start auto negogiation on each phy */
    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
			continue;
        }


        foundPhy = TRUE;
        phyBase = ATHR_PHYBASE(phyUnit);
        phyAddr = ATHR_PHYADDR(phyUnit);

        phy_reg_write(phyBase, phyAddr, ATHR_AUTONEG_ADVERT,
                      ATHR_ADVERTISE_ALL);

        /* Reset PHYs*/
        phy_reg_write(phyBase, phyAddr, ATHR_PHY_CONTROL,
                      ATHR_CTRL_AUTONEGOTIATION_ENABLE
                      | ATHR_CTRL_SOFTWARE_RESET);

	}

    if (!foundPhy) {
        return FALSE; /* No PHY's configured for this ethUnit */
    }

    /*
     * After the phy is reset, it takes a little while before
     * it can respond properly.
     */
    sysMsDelay(1000);
    
    /*
     * Wait up to .75 seconds for ALL associated PHYs to finish
     * autonegotiation.  The only way we get out of here sooner is
     * if ALL PHYs are connected AND finish autonegotiation.
     */
    for (phyUnit=0; (phyUnit < ATHR_PHY_MAX) /*&& (timeout > 0) */; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        timeout=20;
        for (;;) {
			phyHwStatus = 0;
            phy_reg_read(phyBase, phyAddr, ATHR_PHY_CONTROL, &phyHwStatus);

		if (ATHR_RESET_DONE(phyHwStatus)) {
                DRV_PRINT(DRV_DEBUG_PHYSETUP,
                          ("Port %d, Neg Success\n", phyUnit));
                break;
            }
            if (timeout == 0) {
                DRV_PRINT(DRV_DEBUG_PHYSETUP,
                          ("Port %d, Negogiation timeout\n", phyUnit));
                break;
            }
            if (--timeout == 0) {
                DRV_PRINT(DRV_DEBUG_PHYSETUP,
                          ("Port %d, Negogiation timeout\n", phyUnit));
                break;
            }

            sysMsDelay(150);
        }
    }

	/*
     * All PHYs have had adequate time to autonegotiate.
     * Now initialize software status.
     *
     * It's possible that some ports may take a bit longer
     * to autonegotiate; but we can't wait forever.  They'll
     * get noticed by mv_phyCheckStatusChange during regular
     * polling activities.
     */
    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        if (athrs26_phy_is_link_alive(phyUnit)) {
            liveLinks++;
            ATHR_IS_PHY_ALIVE(phyUnit) = TRUE;
        } else {
            ATHR_IS_PHY_ALIVE(phyUnit) = FALSE;
        }

        phy_reg_read(ATHR_PHYBASE(phyUnit), ATHR_PHYADDR(phyUnit), 
                    ATHR_PHY_SPEC_STATUS, &regVal);
        DRV_PRINT(DRV_DEBUG_PHYSETUP,
            ("eth%d: Phy Specific Status=%4.4x\n", ethUnit, regVal)); 
    }
#if 0
    /* if using header for register configuration, we have to     */
    /* configure s26 register after frame transmission is enabled */

    athrs26_reg_write(0x200, 0x200);
    athrs26_reg_write(0x300, 0x200);
    athrs26_reg_write(0x400, 0x200);
    athrs26_reg_write(0x500, 0x200);
    athrs26_reg_write(0x600, 0x200);
	athrs26_reg_write(0x38, 0x50e);
#endif
#ifndef CFG_ATHRHDR_EN       
/* if using header for register configuration, we have to     */
    /* configure s26 register after frame transmission is enabled */
        athrs26_reg_init();
#endif
    
    return (liveLinks > 0);
}

/******************************************************************************
*
* athrs26_phy_is_fdx - Determines whether the phy ports associated with the
* specified device are FULL or HALF duplex.
*
* RETURNS:
*    1  --> FULL
*    0 --> HALF
*/
int
athrs26_phy_is_fdx(int ethUnit)
{
    int         phyUnit;
    uint32_t    phyBase;
    uint32_t    phyAddr;
    uint16_t    phyHwStatus;
    int         ii = 200;
    
    if (ethUnit == ENET_UNIT_LAN)
        return TRUE;
    
    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        if (athrs26_phy_is_link_alive(phyUnit)) {

            phyBase = ATHR_PHYBASE(phyUnit);
            phyAddr = ATHR_PHYADDR(phyUnit);

            do {
                phy_reg_read(phyBase, phyAddr, ATHR_PHY_SPEC_STATUS, &phyHwStatus);
        	    sysMsDelay(10);
            } while((!(phyHwStatus & ATHR_STATUS_RESOVLED)) && --ii);

            if (phyHwStatus & ATHER_STATUS_FULL_DEPLEX)
                return TRUE;
        }
    }

    return FALSE;
}


/******************************************************************************
*
* athrs26_phy_speed - Determines the speed of phy ports associated with the
* specified device.
*
* RETURNS:
*               AG7100_PHY_SPEED_10T, AG7100_PHY_SPEED_100TX;
*               AG7100_PHY_SPEED_1000T;
*/

BOOL
athrs26_phy_speed(int ethUnit)
{
    int         phyUnit;
    uint16_t    phyHwStatus;
    uint32_t    phyBase;
    uint32_t    phyAddr;
    int         ii = 200;
    
    if (ethUnit == ENET_UNIT_LAN)
        return _100BASET;

    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        if (athrs26_phy_is_link_alive(phyUnit)) {

            phyBase = ATHR_PHYBASE(phyUnit);
            phyAddr = ATHR_PHYADDR(phyUnit);
            
            do {
                phy_reg_read(phyBase, phyAddr, 
                                           ATHR_PHY_SPEC_STATUS, &phyHwStatus);
                sysMsDelay(10);
            }while((!(phyHwStatus & ATHR_STATUS_RESOVLED)) && --ii);

            phyHwStatus = ((phyHwStatus & ATHER_STATUS_LINK_MASK) >>
                           ATHER_STATUS_LINK_SHIFT);

            switch(phyHwStatus) {
            case 0:
                return _10BASET;
            case 1:
                return _100BASET;
            case 2:
                return _1000BASET;
            default:
                DRV_PRINT(DRV_DEBUG_PHYERROR, ("Unkown speed read!\n"));
            }
        }
    }

    return _10BASET;
}

/*****************************************************************************
*
* athr_phy_is_up -- checks for significant changes in PHY state.
*
* A "significant change" is:
*     dropped link (e.g. ethernet cable unplugged) OR
*     autonegotiation completed + link (e.g. ethernet cable plugged in)
*
* When a PHY is plugged in, phyLinkGained is called.
* When a PHY is unplugged, phyLinkLost is called.
*/

int
athrs26_phy_is_up(int ethUnit)
{
    int             phyUnit;
    uint16_t        phyHwStatus;
    athrPhyInfo_t  *lastStatus;
    int             linkCount   = 0;
    int             lostLinks   = 0;
    int             gainedLinks = 0;
    uint32_t        phyBase;
    uint32_t        phyAddr;
#ifdef CFG_ATHRHDR_REG
    /* if using header to config s26, the link of MAC0 should always be up */
    if (ethUnit == ENET_UNIT_LAN)
        return 1;
#endif

    for (phyUnit=0; phyUnit < ATHR_PHY_MAX; phyUnit++) {
        if (!ATHR_IS_ETHUNIT(phyUnit, ethUnit)) {
            continue;
        }

        phyBase = ATHR_PHYBASE(phyUnit);
        phyAddr = ATHR_PHYADDR(phyUnit);


        lastStatus = &athrPhyInfo[phyUnit];
        phy_reg_read(phyBase, phyAddr, ATHR_PHY_SPEC_STATUS, &phyHwStatus);

        if (lastStatus->isPhyAlive) { /* last known link status was ALIVE */
            /* See if we've lost link */
            if (phyHwStatus & ATHR_STATUS_LINK_PASS) {
                linkCount++;
            } else {
                lostLinks++;
                DRV_PRINT(DRV_DEBUG_PHYCHANGE,("\nenet%d port%d down\n",
                                               ethUnit, phyUnit));
                lastStatus->isPhyAlive = FALSE;
            }
        } else { /* last known link status was DEAD */
            /* Check for reset complete */
            phy_reg_read(phyBase, phyAddr, ATHR_PHY_STATUS, &phyHwStatus);
            if (!ATHR_RESET_DONE(phyHwStatus))
                continue;

            /* Check for AutoNegotiation complete */            
            if (ATHR_AUTONEG_DONE(phyHwStatus)) {
                //printk("autoneg done\n");
                gainedLinks++;
                linkCount++;
                DRV_PRINT(DRV_DEBUG_PHYCHANGE,("\nenet%d port%d up\n",
                                               ethUnit, phyUnit));
                lastStatus->isPhyAlive = TRUE;
            }
        }
    }

    return (linkCount);

#if 0
    if (linkCount == 0) {
        if (lostLinks) {
            /* We just lost the last link for this MAC */
            phyLinkLost(ethUnit);
        }
    } else {
        if (gainedLinks == linkCount) {
            /* We just gained our first link(s) for this MAC */
            phyLinkGained(ethUnit);
        }
    }
#endif
}

#ifdef CFG_ATHRHDR_EN
void athr_hdr_timeout(void){
	eth_halt();
        NetState = NETLOOP_FAIL; 
}

void athr_hdr_handler(uchar *recv_pkt, unsigned dest, unsigned src, unsigned len){
	header_receive_pkt(recv_pkt);
	NetState = NETLOOP_SUCCESS;
}
static int
athrs26_header_config_reg (struct eth_device *dev, uint8_t wr_flag,
                           uint16_t reg_addr, uint16_t cmd_len,
                           uint8_t *val)
{
    at_header_t at_header;
    reg_cmd_t reg_cmd;
    uchar *AthrHdrPkt;

    AthrHdrPkt = NetTxPacket;

    if(AthrHdrPkt == NULL) {
		printf("Null packet\n");
		return;
    }
    memset(AthrHdrPkt,0,60);

    /*fill at_header*/
    at_header.reserved0 = 0x10;  //default
    at_header.priority = 0;
    at_header.type = 0x5;
    at_header.broadcast = 0;
    at_header.from_cpu = 1;
    at_header.reserved1 = 0x01; //default
    at_header.port_num = 0;

    AthrHdrPkt[0] = at_header.port_num;
    AthrHdrPkt[0] |= at_header.reserved1 << 4;
    AthrHdrPkt[0] |= at_header.from_cpu << 6;
    AthrHdrPkt[0] |= at_header.broadcast << 7;

    AthrHdrPkt[1] = at_header.type;
    AthrHdrPkt[1] |= at_header.priority << 4;
    AthrHdrPkt[1] |= at_header.reserved0 << 6;


    /*fill reg cmd*/
    if(cmd_len > 4)
        cmd_len = 4;//only support 32bits register r/w

    reg_cmd.reg_addr = reg_addr&0x3FFFC;
    reg_cmd.cmd_len = cmd_len;
    reg_cmd.cmd = wr_flag;
    reg_cmd.reserved2 = 0x5; //default
    reg_cmd.seq_num = seqcnt;

    AthrHdrPkt[2] = reg_cmd.reg_addr & 0xff;
    AthrHdrPkt[3] = (reg_cmd.reg_addr & 0xff00) >> 8;
    AthrHdrPkt[4] = (reg_cmd.reg_addr & 0x30000) >> 16;
    AthrHdrPkt[4] |= reg_cmd.cmd_len << 4;
    AthrHdrPkt[5] = reg_cmd.cmd << 4;
    AthrHdrPkt[5] |= reg_cmd.reserved2 << 5;
    AthrHdrPkt[6] = (reg_cmd.seq_num & 0x7f) << 1;
    AthrHdrPkt[7] = (reg_cmd.seq_num & 0x7f80) >> 7;
    AthrHdrPkt[8] = (reg_cmd.seq_num & 0x7f8000) >> 15;
    AthrHdrPkt[9] = (reg_cmd.seq_num & 0x7f800000) >> 23;

    /*fill reg data*/
    if(!wr_flag)//write
        memcpy((AthrHdrPkt + 10), val, cmd_len);
    
    /*start xmit*/
    if(dev == NULL) {
	printf("ERROR device not found\n");
	return -1;
    }
    header_xmit(dev, AthrHdrPkt ,60);
    return 0;
}
void athr_hdr_func(void) {

   NetSetTimeout (1 * CFG_HZ,athr_hdr_timeout );
   NetSetHandler (athr_hdr_handler);

   if(cmd) 
   	athrs26_header_config_reg(lan_mac, cmd, cmd_read.reg_addr, cmd_read.cmd_len, cmd_read.reg_data);
   else 
        athrs26_header_config_reg(lan_mac, cmd, cmd_write.reg_addr, cmd_write.cmd_len, cmd_write.reg_data);
}
static int
athrs26_header_write_reg(uint16_t reg_addr, uint16_t cmd_len, uint8_t *reg_data)
{
    int i = 2;
    cmd_write.reg_addr = reg_addr;
    cmd_write.cmd_len = cmd_len;
    cmd_write.reg_data = reg_data;
    cmd = 0;
    seqcnt++;

    do {
	if (NetLoop(ATHRHDR) >= 0) /* polls for read/write ack from PHY */
	   break;
    } while (i--);

    return i;
}

static int
athrs26_header_read_reg(uint16_t reg_addr, uint16_t cmd_len, uint8_t *reg_data)
{
    int i = 2;

    cmd_read.reg_addr = reg_addr;
    cmd_read.cmd_len = cmd_len;
    cmd_read.reg_data = reg_data;
    cmd = 1;
    seqcnt++;

    do {
        if (NetLoop(ATHRHDR) >= 0) /* polls for read/write ack from PHY */
           break;
    } while (i--);

    if ((i==0) || (seqcnt != cmd_resp.seq) || (cmd_len != cmd_resp.len)) {
        return -1;
    }
    memcpy (cmd_read.reg_data, cmd_resp.data, cmd_len);
    return 0;
}
int header_receive_pkt(uchar *recv_pkt)
{
    cmd_resp.len = recv_pkt[4] >> 4;
    if (cmd_resp.len > 10)
        goto out;

    cmd_resp.seq = recv_pkt[6] >> 1;
    cmd_resp.seq |= recv_pkt[7] << 7;
    cmd_resp.seq |= recv_pkt[8] << 15;
    cmd_resp.seq |= recv_pkt[9] << 23;

    if (cmd_resp.seq < seqcnt)
        goto out;
    memcpy (cmd_resp.data, (recv_pkt + 10), cmd_resp.len);
out:
     return 0;
}

void athrs26_reg_dev(struct eth_device *mac)
{
    lan_mac = mac;
}

#endif

/*static uint32_t
athrs26_reg_read(uint16_t reg_addr)
{
#ifndef CFG_ATHRHDR_REG
    uint16_t reg_word_addr = reg_addr / 2, phy_val;
    uint32_t phy_addr;
    uint8_t phy_reg; 
    
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (reg_word_addr >> 8) & 0x1ff;        
    phy_reg_write (0, phy_addr, phy_reg, phy_val);

    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); 
    phy_reg = reg_word_addr & 0x1f;            
    phy_reg_read(0, phy_addr, phy_reg, &phy_val);
    
    return phy_val;
#else
    uint8_t reg_data[4];

    memset (reg_data, 0, 4);
    athrs26_header_read_reg(reg_addr, 4, reg_data);
    return (reg_data[0] | (reg_data[1] << 8) | (reg_data[2] << 16) | (reg_data[3] << 24));
#endif
}
*/
static void
athrs26_reg_write(uint16_t reg_addr, uint32_t reg_val)
{
#ifndef CFG_ATHRHDR_REG
    uint16_t reg_word_addr = reg_addr / 2, phy_val;
    uint32_t phy_addr;
    uint8_t phy_reg; 

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (reg_word_addr >> 8) & 0x1ff;         /* bit16-8 of reg address*/
    phy_reg_write (0, phy_addr, phy_reg, phy_val);

    /* read register with low address */
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = reg_word_addr & 0x1f;                 /* bit 4-0 of reg address */
    phy_reg_write (0, phy_addr, phy_reg, reg_val);
#else
    uint8_t reg_data[4];

    memset (reg_data, 0, 4);
    reg_data[0] = (uint8_t)(0x00ff & reg_val);
    reg_data[1] = (uint8_t)((0xff00 & reg_val) >> 8);
    reg_data[2] = (uint8_t)((0xff0000 & reg_val) >> 16);
    reg_data[3] = (uint8_t)((0xff000000 & reg_val) >> 24);

    athrs26_header_write_reg (reg_addr, 4, reg_data);
#endif

}

