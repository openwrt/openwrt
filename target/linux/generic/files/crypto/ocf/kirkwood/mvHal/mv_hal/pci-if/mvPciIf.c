/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File under the following licensing terms. 
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer. 

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution. 

    *   Neither the name of Marvell nor the names of its contributors may be 
        used to endorse or promote products derived from this software without 
        specific prior written permission. 
    
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "mvPciIf.h"
#include "ctrlEnv/sys/mvSysPex.h"

#if defined(MV_INCLUDE_PCI)
#include "ctrlEnv/sys/mvSysPci.h"
#endif


/* defines  */       
#ifdef MV_DEBUG         
	#define DB(x)	x
#else                
	#define DB(x)    
#endif	             
					 

/*******************************************************************************
* mvPciInit - Initialize PCI interfaces
*
* DESCRIPTION:
*
* INPUT:
*
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK if function success otherwise MV_ERROR or MV_BAD_PARAM
*
*******************************************************************************/


MV_STATUS mvPciIfInit(MV_U32 pciIf, PCI_IF_MODE pciIfmode)
{
	PCI_IF_TYPE pciIfType = mvPciIfTypeGet(pciIf);

	if (PCI_IF_TYPE_CONVEN_PCIX == pciIfType)
	{
		#if defined(MV_INCLUDE_PCI)
		
        MV_PCI_MOD pciMod;
		
        if (PCI_IF_MODE_HOST == pciIfmode)
        {
            pciMod = MV_PCI_MOD_HOST;
        }
        else if (PCI_IF_MODE_DEVICE == pciIfmode)
        {
            pciMod = MV_PCI_MOD_DEVICE;
        }
        else
        {
            mvOsPrintf("%s: ERROR!!! Bus %d mode %d neither host nor device!\n", 
                        __FUNCTION__, pciIf, pciIfmode);
            return MV_FAIL;
        }
        
        return mvPciInit(pciIf - MV_PCI_START_IF, pciMod);
		#else
		return MV_OK;
		#endif
	}
	else if (PCI_IF_TYPE_PEX == pciIfType)
	{
		#if defined(MV_INCLUDE_PEX)
		
        MV_PEX_TYPE pexType;
		
        if (PCI_IF_MODE_HOST == pciIfmode)
        {
            pexType = MV_PEX_ROOT_COMPLEX;
        }
        else if (PCI_IF_MODE_DEVICE == pciIfmode)
        {
            pexType = MV_PEX_END_POINT;
        }
        else
        {
            mvOsPrintf("%s: ERROR!!! Bus %d type %d neither root complex nor" \
                       " end point\n", __FUNCTION__, pciIf, pciIfmode);
            return MV_FAIL;
        }
		return mvPexInit(pciIf - MV_PEX_START_IF, pexType);

		#else
		return MV_OK;
		#endif

	}
	else
	{
		mvOsPrintf("%s: ERROR!!! Invalid pciIf %d\n", __FUNCTION__, pciIf);
	}

	return MV_FAIL;

}

/* PCI configuration space read write */

/*******************************************************************************
* mvPciConfigRead - Read from configuration space
*
* DESCRIPTION:
*       This function performs a 32 bit read from PCI configuration space.
*       It supports both type 0 and type 1 of Configuration Transactions 
*       (local and over bridge). In order to read from local bus segment, use 
*       bus number retrieved from mvPciLocalBusNumGet(). Other bus numbers 
*       will result configuration transaction of type 1 (over bridge).
*
* INPUT:
*       pciIf   - PCI interface number.
*       bus     - PCI segment bus number.
*       dev     - PCI device number.
*       func    - Function number.
*       regOffs - Register offset.       
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit register data, 0xffffffff on error
*
*******************************************************************************/
MV_U32 mvPciIfConfigRead (MV_U32 pciIf, MV_U32 bus, MV_U32 dev, MV_U32 func, 
                        MV_U32 regOff)
{
	PCI_IF_TYPE pciIfType = mvPciIfTypeGet(pciIf);

	if (PCI_IF_TYPE_CONVEN_PCIX == pciIfType)
	{
		#if defined(MV_INCLUDE_PCI)
		return mvPciConfigRead(pciIf - MV_PCI_START_IF,
								bus,
								dev, 
                                func,
								regOff);
		#else
		return 0xffffffff;
		#endif
	}
	else if (PCI_IF_TYPE_PEX == pciIfType)
	{
		#if defined(MV_INCLUDE_PEX)
		return mvPexConfigRead(pciIf - MV_PEX_START_IF,
								bus,
								dev, 
                                func,
								regOff);
		#else
		return 0xffffffff;
		#endif

	}
	else
	{
		mvOsPrintf("%s: ERROR!!! Invalid pciIf %d\n", __FUNCTION__, pciIf);
	}

	return 0;

}

