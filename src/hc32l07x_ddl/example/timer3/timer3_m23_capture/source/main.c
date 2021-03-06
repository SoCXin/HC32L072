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
 **   - 2019-04-18  1.0  Husj First version for Device Timer3.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "timer3.h"
#include "gpio.h"
#include "flash.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
uint32_t  Tim3_Capture_Value1,Tim3_Capure_Value2;
volatile int32_t   Tim3_Capture_Value;
volatile uint16_t  Tim3_Uev_Cnt;
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
    static uint8_t i;
    
    //Timer3 ????????????A
    if(TRUE == Tim3_GetIntFlag(Tim3CA0Irq))
    {
        if(0 == i)
        {
            Tim3_Capture_Value1 = M0P_TIM3_MODE23->CCR0A_f.CCR0A;   //????????????????????????
            Tim3_Uev_Cnt = 0;
            i++;
        }
        else 
        {
            Tim3_Capure_Value2 = M0P_TIM3_MODE23->CCR0A_f.CCR0A;  //????????????????????????
            Tim3_Capture_Value = Tim3_Uev_Cnt * 0xFFFF + Tim3_Capure_Value2 - Tim3_Capture_Value1;  //???????????????????????????
            
            Tim3_Uev_Cnt = 0;
            
            i = 0;
        }
        
        Tim3_ClearIntFlag(Tim3CA0Irq);  //???????????????
    }
    
    //timer3??????????????????
    if(TRUE == Tim3_GetIntFlag(Tim3UevIrq))
    {
        Tim3_Uev_Cnt++;
        Tim3_ClearIntFlag(Tim3UevIrq);
    }
}

//???????????????
void App_ClockCfg(void)
{
    en_flash_waitcycle_t         enFlashWait;
    stc_sysctrl_pll_cfg_t     stcPLLCfg;
    
    //????????????????????????
    DDL_ZERO_STRUCT(stcPLLCfg);
    
    enFlashWait = FlashWaitCycle1;                      //????????????????????????1??????HCLK??????24MHz??????????????????1???
    Flash_WaitCycle(enFlashWait);                       // Flash ??????1?????????
    
    stcPLLCfg.enInFreq    = SysctrlPllInFreq4_6MHz;     //RCH 4MHz
    stcPLLCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;  //PLL ??????48MHz
    stcPLLCfg.enPllClkSrc = SysctrlPllRch;              //?????????????????????RCH
    stcPLLCfg.enPllMul    = SysctrlPllMul12;            //4MHz x 12 = 48MHz
    Sysctrl_SetPLLFreq(&stcPLLCfg);
    Sysctrl_SetPLLStableTime(SysctrlPllStableCycle16384);
    Sysctrl_ClkSourceEnable(SysctrlClkPLL, TRUE);
    
    Sysctrl_SysClkSwitch(SysctrlClkPLL);///< ????????????
}


//Timer3 Port????????????
void App_Timer3PortCfg(void)
{
    stc_gpio_cfg_t            stcTIM3Port;
    
    //????????????????????????
    DDL_ZERO_STRUCT(stcTIM3Port);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //????????????????????????
    
    stcTIM3Port.enDir  = GpioDirIn;
    
    Gpio_Init(GpioPortA, GpioPin8, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortA,GpioPin8,GpioAf2);               //PA08?????????TIM3_CH0A
    
#if 0

    Gpio_Init(GpioPortA, GpioPin7, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortA,GpioPin7,GpioAf4);               //PA07?????????TIM3_CH0B    
    Gpio_Init(GpioPortB, GpioPin10, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortB,GpioPin10,GpioAf5);              //PB10?????????TIM3_CH1A
    
    Gpio_Init(GpioPortB, GpioPin0, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortB,GpioPin0,GpioAf2);               //PB00?????????TIM3_CH1B
    
    Gpio_Init(GpioPortB, GpioPin8, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortB,GpioPin8,GpioAf6);               //PB08?????????TIM3_CH2A
    
    Gpio_Init(GpioPortB, GpioPin15, &stcTIM3Port);
    Gpio_SetAfMode(GpioPortB,GpioPin15,GpioAf2);              //PB15?????????TIM3_CH2B
#endif
}


//Timer3 ??????
void App_Timer3Cfg(void)
{
    uint16_t                        u16ArrValue;
    uint16_t                        u16CntValue;

    stc_tim3_mode23_cfg_t        stcTim3BaseCfg;
    stc_tim3_m23_input_cfg_t     stcTim3PortCapCfg;
    
    //????????????????????????
    DDL_ZERO_STRUCT(stcTim3BaseCfg);
    DDL_ZERO_STRUCT(stcTim3PortCapCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralTim3, TRUE);   //Timer3??????????????????
    
    stcTim3BaseCfg.enWorkMode    = Tim3WorkMode2;             //???????????????
    stcTim3BaseCfg.enCT          = Tim3Timer;                 //???????????????????????????????????????PCLK
    stcTim3BaseCfg.enPRS         = Tim3PCLKDiv64;             //PCLK/64
    stcTim3BaseCfg.enCntDir      = Tim3CntUp;                 //??????????????????????????????????????????
    
    Tim3_Mode23_Init(&stcTim3BaseCfg);                        //TIM3 ?????????23???????????????
        
    stcTim3PortCapCfg.enCHxACmpCap   = Tim3CHxCapMode;        //CHB???????????????????????????
    stcTim3PortCapCfg.enCHxACapSel   = Tim3CHxCapFallRise;    //CHB???????????????????????????????????????
    stcTim3PortCapCfg.enCHxAInFlt    = Tim3FltPCLKDiv16Cnt3;  //PCLK/16 3???????????????
    stcTim3PortCapCfg.enCHxAPolarity = Tim3PortPositive;      //??????????????????
    
    Tim3_M23_PortInput_Cfg(Tim3CH0, &stcTim3PortCapCfg);   //???????????????????????????
    //Tim3_M23_PortInput_Cfg(Tim3CH1, &stcTim3PortCapCfg);   //???????????????????????????
    //Tim3_M23_PortInput_Cfg(Tim3CH2, &stcTim3PortCapCfg);   //???????????????????????????
    
    u16ArrValue = 0xFFFF;
    Tim3_M23_ARRSet(u16ArrValue, TRUE);                       //???????????????,???????????????
    
    u16CntValue = 0;
    Tim3_M23_Cnt16Set(u16CntValue);                           //??????????????????
    
    Tim3_ClearAllIntFlag();                                   //???????????????
    Tim3_Mode23_EnableIrq(Tim3CA0Irq);                        //??????TIM3 CB0??????/????????????
    Tim3_Mode23_EnableIrq(Tim3UevIrq);                        //??????TIM3 Uev????????????
    EnableNvic(TIM3_IRQn, IrqLevel1, TRUE);                   //TIM3????????????
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
    App_ClockCfg();      //???????????????
    
    App_Timer3PortCfg(); //Timer3 Port????????????
    
    App_Timer3Cfg();     //Timer3 ??????
    
    Tim3_M23_Run();      //?????????
    
    while (1)
    {
      
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


