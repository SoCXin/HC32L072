/******************************************************************************
* Copyright (C) 2017, Huada Semiconductor Co.,Ltd All rights reserved.
*
* This software is owned and published by:
* Huada Semiconductor Co.,Ltd ("HDSC").
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
/** \file main.c
 **
 ** A detailed description is available at
 ** @link Sample Group Some description @endlink
 **
 **   - 2017-05-28 LiuHL    First Version
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "vc.h"
#include "gpio.h"
#include "adc.h"
#include "lpm.h"
#include "bgr.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 ******************************************************************************
 ** \brief  LPTIMER0??????????????????
 **
 ** \return ???
 ******************************************************************************/
void Vc0_IRQHandler(void)
{
    if(Vc_GetItStatus(Vc0_Intf) == TRUE)
    {
        Vc_ClearItStatus(Vc0_Intf);             //??????VC_CH0???????????????
    }
}

/**
 ******************************************************************************
 ** \brief  ???????????????GPIO??????
 **
 ** \return ???
 ******************************************************************************/
void GPIO_Cfg(void)
{
    stc_gpio_cfg_t GpioInitStruct;
    DDL_ZERO_STRUCT(GpioInitStruct);
    
    GpioInitStruct.enDrv = GpioDrvH;
    GpioInitStruct.enDir = GpioDirOut;
    Gpio_Init(GpioPortA,GpioPin6,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortA,GpioPin6,GpioAf5);              //PA06??????VC0_OUT

    Gpio_Init(EVB_LEDR_PORT, EVB_LEDR_PIN, &GpioInitStruct);   //PD14????????????????????????????????????LED
    Gpio_WriteOutputIO(EVB_LEDR_PORT, EVB_LEDR_PIN, FALSE);    //??????????????????LED
    
    GpioInitStruct.enDir = GpioDirIn;
    Gpio_Init(EVB_KEY1_PORT,EVB_KEY1_PIN, &GpioInitStruct);
    
    Gpio_SetAnalogMode(GpioPortC,GpioPin0);                  //????????????
    Gpio_SetAnalogMode(GpioPortA,GpioPin0);                  //????????????    
}

/**
 ******************************************************************************
 ** \brief  ?????????VC0
 **
 ** \return ???
 ******************************************************************************/
void VC_Cfg(void)
{
    stc_vc_channel_cfg_t VcInitStruct;
    DDL_ZERO_STRUCT(VcInitStruct);
    VcInitStruct.enVcChannel = VcChannel0;
    VcInitStruct.enVcCmpDly  = VcDelay10mv;          //VC0??????????????????10mV
    VcInitStruct.enVcBiasCurrent = VcBias10ua;       //VC0?????????10uA
    VcInitStruct.enVcFilterTime  = VcFilter28us;     //VC????????????????????????28us
    VcInitStruct.enVcInPin_P     = VcInPCh0;         //VC0_CH???P?????????PC00
    VcInitStruct.enVcInPin_N     = VcInNCh0;         //VC0_CH???N?????????PA00
    VcInitStruct.enVcOutCfg   = VcOutDisable;     //?????????????????????????????????????????????
    VcInitStruct.bFlten          = TRUE;             //????????????
    Vc_Init(&VcInitStruct);
    Vc_CfgItType(VcChannel0, VcIrqRise);          //??????VC0??????????????????
}

/**
******************************************************************************
    ** \brief  ?????????
    ** 
  ** @param  ???
    ** \retval ???
    **
******************************************************************************/ 
int main(void)
{
    stc_vc_channel_cfg_t stcChannelCfg;
    DDL_ZERO_STRUCT(stcChannelCfg);//?????????0

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);//???GPIO??????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralVcLvd, TRUE);//???LVD??????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE);//???adc??????
    Bgr_BgrEnable();                 //BGR????????????
    GPIO_Cfg();
    VC_Cfg();
    Vc_ClearItStatus(Vc0_Intf);
    EnableNvic(VC0_1_2_LVD_IRQn, IrqLevel3, TRUE);
    Vc_ItCfg(VcChannel0, TRUE);
    Vc_Cmd(VcChannel0, TRUE);

    while(Gpio_GetInputIO(EVB_KEY1_PORT,EVB_KEY1_PIN) == 1);
    Lpm_GotoDeepSleep(FALSE);
    Gpio_WriteOutputIO(EVB_LEDR_PORT, EVB_LEDR_PIN, TRUE);//???????????????LED3    
    
    while (1)
    {

    }
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


