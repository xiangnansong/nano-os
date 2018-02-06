//
// Created by song on 17-4-28.
//

#ifndef APP_TOUCH_H
#define APP_TOUCH_H

#include "../song/sys.h"

#define IOE_I2C                    I2C3
#define TIMEOUT_MAX    0x3000

typedef struct
{
    uint16_t TouchDetected;
    uint16_t X;
    uint16_t Y;
    uint16_t Z;
}TP_STATE;

/**
  * @brief  IO_Expander Error codes
  */
typedef enum
{
    IOE_OK = 0,
    IOE_FAILURE,
    IOE_TIMEOUT,
    PARAM_ERROR,
    IOE_NOT_OPERATIONAL,
}IOE_Status_TypDef;

/**
  * @brief  IO bit values
  */
typedef enum
{
    BitReset = 0,
    BitSet = 1
}IOE_BitValue_TypeDef;

/**
  * @brief  IOE DMA Direction
  */
typedef enum
{
    IOE_DMA_TX = 0,
    IOE_DMA_RX = 1
}IOE_DMADirection_TypeDef;

#define I2C_FLAG_DUALF                  ((uint32_t)0x00800000)
#define I2C_FLAG_SMBHOST                ((uint32_t)0x00400000)
#define I2C_FLAG_SMBDEFAULT             ((uint32_t)0x00200000)
#define I2C_FLAG_GENCALL                ((uint32_t)0x00100000)
#define I2C_FLAG_TRA                    ((uint32_t)0x00040000)
#define I2C_FLAG_BUSY                   ((uint32_t)0x00020000)
#define I2C_FLAG_MSL                    ((uint32_t)0x00010000)

/**
  * @brief  SR1 register flags
  */

#define I2C_FLAG_SMBALERT               ((uint32_t)0x10008000)
#define I2C_FLAG_TIMEOUT                ((uint32_t)0x10004000)
#define I2C_FLAG_PECERR                 ((uint32_t)0x10001000)
#define I2C_FLAG_OVR                    ((uint32_t)0x10000800)
#define I2C_FLAG_AF                     ((uint32_t)0x10000400)
#define I2C_FLAG_ARLO                   ((uint32_t)0x10000200)
#define I2C_FLAG_BERR                   ((uint32_t)0x10000100)
#define I2C_FLAG_TXE                    ((uint32_t)0x10000080)
#define I2C_FLAG_RXNE                   ((uint32_t)0x10000040)
#define I2C_FLAG_STOPF                  ((uint32_t)0x10000010)
#define I2C_FLAG_ADD10                  ((uint32_t)0x10000008)
#define I2C_FLAG_BTF                    ((uint32_t)0x10000004)
#define I2C_FLAG_ADDR                   ((uint32_t)0x10000002)
#define I2C_FLAG_SB                     ((uint32_t)0x10000001)


void touch_init();
void time_out();
uint8_t  I2C_WriteDeviceRegister(uint8_t RegisterAddr, uint8_t RegisterValue);
uint8_t  I2C_ReadDeviceRegister(uint8_t RegisterAddr);
uint16_t I2C_ReadDataBuffer(uint32_t RegisterAddr);
void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState);
FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);
void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction);












#endif //APP_TOUCH_H
