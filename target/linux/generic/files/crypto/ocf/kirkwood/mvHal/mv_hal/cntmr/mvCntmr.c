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

#include "mvCntmr.h"
#include "cpu/mvCpu.h"

/* defines  */       
#ifdef MV_DEBUG         
	#define DB(x)	x
#else                
	#define DB(x)    
#endif	             

extern unsigned int whoAmI(void);

/*******************************************************************************
* mvCntmrLoad - 
*
* DESCRIPTION:
*       Load an init Value to a given counter/timer 
*
* INPUT:
*       countNum - counter number
*       value - value to be loaded
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM on bad parameters , MV_ERROR on error ,MV_OK on sucess
*******************************************************************************/
MV_STATUS mvCntmrLoad(MV_U32 countNum, MV_U32 value)
{
	if (countNum >= MV_CNTMR_MAX_COUNTER )
	{

		mvOsPrintf(("mvCntmrLoad: Err. Illigal counter number \n"));
		return MV_BAD_PARAM;;

	}

	MV_REG_WRITE(CNTMR_RELOAD_REG(countNum),value);
	MV_REG_WRITE(CNTMR_VAL_REG(countNum),value);

	return MV_OK;
}

/*******************************************************************************
* mvCntmrRead - 
*
* DESCRIPTION:
*  	Returns the value of the given Counter/Timer     
*
* INPUT:
*       countNum - counter number
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_U32 counter value
*******************************************************************************/
MV_U32 mvCntmrRead(MV_U32 countNum)
{
	return MV_REG_READ(CNTMR_VAL_REG(countNum));
}

/*******************************************************************************
* mvCntmrWrite - 
*
* DESCRIPTION:
*  	Returns the value of the given Counter/Timer     
*
* INPUT:
*       countNum - counter number
*		countVal - value to write
*
* OUTPUT:
*       None.
*
* RETURN:
*       None
*******************************************************************************/
void mvCntmrWrite(MV_U32 countNum,MV_U32 countVal)
{
	MV_REG_WRITE(CNTMR_VAL_REG(countNum),countVal);
}

/*******************************************************************************
* mvCntmrCtrlSet - 
*
* DESCRIPTION:
*  	Set the Control to a given counter/timer     
*
* INPUT:
*       countNum - counter number
*		pCtrl - pointer to MV_CNTMR_CTRL structure 
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM on bad parameters , MV_ERROR on error ,MV_OK on sucess
*******************************************************************************/
MV_STATUS mvCntmrCtrlSet(MV_U32 countNum, MV_CNTMR_CTRL *pCtrl)
{
	MV_U32 cntmrCtrl;

	if (countNum >= MV_CNTMR_MAX_COUNTER )
	{

		DB(mvOsPrintf(("mvCntmrCtrlSet: Err. Illigal counter number \n")));
		return MV_BAD_PARAM;;

	}

	/* read control register */
	cntmrCtrl = MV_REG_READ(CNTMR_CTRL_REG);

	
	if (pCtrl->enable)	/* enable counter\timer */
	{
		cntmrCtrl |= CTCR_ARM_TIMER_EN(countNum);
	}
	else	/* disable counter\timer */
	{
		cntmrCtrl &= ~CTCR_ARM_TIMER_EN(countNum);
	}

	if ( pCtrl->autoEnable ) /* Auto mode */
	{
		cntmrCtrl |= CTCR_ARM_TIMER_AUTO_EN(countNum);		

	}
	else 	/* no auto mode */
	{
		cntmrCtrl &= ~CTCR_ARM_TIMER_AUTO_EN(countNum);
	}

	MV_REG_WRITE(CNTMR_CTRL_REG,cntmrCtrl);

	return MV_OK;

}

