#include <linux/netdevice.h>
#include <linux/version.h>
#include <linux/skbuff.h>
#ifdef TCSUPPORT_RA_HWNAT
#include <linux/foe_hook.h>
#endif

/*
 * struct port_info lives in <linux/foe_hook.h>, which is only pulled in when
 * hwNAT (TCSUPPORT_RA_HWNAT) is enabled.  Several prototypes below take a
 * struct port_info * regardless, so forward-declare it at file scope -- without
 * this, the first mention inside a prototype parameter list gets prototype
 * scope and the later definition trips -Werror "conflicting types" on 6.18.
 * TODO EN7528: better handled by including the foe_hook shim unconditionally.
 */
struct port_info;

#include "common/drv_global.h"
#include "pwan/gpon_wan.h"
#include "pwan/xpon_netif.h"
#include "gpon/gpon_dvt.h"
#include "gpon/gpon_recovery.h"
#include "common/xpondrv.h"

#include <ecnt_hook/ecnt_hook_traffic_classify.h>
#include <ecnt_hook/ecnt_hook_smux.h>
#include <ecnt_hook/ecnt_hook_net.h>
#include <linux/etherdevice.h>
#include <linux/if_ether.h>
#ifdef TCSUPPORT_CPU_EN7521
#include <ecnt_hook/ecnt_hook_fe.h>
#endif

/* ------------------------------------------------------------------------
 * proc_fs 2.6.36 -> 6.18 compat shim (compile/port milestone, file-local).
 * Keeps the legacy ->read_proc / ->write_proc handler signatures and bridges
 * them to struct proc_ops; create_proc_entry() is re-pointed at
 * econet_xpon_create_proc() which returns a struct compat_proc_priv *.
 * TODO EN7528: priv object is intentionally leaked on remove this milestone.
 * ------------------------------------------------------------------------ */
#include <linux/slab.h>
#include <linux/fs.h>

typedef int (*compat_read_proc_t)(char *page, char **start, off_t off,
				  int count, int *eof, void *data);
typedef int (*compat_write_proc_t)(struct file *file, const char *buffer,
				   unsigned long count, void *data);

struct compat_proc_priv {
	compat_read_proc_t	read_proc;
	compat_write_proc_t	write_proc;
	void			*data;
	struct proc_dir_entry	*pde;
};

static ssize_t econet_xpon_proc_read(struct file *f, char __user *ubuf,
				     size_t cnt, loff_t *ppos)
{
	struct compat_proc_priv *p = pde_data(file_inode(f));
	char *page;
	int len, eof = 0;

	if (!p || !p->read_proc || *ppos > 0)
		return 0;
	page = (char *)__get_free_page(GFP_KERNEL);
	if (!page)
		return -ENOMEM;
	len = p->read_proc(page, NULL, 0, PAGE_SIZE - 1, &eof, p->data);
	if (len < 0)
		len = 0;
	if (len > (int)cnt)
		len = (int)cnt;
	if (len && copy_to_user(ubuf, page, len))
		len = -EFAULT;
	if (len > 0)
		*ppos += len;
	free_page((unsigned long)page);
	return len;
}

static ssize_t econet_xpon_proc_write(struct file *f, const char __user *ubuf,
				      size_t cnt, loff_t *ppos)
{
	struct compat_proc_priv *p = pde_data(file_inode(f));
	int rc;

	if (!p || !p->write_proc)
		return -EIO;
	rc = p->write_proc(f, (const char *)ubuf, cnt, p->data);
	return rc < 0 ? rc : (ssize_t)cnt;
}

static const struct proc_ops econet_xpon_proc_ops = {
	.proc_read	= econet_xpon_proc_read,
	.proc_write	= econet_xpon_proc_write,
	.proc_lseek	= default_llseek,
};

static struct compat_proc_priv *
econet_xpon_create_proc(const char *name, umode_t mode,
			struct proc_dir_entry *parent)
{
	struct compat_proc_priv *p = kzalloc(sizeof(*p), GFP_KERNEL);

	if (!p)
		return NULL;
	if (!(mode & 0777))
		mode |= 0644;
	p->pde = proc_create_data(name, mode, parent, &econet_xpon_proc_ops, p);
	if (!p->pde) {
		kfree(p);
		return NULL;
	}
	return p;
}

#undef create_proc_entry
#define create_proc_entry(name, mode, parent) \
	econet_xpon_create_proc((name), (mode), (parent))
/* ---- end proc_fs compat shim ---- */

/*
 * The OMCI tx-drop timer used to carry its period (ms) in timer_list.data,
 * which was removed in 6.x.  The vendor only ever programs 5000 ms, so keep
 * that as a constant here and bypass the gpon.h GPON_START_TIMER() macro
 * (which still dereferences timer.data).  TODO EN7528: move the period into the
 * driver priv if it ever needs to vary, and port GPON_START_TIMER in gpon.h.
 */
#define TXDROP_TIMER_MS		5000

/*
 * 6.18 timer callbacks take a struct timer_list *.  gwan_update_gem_mib_table()
 * keeps its legacy (unsigned long) prototype (it is also called directly and is
 * declared in gpon_wan.h), so bridge it through a trampoline.
 */
static void econet_xpon_gemmib_timer_cb(struct timer_list *t)
{
	(void)t;
	gwan_update_gem_mib_table(0);
}

#ifdef TCSUPPORT_XPON_IGMP
extern int (*xpon_sfu_up_multicast_incoming_hook)(struct sk_buff *skb, int clone);
extern int (*xpon_sfu_multicast_protocol_hook)(struct sk_buff *skb);
#endif

int g_service_change_flag = 0;


#ifdef TCSUPPORT_PON_VLAN
extern int (*pon_insert_tag_hook)(struct sk_buff **pskb);
#endif
#if defined(TCSUPPORT_PON_VLAN) && defined(TCSUPPORT_PON_VLAN_FILTER)&& defined(TCSUPPORT_XPON_IGMP) && defined(TCSUPPORT_XPON_IGMP)
extern int (*isVlanOperationInMulticastModule_hook)(struct sk_buff *skb);
extern int (*xpon_up_igmp_uni_vlan_filter_hook)(struct sk_buff *skb);
extern int (*xpon_sfu_up_multicast_vlan_hook)(struct sk_buff *skb, int clone);
extern int (*xpon_up_igmp_ani_vlan_filter_hook)(struct sk_buff *skb);
#endif
#ifdef TCSUPPORT_PON_MAC_FILTER
extern int (*pon_check_mac_hook)(struct sk_buff *skb);
#endif

#ifdef TCSUPPORT_GPON_MAPPING
extern int (*gpon_mapping_hook)(struct sk_buff *pskb);
#endif

#if defined(TCSUPPORT_CT_SWQOS) || defined(TCSUPPORT_XPON_HAL_API_QOS) || defined(TCSUPPORT_XPON_HAL_API_EXT)
extern int (*sw_isSWQosActive) (void);
extern int (*sw_isHwnatOffloadEnable) (void);
#endif

#if defined(TCSUPPORT_CT_PON) || defined(TCSUPPORT_XPON_HAL_API_QOS) || defined(TCSUPPORT_XPON_HAL_API_EXT)
#if defined(TCSUPPORT_CT_HWQOS) || defined(TCSUPPORT_XPON_HAL_API_QOS) || defined(TCSUPPORT_XPON_HAL_API_EXT)
extern int isNeedHWQoS;
#endif
extern int trtcmEnable;
extern int trtcmTsid;
#endif

extern int isHighestPriorityPacket(struct sk_buff *skb);
#ifdef TCSUPPORT_WAN_GPON
//ONU Type : 1=SFU 2=HGU
#define SFU 1
#define HGU 2

#if defined(TCSUPPORT_FWC_ENV)
FHNet_Mapping_Result_Out_t gMappingRet = {0};
uint fhNetMappingDebug = 0;
unsigned char  gFhDrop = 0;

typedef int(* P_FHNET_DS_VLAN_HOOK)           (FHNet_Mapping_Vlan_Para_In_t *pktInfo, 
									             unsigned char *dropFlag);
typedef int(* P_FHNET_L3_DS_VLAN_HOOK)        (FHNet_L3Pkt_Mapping_Vlan_Para_In_t *pktInfo, 
										         unsigned char *dropFlag);
typedef int(* P_FHNET_US_VLAN_MAPPING_HOOK)   (FHNet_Mapping_Vlan_Para_In_t *pktInfo, 
											      FHNet_Mapping_Result_Out_t   *mappingRet);
typedef int(* P_FHNET_L3_US_VLAN_MAPPING_HOOK)(FHNet_L3Pkt_Mapping_Vlan_Para_In_t *pktInfo, 
												  FHNet_Mapping_Result_Out_t   *mappingRet);

P_FHNET_DS_VLAN_HOOK             fhnet_ds_vlan_action_hook;
P_FHNET_L3_DS_VLAN_HOOK          fhnet_L3_ds_vlan_action_hook;
P_FHNET_US_VLAN_MAPPING_HOOK     fhnet_us_vlan_action_mapping_hook;
P_FHNET_L3_US_VLAN_MAPPING_HOOK  fhnet_L3_us_vlan_action_mapping_hook;

int fhnet_ds_vlan_action_callback(P_FHNET_DS_VLAN_HOOK func)
{
	rcu_assign_pointer(fhnet_ds_vlan_action_hook,func);
}
int fhnet_cpu_pkt_ds_vlan_action_callback(P_FHNET_L3_DS_VLAN_HOOK func)
{
    printk("%s,%d\n",__FUNCTION__,__LINE__);
	rcu_assign_pointer(fhnet_L3_ds_vlan_action_hook,func);
}
int fhnet_us_vlan_action_mapping_callback(P_FHNET_US_VLAN_MAPPING_HOOK func)
{
	rcu_assign_pointer(fhnet_us_vlan_action_mapping_hook,func);
}
int fhnet_cpu_pkt_us_vlan_action_mapping_callback(P_FHNET_US_VLAN_MAPPING_HOOK func)
{
    printk("%s,%d\n",__FUNCTION__,__LINE__);
	rcu_assign_pointer(fhnet_L3_us_vlan_action_mapping_hook,func);
}