/*******************************************************************************
* mvPciConfigWrite - Write to configuration space
*
* DESCRIPTION:
*       This function performs a 32 bit write to PCI configuration space.
*       It supports both type 0 and type 1 of Configuration Transactions 
*       (local and over bridge). In order to write to local bus segment, use 
*       bus number retrieved from mvPciLocalBusNumGet(). Other bus numbers 
*       will result configuration transaction of type 1 (over bridge).
*
* INPUT:
*       pciIf   - PCI interface number.
*       bus     - PCI segment bus number.
*       dev     - PCI device number.
*       func    - Function number.
*       regOffs - Register offset.       
*       data    - 32bit data.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM for bad parameters ,MV_ERROR on error ! otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPciIfConfigWrite(MV_U32 pciIf, MV_U32 bus, MV_U32 dev, 
                           MV_U32 func, MV_U32 regOff, MV_U32 data)
{
	PCI_IF_TYPE pciIfType = mvPciIfTypeGet(pciIf);

	if (PCI_IF_TYPE_CONVEN_PCIX == pciIfType)
	{
		#if defined(MV_INCLUDE_PCI)
		return mvPciConfigWrite(pciIf - MV_PCI_START_IF,
								bus,
								dev, 
                                func,
								regOff,
								data);
		#else
		return MV_OK;
		#endif
	}
	else if (PCI_IF_TYPE_PEX == pciIfType)
	{
		#if defined(MV_INCLUDE_PEX)
		return mvPexConfigWrite(pciIf - MV_PEX_START_IF,
								bus,
								dev, 
                                func,
								regOff,
								data);
		#else
		return MV_OK;
		#endif

	}
	else
	{
		mvOsPrintf("%s: ERROR!!! Invalid pciIf %d\n", __FUNCTION__, pciIf);
	}

	return MV_FAIL;

}

/*******************************************************************************
* mvPciMasterEnable - Enable/disale PCI interface master transactions.
*
* DESCRIPTION:
*       This function performs read modified write to PCI command status 
*       (offset 0x4) to set/reset bit 2. After this bit is set, the PCI 
*       master is allowed to gain ownership on the bus, otherwise it is 
*       incapable to do so.
*
* INPUT:
*       pciIf  - PCI interface number.
*       enable - Enable/disable parameter.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM for bad parameters ,MV_ERROR on error ! otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPciIfMasterEnable(MV_U32 pciIf, MV_BOOL enable)
{

	PCI_IF_TYPE pciIfType = mvPciIfTypeGet(pciIf);

	if (PCI_IF_TYPE_CONVEN_PCIX == pciIfType)
	{
		#if defined(MV_INCLUDE_PCI)
		return mvPciMasterEnable(pciIf - MV_PCI_START_IF,
								enable);
		#else
		return MV_OK;
		#endif
	}
	else if (PCI_IF_TYPE_PEX == pciIfType)
	{
		#if defined(MV_INCLUDE_PEX)
		return mvPexMasterEnable(pciIf - MV_PEX_START_IF,
								enable);
		#else
		return MV_OK;
		#endif
	}
	else
	{
		mvOsPrintf("%s: ERROR!!! Invalid pciIf %d\n", __FUNCTION__, pciIf);
	}

	return MV_FAIL;

}


/*******************************************************************************
* mvPciSlaveEnable - Enable/disale PCI interface slave transactions.
*
* DESCRIPTION:
*       This function performs read modified write to PCI command status 
*       (offset 0x4) to set/reset bit 0 and 1. After those bits are set, 
*       the PCI slave is allowed to respond to PCI IO space access (bit 0) 
*       and PCI memory space access (bit 1). 
*
* INPUT:
*       pciIf  - PCI interface number.
*       dev     - PCI device number.
*       enable - Enable/disable parameter.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM for bad parameters ,MV_ERROR on error ! otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPciIfSlaveEnable(MV_U32 pciIf,MV_U32 bus, MV_U32 dev, MV_BOOL enable)
{

	PCI_IF_TYPE pciIfType = mvPciIfTypeGet(pciIf);

	if (PCI_IF_TYPE_CONVEN_PCIX == pciIfType)
	{
		#if defined(MV_INCLUDE_PCI)
		return mvPciSlaveEnable(pciIf - MV_PCI_START_IF,bus,dev,
								enable);
		#else
		return MV_OK;
		#endif
	}
	else if (PCI_IF_TYPE_PEX == pciIfType)
	{
		#if defined(MV_INCLUDE_PEX)
		return mvPexSlaveEnable(pciIf - MV_PEX_START_IF,bus,dev,
								enable);
		#else
		return MV_OK;
		#endif
	}
	else
	{
		mvOsPrintf("%s: ERROR!!! Invalid pciIf %d\n", __FUNCTION__, pciIf);
	}

	return MV_FAIL;

}

/*******************************************************************************
* mvPciLocalBusNumSet - Set PCI interface local bus number.
*
* DESCRIPTION:
*       This function sets given PCI interface its local bus number.
*       Note: In case the PCI interface is PCI-X, the information is read-only.
*
* INPUT:
*       pciIf  - PCI interface number.
*       busNum - Bus number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_NOT_ALLOWED in case PCI interface is PCI-X. 
*		MV_BAD_PARAM on bad parameters ,
*       otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPciIfLocalBusNumSet(MV_U32 pciIf, MV_U32 busNum)
{
	PCI_IF_TYPE pciIfType = mvPciIfTypeGet(pciIf);

	if (PCI_IF_TYPE_CONVEN_PCIX == pciIfType)
	{
		#if defined(MV_INCLUDE_PCI)
		return mvPciLocalBusNumSet(pciIf - MV_PCI_START_IF,
								busNum);	
		#else
		return MV_OK;
		#endif
    }
    else if (PCI_IF_TYPE_PEX == pciIfType)
	{
		#if defined(MV_INCLUDE_PEX)
		return mvPexLocalBusNumSet(pciIf - MV_PEX_START_IF,
								busNum);
		#else
		return MV_OK;
		#endif
	}
	else
	{
		mvOsPrintf("%s: ERROR!!! Invalid pciIf %d\n", __FUNCTION__, pciIf);
	}

	return MV_FAIL;

}

/*******************************************************************************
* mvPciLocalBusNumGet - Get PCI interface local bus number.
*
* DESCRIPTION:
*       This function gets the local bus number of a given PCI interface.
*
* INPUT:
*       pciIf  - PCI interface number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Local bus number.0xffffffff on Error
*
*******************************************************************************/
MV_U32 mvPciIfLocalBusNumGet(MV_U32 pciIf)
{
	PCI_IF_TYPE pciIfType = mvPciIfTypeGet(pciIf);

	if (PCI_IF_TYPE_CONVEN_PCIX == pciIfType)
	{
		#if defined(MV_INCLUDE_PCI)
		return mvPciLocalBusNumGet(pciIf - MV_PCI_START_IF);
		#else
		return 0xFFFFFFFF;
		#endif
	}
	else if (PCI_IF_TYPE_PEX == pciIfType)
	{
		#if defined(MV_INCLUDE_PEX)
		return mvPexLocalBusNumGet(pciIf - MV_PEX_START_IF);
		#else
		return 0xFFFFFFFF;
		#endif

	}
	else
	{
		mvOsPrintf("%s: ERROR!!! Invalid pciIf %d\n",__FUNCTION__, pciIf);
	}

	return 0;

}