/*******************************************************************************
* mvCntmrCtrlGet - 
*
* DESCRIPTION:
*  	Get the Control value of a given counter/timer     
*
* INPUT:
*       countNum - counter number
*		pCtrl - pointer to MV_CNTMR_CTRL structure 
*
* OUTPUT:
*       Counter\Timer control value
*
* RETURN:
*       MV_BAD_PARAM on bad parameters , MV_ERROR on error ,MV_OK on sucess
*******************************************************************************/
MV_STATUS mvCntmrCtrlGet(MV_U32 countNum, MV_CNTMR_CTRL *pCtrl)
{
	MV_U32 cntmrCtrl;

	if (countNum >= MV_CNTMR_MAX_COUNTER )
	{
		DB(mvOsPrintf(("mvCntmrCtrlGet: Err. Illigal counter number \n")));
		return MV_BAD_PARAM;;
	}

	/* read control register */
	cntmrCtrl = MV_REG_READ(CNTMR_CTRL_REG);

	/* enable counter\timer */
	if (cntmrCtrl & CTCR_ARM_TIMER_EN(countNum))
	{
		pCtrl->enable = MV_TRUE;
	}
	else
	{
		pCtrl->enable = MV_FALSE;
	}

	/* counter mode */
	if (cntmrCtrl & CTCR_ARM_TIMER_AUTO_EN(countNum))
	{
		pCtrl->autoEnable = MV_TRUE;
	}
	else
	{
		pCtrl->autoEnable = MV_FALSE;
	}

	return MV_OK;
}

/*******************************************************************************
* mvCntmrEnable - 
*
* DESCRIPTION:
*  	Set the Enable-Bit to logic '1' ==> starting the counter     
*
* INPUT:
*       countNum - counter number
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM on bad parameters , MV_ERROR on error ,MV_OK on sucess
*******************************************************************************/
MV_STATUS mvCntmrEnable(MV_U32 countNum)
{
	MV_U32 cntmrCtrl;

	if (countNum >= MV_CNTMR_MAX_COUNTER )
	{

		DB(mvOsPrintf(("mvCntmrEnable: Err. Illigal counter number \n")));
		return MV_BAD_PARAM;;

	}

	/* read control register */
	cntmrCtrl = MV_REG_READ(CNTMR_CTRL_REG);

	/* enable counter\timer */
	cntmrCtrl |= CTCR_ARM_TIMER_EN(countNum);


	MV_REG_WRITE(CNTMR_CTRL_REG,cntmrCtrl);

	return MV_OK;
}

/*******************************************************************************
* mvCntmrDisable - 
*
* DESCRIPTION:
*  	Stop the counter/timer running, and returns its Value     
*
* INPUT:
*       countNum - counter number
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_U32 counter\timer value
*******************************************************************************/
MV_STATUS mvCntmrDisable(MV_U32 countNum)
{
	MV_U32 cntmrCtrl;

	if (countNum >= MV_CNTMR_MAX_COUNTER )
	{

		DB(mvOsPrintf(("mvCntmrDisable: Err. Illigal counter number \n")));
		return MV_BAD_PARAM;;

	}

	/* read control register */
	cntmrCtrl = MV_REG_READ(CNTMR_CTRL_REG);

	/* disable counter\timer */
	cntmrCtrl &= ~CTCR_ARM_TIMER_EN(countNum);

	MV_REG_WRITE(CNTMR_CTRL_REG,cntmrCtrl);

	return MV_OK;
}

/*******************************************************************************
* mvCntmrStart - 
*
* DESCRIPTION:
*  	Combined all the sub-operations above to one function: Load,setMode,Enable     
*
* INPUT:
*       countNum - counter number
*		value - value of the counter\timer to be set
*		pCtrl - pointer to MV_CNTMR_CTRL structure 
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM on bad parameters , MV_ERROR on error ,MV_OK on sucess
*******************************************************************************/
MV_STATUS mvCntmrStart(MV_U32 countNum, MV_U32 value,
                       MV_CNTMR_CTRL *pCtrl)
{

	if (countNum >= MV_CNTMR_MAX_COUNTER )
	{

		mvOsPrintf(("mvCntmrDisable: Err. Illigal counter number \n"));
		return MV_BAD_PARAM;;

	}

	/* load value onto counter\timer */
	mvCntmrLoad(countNum,value);

	/* set the counter to load in the first time */
	mvCntmrWrite(countNum,value);

	/* set control for timer \ cunter and enable */
	mvCntmrCtrlSet(countNum,pCtrl);

	return MV_OK;
}