EXPORT_SYMBOL(fhnet_ds_vlan_action_callback);
EXPORT_SYMBOL(fhnet_cpu_pkt_ds_vlan_action_callback);
EXPORT_SYMBOL(fhnet_us_vlan_action_mapping_callback);
EXPORT_SYMBOL(fhnet_cpu_pkt_us_vlan_action_mapping_callback);


static int fhnet_us_vlan_action_mapping_debug(FHNet_Mapping_Vlan_Para_In_t *pktInfo, 
											   FHNet_Mapping_Result_Out_t   *mappingRet)
{
	*mappingRet=gMappingRet;
	PON_MSG(MSG_ERR, "fhnet_us_vlan_action_mapping_debug gemport %d queueId %d dropFlag %d \n",
		mappingRet->gemportId,mappingRet->queueId,mappingRet->dropFlag);
	return 0;
}
static int fhnet_l3_us_vlan_action_mapping_debug(FHNet_L3Pkt_Mapping_Vlan_Para_In_t *pktInfo, 
											   FHNet_Mapping_Result_Out_t   *mappingRet)
{
	*mappingRet=gMappingRet;
	PON_MSG(MSG_ERR, "fhnet_l3_us_vlan_action_mapping_debug gemport %d queueId %d dropFlag %d \n",
		mappingRet->gemportId,mappingRet->queueId,mappingRet->dropFlag);
	return 0;
}

static int fhnet_ds_vlan_action (FHNet_Mapping_Vlan_Para_In_t *pktInfo, 
						  unsigned char *dropFlag)
{
	*dropFlag = gFhDrop;
	return 0;
}

static int fhnet_l3_ds_vlan_action (FHNet_Mapping_Vlan_Para_In_t *pktInfo, 
						  unsigned char *dropFlag)
{
	*dropFlag = gFhDrop;
	return 0;
}

void fhnet_set_vlan_mapping_hook(void)
{
	static P_FHNET_US_VLAN_MAPPING_HOOK     us_vlan_mapping_hook = NULL;
	static P_FHNET_L3_US_VLAN_MAPPING_HOOK  l3_us_vlan_mapping_hook = NULL;
	static P_FHNET_DS_VLAN_HOOK 			ds_vlan_hook = NULL;
	static P_FHNET_L3_DS_VLAN_HOOK  		l3_ds_vlan_hook = NULL;	
	
	if(fhNetMappingDebug)
	{
		if(fhnet_L3_us_vlan_action_mapping_hook != fhnet_l3_us_vlan_action_mapping_debug)
		{
			us_vlan_mapping_hook    = fhnet_us_vlan_action_mapping_hook;
			l3_us_vlan_mapping_hook = fhnet_L3_us_vlan_action_mapping_hook;
			ds_vlan_hook    		= fhnet_ds_vlan_action_hook;
			l3_ds_vlan_hook 		= fhnet_L3_ds_vlan_action_hook;			
		}
		rcu_assign_pointer(fhnet_L3_us_vlan_action_mapping_hook,fhnet_l3_us_vlan_action_mapping_debug);
		rcu_assign_pointer(fhnet_us_vlan_action_mapping_hook,fhnet_us_vlan_action_mapping_debug);
		rcu_assign_pointer(fhnet_ds_vlan_action_hook,fhnet_ds_vlan_action);
		rcu_assign_pointer(fhnet_L3_ds_vlan_action_hook,fhnet_l3_ds_vlan_action);		
	}
	else
	{
		rcu_assign_pointer(fhnet_L3_us_vlan_action_mapping_hook,l3_us_vlan_mapping_hook);
		rcu_assign_pointer(fhnet_us_vlan_action_mapping_hook,us_vlan_mapping_hook);	
		rcu_assign_pointer(fhnet_ds_vlan_action_hook,ds_vlan_hook);
		rcu_assign_pointer(fhnet_L3_ds_vlan_action_hook,l3_ds_vlan_hook);			
	}
}

static int fhnet_get_inport_from_skb(struct sk_buff *skb)
{
	/* to do, inport may from lan, wifi and usb, so port number should be get from skb */
	unchar port = ETH_GET_LAN_PORT(skb);
	
	if(port >= E_ETH_PORT_0 && port <= E_ETH_PORT_3)
	{
		PON_MSG(MSG_ERR,"port from skb->mark is Lan %d",port);
	}
	else if(port >= E_WIFI_PORT_0 && port <= E_WIFI_PORT_3)
	{
		port = port - E_WIFI_PORT_0 + ECNT_PORT_DEVICE_RA0;
		PON_MSG(MSG_ERR,"port from skb->mark is Wifi %d",port);
	}
	#if 0 /* todo add usb port  */
	if(port < ETH_SKB_MARK_PORT_MAX_USB)
	{
		port = port - E_WIFI_PORT_3 - 1 + ECNT_PORT_DEVICE_USB;
		PON_MSG(MSG_ERR,"port from skb->mark is Usb %d",port);	
	}
	#endif
	else if(port == E_WAN_PORT)
	{
		port = ECNT_PORT_DEVICE_PON0;
		PON_MSG(MSG_ERR,"port from skb->mark is wan %d",port);			
	}
	else
	{
		PON_MSG(MSG_ERR,"port from skb->mark is unknow %d",port);	
	}
	
	return port;
}

static int fhnet_get_dev_by_skb(struct sk_buff *skb,struct net_device **dev)
{
	
    struct smux_api_data_s smux_data = {0};
	SMUX_Dev_By_Mac indate = {0};
	if(skb == NULL || dev == NULL)
	{
		printk("input para is null, skb %x dev %x\n",skb,dev);
		return RETURN_RET_NORMAL_FAIL;
	}
	skb_reset_mac_header(skb);
	
	indate.skb = skb;
	indate.dev = dev;
	
    smux_data.api_type = SMUX_API_TYPE_GET;
    smux_data.cmd_id   = SMUX_GET_DEV_BY_SRC_MAC;
    smux_data.smux_dev_by_mac      = &indate ;	

    if(ECNT_HOOK_ERROR == __ECNT_HOOK(ECNT_SMUX, ECNT_SMUX_API, (struct ecnt_data * )&smux_data) ){
        panic("ECNT_HOOK_ERROR occur with cmd_id:0x%x\n", smux_data.cmd_id);
		return RETURN_RET_NORMAL_FAIL;
    }	

	if(smux_data.ret != SMUX_ECNT_HOOK_PROCESS_SUCESS)
	{
		PON_MSG(MSG_ERR, "fhnet_get_dev_by_skb Fail \n");
		return RETURN_RET_NORMAL_FAIL;
	}
	return RETURN_RET_SUCCESS;
}


static int fhnet_check_l3_packet(struct sk_buff *skb, struct net_device ** dev)
{
    if(1 == skb->bridge_flag ){
        return 0;
    }

    if(dev != NULL){
        fhnet_get_dev_by_skb(skb, dev);
    }
    
	/* todo change skb->mark defination, bit 14 is bridge_flag */
    if(FHWC_IS_MARKED_ROUTE_PKT(skb) ){
        return 1;
    }

    return 0;
}

static int fhnet_get_port_from_dev(struct net_device *dev, unchar *uniPort)
{
    int name_len =0 ;
	if(NULL == dev)
	{
		printk("fenet_get_port_from_dev dev = %x is NULL\n",dev);
		return RETURN_RET_NORMAL_FAIL;
	}
    name_len = strlen(dev->name);
    if(name_len <= 0)
    {
        printk("dev name len is 0 \n");
        return RETURN_RET_SUCCESS;
    }
    
	switch(dev->name[0])
	{
		case 'e':
			*uniPort = atoi_temp(&dev->name[name_len -1]);
			break;

		case 'r':
			*uniPort = atoi_temp(&dev->name[name_len -1]) + ECNT_PORT_DEVICE_RA0;
			break;

		case 'u':
			*uniPort = atoi_temp(&dev->name[name_len -1]) + ECNT_PORT_DEVICE_USB;
			break;

		default:
			return RETURN_RET_NORMAL_FAIL;
	}
	PON_MSG(MSG_ERR,"dev name is %s len is %d uniport is %d\n",dev->name,name_len,*uniPort);
	return RETURN_RET_SUCCESS;
}

