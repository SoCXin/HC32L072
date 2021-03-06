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
static void App_GpioInit(void);
static void App_VC0Init(void);
static void App_Tim0Mode0Init(void);
/**
 ******************************************************************************
 ** \brief  ?????????
 **
 ** \return ???
 ******************************************************************************/
int main(void)
{
    App_GpioInit();                         //??????GPIO
    App_Tim0Mode0Init();                    //??????TIM0
    App_VC0Init();                          //??????VC

    Vc_Cmd(VcChannel0, TRUE);               //??????VC0
    Bt_M0_Run(TIM0);                        //??????TIM0??????
    
    while (1)
    {
        ;
    }
}


void Vc0_IRQHandler(void)
{
    Vc_ClearItStatus(Vc0_Intf);
    
    Gpio_WriteOutputIO(EVB_LEDR_PORT, EVB_LEDR_PIN, TRUE);
    delay1ms(1000);
    Gpio_WriteOutputIO(EVB_LEDR_PORT, EVB_LEDR_PIN, FALSE);
    delay1ms(1000);
}
/**
 ******************************************************************************
 ** \brief  ???????????????GPIO??????
 **
 ** \return ???
 ******************************************************************************/
static void App_GpioInit(void)
{
    stc_gpio_cfg_t GpioInitStruct;
    DDL_ZERO_STRUCT(GpioInitStruct);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);//???GPIO??????
     
    GpioInitStruct.enDrv = GpioDrvH;
    GpioInitStruct.enDir = GpioDirOut;
    Gpio_Init(GpioPortA,GpioPin6,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortA,GpioPin6,GpioAf5);              //PA06??????VC0_OUT

    Gpio_Init(EVB_LEDR_PORT, EVB_LEDR_PIN, &GpioInitStruct);   //PD14????????????????????????????????????LED
    Gpio_WriteOutputIO(EVB_LEDR_PORT, EVB_LEDR_PIN, TRUE);    
    
    Gpio_SetAnalogMode(GpioPortC,GpioPin0);                  //VC0???P?????????

    GpioInitStruct.enDir  = GpioDirOut;   
    Gpio_Init(GpioPortA, GpioPin0, &GpioInitStruct);
    Gpio_SetAfMode(GpioPortA,GpioPin0,GpioAf7);            //PA00?????????TIM0_CHA
    
    Gpio_Init(GpioPortA, GpioPin1, &GpioInitStruct);
    Gpio_SetAfMode(GpioPortA,GpioPin1,GpioAf3);            //PA01?????????TIM0_CHB    
}

/**
 ******************************************************************************
 ** \brief  ?????????VC0
 **
 ** \return ???
 ******************************************************************************/
static void App_VC0Init(void)
{
    stc_vc_channel_cfg_t VcInitStruct;
    DDL_ZERO_STRUCT(VcInitStruct);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralVcLvd, TRUE);//???LVD??????
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE);//???adc??????  
        
    Bgr_BgrEnable();                 //BGR????????????
    
    VcInitStruct.enVcChannel = VcChannel0;
    VcInitStruct.enVcCmpDly  = VcDelay10mv;          //VC0??????????????????10mV
    VcInitStruct.enVcBiasCurrent = VcBias10ua;       //VC0?????????10uA
    VcInitStruct.enVcFilterTime  = VcFilter28us;     //VC????????????????????????28us
    VcInitStruct.enVcInPin_P     = VcInPCh0;         //VC0_CH???P?????????PC00
    VcInitStruct.enVcInPin_N     = AiBg1p2;          //VC0_CH???N???????????????1.2V
    VcInitStruct.enVcOutCfg      = VcOutTIMBK;       //?????????????????????
    VcInitStruct.bFlten          = TRUE;             //????????????
    Vc_Init(&VcInitStruct);
    
    ///< VC0????????????
    Vc_CfgItType(VcChannel0, VcIrqRise);          //??????VC0??????????????????
    Vc_ClearItStatus(Vc0_Intf);    
    Vc_ItCfg(VcChannel0, TRUE);    
    EnableNvic(VC0_1_2_LVD_IRQn, IrqLevel3, TRUE);

}
/**
 ******************************************************************************
 ** \brief  ?????????TIM2
 **
 ** \return ???
 ******************************************************************************/
