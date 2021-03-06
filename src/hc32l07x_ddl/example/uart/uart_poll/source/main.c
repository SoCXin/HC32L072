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
 **   - 2017-05-17  1.0  cj First version for Device Driver Library of Module.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "uart.h"
#include "gpio.h"
#include "sysctrl.h"

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
uint8_t u8TxData[2] = {0x00,0x55};

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
void App_PortInit(void);
void App_UartCfg(void);

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
    uint8_t i;
    
    App_PortInit();       //???????????????
    
    App_UartCfg();        //??????????????????
    
    while(1)
    {
        if((Uart_GetStatus(M0P_UART0, UartFE))||(Uart_GetStatus(M0P_UART0, UartPE)))  //????????????
        {
            Uart_ClrStatus(M0P_UART0, UartFE);            //?????????????????????
            Uart_ClrStatus(M0P_UART0, UartPE);            //??????????????????????????????
        }
        if(Uart_GetStatus(M0P_UART0,UartRC))              //???????????????
        {
            Uart_ClrStatus(M0P_UART0,UartRC);
            u8TxData[0] = Uart_ReceiveData(M0P_UART0);    //????????????
            for(i=0;i<2;i++)
            {
                Uart_SendDataPoll(M0P_UART0,u8TxData[i]); //????????????????????????
            }
        }
    }
}

//??????????????????
void App_PortInit(void)
{
    stc_gpio_cfg_t stcGpioCfg;
    
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); //GPIO????????????????????????
    
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(GpioPortA,GpioPin9,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin9,GpioAf1); //??????PA09 ???UART0 TX
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(GpioPortA,GpioPin10,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortA,GpioPin10,GpioAf1);//??????PA10 ???UART0 RX
}

//??????????????????
void App_UartCfg(void)
{
    stc_uart_cfg_t  stcCfg;
    stc_uart_multimode_t stcMulti;
    stc_uart_baud_t stcBaud;

    DDL_ZERO_STRUCT(stcCfg);
    DDL_ZERO_STRUCT(stcMulti);
    DDL_ZERO_STRUCT(stcBaud);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE);//UART0????????????????????????
    
    stcCfg.enRunMode = UartMskMode3;     //??????3
    stcCfg.enStopBit = UartMsk1bit;      //1????????????
    stcCfg.enMmdorCk = UartMskEven;      //?????????
    stcCfg.stcBaud.u32Baud = 9600;       //?????????9600
    stcCfg.stcBaud.enClkDiv = UartMsk8Or16Div;         //????????????????????????
    stcCfg.stcBaud.u32Pclk = Sysctrl_GetPClkFreq();    //?????????????????????PCLK????????????
    Uart_Init(M0P_UART0, &stcCfg);       //???????????????

    Uart_ClrStatus(M0P_UART0,UartRC);    //???????????????
    Uart_ClrStatus(M0P_UART0,UartTC);    //???????????????
    Uart_EnableIrq(M0P_UART0,UartRxIrq); //????????????????????????
    Uart_EnableIrq(M0P_UART0,UartTxIrq); //????????????????????????

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


