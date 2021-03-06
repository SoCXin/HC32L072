/*******************************************************************************
 * Copyright (C) 2016, Huada Semiconductor Co., Ltd. All rights reserved.
 *
 * This software is owned and published by:
 * Huada Semiconductor Co., Ltd. ("HDSC").
 *
 * BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
 * BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
 *
 * This software contains source code for use with HDSC
 * components. This software is licensed by HDSC to be adapted only
 * for use in systems utilizing HDSC components. HDSC shall not be
 * responsible for misuse or illegal use of this software for devices not
 * supported herein. HDSC is providing this software "AS IS" and will
 * not be responsible for issues arising from incorrect user implementation
 * of the software.
 *
 * Disclaimer:
 * HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
 * REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS),
 * ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
 * WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
 * WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
 * WARRANTY OF NONINFRINGEMENT.
 * HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
 * NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
 * LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
 * LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
 * INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
 * SAVINGS OR PROFITS,
 * EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
 * INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
 * FROM, THE SOFTWARE.
 *
 * This software may be replicated in part or whole for the licensed use,
 * with the restriction that this Disclaimer and Copyright notice must be
 * included with each copy of this software, whether used in part or whole,
 * at all times.
 */
/******************************************************************************/
/** \file wm8731.c
 **
 ** \brief I2S codec WM8731 driver API functions
 **
 **   - 2019-08-13 Linsq   First version
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "wm8731.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/


/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
 
/**
 ******************************************************************************
    ** \brief  ????I2C1????2????????
    ** \param  i2cx: M0P_I2C0????M0P_I2C1
    ** \param  buf????????????????????            
    ** \retval len??????????????????
    **         
 ******************************************************************************/
void I2C_WriteWm8731R(M0P_I2C_TypeDef *i2cx, uint8_t *buf, uint8_t len)
{
	uint8_t u8State = 0;
	uint8_t tmp=0, tmp_1;
    I2C_SetFunc(i2cx,I2cStart_En);
	while(1)
	{
		while(1!=i2cx->CR_f.SI)
        {}
        u8State = I2C_GetState(i2cx);
		switch(u8State)
		{
			case 0x08:
                I2C_ClearFunc(i2cx,I2cStart_En);
                I2C_WriteByte(i2cx,I2c_SlaveWt);
				break;
			case 0x10:
                I2C_ClearFunc(i2cx,I2cStart_En);
                I2C_WriteByte(i2cx,I2c_SlaveWt);            
				break;
			case 0x18:
				tmp_1=buf[tmp++];
                I2C_ClearFunc(i2cx,I2cStart_En);
                I2C_WriteByte(i2cx,tmp_1);
				break;
			case 0x20:
			case 0x38:
			case 0x30:
			case 0x48:
                I2C_SetFunc(i2cx,I2cStart_En);
                I2C_SetFunc(i2cx,I2cStop_En);
			break;							
			case 0x28:
				if(tmp>=len)//??????????????2????????????I2C
				{
                    I2C_SetFunc(i2cx,I2cStop_En);
                    I2C_ClearIrq(i2cx);
					return;
				}		
                I2C_WriteByte(i2cx,buf[tmp++]);//??????????????	
				break;	
		}
	 	I2C_ClearIrq(i2cx);
	}		 
}

/**
 ******************************************************************************
    * @brief  ????I2Cx??????WM8731??????????????????????????
    * \param  i2cx: M0P_I2C0????M0P_I2C1
    * @param  pstcReg: ????????????????????????
    * @retval None
    **
 ******************************************************************************/
