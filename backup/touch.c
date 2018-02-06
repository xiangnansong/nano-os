//
// Created by song on 17-4-28.
//

#include "touch.h"

uint32_t IOE_TimeOut = TIMEOUT_MAX;
TP_STATE TP_State;

void touch_init() {
    uint32_t reg = 0;
    //GPIO 配置
    RCC->APB1ENR |= 1 << 23;
    RCC->AHB1ENR |= 1 << 0;
    RCC->AHB1ENR |= 1 << 2;
    RCC->APB2ENR |= 1 << 14;

    RCC->APB1RSTR |= 1 << 23;
    RCC->APB1RSTR &= ~(1 << 23);

    gpio_af_set(GPIOA, 8, 4);
    gpio_af_set(GPIOC, 9, 4);

    gpio_set(GPIOA, PIN8, GPIO_MODE_AF, GPIO_OTYPE_OD, GPIO_SPEED_50M, GPIO_PUPD_NONE);
    gpio_set(GPIOC, PIN9, GPIO_MODE_AF, GPIO_OTYPE_OD, GPIO_SPEED_50M, GPIO_PUPD_NONE);

    //iic配置
    if((I2C3->CR1&0x0001) == 0){
        I2C3->CR1 |= 1<<10;
        I2C3->CR1 &= ~(1<<1);
        reg = I2C3->CR2;
        reg &= ~(0x1f);
        reg |= 45;
        I2C3->CR2 = reg;
        I2C3->CR1 &= ~(1<<0);
        reg = 0;
        reg = 225;
        I2C3->TRISE  = 46;
        I2C3->CCR = reg;
        I2C3->CR1 |= (1<<0);
        I2C3->OAR1 = 0x4000;
        I2C3->CR1 |= (1<<0);

    }


}

uint8_t  I2C_WriteDeviceRegister(uint8_t RegisterAddr, uint8_t RegisterValue){
    uint32_t read_verif = 0;

    /* Begin the configuration sequence */
    I2C_GenerateSTART(IOE_I2C, ENABLE);

    /* Test on EV5 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while (!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_SB))
    {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    /* Transmit the slave address and enable writing operation */
    I2C_Send7bitAddress(IOE_I2C, IOE_ADDR, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while (!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_ADDR))
    {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    /* Read status register 2 to clear ADDR flag */
    IOE_I2C->SR2;

    /* Test on EV8_1 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while (!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_TXE))
    {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    /* Transmit the first address for r/w operations */
    I2C_SendData(IOE_I2C, RegisterAddr);

    /* Test on EV8 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while (!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_TXE))
    {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    /* Prepare the register value to be sent */
    I2C_SendData(IOE_I2C, RegisterValue);

    /* Test on EV8_2 and clear it */
    IOE_TimeOut = TIMEOUT_MAX;
    while ((!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_TXE)) || (!I2C_GetFlagStatus(IOE_I2C, I2C_FLAG_BTF)))
    {
        if (IOE_TimeOut-- == 0) return(IOE_TimeoutUserCallback());
    }

    /* End the configuration sequence */
    I2C_GenerateSTOP(IOE_I2C, ENABLE);

#ifdef VERIFY_WRITTENDATA
    /* Verify (if needed) that the loaded data is correct  */

  /* Read the just written register*/
  read_verif = IOE_I2C_ReadDeviceRegister(RegisterAddr);

  /* Load the register and verify its value  */
  if (read_verif != RegisterValue)
  {
    /* Control data wrongly transferred */
    read_verif = IOE_FAILURE;
  }
  else
  {
    /* Control data correctly transferred */
    read_verif = 0;
  }
#endif

    /* Return the verifying value: 0 (Passed) or 1 (Failed) */
    return read_verif;
}
uint8_t  I2C_ReadDeviceRegister(uint8_t RegisterAddr){

}
uint16_t I2C_ReadDataBuffer(uint32_t RegisterAddr){

}

void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
    /* Check the parameters */

    if (NewState != DISABLE)
    {
        /* Generate a START condition */
        I2Cx->CR1 |= I2C_CR1_START;
    }
    else
    {
        /* Disable the START condition generation */
        I2Cx->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_START);
    }
}

void I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
    /* Check the parameters */

    if (NewState != DISABLE)
    {
        /* Generate a STOP condition */
        I2Cx->CR1 |= I2C_CR1_STOP;
    }
    else
    {
        /* Disable the STOP condition generation */
        I2Cx->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_STOP);
    }
}


FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG)
{
    FlagStatus bitstatus = RESET;
    __IO uint32_t i2creg = 0, i2cxbase = 0;

    /* Check the parameters */
    /* Get the I2Cx peripheral base address */
    i2cxbase = (uint32_t)I2Cx;

    /* Read flag register index */
    i2creg = I2C_FLAG >> 28;

    /* Get bit[23:0] of the flag */
    I2C_FLAG &= FLAG_MASK;

    if(i2creg != 0)
    {
        /* Get the I2Cx SR1 register address */
        i2cxbase += 0x14;
    }
    else
    {
        /* Flag in I2Cx SR2 Register */
        I2C_FLAG = (uint32_t)(I2C_FLAG >> 16);
        /* Get the I2Cx SR2 register address */
        i2cxbase += 0x18;
    }

    if(((*(__IO uint32_t *)i2cxbase) & I2C_FLAG) != (uint32_t)RESET)
    {
        /* I2C_FLAG is set */
        bitstatus = SET;
    }
    else
    {
        /* I2C_FLAG is reset */
        bitstatus = RESET;
    }

    /* Return the I2C_FLAG status */
    return  bitstatus;
}

void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction)
{
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
    assert_param(IS_I2C_DIRECTION(I2C_Direction));
    /* Test on the direction to set/reset the read/write bit */
    if (I2C_Direction != I2C_Direction_Transmitter)
    {
        /* Set the address bit0 for read */
        Address |= I2C_OAR1_ADD0;
    }
    else
    {
        /* Reset the address bit0 for write */
        Address &= (uint8_t)~((uint8_t)I2C_OAR1_ADD0);
    }
    /* Send the address */
    I2Cx->DR = Address;
}

void time_out() {

}






























