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
 **   - 2017-05-28      First Version
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "vc.h"
#include "gpio.h"
#include "adc.h"
#include "bt.h"
#include "flash.h"
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
 ** \brief  ??????????????????
 **
 ** \return ???
 ******************************************************************************/
void Tim2_IRQHandler(void)
{
    if(TRUE == Bt_GetIntFlag(TIM2, BtCA0Irq))
    {
        if(Gpio_GetInputIO(EVB_LEDR_PORT, EVB_LEDR_PIN) == FALSE)
        {
            Gpio_WriteOutputIO(EVB_LEDR_PORT, EVB_LEDR_PIN, TRUE);
        }
        else if(Gpio_GetInputIO(EVB_LEDR_PORT, EVB_LEDR_PIN) == TRUE)
        {
            Gpio_WriteOutputIO(EVB_LEDR_PORT, EVB_LEDR_PIN, FALSE);
        }
        Bt_ClearIntFlag(TIM2,BtCA0Irq);
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
    Gpio_WriteOutputIO(EVB_LEDR_PORT, EVB_LEDR_PIN, TRUE);    
    
    Gpio_SetAnalogMode(GpioPortC,GpioPin0);                  //????????????

    GpioInitStruct.enDir  = GpioDirOut;
    
    Gpio_SfTimCCfg(GpioSfTim2CA,GpioSf2);               //VC0_OUT??????TIM2_CHA
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
    VcInitStruct.enVcInPin_N     = AiBg1p2;          //VC0_CH???N???????????????1.2V
    VcInitStruct.enVcOutCfg   = VcOutTIM2RCLR;    //?????????????????????????????????????????????
    VcInitStruct.bFlten          = TRUE;             //????????????
    Vc_Init(&VcInitStruct);
    Vc_CfgItType(VcChannel0, VcIrqRise);          //??????VC0??????????????????    
}

/**
 ******************************************************************************
 ** \brief  ?????????TIM2
 **
 ** \return ???
 ******************************************************************************/
void TIM2M23_Cfg(void)
{
    stc_bt_mode23_cfg_t        stcBtBaseCfg;
    stc_bt_m23_input_cfg_t     stcBtPortInputCfg;
    stc_bt_m23_bk_input_cfg_t  stcBtBkInputCfg;
    uint16_t                      u16CntValue;
    uint16_t                      u16ArrValue;
    uint8_t                       u8ValidPeriod;

    DDL_ZERO_STRUCT(stcBtBaseCfg);
    DDL_ZERO_STRUCT(stcBtPortInputCfg);   
    DDL_ZERO_STRUCT(stcBtBkInputCfg);    

    stcBtBaseCfg.enWorkMode    = BtWorkMode2;              //???????????????
    stcBtBaseCfg.enCT          = BtTimer;                  //???????????????????????????????????????PCLK
    stcBtBaseCfg.enPRS         = BtPCLKDiv8;               //PCLK
    stcBtBaseCfg.enCntDir      = BtCntUp;                  //??????????????????????????????????????????

    Bt_Mode23_Init(TIM2, &stcBtBaseCfg);                   //TIM0 ?????????2/3???????????????
        
    stcBtPortInputCfg.enCh0ACmpCap    = BtCHxCapMode;
    stcBtPortInputCfg.enCH0ACapSel    = BtCHxCapRise;
    stcBtPortInputCfg.enCH0AInFlt     = BtFltNone;
    stcBtPortInputCfg.enCH0APolarity  = BtPortPositive;

    Bt_M23_PortInput_Cfg(TIM2,&stcBtPortInputCfg);      
    
    u8ValidPeriod = 0;                                     //???????????????????????????0?????????????????????????????????????????????+1????????????1?????????
    Bt_M23_SetValidPeriod(TIM2,u8ValidPeriod);             //??????????????????
    
    u16CntValue = 0;
    Bt_M23_Cnt16Set(TIM2, u16CntValue);                    //??????????????????    

    u16ArrValue = 0xFFFF;
    Bt_M23_ARRSet(TIM2, u16ArrValue, TRUE);                //???????????????,???????????????
}

/**
 ******************************************************************************
 ** \brief  ?????????
 **
 ** \return ???
 ******************************************************************************/
int main(void)
{
    stc_vc_channel_cfg_t       stcChannelCfg;    
    stc_sysctrl_clk_cfg_t      stcClkCfg;    
    en_flash_waitcycle_t          enFlashWait;

    DDL_ZERO_STRUCT(stcChannelCfg);  
    DDL_ZERO_STRUCT(stcClkCfg);    
    enFlashWait = FlashWaitCycle1;                      //????????????????????????1??????HCLK??????24MHz??????????????????1???
    Flash_WaitCycle(enFlashWait);                       // Flash ??????1?????????
    stcClkCfg.enClkSrc    = SysctrlClkXTH;           //????????????????????????,32M
    stcClkCfg.enHClkDiv   = SysctrlHclkDiv1;         // HCLK = SystemClk/1
    stcClkCfg.enPClkDiv   = SysctrlPclkDiv1;         // PCLK = HCLK/1
    Sysctrl_SetXTHFreq(SysctrlXthFreq24_32MHz);         //???????????????????????????20~32M
    Sysctrl_ClkInit(&stcClkCfg);
        
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);//???GPIO??????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralVcLvd, TRUE);//???LVD??????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE);//???adc??????   
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE); //Base Timer??????????????????

    Bgr_BgrEnable();                 //BGR????????????

    GPIO_Cfg();                                        //????????????IO???
    VC_Cfg();                                          //??????VC
    TIM2M23_Cfg();                                     //??????TIM2?????????
    Vc_Cmd(VcChannel0, TRUE);                                //??????VC0
    
    Bt_ClearAllIntFlag(TIM2);                              //???????????????
    EnableNvic(TIM2_IRQn, IrqLevel0, TRUE);                //TIM0????????????
    Bt_Mode23_EnableIrq(TIM2,BtCA0Irq);                    //??????TIM0 UEV????????????    
    Bt_M23_Run(TIM2);                                      //TIM0 ??????    
    while (1)
    {

    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