static int fhnet_gwan_vlan_mapping_proc(struct sk_buff *skb)
{
	FHNet_Mapping_Vlan_Para_In_t packetInfo;
    FHNet_L3Pkt_Mapping_Vlan_Para_In_t L3packetInfo;
	FHNet_Mapping_Result_Out_t mappingRet;
	struct net_device *dev = NULL;
	int ret = 0;

	if(NULL == skb)
	{
		printk("ERR %s input para is NULL \n",__FUNCTION__);
		return RETURN_RET_NORMAL_FAIL;
	}

	memset(&packetInfo, 0, sizeof(packetInfo));
    memset(&L3packetInfo, 0, sizeof(L3packetInfo));
	memset(&mappingRet, 0, sizeof(mappingRet));

	if(gpPonSysData->debugLevel & MSG_ERR) 
	{
		printk("====before fh mapping====\n");
		__dump_skb(skb, skb->len) ;
	} 

    if(fhnet_check_l3_packet(skb, &dev))
    {
		if(dev == NULL) 
		{
			PON_MSG(MSG_ERR, "fhnet_get_dev_by_skb dev is NULL \n");
			return RETURN_RET_NORMAL_FAIL;
		}
		L3packetInfo.skb = skb;
        strcpy(L3packetInfo.interfaceName,dev->name);
        PON_MSG(MSG_ERR, "Us L3packetInfo.interfaceName %s hook fh L3 vlan mapping\n",L3packetInfo.interfaceName);
        if(fhnet_L3_us_vlan_action_mapping_hook != NULL)
    	{
    		ret = fhnet_L3_us_vlan_action_mapping_hook(&L3packetInfo,&mappingRet);
    		if(ret != 0 || mappingRet.dropFlag == FHNET_IS_DROP_TRUE)
    		{
    			if(gpPonSysData->debugLevel & MSG_ERR) 
    			{
    				printk("ERR fhnet_L3_us_vlan_action_mapping_hook proc ret %d, dropFlag %d \n",ret,mappingRet.dropFlag);
    				printk("fhnet_L3_gwan_vlan_mapping_proc drop packet\n");
    				__dump_skb(skb, skb->len) ;
    			} 
    			return RETURN_RET_NORMAL_FAIL;
    	    }
        }
		else
		{
			PON_MSG(MSG_ERR, "fhnet_L3_us_vlan_action_mapping_hook is NULL return\n");
		}
    }
    else
    {
		packetInfo.skb = skb;
		packetInfo.inPort = fhnet_get_inport_from_skb(skb);
		packetInfo.outPort = ECNT_PORT_DEVICE_PON0;
		PON_MSG(MSG_ERR, "US L2 skb->mark %x inport %d, outport %d, hook fh vlan mapping\n",
		skb->mark, packetInfo.inPort, packetInfo.outPort);
		if(fhnet_us_vlan_action_mapping_hook != NULL)
		{
			ret = fhnet_us_vlan_action_mapping_hook(&packetInfo,&mappingRet);
			if(ret != 0 || mappingRet.dropFlag == FHNET_IS_DROP_TRUE)
			{
				if(gpPonSysData->debugLevel & MSG_ERR) 
				{
					printk("ERR fhnet_us_vlan_action_mapping_hook proc ret %d, dropFlag %d \n",ret,mappingRet.dropFlag);
					printk("fhnet_gwan_vlan_mapping_proc drop packet\n");
					__dump_skb(skb, skb->len) ;
				} 
				return RETURN_RET_NORMAL_FAIL;
			}
		}
		else
		{
			PON_MSG(MSG_ERR, "fhnet_us_vlan_action_mapping_hook is NULL return\n");
		}
	}
    PON_MSG(MSG_ERR, "mapping result gem %d, allocid %d, queue %d, dropflag %d\n",
	    mappingRet.gemportId,mappingRet.allocId,mappingRet.queueId, mappingRet.dropFlag);
	XPON_SKB(skb)->gem_port = mappingRet.gemportId;//mappingRet.gemportId;
    skb->mark = skb->mark |((mappingRet.queueId & 0x07) << 11); //((mappingRet.queueId & 0x07) << 10);	
	
	return RETURN_RET_SUCCESS;
}


int fhnet_ds_vlan_proc(struct sk_buff *skb, struct net_device *dev)
{
	FHNet_Mapping_Vlan_Para_In_t paraIn;
    FHNet_L3Pkt_Mapping_Vlan_Para_In_t L3paraIn;
	unsigned char inport = 0;
	unsigned char dropFlag = 0;
	struct net_device *l3dev = NULL;
	int ret = 0;
	unchar outPort = 0;
	PWAN_NetPriv_T * pNetPriv;
	
	if(NULL == skb || NULL == dev)
	{
		printk("fhnet_ds_vlan_proc input pointer skb = %x or dev =%x is null\n",skb,dev);
		return RETURN_RET_NORMAL_FAIL;
	}
	
	if(0 == strcmp(skb->dev->name, "omci") ){
		PON_MSG(MSG_ERR,"======pkt from omci, bypass!!========\n");
		return RETURN_RET_SUCCESS;
	}
	
	ret = fhnet_get_port_from_dev(dev, &outPort);
	if(ret == RETURN_RET_NORMAL_FAIL)
	{
		PON_MSG(MSG_ERR,"fhnet_get_port_from_dev return err\n");
		return RETURN_RET_SUCCESS;
	}

	inport = fhnet_get_inport_from_skb(skb);

	memset(&paraIn, 0, sizeof(paraIn));
	memset(&L3paraIn, 0, sizeof(L3paraIn));

	/*if packet not from cpu, call fhnet_ds_vlan_action_hook*/
	if(!fhnet_check_l3_packet(skb, NULL))
	{	
		paraIn.skb = skb;
		paraIn.outPort = outPort;
		paraIn.inPort =  inport;/* inport will be wan if skb->mark port is exist*/
		PON_MSG(MSG_ERR,"L2 DS fhnet_ds_vlan_action_hook dev %s, input outPort %d, inPort %d \n",dev->name,paraIn.outPort, paraIn.inPort);
		if(fhnet_ds_vlan_action_hook != NULL)
		{
			ret = fhnet_ds_vlan_action_hook(&paraIn, &dropFlag);
			if(ret !=RETURN_RET_SUCCESS || dropFlag == FHNET_IS_DROP_TRUE)
			{
				if(gpPonSysData->debugLevel & MSG_ERR) 
				{
                	printk("fhnet_ds_vlan_action_hook fail ret %d, dropFlag %d\n", ret, dropFlag);
					printk("fhnet_ds_vlan_action_hook drop packet\n");
    				__dump_skb(skb, skb->len) ;
				}
				return RETURN_RET_NORMAL_FAIL;
			}
		}
		else
		{
			PON_MSG(MSG_ERR, "fhnet_ds_vlan_action_hook dev is NULL return\n");
		}

	}
	else/* call another function to process cpu packet */
	{
		if(dev == NULL) 
		{
			PON_MSG(MSG_ERR, "fhnet_get_dev_by_skb is NULL \n");
			return RETURN_RET_NORMAL_FAIL;
		}
		L3paraIn.skb = skb;
        strcpy(L3paraIn.interfaceName,dev->name);
        PON_MSG(MSG_ERR, "DS L3packetInfo.interfaceName %s hook fh L3 ds vlan action\n",L3paraIn.interfaceName);
        if(fhnet_L3_ds_vlan_action_hook != NULL)
    	{
    		ret = fhnet_L3_ds_vlan_action_hook(&L3paraIn,&dropFlag);
    		if(ret != 0 || dropFlag == FHNET_IS_DROP_TRUE)
    		{
    			if(gpPonSysData->debugLevel & MSG_ERR) 
    			{
    				printk("ERR fhnet_L3_ds_vlan_action_hook proc ret %d, dropFlag %d \n",ret,dropFlag);
    				printk("fhnet_L3_ds_vlan_action_hook drop packet\n");
    				__dump_skb(skb, skb->len) ;
    			} 
    			return RETURN_RET_NORMAL_FAIL;
    	    }
        }
		else
		{
			PON_MSG(MSG_ERR, "fhnet_L3_ds_vlan_action_hook is NULL return\n");
		}
	}

	return RETURN_RET_SUCCESS;
}

static int ecnt_FhNetDsVlanAction_hook(struct ecnt_data * int_data)
{

    struct net_data_s * net_data = (struct net_data_s *)int_data;
    struct sk_buff * skb = NULL;
    struct net_device * dev = NULL;
    int ret = 0;
	
	if(net_data == NULL)
	{
		printk("%s para is null,line %d\n",__FUNCTION__,__LINE__);
        return ECNT_CONTINUE;
    }
	
	skb = skb_unshare(*(net_data->pskb), GFP_ATOMIC );
    if(skb == NULL)
	{
		printk("%s para is null,line %d\n",__FUNCTION__,__LINE__);
        return ECNT_CONTINUE;
    }

    *(net_data->pskb) = skb;    
	dev = skb->dev;
	
    ret = fhnet_ds_vlan_proc(skb, dev);
	if(ret != 0)
	{
		PON_MSG(MSG_ERR,"fhnet_ds_vlan_proc process fail return drop\n");
		return ECNT_RETURN_DROP;
	}
	PON_MSG(MSG_ERR,"ecnt_FhNetDsVlanAction_hook return %d\n",ECNT_CONTINUE);
    return ECNT_CONTINUE;
   
}

struct ecnt_hook_ops ecnt_FhNetDsVlanAction_op = {
    .name = "ecnt_checkTrafficWan2Wan_hook",
    .is_execute = 1,
    .hookfn = ecnt_FhNetDsVlanAction_hook,
    .maintype = ECNT_NET_CORE_DEV,
    .subtype = ECNT_DEV_QUE_XMIT,
    .priority = 1
};
#endif

typedef struct Omci_Hander{
    u16  transId;
    u8   msgType;
    u8   devId;
    u32    MeId;
    /* use in extended omci*/
    u16  msgContLen;
}__attribute__((packed)) Omci_Header_T, *pOmci_Header;

#define OMCI_CRC_LEN                (4)
#define OMCI_BASIC_MSG_FIX_LEN      (48)
#define OMCI_BASIC_MSG_DEV_ID       (0x0a)
#define OMCI_EXTENDED_MSG_DEV_ID    (0x0b)

