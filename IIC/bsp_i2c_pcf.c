#include "bsp_i2c_pcf.h"
#include "bsp_i2c.h"
#include <stdio.h>

extern timTypeDef tim1;

uint8_t Pcf_ReadBytes(uint8_t *_pReadBuf, uint8_t PCF8574_DX_ADDR)
{
	uint16_t m;
	/* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于 10ms
	CLK频率为200KHz时，查询次数为30次左右
	*/
	for ( m = 0; m < 1000; m++)
	{
		/* 第1步：发起I2C总线启动信号 */
		i2c_Start();
		/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
		i2c_SendByte(PCF8574_DX_ADDR | PCF8574T_I2C_RD);	/* 此处是写指令 */
		/* 第3步：发送一个时钟，判断器件是否正确应答 */
		if (i2c_WaitAck() == 0)
		{
			break;
		}
	}
	if (m  == 1000)
	{
		goto cmd_fail;	/* EEPROM器件写超时 */
	}
	/* 第9步：循环读取数据 */
	*_pReadBuf = ~(I2c_ReadByte());	/* 读1个字节 */
	i2c_Ack();
	*_pReadBuf = ~(I2c_ReadByte());	/* 读1个字节 */
	i2c_NAck();
	i2c_Stop();
	return 1;	/* 执行成功 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	i2c_Stop();
	return 0;
}


uint8_t Pcf_WriteBytes(uint8_t *_pWriteBuf, uint8_t PCF8574_DX_ADDR)
{
	uint16_t m;
	i2c_Stop();

	/* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于 10ms
	CLK频率为200KHz时，查询次数为30次左右
	*/
	for (m = 0; m < 1000; m++)
	{
		/* 第1步：发起I2C总线启动信号 */
		i2c_Start();
		/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
		i2c_SendByte(PCF8574_DX_ADDR | PCF8574T_I2C_WR);	/* 此处是写指令 */
		/* 第3步：发送一个时钟，判断器件是否正确应答 */
		if (i2c_WaitAck() == 0)
		{
			break;
		}
	}
	if (m  == 1000)
	{
		goto cmd_fail;	/* EEPROM器件写超时 */
	}
	/* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
	i2c_SendByte(*(uint8_t *)_pWriteBuf);
	/* 第5步：等待ACK */
	if (i2c_WaitAck() != 0)
	{
		goto cmd_fail;	/* PCF8574T无应答 */
	}
	/* 命令执行成功，发送I2C总线停止信号 */
	i2c_Stop();
	HAL_Delay(2);
	return 1;

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	i2c_Stop();
	return 0;
}
/***************************************************************************
 * @brief:将pcf8574t按照相应模式进行配置.
 * @param:DI16/DO16.
 * @ret:配置结束返回0，异常返回1.
 * **************************************************************************/
uint8_t Pcf_DxConfig (modleTypeDef *modle)
{
	uint8_t temp_buff[3]={0x00, 0xff, 0x00};
	/*iic端口配置*/
	i2c_GPIO_Config();

	/*PCF初始化结构体*/
	DX1_Struct.ADDR = PCF8574T_DX1_ADDR;
	DX2_Struct.ADDR = PCF8574T_DX2_ADDR;
	if (DI16 == *modle) {
		/*PCF初始化模式*/
		DX1_Struct.MODLE = DI;
		DX2_Struct.MODLE = DI;
		/*打开传感器供电*/
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
	}

	if (DX1_Struct.MODLE == DI) {
		temp_buff[2] = temp_buff[1];
		(1== Pcf_WriteBytes(&temp_buff[2], DX1_Struct.ADDR))?(DX1_Struct.DIAG = PCF_OK):(DX1_Struct.DIAG = PCF_ERROR);
		if (DX1_Struct.DIAG == PCF_OK) {
			Pcf_ReadBytes(&DX1_Struct.PORTDATA, DX1_Struct.ADDR);
		}
	}
	if (DX2_Struct.MODLE == DI) {
		temp_buff[2] = temp_buff[1];
		(1== Pcf_WriteBytes(&temp_buff[2], DX2_Struct.ADDR))?(DX2_Struct.DIAG = PCF_OK):(DX2_Struct.DIAG = PCF_ERROR);
		if (DX2_Struct.DIAG == PCF_OK) {
			Pcf_ReadBytes(&DX2_Struct.PORTDATA, DX2_Struct.ADDR);
		}
	}

	if ((DX1_Struct.DIAG == PCF_OK)&&(DX2_Struct.DIAG == PCF_OK)) {
		return 0;
	}
	else {
		return 1;
	}
}
/***************************************************************************
 * @brief:pcf诊断检测
 * @param:NULL.
 * @ret:NULL.
 * **************************************************************************/
void DX_DiagCheck (void)
{
	  if (DX1_Struct.DIAG == PCF_ERROR) {
		  printf("DX1诊断异常\r\n");
	  }
	  if (DX2_Struct.DIAG == PCF_ERROR) {
		  printf("DX2诊断异常\r\n");
	  }
}
/***************************************************************************
 * @brief:当产生中断进行执行，可以硬件和软件共同产生.
 * @param:中断标志位地址.
 * @ret:NULL.
 * **************************************************************************/
void DX_IntDeal (pcfintTypeDef *int_state)
{
	  if(PCFSET == *int_state) {
		  if ((DX1_Struct.DIAG == PCF_OK)&&(DX1_Struct.MODLE == DI)) {
			  (1== Pcf_ReadBytes(&DX1_Struct.PORTDATA, DX1_Struct.ADDR))?(DX1_Struct.DIAG = PCF_OK):(DX1_Struct.DIAG = PCF_ERROR);
		  }
		  if ((DX2_Struct.DIAG == PCF_OK)&&(DX2_Struct.MODLE == DI)) {
			  (1== Pcf_ReadBytes(&DX2_Struct.PORTDATA, DX2_Struct.ADDR))?(DX2_Struct.DIAG = PCF_OK):(DX2_Struct.DIAG = PCF_ERROR);
		  }

		  *int_state = PCFRESET;
		  tim1.PCF_TEST = __HAL_TIM_GetCounter(&htim1);
		  HAL_TIM_Base_Stop(&htim1);

		  if ((DX1_Struct.DIAG == PCF_OK)&&(DX2_Struct.DIAG == PCF_OK)) {
			  printf("用时：%#X\r\n", tim1.PCF_TEST);
			  printf("数据1:%#X\r\n", DX1_Struct.PORTDATA);
			  printf("数据2:%#X\r\n", DX2_Struct.PORTDATA);
		  }
	  }
}
