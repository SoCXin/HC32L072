/******************************************************************************
* Copyright (C) 2019, Huada Semiconductor Co.,Ltd All rights reserved.
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
 **   - 2019-03-01  1.0  Lux First version
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "sysctrl.h"
#include "dac.h"
#include "flash.h"
#include "gpio.h"
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
static void App_SysClkInit(void);               ///< ?????????????????????
static void App_GpioInit(void);                 ///< GPIO?????????
static void App_DACInit(void);                  ///< DAC?????????

/**
******************************************************************************
    ** \brief  ?????????
    **
    ** @param  ???
    ** \retval ???
    **
******************************************************************************/
int32_t main(void)
{
    App_SysClkInit();               ///< ?????????????????????
    App_GpioInit();                 ///< GPIO?????????
    App_DACInit();                  ///< DAC?????????

    while(1)
    {
        Dac0_SoftwareTriggerCmd();   ///< ????????????
        delay10us(12);
    }

}

/**
 ******************************************************************************
 ** \brief  ???????????????????????????
 **
 ** \return ???
 ******************************************************************************/
static void App_SysClkInit(void)
{
    stc_sysctrl_clk_cfg_t stcCfg;
    stc_sysctrl_pll_cfg_t stcPLLCfg;


    Sysctrl_SetPeripheralGate(SysctrlPeripheralFlash, TRUE);    ///< ??????FLASH?????????????????????
    Flash_WaitCycle(FlashWaitCycle1);
    Sysctrl_SetRCHTrim(SysctrlRchFreq4MHz);             ///< PLL??????RCH???????????????????????????????????????RCH

    stcPLLCfg.enInFreq    = SysctrlPllInFreq4_6MHz;     ///< RCH 4MHz
    stcPLLCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;  ///< PLL ??????48MHz
    stcPLLCfg.enPllClkSrc = SysctrlPllRch;              ///< ?????????????????????RCH
    stcPLLCfg.enPllMul    = SysctrlPllMul12;            ///< 4MHz x 12 = 48MHz
    Sysctrl_SetPLLFreq(&stcPLLCfg);

    ///< ??????PLL??????HCLK?????????;
    stcCfg.enClkSrc  = SysctrlClkPLL;
    ///< HCLK SYSCLK/2
    stcCfg.enHClkDiv = SysctrlHclkDiv1;
    ///< PCLK ???HCLK/8
    stcCfg.enPClkDiv = SysctrlPclkDiv1;
    ///< ?????????????????????
    Sysctrl_ClkInit(&stcCfg);
}



/**
 ******************************************************************************
 ** \brief  ???????????????GPIO??????
 **
 ** \return ???
 ******************************************************************************/
static void App_GpioInit(void)
{
    ///< ??????GPIO????????????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);    // ??????GPIO?????????????????????

    Gpio_SetAnalogMode(GpioPortA, GpioPin4);       //PA04??????DAC???????????????
}

/**
******************************************************************************
    ** \brief  ??????DAC
    **
    ** @param  ???
    ** \retval ???
    **
******************************************************************************/
static void App_DACInit(void)
{
    stc_dac_cfg_t  dac_initstruct;

    Sysctrl_SetPeripheralGate(SysctrlPeripheralDac, TRUE);     ///< ??????DAC???????????????

    dac_initstruct.boff_t = DacBoffDisable;
    dac_initstruct.ten_t  = DacTenEnable;
    dac_initstruct.sref_t = DacVoltageAvcc;
    dac_initstruct.wave_t = DacTrWaveEnable;      ///< ???????????????
    dac_initstruct.mamp_t = DacMenp4095;
    dac_initstruct.tsel_t = DacSwTriger;          ///< ??????????????????
    dac_initstruct.align  = DacRightAlign;        ///< ?????????
    dac_initstruct.dhr12  = 0;

    Dac0_Init(&dac_initstruct);
    Dac0_Cmd(TRUE);
    Dac0_DmaCmd(TRUE);                       ///< DAC??????DMA??????

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


