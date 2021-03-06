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
 ** \brief  ?????????
 **
 ** \return ???
 ******************************************************************************/
int32_t main(void)
{   
    stc_gpio_cfg_t GpioInitStruct;
    stc_opa_oenx_config_t OpaOenxStruct;
    stc_opa_zconfig_t OpaInitStruct;
    
    DDL_ZERO_STRUCT(GpioInitStruct);
    DDL_ZERO_STRUCT(OpaOenxStruct);
    DDL_ZERO_STRUCT(OpaInitStruct);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralOpa, TRUE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE); /* ??????OPA,????????????BGR?????? */
    Bgr_BgrEnable(); /* ??????OPA,????????????BGR */
    
    Gpio_SetAnalogMode(GpioPortB, GpioPin15);      /* OPA_INN????????? PB15 */
    Gpio_SetAnalogMode(GpioPortC, GpioPin6);       /* OPA_INP????????? PC06 */
    Gpio_SetAnalogMode(GpioPortC, GpioPin7);       /* OPA_OUT????????? PC07 */
    Gpio_SetAnalogMode(GpioPortD, GpioPin8);       /* OPA_OUT4?????????PD08 */

    /* ??????Opa0?????????????????? */
    OpaOenxStruct.opa_ch1 = FALSE;
    OpaOenxStruct.opa_ch2 = FALSE;
    OpaOenxStruct.opa_ch3 = FALSE;
    OpaOenxStruct.opa_ch4 = TRUE;
    Opa_OutChannelConfig(Opa0, OpaOenxStruct);

    /* ??????Opa0?????? */
    Opa_Cmd(Opa0, TRUE);

    /* ??????Opa?????? */
    OpaInitStruct.bAdctr_en = FALSE;                     /* ??????DAC?????????????????????*/
    OpaInitStruct.bAz_pulse = FALSE;                     /* ?????????????????????????????????????????????*/
    OpaInitStruct.bClk_sw_set = FALSE;                   /* ??????????????????*/
    OpaInitStruct.enClksrc = Opa_M1024Pclk;              /* ??????????????????????????????*/
    Opa_CalConfig(&OpaInitStruct);
    
    /* ??????Opa0?????????????????????OPA??????????????????????????????????????????????????????OPA_CR */
    Opa_CalCmd(Opa0, TRUE);
    
    /* ?????????????????? */
    Opa_CalSwTrig();
    
    
    while (1)
    {   
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


