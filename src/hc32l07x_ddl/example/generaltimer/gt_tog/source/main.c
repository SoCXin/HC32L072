/******************************************************************************
* Copyright (C) 2016, Huada Semiconductor Co.,Ltd All rights reserved.
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
 **   - 2019-04-17  1.0  Husj First version for General Timer012.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "bt.h"
#include "flash.h"
#include "gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/


/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/


/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/*******************************************************************************
 * TIM0??????????????????
 ******************************************************************************/
void Tim0_IRQHandler(void)
{
    //Timer0 ??????0 ???????????????????????????Timer0 ???????????????????????????
    if(TRUE == Bt_GetIntFlag(TIM0, BtUevIrq))
    {

        Bt_ClearIntFlag(TIM0,BtUevIrq);
    }
}

//?????????????????????
void App_ClockCfg(void)
{
    en_flash_waitcycle_t      enWaitCycle;
    stc_sysctrl_pll_cfg_t     stcPLLCfg;
    
    //????????????????????????
    DDL_ZERO_STRUCT(stcPLLCfg);
    
    enWaitCycle = FlashWaitCycle1;
    Flash_WaitCycle(enWaitCycle);
    
    stcPLLCfg.enInFreq    = SysctrlPllInFreq4_6MHz;     //RCH 4MHz
    stcPLLCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;  //PLL ??????48MHz
    stcPLLCfg.enPllClkSrc = SysctrlPllRch;              //?????????????????????RCH
    stcPLLCfg.enPllMul    = SysctrlPllMul12;            //4MHz x 12 = 48MHz
    Sysctrl_SetPLLFreq(&stcPLLCfg);
    Sysctrl_SetPLLStableTime(SysctrlPllStableCycle16384);
    Sysctrl_ClkSourceEnable(SysctrlClkPLL, TRUE);
    
    Sysctrl_SysClkSwitch(SysctrlClkPLL);  ///< ????????????
}

//?????????????????????
void App_Timer0PortCfg(void)
{
    stc_gpio_cfg_t         stcTIM0APort;
    stc_gpio_cfg_t         stcTIM0BPort;
    
    DDL_ZERO_STRUCT(stcTIM0APort);
    DDL_ZERO_STRUCT(stcTIM0BPort);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //GPIO ??????????????????
    
    //PA00?????????TIM0_CHA
    stcTIM0APort.enDir  = GpioDirOut;
    Gpio_Init(GpioPortA, GpioPin0, &stcTIM0APort);
    Gpio_SetAfMode(GpioPortA,GpioPin0,GpioAf7);
    
    //PA01?????????TIM0_CHB
    stcTIM0BPort.enDir  = GpioDirOut;
    Gpio_Init(GpioPortA, GpioPin1, &stcTIM0BPort);
    Gpio_SetAfMode(GpioPortA,GpioPin1,GpioAf3);
}

//Timer0???????????????
void App_Timer0Cfg(uint16_t u16Period)
{
    uint16_t                  u16ArrValue;
    uint16_t                  u16CntValue;
    stc_bt_mode0_cfg_t     stcBtBaseCfg;
    
    DDL_ZERO_STRUCT(stcBtBaseCfg);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE); //Base Timer??????????????????
    
    stcBtBaseCfg.enWorkMode = BtWorkMode0;                  //???????????????
    stcBtBaseCfg.enCT       = BtTimer;                      //???????????????????????????????????????PCLK
    stcBtBaseCfg.enPRS      = BtPCLKDiv16;                  //PCLK/16
    stcBtBaseCfg.enCntMode  = Bt16bitArrMode;               //????????????16????????????/?????????
    stcBtBaseCfg.bEnTog     = TRUE;
    stcBtBaseCfg.bEnGate    = FALSE;
    stcBtBaseCfg.enGateP    = BtGatePositive;
    
    Bt_Mode0_Init(TIM0, &stcBtBaseCfg);                     //TIM0 ?????????0???????????????
    
    u16ArrValue = 0x10000 - u16Period;
    Bt_M0_ARRSet(TIM0, u16ArrValue);                        //???????????????(ARR = 0x10000 - ??????)
    
    u16CntValue = 0x10000 - u16Period;
    
    Bt_M0_Cnt16Set(TIM0, u16CntValue);                      //??????????????????
    
    Bt_M0_Enable_Output(TIM0, TRUE);                         //TIM0 ??????????????????
    
    //Bt_ClearIntFlag(TIM0,BtUevIrq);                       //???????????????
    //EnableNvic(TIM0_IRQn, IrqLevel3, TRUE);               //TIM0????????????
    //Bt_Mode0_EnableIrq(TIM0);                             //??????TIM0??????(??????0?????????????????????)
}

/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/

int32_t main(void)
{
    App_ClockCfg();         //?????????????????????
    
    App_Timer0PortCfg();    //?????????????????????
    
    App_Timer0Cfg(48000);   //Timer0???????????????(?????? = 48000*(1/48M)*16 = 16ms)
    
    Bt_M0_Run(TIM0);        //TIM0 ?????????
    
    while (1);

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