void WM8731_Init(M0P_I2C_TypeDef *i2cx, stc_wm8731_reg_t* pstcReg)
{
	uint8_t buf[2];
	uint8_t tmp;

	//Reset Register ????????????
	tmp = (uint8_t)((pstcReg->RESET>>8) & 0x01);	
	buf[0] = (WM8731_REG_RESET<<1) & (~1);
	buf[0] = buf[0] | tmp;
	buf[1] = (uint8_t)(pstcReg->RESET & 0xff);
	I2C_WriteWm8731R(i2cx, buf, 2);          //????Reset Register??????	
	//Left Line In ????????????
	tmp = (uint8_t)((pstcReg->LLIN>>8) & 0x01);	
	buf[0] = (WM8731_REG_LLINE_IN<<1) & (~1);
	buf[0] = buf[0] | tmp;
	buf[1] = (uint8_t)(pstcReg->LLIN & 0xff);
	I2C_WriteWm8731R(i2cx, buf, 2);          //????Left Line In??????
	//Right Line In????????????
	tmp = (uint8_t)((pstcReg->RLIN>>8) & 0x01);
	buf[0] = (WM8731_REG_RLINE_IN<<1) & (~1);
	buf[0] = buf[0] | tmp;
	buf[1] = (uint8_t)(pstcReg->RLIN & 0xff);
	I2C_WriteWm8731R(i2cx, buf, 2);          //????Right Line In??????
	//Left Headphone Out ????????????
	tmp = (uint8_t)((pstcReg->LHOUT>>8) & 0x01);
	buf[0] = (WM8731_REG_LHPHONE_OUT<<1) & (~1);
	buf[0] = buf[0] | tmp;
	buf[1] = (uint8_t)(pstcReg->LHOUT & 0xff);
	I2C_WriteWm8731R(i2cx, buf, 2);          //????Left Headphone Out??????
	//Right Headphone Out ????????????
	tmp = (uint8_t)((pstcReg->RHOUT>>8) & 0x01);
	buf[0] = (WM8731_REG_RHPHONE_OUT<<1) & (~1);
	buf[0] = buf[0] | tmp;
	buf[1] = (uint8_t)(pstcReg->RHOUT & 0xff);
	I2C_WriteWm8731R(i2cx, buf, 2);          //????Right Headphone Out ??????
	//Analogue Audio Path Control????????????
	tmp = (uint8_t)((pstcReg->AAPC>>8) & 0x01);
	buf[0] = (WM8731_REG_ANALOG_PATH<<1) & (~1);
	buf[0] = buf[0] | tmp;
	buf[1] = (uint8_t)(pstcReg->AAPC & 0xff);
	I2C_WriteWm8731R(i2cx, buf, 2);          //????Analogue Audio Path Control??????
	//Digital Audio Path Control ????????????
	tmp = (uint8_t)((pstcReg->DAPC>>8) & 0x01);
	buf[0] = (WM8731_REG_DIGITAL_PATH<<1) & (~1);
	buf[0] = buf[0] | tmp;
	buf[1] = (uint8_t)(pstcReg->DAPC & 0xff);
	I2C_WriteWm8731R(i2cx, buf, 2);          //????Digital Audio Path Control??????
	//Power Down Control ????????????
	tmp = (uint8_t)((pstcReg->PDC>>8) & 0x01);
	buf[0] = (WM8731_REG_PDOWN_CTRL<<1) & (~1);
	buf[0] = buf[0] | tmp;
	buf[1] = (uint8_t)(pstcReg->PDC & 0xff);
	I2C_WriteWm8731R(i2cx, buf, 2);          //????Power Down Control??????
	//Digital Audio Interface Format ????????????
	tmp = (uint8_t)((pstcReg->DAIF>>8) & 0x01);
	buf[0] = (WM8731_REG_DIGITAL_IF<<1) & (~1);
	buf[0] = buf[0] | tmp;
	buf[1] = (uint8_t)(pstcReg->DAIF & 0xff);
	I2C_WriteWm8731R(i2cx, buf, 2);           //????Digital Audio Interface Format??????
	//Sampling Control ???????????? 
	tmp = (uint8_t)((pstcReg->SC>>8) & 0x01);
	buf[0] = (WM8731_REG_SAMPLING_CTRL<<1) & (~1);
	buf[0] = buf[0] | tmp;
	buf[1] = (uint8_t)(pstcReg->SC & 0xff);
	I2C_WriteWm8731R(i2cx, buf, 2);           //????Sampling Control ??????	
	//Active Control ????????????
	tmp = (uint8_t)((pstcReg->AC>>8) & 0x01);
	buf[0] = (WM8731_REG_ACTIVE_CTRL<<1) & (~1);
	buf[0] = buf[0] | tmp;
	buf[1] = (uint8_t)(pstcReg->AC & 0xff);
	I2C_WriteWm8731R(i2cx, buf, 2);           //????Active Control ??????		
}

/**
 ******************************************************************************
    * @brief  ????I2Cx????????????????????????????????????Left Headphone Out??Right Headphone Out??????
    * \param  i2cx: M0P_I2C0????M0P_I2C1
    * @param  pstcReg: ????????????????????????
    * @param  lhpvol : ????????????????????????????0-2^7
    * @param  rhpvol : ????????????????????????????0-2^7
    * @retval None
    **
 ******************************************************************************/
void WM8731_HPhoneVolume(M0P_I2C_TypeDef *i2cx, stc_wm8731_reg_t* pstcReg, uint8_t lhpvol, uint8_t rhpvol)
{
	uint8_t buf[2];
	uint8_t tmp;
	
	pstcReg->LHOUT_f.LHPVOL=lhpvol;
	tmp = (uint8_t)((pstcReg->LHOUT>>8) & 0x01);
	buf[0] = (WM8731_REG_LHPHONE_OUT<<1) & (~1);
	buf[0] = buf[0] | tmp;
	buf[1] = (uint8_t)(pstcReg->LHOUT & 0xff);
	I2C_WriteWm8731R(i2cx, buf, 2); 	
	
	pstcReg->RHOUT_f.RHPVOL=rhpvol;
	tmp = (uint8_t)((pstcReg->RHOUT>>8) & 0x01);
	buf[0] = (WM8731_REG_RHPHONE_OUT<<1) & (~1);
	buf[0] = buf[0] | tmp;
	buf[1] = (uint8_t)(pstcReg->RHOUT & 0xff);
	I2C_WriteWm8731R(i2cx, buf, 2); 
}
/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
