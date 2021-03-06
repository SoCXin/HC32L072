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
 * REGARDING THE SOFTWARE (INCLUDING ANY ACCOMPANYING WRITTEN MATERIALS),
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
 ** \brief can rx tx poll mode sample
 **
 **   - 2018-12-22  1.0  Lux First version
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "sysctrl.h"
#include "gpio.h"
#include "can.h"

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
static void App_SysClkInit(void);
static void App_CanGpioInit(void);
static void App_CanInit(void);

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
__IO uint32_t can_id, can_data[2]={0};
stc_can_txframe_t       stcTxFrame;
stc_can_rxframe_t       stcRxFrame;
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/**
 *******************************************************************************
 ** \brief  Main function of can rx tx poll mode project
 **
 ** \param  None
 **
 ** \retval int32_t return value, if needed
 **
 ******************************************************************************/
int32_t main(void)
{
    uint8_t u8Idx = 0;

    ///< ?????????????????????(8MHz for CanClk)
    App_SysClkInit();

    ///< CAN GPIO ??????
    App_CanGpioInit();

    ///< CAN ???????????????
    App_CanInit();

    while(1)
    {
        if(TRUE == CAN_IrqFlgGet(CanRxIrqFlg))
        {
            CAN_IrqFlgClr(CanRxIrqFlg);

            CAN_Receive(&stcRxFrame);

            if(1 == stcRxFrame.Cst.Control_f.RTR)
            {
                continue;
            }

            //<<Can Tx
            stcTxFrame.StdID         = stcRxFrame.StdID;
            stcTxFrame.Control_f.DLC = stcRxFrame.Cst.Control_f.DLC;
            stcTxFrame.Control_f.IDE = stcRxFrame.Cst.Control_f.IDE;
            stcTxFrame.Control_f.RTR = stcRxFrame.Cst.Control_f.RTR;

            for(u8Idx=0; u8Idx<stcRxFrame.Cst.Control_f.DLC; u8Idx++)
            {
                stcTxFrame.Data[u8Idx] = stcRxFrame.Data[u8Idx];
            }

            CAN_SetFrame(&stcTxFrame);
            CAN_TransmitCmd(CanPTBTxCmd);
        }

    }

}


static void App_SysClkInit(void)
{
    ///< ???????????????????????????????????????????????????XTH????????????,???????????????????????????????????????????????????8MHz
    Sysctrl_SetXTHFreq(SysctrlXthFreq4_8MHz);
    Sysctrl_XTHDriverCfg(SysctrlXtalDriver1);
    Sysctrl_SetXTHStableTime(SysctrlXthStableCycle16384);
    Sysctrl_ClkSourceEnable(SysctrlClkXTH, TRUE);
    delay1ms(10);

    ///< ????????????
    Sysctrl_SysClkSwitch(SysctrlClkXTH);

}

static void App_CanGpioInit(void)
{
    stc_gpio_cfg_t stcGpioCfg;

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);

    ///< ??????????????????->??????
    stcGpioCfg.enDir = GpioDirIn;
    ///< ????????????????????????->???????????????
    stcGpioCfg.enDrv = GpioDrvL;
    ///< ?????????????????????->???
    stcGpioCfg.enPu = GpioPuDisable;
    stcGpioCfg.enPd = GpioPdDisable;
    ///< ????????????????????????->??????????????????
    stcGpioCfg.enOD = GpioOdDisable;
    ///< ????????????/??????????????????????????????????????????->AHB
    stcGpioCfg.enCtrlMode = GpioAHB;

    Gpio_Init(EVB_CAN_RX_PORT, EVB_CAN_RX_PIN, &stcGpioCfg);
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(EVB_CAN_TX_PORT, EVB_CAN_TX_PIN, &stcGpioCfg);
    Gpio_Init(EVB_CAN_STB_PORT, EVB_CAN_STB_PIN, &stcGpioCfg);

    ///<CAN RX\TX??????????????????
    Gpio_SetAfMode(EVB_CAN_RX_PORT, EVB_CAN_RX_PIN, GpioAf1);
    Gpio_SetAfMode(EVB_CAN_TX_PORT, EVB_CAN_TX_PIN, GpioAf1);

    ///<STB ???-PHY??????
    Gpio_ClrIO(EVB_CAN_STB_PORT, EVB_CAN_STB_PIN);
}

static void App_CanInit(void)
{
    stc_can_init_config_t   stcCanInitCfg;
    stc_can_filter_t        stcFilter;


    Sysctrl_SetPeripheralGate(SysctrlPeripheralCan, TRUE);

    //<<CAN ???????????????
    stcCanInitCfg.stcCanBt.PRESC = 1-1;
    stcCanInitCfg.stcCanBt.SEG_1 = 5-2;
    stcCanInitCfg.stcCanBt.SEG_2 = 3-1;
    stcCanInitCfg.stcCanBt.SJW   = 3-1;

    stcCanInitCfg.stcWarningLimit.CanErrorWarningLimitVal = 10;
    stcCanInitCfg.stcWarningLimit.CanWarningLimitVal = 16-1;

    stcCanInitCfg.enCanRxBufAll  = CanRxNormal;
    stcCanInitCfg.enCanRxBufMode = CanRxBufNotStored;
    stcCanInitCfg.enCanSTBMode   = CanSTBFifoMode;

    CAN_Init(&stcCanInitCfg);

    //<<CAN ???????????????
    stcFilter.enAcfFormat = CanAllFrames;
    stcFilter.enFilterSel = CanFilterSel1;
    stcFilter.u32CODE     = 0x00000000;
    stcFilter.u32MASK     = 0x1FFFFFFF;
    CAN_FilterConfig(&stcFilter, TRUE);

    //<<Can Irq Enable
    CAN_IrqCmd(CanRxIrqEn, TRUE);

}
/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