static void App_Tim0Mode0Init(void)
{
    uint16_t                      u16ArrValue;
    uint16_t                      u16CompareAValue;
    uint16_t                      u16CompareBValue;
    uint16_t                      u16CntValue;
    uint8_t                       u8ValidPeriod;

    stc_vc_channel_cfg_t       stcChannelCfg;    
    stc_bt_mode23_cfg_t        stcBtBaseCfg;
    stc_bt_m23_compare_cfg_t   stcBtPortCmpCfg;  
    stc_bt_m23_bk_input_cfg_t  stcBtBkInputCfg;

    DDL_ZERO_STRUCT(stcChannelCfg);
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    DDL_ZERO_STRUCT(stcBtPortCmpCfg);   
    DDL_ZERO_STRUCT(stcBtBkInputCfg);   

    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE); //Base Timer??????????????????  
    
    stcBtBaseCfg.enWorkMode    = BtWorkMode2;              //???????????????
    stcBtBaseCfg.enCT          = BtTimer;                  //???????????????????????????????????????PCLK
    stcBtBaseCfg.enPRS         = BtPCLKDiv1;               //PCLK
    stcBtBaseCfg.enCntDir      = BtCntUp;                  //????????????
    stcBtBaseCfg.enPWMTypeSel  = BtIndependentPWM;         //????????????PWM
    stcBtBaseCfg.enPWM2sSel    = BtSinglePointCmp;         //??????????????????
    stcBtBaseCfg.bOneShot      = FALSE;                    //????????????
    stcBtBaseCfg.bURSSel       = FALSE;                    //???????????????
    Bt_Mode23_Init(TIM0, &stcBtBaseCfg);                   //TIM0 ?????????0???????????????
     
    u16ArrValue = 0x9000;
    Bt_M23_ARRSet(TIM0, u16ArrValue, TRUE);                //???????????????,???????????????

    u16CompareAValue = 0x6000;
    Bt_M23_CCR_Set(TIM0, BtCCR0A, u16CompareAValue);       //???????????????A
    u16CompareBValue = 0x3000;
    Bt_M23_CCR_Set(TIM0, BtCCR0B, u16CompareBValue);       //???????????????B

    stcBtPortCmpCfg.enCH0ACmpCtrl   = BtPWMMode2;          //OCREFA????????????OCMA:PWM??????2
    stcBtPortCmpCfg.enCH0APolarity  = BtPortPositive;      //????????????
    stcBtPortCmpCfg.bCh0ACmpBufEn   = TRUE;                //A??????????????????
    stcBtPortCmpCfg.enCh0ACmpIntSel = BtCmpIntNone;        //A??????????????????:???

    stcBtPortCmpCfg.enCH0BCmpCtrl   = BtPWMMode2;          //OCREFB????????????OCMB:PWM??????2
    stcBtPortCmpCfg.enCH0BPolarity  = BtPortPositive;      //????????????
    stcBtPortCmpCfg.bCH0BCmpBufEn   = TRUE;                //B????????????????????????
    stcBtPortCmpCfg.enCH0BCmpIntSel = BtCmpIntNone;        //B??????????????????:???
    Bt_M23_PortOutput_Cfg(TIM0, &stcBtPortCmpCfg);      //????????????????????????

    u8ValidPeriod = 0;                                     //???????????????????????????0?????????????????????????????????????????????+1????????????1?????????
    Bt_M23_SetValidPeriod(TIM0,u8ValidPeriod);             //??????????????????
        
    u16CntValue = 0;   
    Bt_M23_Cnt16Set(TIM0, u16CntValue);                    //??????????????????   
    Bt_M23_EnPWM_Output(TIM0, TRUE, FALSE);                //TIM0 ??????????????????

    stcBtBkInputCfg.bEnBrake = TRUE;                       //????????????
    stcBtBkInputCfg.bEnVCBrake = TRUE;                     //??????VC??????
    stcBtBkInputCfg.bEnSafetyBk = FALSE;
    stcBtBkInputCfg.bEnBKSync =   FALSE;
    stcBtBkInputCfg.enBkCH0AStat =BtCHxBksLow;             //??????????????????????????????
    stcBtBkInputCfg.enBkCH0BStat =BtCHxBksLow;             //?????????????????????????????? 
    stcBtBkInputCfg.enBrakePolarity = BtPortPositive;
    stcBtBkInputCfg.enBrakeFlt = BtFltNone;
    Bt_M23_BrakeInput_Cfg(TIM0,&stcBtBkInputCfg);
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


