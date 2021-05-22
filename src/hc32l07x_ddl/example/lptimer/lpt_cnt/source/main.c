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
#include "lptim.h"
#include "lpm.h"
#include "gpio.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
//LPTIMER0的ETR
#define LptETR_PORT  GpioPortB
#define LptETR_PIN   GpioPin4

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
__IO uint8_t ItFlag;
/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/


/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

static void App_SysClkInit(void);     
static void App_GPIOInit(void);
static void App_LPTimer0Init(void);
static void App_LowPowerModeGpioSet(void);

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
    ItFlag = 0;
    
    ///< 系统时钟初始化
    App_SysClkInit();     
    ///< GPIO初始化
    App_GPIOInit();
    ///< LPTimer0初始化
    App_LPTimer0Init();     
    
    Lptim_Cmd(M0P_LPTIMER0, TRUE);   ///< 启动lptimer0运行
    
    ///< 警告！！！请勿删除，否则会导致进入低功耗模式后无法再次调试
    while (1 == Gpio_GetInputIO(EVB_KEY1_PORT,EVB_KEY1_PIN));       ///< 等待user key按下
    Gpio_WriteOutputIO(EVB_LEDR_PORT, EVB_LEDR_PIN, FALSE);           ///< 熄灭LED
    
    ///< 休眠模式GPIO配置
    App_LowPowerModeGpioSet();
    Lpm_GotoDeepSleep(TRUE);                                        ///< 进入深度睡眠模式，使能退出中断服务后再次进入休眠特性
    while (1)
    {
        if(ItFlag == 1)
        {
            ItFlag = 0;
        }
    }
}


/**
 ******************************************************************************
 ** \brief  LPTIMER0中断服务函数
 **
 ** \return 无
 ******************************************************************************/
void LpTim0_IRQHandler(void)
{
    if (TRUE == Lptim_GetItStatus(M0P_LPTIMER0))
    {
        ItFlag = 1;
        Lptim_ClrItStatus(M0P_LPTIMER0);//清除LPTIMER0的中断标志位      

        Gpio_WriteOutputIO(EVB_LEDR_PORT, EVB_LEDR_PIN, TRUE);     //输出高，点亮LED
        delay1ms(500);
        Gpio_WriteOutputIO(EVB_LEDR_PORT, EVB_LEDR_PIN, FALSE);     //输出高，点亮LED
    }
}


static void App_SysClkInit(void)
{
    stc_sysctrl_clk_cfg_t  stcClkCfg;
    
    //CLK INIT
    stcClkCfg.enClkSrc  = SysctrlClkRCH;
    stcClkCfg.enHClkDiv = SysctrlHclkDiv1;
    stcClkCfg.enPClkDiv = SysctrlPclkDiv1;
    Sysctrl_ClkInit(&stcClkCfg);
    
    //使能RCL
    Sysctrl_ClkSourceEnable(SysctrlClkRCL, TRUE);
}


/**
 ******************************************************************************
 ** \brief  初始化外部GPIO引脚
 **
 ** \return 无
 **
 ** 
 **
 ******************************************************************************/
static void App_GPIOInit(void)
{
    stc_gpio_cfg_t         GpioInitStruct;
    DDL_ZERO_STRUCT(GpioInitStruct);
    
    //使能GPIO外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 
    
    GpioInitStruct.enDrv  = GpioDrvH;
    GpioInitStruct.enDir  = GpioDirIn;
    Gpio_Init(LptETR_PORT, LptETR_PIN, &GpioInitStruct);
    Gpio_SetAfMode(LptETR_PORT,LptETR_PIN,GpioAf7);
    
    //PD14设置为GPIO-->LED控制脚
    GpioInitStruct.enDir  = GpioDirOut;
    Gpio_Init(EVB_LEDR_PORT, EVB_LEDR_PIN, &GpioInitStruct);

    //PA07作为按键输入
    GpioInitStruct.enDir  = GpioDirIn;
    Gpio_Init(EVB_KEY1_PORT, EVB_KEY1_PIN, &GpioInitStruct);
    
    Gpio_WriteOutputIO(EVB_LEDR_PORT, EVB_LEDR_PIN, TRUE);     //输出高，点亮LED    
}

/**
 ******************************************************************************
 ** \brief  初始化LPTIMER0
 **
 ** \return 无
 ******************************************************************************/
static void App_LPTimer0Init(void)
{
    stc_lptim_cfg_t    stcLptCfg;    
    DDL_ZERO_STRUCT(stcLptCfg);

    ///< 使能LPTIM0 外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralLpTim0, TRUE);
    
    stcLptCfg.enGate   = LptimGateLow;
    stcLptCfg.enGatep  = LptimGatePLow;
    stcLptCfg.enTcksel = LptimRcl;
    stcLptCfg.enTogen  = LptimTogEnLow;
    stcLptCfg.enCt     = LptimCntFun;           //计数器功能
    stcLptCfg.enMd     = LptimMode2;            //工作模式为模式2：自动重装载16位计数器/定时器
    stcLptCfg.u16Arr   = 65530;                 //预装载寄存器值，计数初值
    Lptim_Init(M0P_LPTIMER0, &stcLptCfg);
    
    Lptim_ClrItStatus(M0P_LPTIMER0);            //清除中断标志位
    Lptim_ConfIt(M0P_LPTIMER0, TRUE);           //允许LPTIMER中断    
    EnableNvic(LPTIM_0_1_IRQn, IrqLevel3, TRUE); 
}


static void App_LowPowerModeGpioSet(void)
{
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    //swd as gpio
    Sysctrl_SetFunc(SysctrlSWDUseIOEn, TRUE);
    
    ///< 配置为数字端口
    M0P_GPIO->PAADS = 0;
    M0P_GPIO->PBADS = 0;
    M0P_GPIO->PCADS = 0;
    M0P_GPIO->PDADS = 0;
    M0P_GPIO->PEADS = 0;
    M0P_GPIO->PFADS = 0;
    
    ///< 配置为端口输入（除LED端口外）
    M0P_GPIO->PADIR = 0XFFFF;
    M0P_GPIO->PBDIR = 0XFFFF;
    M0P_GPIO->PCDIR = 0XFFFF;
    M0P_GPIO->PDDIR = 0XFFFF; 
    M0P_GPIO->PEDIR = 0XFFF7;
    M0P_GPIO->PFDIR = 0XFFFF;
    
}
/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