static int remove_omci_crc_if_exist(struct sk_buff * skb)
{
    pOmci_Header pOmciHeader = NULL;
    u16 omci_msg_len = 0;
    u16 extra_len = 0;
    
    pOmciHeader = (pOmci_Header)skb->data;	/* 6.18: skb->data is u8 *, cast explicitly */

    if(unlikely(skb->len < sizeof(Omci_Header_T) )) /* exception, all basic and extended omci msg should longer than this! */
    {
        return XPON_FAIL;
    }
    
    if(OMCI_BASIC_MSG_DEV_ID == pOmciHeader->devId ) /*baseline omci*/
    {
        if(OMCI_BASIC_MSG_FIX_LEN == skb->len)
        {
            /*remove CRC*/
            skb->len -= OMCI_CRC_LEN;
        }
        else if((OMCI_BASIC_MSG_FIX_LEN - OMCI_CRC_LEN) != skb->len)
        {
            return XPON_FAIL;
        }
    }
    else if(OMCI_EXTENDED_MSG_DEV_ID == pOmciHeader->devId) /*extended omci*/
    {
        omci_msg_len = ntohs(pOmciHeader->msgContLen) + sizeof(Omci_Header_T);
        if(skb->len > omci_msg_len ) {
            /*remove Message integrity check, or any redundant data */
            extra_len =  skb->len - omci_msg_len;
            if(unlikely(OMCI_CRC_LEN != extra_len) ){
                PON_MSG(MSG_ERR, "Message integrity check length fault\n");
            }
            skb->len -= extra_len;
        }
    }

    return XPON_SUCCESS;
}

/*****************************************************************************
******************************************************************************/
int gwan_prepare_tx_message(PWAN_FETxMsg_T *pTxMsg, unchar netIdx, struct sk_buff *skb, int txq, struct port_info *xpon_info)
{
	int onu_type;
	uint gemIdx ;
	int isDefineTcsupportQoS;
#if defined(TCSUPPORT_PON_VLAN) && defined(TCSUPPORT_PON_VLAN_FILTER)&& defined(TCSUPPORT_XPON_IGMP)
	int is_up_igmp_pkt = 0;
#endif
	uint chnMask, qMask;

	if (isEN751221) {
		/* PPE foe's channel field has 8 bits for EN751221 and 4 bits for MT751020  */
		chnMask = 0xff;
		qMask = 0x7;
	} else {
		/* PPE foe's queue field has 3 bits for EN751221 and 4 bits for MT751020 */
		chnMask = 0xf;
		qMask = 0xf;
	}

#ifdef TCSUPPORT_QOS
	isDefineTcsupportQoS = XPON_ENABLE;
#else
	isDefineTcsupportQoS = XPON_DISABLE;
#endif

	pTxMsg->raw.fport = TXMSG_FPORT_GMAC ;
	if(netIdx == PWAN_IF_OMCI) {
		pTxMsg->raw.oam = 1 ;

        if(XPON_SUCCESS != remove_omci_crc_if_exist(skb)){
            XPON_DROP_PRINT;
            return -1;
        }
        
#ifndef TCSUPPORT_CPU_EN7521
		/* EN7512 tx queue is configured by QDMA */
		pTxMsg->raw.queue = 7 ;
#endif
		pTxMsg->raw.channel = 0 ;
		pTxMsg->raw.gem = GPON_OMCC_ID ;
#ifdef XPON_MAC_CONFIG_DEBUG
		if(gpPonSysData->debugLevel & MSG_OMCI) {
			PON_MSG(MSG_OMCI, "TX SKB LEN:%d, TX MSG: %.8x, %.8x", skb->len, pTxMsg->word[0], pTxMsg->word[1]) ;
			__dump_skb(skb, skb->len) ;
		}
#endif
	} else {
		pTxMsg->raw.oam = 0 ;
/*Warning: operations on skb must behind ct hwnat offload*/
#if defined(TCSUPPORT_CT_HWNAT_OFFLOAD)
	if((skb->qosEnque & 0x02) == 0x02){ /*from hw nat offload*/
		if(restore_offload_info_hook)
			restore_offload_info_hook(skb, xpon_info, FOE_MAGIC_GPON);
#ifdef TCSUPPORT_CPU_EN7521
			pTxMsg->raw.gem = xpon_info->stag;
			pTxMsg->raw.channel = xpon_info->channel & chnMask;
#else
			pTxMsg->raw.gem = xpon_info->stag>>4;
			pTxMsg->raw.channel = xpon_info->stag & chnMask;
			pTxMsg->raw.tse = xpon_info->tsid & 0x1;
			pTxMsg->raw.tsid = (xpon_info->tsid >> 1) & 0x1f;
			/* EN7512 tx queue is configured by QDMA */
			pTxMsg->raw.queue = xpon_info->txq & qMask;
#endif
		skb->qosEnque &= ~(0x02);
		return 0;
	}
#endif
/*Warning: operations on skb must behind ct hwnat offload*/

    #if defined(TCSUPPORT_PON_VLAN) && defined(TCSUPPORT_PON_VLAN_FILTER)&& defined(TCSUPPORT_XPON_IGMP)
        if((xpon_sfu_multicast_protocol_hook) && (xpon_sfu_multicast_protocol_hook(skb)) && (1 == get_pon_link_type()) //1:SFU
	        && (isVlanOperationInMulticastModule_hook) && (isVlanOperationInMulticastModule_hook(skb))) {
            is_up_igmp_pkt = 1;
	    }

	    if (is_up_igmp_pkt) {
            if ((xpon_sfu_up_multicast_vlan_hook) && (xpon_sfu_up_multicast_vlan_hook(skb,0) == -1)){
                XPON_DROP_PRINT;
                return -1;
            }

            if ((xpon_up_igmp_uni_vlan_filter_hook) && (xpon_up_igmp_uni_vlan_filter_hook(skb) == -1)){
                XPON_DROP_PRINT;
            	return -1;
            }
        }
    #endif
	
#ifdef TCSUPPORT_GPON_MAPPING
		if(gpon_mapping_hook && (-1 == gpon_mapping_hook(skb))){
            XPON_DROP_PRINT;
			return -1;
		}
#endif

#if defined(TCSUPPORT_FWC_ENV)
	if(0 != fhnet_gwan_vlan_mapping_proc(skb))
	{
		XPON_DROP_PRINT;
		return -1;
	}
#endif
		

		/* Just for FPGA Verification. the default gem port msut use default interface or dropped. */
		gemIdx = (XPON_SKB(skb)->gem_port) ? (gpWanPriv->gpon.gemIdToIndex[XPON_SKB(skb)->gem_port] & GPON_GEM_IDX_MASK) : CONFIG_GPON_MAX_GEMPORT ;
		if(gemIdx>=CONFIG_GPON_MAX_GEMPORT || !gpWanPriv->gpon.gemPort[gemIdx].info.valid || 
											  (gpWanPriv->gpon.gemPort[gemIdx].info.channel==GPON_MULTICAST_CHANNEL || gpWanPriv->gpon.gemPort[gemIdx].info.channel==GPON_UNKNOWN_CHANNEL) ||
											   gpWanPriv->gpon.gemPort[gemIdx].info.ani >= GPON_MAX_ANI_INTERFACE) {
			XPON_DROP_PRINT;
			return -1 ;
		}
		
		//Do GPON MAC filter
		XPON_SKB(skb)->v_if = gpWanPriv->gpon.gemPort[gemIdx].info.ani ;
    #if defined(TCSUPPORT_PON_VLAN) && defined(TCSUPPORT_PON_VLAN_FILTER) && defined(TCSUPPORT_XPON_IGMP)
        if (is_up_igmp_pkt)
		{
            if ((xpon_up_igmp_ani_vlan_filter_hook) && (xpon_up_igmp_ani_vlan_filter_hook(skb) == -1)){
                XPON_DROP_PRINT;
                return -1;
            }
    	}
	#endif
	
#ifdef TCSUPPORT_PON_VLAN
#if defined(TCSUPPORT_PON_VLAN) && defined(TCSUPPORT_PON_VLAN_FILTER)&& defined(TCSUPPORT_XPON_IGMP)
    	if (is_up_igmp_pkt == 0)
#endif
    {

		XPON_SKB(skb)->pon_vlan_flag |= PON_PKT_SEND_TO_WAN;
		if(pon_insert_tag_hook)
		{
			if(pon_insert_tag_hook(&skb) == -1){
                XPON_DROP_PRINT;
				return -1;
			}
		}
    }
#endif

		#ifdef TCSUPPORT_PON_MAC_FILTER
		XPON_SKB(skb)->pon_mac_filter_flag |= PKT_SEND_TO_WAN;
		if(pon_check_mac_hook)
		{
			if(pon_check_mac_hook(skb) == -1){
                XPON_DROP_PRINT;
				return -1;
			}
		}
		#endif

#if defined(TCSUPPORT_XPON_IGMP)
	    if(xpon_sfu_multicast_protocol_hook && xpon_sfu_multicast_protocol_hook(skb)) {
	        if(xpon_sfu_up_multicast_incoming_hook) {
	            if(0 >= xpon_sfu_up_multicast_incoming_hook(skb, 1)) {
                    XPON_DROP_PRINT;
	                return -1;
	            }
	        }
	    }
#endif
        
		//if(GPON_MAX_FILTER_PSEUDO(skb) != 0) {
		//	return -1 ;
		//}

#ifndef TCSUPPORT_CPU_EN7521
		/* EN7512 tx queue is configured by QDMA */
#if defined(TCSUPPORT_CT_PON) || defined(TCSUPPORT_XPON_HAL_API_QOS) || defined(TCSUPPORT_XPON_HAL_API_EXT)
		xmcs_get_onu_type(&onu_type);
		if(isDefineTcsupportQoS == XPON_DISABLE ||
		   (isDefineTcsupportQoS == XPON_ENABLE && SFU == onu_type)) {
			pTxMsg->raw.queue = (skb->mark & QOS_FILTER_MARK) >> 4 ;
		} else {
			pTxMsg->raw.queue = txq ;
		}
#else
		pTxMsg->raw.queue = (skb->mark & QOS_FILTER_MARK) >> 4 ;
#endif

		pTxMsg->raw.tse = (XPON_SKB(skb)->pon_mark&QOS_TSE_MARK) ? 1 : 0 ;
		pTxMsg->raw.tsid = (pTxMsg->raw.tse) ? (XPON_SKB(skb)->pon_mark&QOS_TSID_MARK) : 0 ;	

		/* Set pppoe packet to highest queue, queue 6 */
		if(isHighestPriorityPacket(skb)) {
			pTxMsg->raw.queue = queue6;
		}
#endif /* TCSUPPORT_CPU_EN7521 */
		pTxMsg->raw.gem = gpWanPriv->gpon.gemPort[gemIdx].info.portId ;
		pTxMsg->raw.channel = gpWanPriv->gpon.gemPort[gemIdx].info.channel ;

#if defined(TCSUPPORT_FWC_ENV)
		/* fhNet queueid is skb->mark bit 10 -bit 13 */
		pTxMsg->raw.queue = (skb->mark >> 11) & 0x07 ;
#endif

#ifdef TCSUPPORT_RA_HWNAT
		if((ra_sw_nat_hook_magic) && (ra_sw_nat_hook_magic(skb, FOE_MAGIC_PPE))) {
			pTxMsg->raw.fport = TXMSG_FPORT_PPE ;//DPORT_PPE
		} 

		if((ra_sw_nat_hook_tx != NULL)
#if defined(TCSUPPORT_CT_PON) || defined(TCSUPPORT_XPON_HAL_API_QOS) || defined(TCSUPPORT_XPON_HAL_API_EXT)			
#if defined(TCSUPPORT_CT_HWQOS) || defined(TCSUPPORT_XPON_HAL_API_QOS) || defined(TCSUPPORT_XPON_HAL_API_EXT)	
			&& (isNeedHWQoS == 1 )
#endif
#endif
		) {
    	 	 xpon_info->magic = FOE_MAGIC_GPON;
#ifdef TCSUPPORT_CPU_EN7521
			/* In EN7512/21, GPON's tcon is filled in PPE foe's channel field, GPON's gem port is filled in PPE foe's stag field. */
			/* EN7512 tx queue is configured by QDMA */
			xpon_info->stag = pTxMsg->raw.gem ;
			xpon_info->channel = pTxMsg->raw.channel & chnMask;
            
            xmcs_get_onu_type(&onu_type);
            if(isDefineTcsupportQoS == XPON_DISABLE ||
                (isDefineTcsupportQoS == XPON_ENABLE && SFU == onu_type)) {
                xpon_info->txq_is_valid = TRUE;
                xpon_info->txq = ((skb->mark & QOS_FILTER_MARK) >> 4) & qMask ;
            }
            else
            {
                xpon_info->txq_is_valid = FALSE;
            }

			PON_MSG(MSG_TRACE, "pwan_net_start_xmit:---gemid = 0x%x\n",xpon_info->stag) ; 
			PON_MSG(MSG_TRACE, "pwan_net_start_xmit:---xpon_info->channel = 0x%x\n",xpon_info->channel) ;
#else /* TCSUPPORT_CPU_EN7521 */
			/* In MT7510/20, GPON's tcon/gem port are all filled in PPE foe's stag field. */
			xpon_info->txq = pTxMsg->raw.queue & qMask;
			xpon_info->stag = (pTxMsg->raw.channel & chnMask) | (pTxMsg->raw.gem << 4);
			
		/* In MT7510/20, bit5~1 and bit0 of PPE foe's tsid field stand for tsid and tse respectively */
			xpon_info->tsid = ((pTxMsg->raw.tsid&0x1f)<<1) |  (pTxMsg->raw.tse&0x1);	
#if defined(TCSUPPORT_CT_PON) || defined(TCSUPPORT_XPON_HAL_API_QOS) || defined(TCSUPPORT_XPON_HAL_API_EXT)						
		//for TRTCM test --Trey
			if (trtcmEnable) {
#if defined(TCSUPPORT_CT_HWQOS) || defined(TCSUPPORT_XPON_HAL_API_QOS) || defined(TCSUPPORT_XPON_HAL_API_EXT)	
				if(xpon_info->txq == 0)	{			
					xpon_info->tsid = 0;
			}else{
					xpon_info->tsid = (((CAR_QUEUE_NUM - xpon_info->txq +1)&0x1f)<<1) | 0x1;
			}
#else		
				xpon_info->tsid = ((trtcmTsid&0x1f)<<1) | 0x1;
#endif
		}	
#endif

			PON_MSG(MSG_TRACE, "pwan_net_start_xmit:---gemid = 0x%x\n",xpon_info->stag>>4) ; 
			PON_MSG(MSG_TRACE, "pwan_net_start_xmit:---xpon_info->channel = 0x%x\n",xpon_info->stag&0xf) ;
			PON_MSG(MSG_TRACE, "pwan_net_start_xmit:---xpon_info->txq = 0x%x\n",xpon_info->txq) ;
			PON_MSG(MSG_TRACE, "pwan_net_start_xmit:---xpon_info->tse = 0x%x\n",xpon_info->tsid&0x1) ;
			PON_MSG(MSG_TRACE, "pwan_net_start_xmit:---xpon_info->tsid = 0x%x\n",(xpon_info->tsid>>1)&0x1f) ;

#if defined(TCSUPPORT_CT_SWQOS) || defined(TCSUPPORT_XPON_HAL_API_QOS) || defined(TCSUPPORT_XPON_HAL_API_EXT)	
		if((sw_isSWQosActive && !sw_isSWQosActive()) ||
		   (sw_isSWQosActive && sw_isSWQosActive() && sw_isHwnatOffloadEnable && sw_isHwnatOffloadEnable()))
#endif
		{
			if(ra_sw_nat_hook_tx(skb, xpon_info, xpon_info->magic) == 0) {
                XPON_DROP_PRINT;
				return 1 ; //don't printf err msg when drop keepalive packet.
			}
#if defined(TCSUPPORT_CT_HWNAT_OFFLOAD)		
			skb->qosEnque &= ~(0x02);
#endif
		}
#endif /* TCSUPPORT_CPU_EN7521 */
		}
#endif /* TCSUPPORT_RA_HWNAT */

	} 
	
	return 0 ;
}