/*******************************************************************************
* mvPciLocalDevNumSet - Set PCI interface local device number.
*
* DESCRIPTION:
*       This function sets given PCI interface its local device number.
*       Note: In case the PCI interface is PCI-X, the information is read-only.
*
* INPUT:
*       pciIf  - PCI interface number.
*       devNum - Device number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_NOT_ALLOWED in case PCI interface is PCI-X. MV_BAD_PARAM on bad parameters ,
*       otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPciIfLocalDevNumSet(MV_U32 pciIf, MV_U32 devNum)
{
	PCI_IF_TYPE pciIfType = mvPciIfTypeGet(pciIf);

	if (PCI_IF_TYPE_CONVEN_PCIX == pciIfType)
	{
		#if defined(MV_INCLUDE_PCI)
		return mvPciLocalDevNumSet(pciIf - MV_PCI_START_IF,
								devNum);	
		#else
		return MV_OK;
		#endif
	}
	else if (PCI_IF_TYPE_PEX == pciIfType)
	{
		#if defined(MV_INCLUDE_PEX)
		return mvPexLocalDevNumSet(pciIf - MV_PEX_START_IF,
								devNum);
		#else
		return MV_OK;
		#endif
	}
	else
	{
		mvOsPrintf("%s: ERROR!!! Invalid pciIf %d\n", __FUNCTION__, pciIf);
	}

	return MV_FAIL;

}

/*******************************************************************************
* mvPciLocalDevNumGet - Get PCI interface local device number.
*
* DESCRIPTION:
*       This function gets the local device number of a given PCI interface.
*
* INPUT:
*       pciIf  - PCI interface number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Local device number. 0xffffffff on Error
*
*******************************************************************************/
MV_U32 mvPciIfLocalDevNumGet(MV_U32 pciIf)
{
	PCI_IF_TYPE pciIfType = mvPciIfTypeGet(pciIf);

	if (PCI_IF_TYPE_CONVEN_PCIX == pciIfType)
	{
		#if defined(MV_INCLUDE_PCI)
		return mvPciLocalDevNumGet(pciIf - MV_PCI_START_IF);
		#else
		return 0xFFFFFFFF;
		#endif
	}
	else if (PCI_IF_TYPE_PEX == pciIfType)
	{
		#if defined(MV_INCLUDE_PEX)
		return mvPexLocalDevNumGet(pciIf - MV_PEX_START_IF);
		#else
		return 0xFFFFFFFF;
		#endif

	}
	else
	{
		mvOsPrintf("%s: ERROR!!! Invalid pciIf %d\n", __FUNCTION__, pciIf);
	}

	return 0;

}

