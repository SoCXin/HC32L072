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
 **   - 2019-04-18  1.0  Husj First version for Timer3.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "timer3.h"
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
 * TIM3??????????????????
 ******************************************************************************/
void Tim3_IRQHandler(void)
{
     //Timer3 ??????0 ???????????????????????????Timer3 ???????????????????????????
     if(TRUE == Tim3_GetIntFlag(Tim3UevIrq))
     {
         
        Tim3_ClearIntFlag(Tim3UevIrq);
     }
}

//Timer3 CHx ????????????
void App_LEDPortCfg(void)
{
    stc_gpio_cfg_t         stcTIM3Port;
    
    //????????????????????????
    DDL_ZERO_STRUCT(stcTIM3Port);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //GPIO ??????????????????
    
    stcTIM3Port.enDir  = GpioDirOut;
    //PA08?????????TIM3_CH0A
    Gpio_Init(GpioPortA, GpioPin8, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortA,GpioPin8,GpioAf2);
    
    //PA07?????????TIM3_CH0B
    Gpio_Init(GpioPortA, GpioPin7, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortA,GpioPin7,GpioAf4);
}

//Timer3 ??????
void App_Timer3Cfg(uint16_t u16Period)
{
    uint16_t                  u16ArrValue;
    uint16_t                  u16CntValue;
    stc_tim3_mode0_cfg_t   stcTim3BaseCfg;
    
    //????????????????????????
    DDL_ZERO_STRUCT(stcTim3BaseCfg);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralTim3, TRUE); //Timer3??????????????????
    
    stcTim3BaseCfg.enWorkMode = Tim3WorkMode0;               //???????????????
    stcTim3BaseCfg.enCT       = Tim3Timer;                   //???????????????????????????????????????PCLK
    stcTim3BaseCfg.enPRS      = Tim3PCLKDiv16;               //PCLK/16
    stcTim3BaseCfg.enCntMode  = Tim316bitArrMode;            //????????????16????????????/?????????
    stcTim3BaseCfg.bEnTog     = TRUE;
    stcTim3BaseCfg.bEnGate    = FALSE;
    stcTim3BaseCfg.enGateP    = Tim3GatePositive;
    
    Tim3_Mode0_Init(&stcTim3BaseCfg);                        //TIM3 ?????????0???????????????
        
    u16ArrValue = 0x10000 - u16Period;
    Tim3_M0_ARRSet(u16ArrValue);                             //???????????????
    
    u16CntValue = 0x10000 - u16Period;
    Tim3_M0_Cnt16Set(u16CntValue);                           //??????????????????
    
    //Tim3_ClearIntFlag(Tim3UevIrq);                         //???????????????
    //EnableNvic(TIM3_IRQn, 3, TRUE);                        //TIM3 ?????????
    //Tim3_Mode0_EnableIrq();                                //??????TIM3??????(??????0?????????????????????)    
    
    Tim3_M0_Enable_Output(TRUE);                             //TIM3 ??????????????????
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
    App_LEDPortCfg();     //LED????????????

    App_Timer3Cfg(25000); //Timer3 ??????; 16??????,??????25000-->25000*(1/4M) * 16 = 100000us = 100ms
    
    Tim3_M0_Run();        //TIM3 ?????????
    
    while (1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