int isBroadcastPkt(unchar *addr)
{
	if(addr == NULL)
		return -1;

	if((addr[0] == 0xff) &&  (addr[1] == 0xff) && (addr[2] == 0xff) && (addr[3] == 0xff) && (addr[4] == 0xff) && (addr[5] == 0xff))
		return 0;
	else
		return -1;
}

int isUnknownUnicastPkt(unchar *addr)
{
	unchar *myAddr = gpWanPriv->pPonNetDev[PWAN_IF_DATA]->dev_addr ;
	if(addr == NULL)
		return -1;

	if((addr[0] == myAddr[0]) &&  (addr[1] == myAddr[1]) && (addr[2] == myAddr[2]) && (addr[3] == myAddr[3]) && (addr[4] == myAddr[4]) && (addr[5] == myAddr[5]))
		return -1;
	else
		return 0;
}

int isMulticastPkt(unchar *addr)
{
	if(addr == NULL)
		return -1;

	if((addr[0] == 0x01) )
		return 0;
	else
		return -1;
}

void gwan_omci_check_start(struct sk_buff *skb)
{
	unchar *dp = ((unchar*)skb->data) + 2;
	static int cnt = 0;

	if (gpWanPriv->devCfg.flags.isTxDropOmcc && (GPON_CURR_STATE == GPON_STATE_O5)){
        /* if receive ONU-Data ME or 'set', 'create' msg  */
		if (/*(((*dp)&0xF)==0x9) && */(((*(dp+1))==0x0a) &&((*(dp+2))==0x00) &&((*(dp+3))==0x02)) 
			|| (((*dp)&0xF)==0x8)|| (((*dp)&0xF)==0x4)){
			if ((cnt++ % 20) == 0)
				PON_MSG(MSG_DBG, "%u Reset Drop Timer to %us\n",cnt, (TXDROP_TIMER_MS/1000));

			/* was GPON_START_TIMER() which used the removed timer.data */
			mod_timer(&gpWanPriv->txDropTimer,
				  jiffies + msecs_to_jiffies(TXDROP_TIMER_MS));
		}
	}
}


