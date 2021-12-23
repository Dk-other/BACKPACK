#ifndef _BSP_I2C_H
#define _BSP_I2C_H

#include <inttypes.h>

#define I2C_WR	0		/* 写控制bit */
#define I2C_RD	1		/* 读控制bit */
#define u8 uint8_t


/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */
#define GPIO_PORT_I2C	GPIOB			/* GPIO端口 */
#define I2C_SCL_PIN		GPIO_PIN_6			/* 连接到SCL时钟线的GPIO */
#define I2C_SDA_PIN		GPIO_PIN_7		/* 连接到SDA数据线的GPIO */


#define I2C_SCL_1()  HAL_GPIO_WritePin(GPIO_PORT_I2C, I2C_SCL_PIN, GPIO_PIN_SET)		/* SCL = 1 */
#define I2C_SCL_0()  HAL_GPIO_WritePin(GPIO_PORT_I2C, I2C_SCL_PIN, GPIO_PIN_RESET)		/* SCL = 0 */

#define I2C_SDA_1()  HAL_GPIO_WritePin(GPIO_PORT_I2C, I2C_SDA_PIN, GPIO_PIN_SET)			/* SDA = 1 */
#define I2C_SDA_0()  HAL_GPIO_WritePin(GPIO_PORT_I2C, I2C_SDA_PIN, GPIO_PIN_RESET)		/* SDA = 0 */

#define I2C_SDA_READ()  HAL_GPIO_ReadPin(GPIO_PORT_I2C, I2C_SDA_PIN)	/* 读SDA口线状态 */

void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(uint8_t ack);
uint8_t I2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
uint8_t i2c_CheckDevice(uint8_t _Address);
void i2c_GPIO_Config(void);

#endif
