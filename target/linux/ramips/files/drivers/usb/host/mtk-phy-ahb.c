#include "mtk-phy.h"
#ifdef CONFIG_U3D_HAL_SUPPORT
#include "mu3d_hal_osal.h"
#endif

#ifdef CONFIG_U3_PHY_AHB_SUPPORT
#include <linux/gfp.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#ifndef CONFIG_U3D_HAL_SUPPORT
#define os_writel(addr,data) {\
		(*((volatile PHY_UINT32*)(addr)) = data);\
	}
#define os_readl(addr)  *((volatile PHY_UINT32*)(addr))
#define os_writelmsk(addr, data, msk) \
		{ os_writel(addr, ((os_readl(addr) & ~(msk)) | ((data) & (msk)))); \
	}
#define os_setmsk(addr, msk) \
	{ os_writel(addr, os_readl(addr) | msk); \
	}
#define os_clrmsk(addr, msk) \
   { os_writel(addr, os_readl(addr) &~ msk); \
   }
/*msk the data first, then umsk with the umsk.*/
#define os_writelmskumsk(addr, data, msk, umsk) \
{\
   os_writel(addr, ((os_readl(addr) & ~(msk)) | ((data) & (msk))) & (umsk));\
}

#endif

PHY_INT32 U3PhyWriteReg32(PHY_UINT32 addr, PHY_UINT32 data)
{
	os_writel(addr, data);

	return 0;
}

PHY_INT32 U3PhyReadReg32(PHY_UINT32 addr)
{
	return os_readl(addr);
}

PHY_INT32 U3PhyWriteReg8(PHY_UINT32 addr, PHY_UINT8 data)
{
	os_writelmsk(addr&0xfffffffc, data<<((addr%4)*8), 0xff<<((addr%4)*8));
	
	return 0;
}

PHY_INT8 U3PhyReadReg8(PHY_UINT32 addr)
{
	return ((os_readl(addr)>>((addr%4)*8))&0xff);
}

#endif