/*****************************************************************************
******************************************************************************/
int gwan_process_rx_message(PWAN_FERxMsg_T *pRxMsg, struct sk_buff *skb, uint pktLens, unchar *pFlag)
{
	uint netIdx ;
	uint gemPortId, gemIdx ;
	int i, foo ;
	unsigned char * mac = NULL;
	unsigned short tpid = 0;
	unsigned char tagNum = 0;


	if(pRxMsg->raw.crcer) {
#ifdef XPON_MAC_CONFIG_DEBUG
		if(gpPonSysData->debugLevel & MSG_ERR) {
			PON_MSG(MSG_ERR, "Packet Ethernet CRC Error:%.8x, %.8x, %.8x, %.8x\n", pRxMsg->word[0], pRxMsg->word[1], pRxMsg->word[2], pRxMsg->word[3]) ;
			__dump_skb(skb, pktLens) ;
		}
#endif
	}
	
	if(pRxMsg->raw.oam) {
		netIdx = PWAN_IF_OMCI ;
		gemIdx = (gpWanPriv->gpon.gemIdToIndex[GPON_OMCC_ID] & GPON_GEM_IDX_MASK);
		gpWanPriv->gpon.gemPort[gemIdx].stats.rx_packets++ ;
		gpWanPriv->gpon.gemPort[gemIdx].stats.rx_bytes += pktLens ;
		
		gwan_omci_check_start(skb);
		
#ifdef XPON_MAC_CONFIG_DEBUG
		if(gpPonSysData->debugLevel & MSG_OMCI) {
			PON_MSG(MSG_OMCI, "RX PKT LEN: %d, Rx Msg: %.8x, %.8x", pktLens, pRxMsg->word[0], pRxMsg->word[1]) ;
			__dump_skb(skb, pktLens) ;
		}
#endif
	} else {
		netIdx = PWAN_IF_DATA ;
/* handle broadcast /unknown unicast /multicast pkt */
		mac = skb->data ;
		if (!isBroadcastPkt(mac)) 
			netIdx += BROADCAST_OFFSET;
		else if (!isMulticastPkt(mac))
			netIdx += MULTICAST_OFFSET;
		else if(!isUnknownUnicastPkt(mac))
			netIdx += UNKNOWN_UNICAST_OFFSET;			
		
		gemPortId = pRxMsg->raw.gem ;
		gemIdx = (gpWanPriv->gpon.gemIdToIndex[gemPortId] & GPON_GEM_IDX_MASK);		
		if(gemIdx>=CONFIG_GPON_MAX_GEMPORT || !gpWanPriv->gpon.gemPort[gemIdx].info.valid || gpWanPriv->gpon.gemPort[gemIdx].info.ani>=GPON_MAX_ANI_INTERFACE) {
			if (gpGponPriv->gponCfg.flags.sniffer) {
				struct sk_buff *skb2 = NULL;
				int len = 6;
				const u_char omciHdr[14] = {0x00,0x00,0x00,0x00,0x00,0x01, 0x00,0x00,0x00,0x00,0x00,0x02, 0x88, 0xb5};
				int isOmciPkt = (pktLens == 44) ? XPON_ENABLE : XPON_DISABLE ;

				PON_MSG(MSG_WARN, "Packet Sniffer Gemport Id: %x\n", gemPortId) ;
				
				if(isOmciPkt)
					len += 14;

				/* process the packet data to high layer */		
				skb_put(skb, pktLens) ; 
				
				/* Check the skb headroom is enough or not.*/
				if(skb_headroom(skb) < len)
					skb2 = skb_copy_expand(skb, len, skb_tailroom(skb) , GFP_ATOMIC);
				else
					skb2 = skb_copy(skb, GFP_ATOMIC);

				if(skb2 == NULL){
					PON_MSG(MSG_ERR, "packet sniffer for skb alloc fail\n") ;
				} else {		
					if (isOmciPkt)
					{
						skb_push(skb2, 14);
						memcpy(skb2->data, omciHdr, 14);
					}
				
					skb2->mark |= SKBUF_COPYTOLAN;
					macSend(WAN2LAN_CH_ID,skb2); /* tc3262_mac_tx */
				}	

				/* restore the skb */
				skb_trim(skb, skb->len -pktLens);
			}
			goto fail ;
		}
		
		/* do PCP encode/decode check, if PCP self test enabled */
		if(gpGponPriv->gponCfg.dvtPcpCheck) {
			gpon_dvt_pcp_check(gpGponPriv->gponCfg.dvtPcpCheck, skb) ;
		}

		gpWanPriv->gpon.gemPort[gemIdx].stats.rx_packets++ ;
		gpWanPriv->gpon.gemPort[gemIdx].stats.rx_bytes += pktLens ;
        if(GPON_MULTICAST_CHANNEL == gpWanPriv->gpon.gemPort[gemIdx].info.channel){
            XPON_SKB(skb)->gem_type = GPON_MULTICAST_GEM ;
        }else{
            XPON_SKB(skb)->gem_type = GPON_UNICAST_GEM ;           
        }

		XPON_SKB(skb)->gem_port = gemPortId ;
		XPON_SKB(skb)->v_if = gpWanPriv->gpon.gemPort[gemIdx].info.ani ;
		XPON_SKB(skb)->pon_mark |= DS_PKT_FORM_WAN;
		
#ifdef TCSUPPORT_HWNAT_L2VID
		if(ra_sw_nat_hook_rx_set_l2lu)
			ra_sw_nat_hook_rx_set_l2lu(skb);
#endif
		
/*excute ani port mac filter function here*/		
#ifdef TCSUPPORT_PON_MAC_FILTER
		XPON_SKB(skb)->pon_mac_filter_flag |= PKT_SEND_TO_LAN;
		mac = skb->data + 12;
		 
		while((tpid =((*mac << 8) + *(mac + 1))) == 0x8100 ||(tpid == 0x88a8)||(tpid == 0x9100))
		{
			tagNum ++;
			mac += 4;
		}

		XPON_SKB(skb)->pon_tag_num = tagNum;
		if(pon_check_mac_hook)
		{
			if(pon_check_mac_hook(skb) == -1)
				goto fail;
		}
		XPON_SKB(skb)->pon_mac_filter_flag &= ~(PKT_SEND_TO_LAN) ;	
		XPON_SKB(skb)->pon_tag_num = 0;
#endif

		if(gpWanPriv->gpon.gemPort[gemIdx].info.rxLb) {
#ifdef CONFIG_USE_FOR_TEST
			if(gpWanPriv->devCfg.flags.isRandomLb) {
				foo = random32() % gpWanPriv->gpon.gemNumbers ;
				for(i=0 ; i<CONFIG_GPON_MAX_GEMPORT ; i++) {
					if(gpWanPriv->gpon.gemPort[i].info.valid) {
						if(!foo) {
							XPON_SKB(skb)->gem_port = gpWanPriv->gpon.gemPort[i].info.portId ;
							skb->mark = (skb->mark & ~QOS_FILTER_MARK) | ((random32()&0x7)<<4) ;
							break ;
						}
						foo-- ;
					}
				}
				if(i == CONFIG_GPON_MAX_GEMPORT) {
					skb->mark = (skb->mark & ~QOS_FILTER_MARK) | ((random32()&0x7)<<4) ;
					XPON_SKB(skb)->gem_port = gemPortId ;
				}
				*pFlag = 1 ;
			} else 
#endif /* CONFIG_USE_FOR_TEST */
			{
				if(gpWanPriv->gpon.gemPort[gemIdx].info.channel>=0 && gpWanPriv->gpon.gemPort[gemIdx].info.channel<CONFIG_GPON_MAX_TCONT) {
					XPON_SKB(skb)->gem_port = gemPortId ;
					*pFlag = 1 ;
				} else {
#ifdef XPON_MAC_CONFIG_DEBUG
					if(gpPonSysData->debugLevel & MSG_WARN) {
						PON_MSG(MSG_ERR, "Receive frame from multicast GEM port %d.\n", gemPortId) ;
						__dump_skb(skb, pktLens) ;
					}
#endif
				}
			}
		}
	}

	#ifdef TCSUPPORT_XPON_IGMP_CHT
	mac = skb->data;
	if(!isMulticastPkt(mac))
	{
		#ifdef TCSUPPORT_VLAN_TAG
		skb->vlan_tag_flag |= VLAN_TAG_INSERT_FLAG;
		skb->vlan_tag_flag |= VLAN_TAG_FROM_INDEV;
		#endif

		#ifdef TCSUPPORT_PON_VLAN
		XPON_SKB(skb)->pon_vlan_flag |= PON_PKT_INSERT_FLAG;
		#endif
	}
	#endif
	
#if defined(TCSUPPORT_FWC_ENV)
    FHWC_MARK_BRIDGE_PKT(skb);
#endif
	
	return netIdx ;
	
fail:
#ifdef XPON_MAC_CONFIG_DEBUG
	if(gpPonSysData->debugLevel & MSG_WARN) {
		PON_MSG(MSG_WARN, "Receive frame from invalid GEM port %d.\n", gemPortId) ;
		__dump_skb(skb, pktLens) ;
	}
#endif
	return -1-netIdx ;
}

/*****************************************************************************
******************************************************************************/
void gwan_update_gem_mib_table(unsigned long arg)
{
	int idx=0, i, j ;
	uint gemIdx ;
	
#ifndef TCSUPPORT_CPU_EN7521
	if(atomic_read(&fe_reset_flag) ) {
		return ;
	}
#endif				
	gponDevGemMibTablesInit() ;	
	gponDevUpdateGemMibIdxTable(0, 0) ;
	for(j=1, i=0 ; i<GPON_MAX_GEM_ID ; i++) {
		gemIdx = (gpWanPriv->gpon.gemIdToIndex[i] & GPON_GEM_IDX_MASK);
		if(gemIdx<CONFIG_GPON_MAX_GEMPORT && gpWanPriv->gpon.gemPort[gemIdx].info.valid) {
			idx++ ;
		}
		if((i&0xF)==0xF && j!=256) {
			gponDevUpdateGemMibIdxTable(j, idx) ;
			j++ ;
		}
	}
}