/*******************************************************************************
* mvPciIfTypeGet - 
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*******************************************************************************/

PCI_IF_TYPE mvPciIfTypeGet(MV_U32 pciIf)
{

	if ((pciIf >= MV_PCI_START_IF)&&(pciIf < MV_PCI_MAX_IF + MV_PCI_START_IF))
	{
		return PCI_IF_TYPE_CONVEN_PCIX;
	}
	else if ((pciIf >= MV_PEX_START_IF) &&
			 (pciIf < MV_PEX_MAX_IF + MV_PEX_START_IF))
	{
		return PCI_IF_TYPE_PEX;

	}
	else
	{
		mvOsPrintf("%s: ERROR!!! Invalid pciIf %d\n", __FUNCTION__, pciIf);
	}

	return 0xffffffff;

}

/*******************************************************************************
* mvPciIfTypeGet - 
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*******************************************************************************/

MV_U32  mvPciRealIfNumGet(MV_U32 pciIf)
{

	PCI_IF_TYPE pciIfType = mvPciIfTypeGet(pciIf);

	if (PCI_IF_TYPE_CONVEN_PCIX == pciIfType)
	{
		return (pciIf - MV_PCI_START_IF);
	}
	else if (PCI_IF_TYPE_PEX == pciIfType)
	{
		return (pciIf - MV_PEX_START_IF);

	}
	else
	{
		mvOsPrintf("%s: ERROR!!! Invalid pciIf %d\n", __FUNCTION__, pciIf);
	}

	return 0xffffffff;

}



