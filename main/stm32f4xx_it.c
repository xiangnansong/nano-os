/**
  ******************************************************************************
  * @file    LTDC/LTDC_Display_2Layers/Src/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.2.7
  * @date    04-November-2016 
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stm32f429xx.h>
#include <stdio.h>
#include "main.h"
#include "stm32f4xx_it.h"
#include "usart.h"

uint8_t exti0_toggle = 0;
extern uint8_t uBuffer[BUFLEN + 2];
/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup LTDC_Display_2Layer
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
//extern LTDC_HandleTypeDef LtdcHandle;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void) {
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void) {
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1) {
    }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void) {
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1) {
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void) {
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1) {
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void) {
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1) {
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */


/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void) {
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s/startup_stm32f429x.s).    */
/******************************************************************************/

/**
  * @brief  This function handles LTDC global interrupt request.
  * @param  None
  * @retval None
  */
void LTDC_IRQHandler(void) {
//    HAL_LTDC_IRQHandler(&LtdcHandle);
}

void EXTI0_IRQHandler(void) {
    delay_ms(10);
    LED_RED = !LED_RED;
    LED_GREEN = !LED_GREEN;
    EXTI->PR = 1 << 0;
    printf("enter exti\n");
    if (exti0_toggle) {
        task_suspend_irq(2);
        exti0_toggle = 0;
    } else {
        task_unsuspend_irq(2);
        exti0_toggle = 1;
    }

//    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//    HAL_Delay(50);
//    HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13 | GPIO_PIN_14);
//    printf("Hello\n\r");
}



/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

void TIM3_IRQHandler(void) {
    if (TIM3->SR &= 0x01) {
        LED_RED = !LED_RED;
        LED_GREEN = !LED_GREEN;
    }
    TIM3->SR &= ~(1 << 0);
}


void USART1_IRQHandler(void) {
    uint8_t res;
    if (USART1->SR & (1 << 5)) {
        res = (uint8_t) USART1->DR;
        if (res == 0x0d) {
            uBuffer[BUFLEN + 1] = 1;
            LED_RED = !LED_RED;
            LED_GREEN = !LED_GREEN;
//            uBuffer[BUFLEN + 1] = 0;
//            uBuffer[uBuffer[BUFLEN]] = res;
//            uBuffer[BUFLEN]++;
        } else {
            uBuffer[BUFLEN + 1] = 0;
            uBuffer[uBuffer[BUFLEN]] = res;
            uBuffer[BUFLEN]++;

//            uBuffer[BUFLEN + 1] = 1;
//            uBuffer[BUFLEN] = 0;
//            LED_RED = !LED_RED;
//            LED_GREEN = !LED_GREEN;
        }
    }
}

/**
  * @}
  */

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