/*****************************************************************************
******************************************************************************/
int gwan_create_new_tcont(ushort allocId)
{
	char retValue ;
	unchar channel ;
	uint regVal = 0;
		
	retValue = gponDevEnableTCont(allocId) ;
	if(retValue>=0 || retValue<CONFIG_GPON_MAX_TCONT) {
		channel = retValue ;

		gpWanPriv->gpon.allocId[channel] = allocId ;

		#ifdef TCSUPPORT_CPU_EN7521
		/* Enable GDM2 channel Tx */
		FE_API_SET_CHANNEL_ENABLE(FE_GDM_SEL_GDMA2, FE_GDM_SEL_TX, channel, FE_ENABLE);
		/* Enable CDM2 Rx DMA */
		FE_API_SET_HWFWD_CHANNEL(FE_CDM_SEL_CDMA2, channel, FE_ENABLE);
		/* ★ P2b (2026-07-08): the FE_API above is a no-op __ECNT_HOOK stub, so the OMCI/data tcont TX
		 * channel is never enabled in GDM2 -> upstream OMCI can't egress -> OLT can't complete O5 reg.
		 * Enable GDM2 enabled_tx_channels directly. 0xBFB51524 = econet-eth en751221_regs GDM2 base
		 * 0xBFB51400 +0x124 (sibling of the confirmed rx_ch 0xBFB51528). */
		regVal = IO_GREG(0xBFB51524);
		{ extern int no_tcont_tx;
		  if(!no_tcont_tx)                                       /* rogue-US decisive test: 1 = skip US TX-enable */
			IO_SREG(0xBFB51524, regVal | (1u << channel));
		  printk(KERN_DEBUG "OMCIDBG: GDM2 TX ch%u %s: tx_ch %08x->%08x (no_tcont_tx=%d)\n",
			channel, no_tcont_tx ? "ENABLE-SKIPPED" : "enabled", regVal, IO_GREG(0xBFB51524), no_tcont_tx);
		}
		/* ★ 2026-07-13: mirror stock fe_set_hwfwd_channel() — CDMA2_HWF_CHN_EN(0xBFB5140C) |= (1<<channel).
		 * The FE_API_SET_HWFWD_CHANNEL(CDMA2,...) call above is a no-op __ECNT_HOOK stub with NO real
		 * register fallback (unlike the GDM2 TX-enable). Stock does this at EVERY T-CONT create
		 * (gpon_wan.c:1156 -> fe_api.c:437: CDMA2_HWF_CHN_EN |= 1<<ch). CDMA2_BASE 0xBFB51400 +0x0c. */
		{ extern int no_hwfwd_en; uint hwfVal = IO_GREG(0xBFB5140C);
		  if(!no_hwfwd_en)
			IO_SREG(0xBFB5140C, hwfVal | (1u << channel));
		  printk(KERN_DEBUG "OMCIDBG: CDMA2 HWF ch%u %s: hwf_ch %08x->%08x (no_hwfwd_en=%d)\n",
			channel, no_hwfwd_en ? "ENABLE-SKIPPED" : "enabled", hwfVal, IO_GREG(0xBFB5140C), no_hwfwd_en);
		}
		#else
		/* Enable LMGR channel Tx */
		regVal = IO_GREG(0xBFB51844);
		regVal = regVal & (~(1<<channel));
		IO_SREG(0xBFB51844, regVal);

		/* Enable GDM2 channel Tx */
		regVal = IO_GREG(0xBFB5152C);
		regVal = (regVal | (1<<channel));
		IO_SREG(0xBFB5152C, regVal);

		/* MT7520 has no CDM2 rx DMA register */
		#endif

		if(GPON_ONU_ID != allocId)
		{
			gpon_recover_backup_allocId(allocId);
		}
		
		return 0 ;
	} 
	
	return retValue ;
}

/*****************************************************************************
******************************************************************************/
int gwan_remove_tcont(ushort allocId)
{
	int i ;
	uint regVal = 0;
	unchar change_service_flag = false;

	for(i=0 ; i<CONFIG_GPON_MAX_TCONT ; i++) {
		if(gpWanPriv->gpon.allocId[i] == allocId) {
		
            if((2 == gpWanPriv->activeChannelNum) && (allocId != gpGponPriv->gponCfg.onu_id))
            {
                xpon_set_qdma_qos(QDMA_ENABLE);
                change_service_flag = true;
            }
            
			gpWanPriv->gpon.allocId[i] = GPON_UNASSIGN_ALLOC_ID ;

			#ifdef TCSUPPORT_CPU_EN7521
			/* Disable Channel for GDM2 Tx */
			FE_API_SET_CHANNEL_ENABLE(FE_GDM_SEL_GDMA2, FE_GDM_SEL_TX, i, FE_DISABLE);
			/* Disable CDM2 Rx DMA */
			FE_API_SET_HWFWD_CHANNEL(FE_CDM_SEL_CDMA2, i, FE_DISABLE);
			#else
			/* Disable LMGR channel Tx */
			regVal = IO_GREG(0xBFB51844);
			regVal |= (1<<i);
			IO_SREG(0xBFB51844, regVal);
			
			/* Disable GDM2 channel Tx */
			regVal = IO_GREG(0xBFB5152C);
			regVal &= ~(1<<i);
			IO_SREG(0xBFB5152C, regVal);

			/* MT7520 has no CDM2 rx DMA register */
			#endif

			gponDevDisableTCont(allocId);
#if !defined(TCSUPPORT_XPON_HAL_API) && !defined(TCSUPPORT_XPON_HAL_API_EXT) 
			if(change_service_flag)
#endif
			{
			    g_service_change_flag = 1;
			    change_service_flag   = false;
			}

			if(allocId != GPON_ONU_ID){
				gpon_recover_delete_allocId(allocId);
			}
			return 0 ;
		}
	}
	
	return -ENOENT ;
}

/*****************************************************************************
******************************************************************************/
int gwan_remove_all_tcont(void)
{
	int i ;

	for(i=0 ; i<CONFIG_GPON_MAX_TCONT ; i++) {
		gpWanPriv->gpon.allocId[i] = GPON_UNASSIGN_ALLOC_ID ;
	}

	return 0 ;
}

