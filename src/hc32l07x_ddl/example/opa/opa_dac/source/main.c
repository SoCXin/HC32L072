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
 **   - 2016-02-16  1.0  XYZ First version for Device Driver Library of Module.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "gpio.h"
#include "opa.h"
#include "bgr.h"
#include "dac.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
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

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/**
******************************************************************************
    ** \brief  ??????DAC
    ** 
    ** @param  ???
    ** \retval ???
    **
******************************************************************************/ 
void DAC_Config(void)
{
    stc_dac_cfg_t  dac_initstruct;
    dac_initstruct.boff_t = DacBoffEnable;    //DAC0???????????????????????????
    dac_initstruct.ten_t  = DacTenEnable;     //DAC0??????????????????
    dac_initstruct.sref_t = DacVoltageAvcc;
    dac_initstruct.mamp_t = DacMenp2047;
    dac_initstruct.wave_t = DacTrWaveEnable;
    dac_initstruct.tsel_t = DacSwTriger;      //??????????????????
    dac_initstruct.align  = DacRightAlign;    //?????????
    dac_initstruct.dhr12  = 50;           //???????????????
    Dac0_Init(&dac_initstruct);
    Dac0_Cmd(TRUE);
}
/**
 ******************************************************************************
 ** \brief  ?????????
 **
 ** \return ???
 ******************************************************************************/
int32_t main(void)
{   
    uint16_t tmp;
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);    /* ??????GPIO??????????????? */
    Sysctrl_SetPeripheralGate(SysctrlPeripheralOpa, TRUE);     /* ??????OPA??????????????? */
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE);  /* ??????BGR??????????????? */
    Sysctrl_SetPeripheralGate(SysctrlPeripheralDac, TRUE);     /* ??????DAC??????????????? */
    Bgr_BgrEnable(); /* ??????BAR?????? */

    Gpio_SetAnalogMode(GpioPortA, GpioPin4);  /* PA04??????DAC?????????????????? */
    
    Opa_DacBufCmd(Opa_Dac0Buff, TRUE);        /* ??????DAC0???????????? */

    DAC_Config();                             /* DAC?????? */
    while (1)
    {
        Dac0_SoftwareTriggerCmd();             /* ?????????????????? */
        for(tmp=0; tmp<100; tmp++);           /* ?????? */
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


