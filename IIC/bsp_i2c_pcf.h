#ifndef __I2C_PCF_H
#define	__I2C_PCF_H


#include "stm32f4xx_hal.h"
#include "main.h"
#include "tim.h"

#define PCF8574T_DX1_ADDR		  0x40
#define PCF8574T_DX2_ADDR		  0x48


#define PCF8574T_I2C_WR           0
#define PCF8574T_I2C_RD           1

struct DXSTRUCT
{
	uint8_t ADDR;      /*DX地址*/
	uint8_t MODLE;     /*DX模式*/
	uint8_t DIAG;      /*DX诊断*/
	uint8_t PORTDATA;  /*DX端口数据*/

}DX1_Struct, DX2_Struct;

enum DXDIAG
{
	PCF_OK = 0x0u,
	PCF_ERROR
};

enum MODLE
{
	DI = 0x0u,
	DO
};
uint8_t Pcf_ReadBytes(uint8_t *_pReadBuf, uint8_t PCF8574_DX_ADDR);
uint8_t Pcf_WriteBytes(uint8_t *_pWriteBuf, uint8_t PCF8574_DX_ADDR);
uint8_t Pcf_DxConfig (modleTypeDef *modle);
void DX_DiagCheck (void);
void DX_IntDeal (pcfintTypeDef *int_state);

#endif /* __I2C_PCF_H */