int gwanCheckAllocIdExist(ushort allocId)
{
	int i = 0;

	for(i=0 ; i<CONFIG_GPON_MAX_TCONT ; i++) 
	{
		if(gpWanPriv->gpon.allocId[i] == allocId )
			/*&& gponDevCheckTContReg(i, allocId))*/
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*****************************************************************************
******************************************************************************/
int gwan_create_new_gemport(ushort gemPortId, unchar channel, unchar encryption)
{
	int i = 0;
	ushort encry_info;

	if(gemPortId>=GPON_MAX_GEM_ID || (channel>=CONFIG_GPON_MAX_TCONT && channel!=GPON_MULTICAST_CHANNEL && channel!=GPON_UNKNOWN_CHANNEL)) {
		return -EINVAL ;
	}

	if(gpWanPriv->gpon.gemNumbers >= CONFIG_GPON_MAX_GEMPORT) {
		return -ENOSPC ;
	}

	for(i=0 ; i<CONFIG_GPON_MAX_GEMPORT ; i++) 
	{
		if(gpWanPriv->gpon.gemPort[i].info.valid){
			continue;
		}
		
		memset(&gpWanPriv->gpon.gemPort[i].info, 0, sizeof(GWAN_GemInfo_T)) ;
		memset(&gpWanPriv->gpon.gemPort[i].stats, 0, sizeof(struct net_device_stats)) ;

		encry_info = (gpWanPriv->gpon.gemIdToIndex[gemPortId] & GPON_GEM_ENCRY_MASK);
		gpWanPriv->gpon.gemIdToIndex[gemPortId] = i + encry_info;
		gpWanPriv->gpon.gemPort[i].info.portId = gemPortId ;
		gpWanPriv->gpon.gemPort[i].info.ani = (gemPortId==GPON_OMCC_ID) ? GPON_OMCI_INTERFACE : GPON_MAX_ANI_INTERFACE ;
		gpWanPriv->gpon.gemPort[i].info.channel = channel ;
		gpWanPriv->gpon.gemPort[i].info.rxEncrypt = (encryption) ? 1 : (encry_info ? 1 : 0) ;
		gpWanPriv->gpon.gemPort[i].info.valid = 1 ;
		gpWanPriv->gpon.gemNumbers++ ;

		if(gemPortId != GPON_OMCC_ID)
		{
			ushort allocid = GPON_UNASSIGN_ALLOC_ID;
			
			if(GPON_MULTICAST_CHANNEL !=  channel)
			{
				allocid = gpWanPriv->gpon.allocId[channel];
			}
			
			struct Gem_Recovery_Info bakGemPort = {0};
			
			bakGemPort.allocId = allocid;
			bakGemPort.gemPortId = gemPortId;
			bakGemPort.channel = channel;
			bakGemPort.encryption = encryption;
			gpon_recover_backup_gemport(&bakGemPort);
		}
		
		gponDevSetGemInfo(gemPortId, XPON_ENABLE, (gpWanPriv->gpon.gemPort[i].info.rxEncrypt)?XPON_ENABLE:XPON_DISABLE) ;

		mod_timer(&gpWanPriv->gpon.gemMibTimer, (jiffies + (2*HZ))) ; /* 2000 ms */
		
		return 0 ;
	}
	
	return -ENOENT ;
}

/*****************************************************************************
******************************************************************************/
int gwan_config_gemport(ushort gemPortId, ENUM_GWanGemCfgType_t cfgType, uint value)
{
	uint gemIdx ;
	struct Gem_Recovery_Info * tmp = NULL;

	if(gemPortId >= GPON_MAX_GEM_ID) {
		return -EINVAL ;
	}

	gemIdx = (gpWanPriv->gpon.gemIdToIndex[gemPortId] & GPON_GEM_IDX_MASK);
	if(gemIdx>=CONFIG_GPON_MAX_GEMPORT || !gpWanPriv->gpon.gemPort[gemIdx].info.valid || gpWanPriv->gpon.gemPort[gemIdx].info.portId!=gemPortId) {
		return -ENOENT ;
	}

	tmp = find_backup_gemport(gemPortId);

	switch(cfgType) {
		case ENUM_CFG_NETIDX:
			gpWanPriv->gpon.gemPort[gemIdx].info.ani = (value>=GPON_MAX_ANI_INTERFACE) ? GPON_MAX_ANI_INTERFACE : value ;
			break ;
			
		case ENUM_CFG_CHANNEL:
			if(value>=CONFIG_GPON_MAX_TCONT && value!=GPON_MULTICAST_CHANNEL) {
				return -EINVAL ;
			}			
			gpWanPriv->gpon.gemPort[gemIdx].info.channel = value ;

			if(NULL != tmp)
			{
				if(GPON_MULTICAST_CHANNEL == value){
					tmp->allocId = GPON_UNASSIGN_ALLOC_ID;
				}else{
					tmp->allocId = gpWanPriv->gpon.allocId[value];
				}
				tmp->channel = value;
			
			}
			else if(gponRecovery.dbgPrint)
			{
			    printk("%s %d line change channel err\n", __FUNCTION__,__LINE__);
			}
			
			break ;
			
		case ENUM_CFG_ENCRYPTION:
			if(gpWanPriv->gpon.gemPort[gemIdx].info.channel==GPON_MULTICAST_CHANNEL || gpWanPriv->gpon.gemPort[gemIdx].info.channel==GPON_UNKNOWN_CHANNEL) {
				return -EINVAL ;
			}			
			gpWanPriv->gpon.gemPort[gemIdx].info.rxEncrypt = (value)?1:0 ;
			gponDevSetGemInfo(gemPortId, XPON_ENABLE, (value)?XPON_ENABLE:XPON_DISABLE) ;
			
			if(NULL != tmp)
			{
				tmp->encryption = (value)?1:0;
			}
			else if(gponRecovery.dbgPrint)
			{
				printk("%s %d line change encryption err\n", __FUNCTION__,__LINE__);
			}
			
			break ;
			
		case ENUM_CFG_LOOPBACK:
			if(gpWanPriv->gpon.gemPort[gemIdx].info.channel==GPON_MULTICAST_CHANNEL || gpWanPriv->gpon.gemPort[gemIdx].info.channel==GPON_UNKNOWN_CHANNEL) {
				return -EINVAL ;
			}

			gpWanPriv->gpon.gemPort[gemIdx].info.rxLb = (value)?1:0 ;
			break ;
			
		default:
			return -EINVAL ;
	}
	return 0 ;
}

/*****************************************************************************
******************************************************************************/
int gwan_remove_gemport(ushort gemPortId)
{
	uint gemIdx ;

	if(gemPortId>=GPON_MAX_GEM_ID || gemPortId==GPON_OMCC_ID) {
		return -EINVAL ;
	}

	gemIdx = (gpWanPriv->gpon.gemIdToIndex[gemPortId] & GPON_GEM_IDX_MASK);
	if(gemIdx>=CONFIG_GPON_MAX_GEMPORT || !gpWanPriv->gpon.gemPort[gemIdx].info.valid) {
		return -ENOENT ;
	}

	memset(&gpWanPriv->gpon.gemPort[gemIdx].info, 0, sizeof(GWAN_GemInfo_T)) ;
	memset(&gpWanPriv->gpon.gemPort[gemIdx].stats, 0, sizeof(struct net_device_stats)) ;			
	gpWanPriv->gpon.gemIdToIndex[gemPortId] |= GPON_GEM_IDX_MASK ;
	
	gpWanPriv->gpon.gemNumbers-- ;
	gponDevSetGemInfo(gemPortId, XPON_DISABLE, XPON_DISABLE) ;
	mod_timer(&gpWanPriv->gpon.gemMibTimer, (jiffies + (2*HZ))) ; /* 2000 ms */

	if(GPON_OMCC_ID != gemPortId){
		gpon_recover_delete_gemport(gemPortId);
	}
	
	return 0 ;
}

/*****************************************************************************
******************************************************************************/
int gwan_remove_all_gemport_for_disable(void)
{
	ushort gemPortId ;
	int i ;
	
	for(i=0 ; i<CONFIG_GPON_MAX_GEMPORT ; i++) {
		if(gpWanPriv->gpon.gemPort[i].info.valid && gpWanPriv->gpon.gemPort[i].info.portId!=GPON_OMCC_ID) {
			gemPortId = gpWanPriv->gpon.gemPort[i].info.portId ;
			gpWanPriv->gpon.gemIdToIndex[gemPortId] = GPON_GEM_IDX_MASK ;
			
			memset(&gpWanPriv->gpon.gemPort[i].info, 0, sizeof(GWAN_GemInfo_T)) ;
			memset(&gpWanPriv->gpon.gemPort[i].stats, 0, sizeof(struct net_device_stats)) ;
		}
	}

	gpWanPriv->gpon.gemNumbers = 0 ;
	gwan_update_gem_mib_table(0) ;

	return 0 ;
}


/*****************************************************************************
******************************************************************************/
int gwan_remove_all_gemport(void)
{
	ushort gemPortId ;
	int i ;
	
	for(i=0 ; i<CONFIG_GPON_MAX_GEMPORT ; i++) {
		if(gpWanPriv->gpon.gemPort[i].info.valid && gpWanPriv->gpon.gemPort[i].info.portId!=GPON_OMCC_ID) {
			gemPortId = gpWanPriv->gpon.gemPort[i].info.portId ;
			gpWanPriv->gpon.gemIdToIndex[gemPortId] = GPON_GEM_IDX_MASK ;
			
			memset(&gpWanPriv->gpon.gemPort[i].info, 0, sizeof(GWAN_GemInfo_T)) ;
			memset(&gpWanPriv->gpon.gemPort[i].stats, 0, sizeof(struct net_device_stats)) ;

			gponDevSetGemInfo(gemPortId, XPON_DISABLE, XPON_DISABLE) ;
		}
	}

	gpWanPriv->gpon.gemNumbers = 0 ;
	gponDevResetGemInfo() ;
	gwan_update_gem_mib_table(0) ;

	return 0 ;
}

/*****************************************************************************
******************************************************************************/
int gwan_is_gemport_valid(ushort gemPortId)
{
	uint gemIdx ;

	if(gemPortId >= GPON_MAX_GEM_ID) {
		return -EINVAL ;
	}

	gemIdx = (gpWanPriv->gpon.gemIdToIndex[gemPortId] & GPON_GEM_IDX_MASK);
	return ((gemIdx<CONFIG_GPON_MAX_GEMPORT) ? gpWanPriv->gpon.gemPort[gemIdx].info.valid : -1) ;
}

/*****************************************************************************
******************************************************************************/
static int service_change_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int len;

	len = sprintf(page, "%d\n", g_service_change_flag);

	len -= off;
	*start = page + off;

	if (len > count)
		len = count;
	else
		*eof = 1;

	if (len < 0)
		len = 0;

	return len;
}

static int service_change_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
    
	char val_string[8]={0};
    uint val=0;;
	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
    
	val_string[count] = '\0';

	sscanf(val_string, "%d", &val);

    g_service_change_flag = val;
	
	return count;

}

int service_change_proc_init(void)
{
    g_service_change_flag = 0;

    //with traffic, change service, reset
    struct compat_proc_priv *service_change_proc = NULL;
    service_change_proc = create_proc_entry("tc3162/service_change", 0, NULL);
    if(NULL == service_change_proc)
    {
        printk("creat service change proc faile.\n");
        return -1;
    }
    
    service_change_proc->read_proc  = service_change_read_proc;
    service_change_proc->write_proc = service_change_write_proc;
    
    return 0;
    
}

GPON_TRAFFIC_STATUS_t gwan_get_traffic_status()
{
	if( (gpGponPriv->typeBOnGoing) || \
	    (GPON_STATE_O6 == GPON_CURR_STATE) ){
		return GPON_TRAFFIC_UP;
	}

	if( MONITOR_DONE != gpPonSysData->Omci_Oam_Monitor.run_state || \
		GPON_CURR_STATE != GPON_STATE_O5){
        return GPON_TRAFFIC_DOWN;
    }

    return GPON_TRAFFIC_UP;
}

/*****************************************************************************
******************************************************************************/
int gwan_init(GWAN_Priv_T *pGWanPriv)
{
	int i ;
	
	for(i=0 ; i<CONFIG_GPON_MAX_TCONT ; i++) {
		pGWanPriv->allocId[i] = GPON_UNASSIGN_ALLOC_ID;
	}

	for(i=0 ; i<GPON_MAX_GEM_ID ; i++) {
		pGWanPriv->gemIdToIndex[i] = GPON_GEM_IDX_MASK;
	}
	
	for(i=0 ; i<CONFIG_GPON_MAX_GEMPORT ; i++) {
		memset(&pGWanPriv->gemPort[i].info, 0, sizeof(GWAN_GemInfo_T)) ;
		memset(&pGWanPriv->gemPort[i].stats, 0, sizeof(struct net_device_stats)) ;
	}
	pGWanPriv->gemNumbers = 0 ;

	timer_setup(&pGWanPriv->gemMibTimer, econet_xpon_gemmib_timer_cb, 0) ;
    service_change_proc_init();
	pwan_create_net_interface(PWAN_IF_OMCI) ;

#ifdef TCSUPPORT_CPU_EN7521
#ifdef TCSUPPORT_WAN_GPON
	/* disable all tx channel */
	for(i = 0; i < CONFIG_GPON_MAX_TCONT; i++) {
		FE_API_SET_CHANNEL_ENABLE(FE_GDM_SEL_GDMA2, FE_GDM_SEL_TX, i, FE_DISABLE);
	}
	/* disable all rx channel */
	for(i = 0; i < 16; i++) {
		FE_API_SET_CHANNEL_ENABLE(FE_GDM_SEL_GDMA2, FE_GDM_SEL_RX, i, FE_DISABLE);
	}
	/* enable 2 rx channel */
	for(i = 0; i < 2; i++) {
		FE_API_SET_CHANNEL_ENABLE(FE_GDM_SEL_GDMA2, FE_GDM_SEL_RX, i, FE_ENABLE);
	}
	/* set GPON packet length */
	FE_API_SET_PACKET_LENGTH(FE_GDM_SEL_GDMA2, GPON_PACKET_LEN_UPPER_LIMIT, GPON_PACKET_LEN_LOWER_LIMIT);
#endif
#endif
	return 0 ;	
}

#endif /* TCSUPPORT_WAN_GPON */
