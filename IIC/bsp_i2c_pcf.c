#include "bsp_i2c_pcf.h"
#include "bsp_i2c.h"
#include <stdio.h>

extern timTypeDef tim1;

uint8_t Pcf_ReadBytes(uint8_t *_pReadBuf, uint8_t PCF8574_DX_ADDR)
{
	uint16_t m;
	/* ͨ���������Ӧ��ķ�ʽ���ж��ڲ�д�����Ƿ����, һ��С�� 10ms
	CLKƵ��Ϊ200KHzʱ����ѯ����Ϊ30������
	*/
	for ( m = 0; m < 1000; m++)
	{
		/* ��1��������I2C���������ź� */
		i2c_Start();
		/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
		i2c_SendByte(PCF8574_DX_ADDR | PCF8574T_I2C_RD);	/* �˴���дָ�� */
		/* ��3��������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
		if (i2c_WaitAck() == 0)
		{
			break;
		}
	}
	if (m  == 1000)
	{
		goto cmd_fail;	/* EEPROM����д��ʱ */
	}
	/* ��9����ѭ����ȡ���� */
	*_pReadBuf = ~(I2c_ReadByte());	/* ��1���ֽ� */
	i2c_Ack();
	*_pReadBuf = ~(I2c_ReadByte());	/* ��1���ֽ� */
	i2c_NAck();
	i2c_Stop();
	return 1;	/* ִ�гɹ� */

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	i2c_Stop();
	return 0;
}


uint8_t Pcf_WriteBytes(uint8_t *_pWriteBuf, uint8_t PCF8574_DX_ADDR)
{
	uint16_t m;
	i2c_Stop();

	/* ͨ���������Ӧ��ķ�ʽ���ж��ڲ�д�����Ƿ����, һ��С�� 10ms
	CLKƵ��Ϊ200KHzʱ����ѯ����Ϊ30������
	*/
	for (m = 0; m < 1000; m++)
	{
		/* ��1��������I2C���������ź� */
		i2c_Start();
		/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
		i2c_SendByte(PCF8574_DX_ADDR | PCF8574T_I2C_WR);	/* �˴���дָ�� */
		/* ��3��������һ��ʱ�ӣ��ж������Ƿ���ȷӦ�� */
		if (i2c_WaitAck() == 0)
		{
			break;
		}
	}
	if (m  == 1000)
	{
		goto cmd_fail;	/* EEPROM����д��ʱ */
	}
	/* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
	i2c_SendByte(*(uint8_t *)_pWriteBuf);
	/* ��5�����ȴ�ACK */
	if (i2c_WaitAck() != 0)
	{
		goto cmd_fail;	/* PCF8574T��Ӧ�� */
	}
	/* ����ִ�гɹ�������I2C����ֹͣ�ź� */
	i2c_Stop();
	HAL_Delay(2);
	return 1;

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	i2c_Stop();
	return 0;
}
/***************************************************************************
 * @brief:��pcf8574t������Ӧģʽ��������.
 * @param:DI16/DO16.
 * @ret:���ý�������0���쳣����1.
 * **************************************************************************/
uint8_t Pcf_DxConfig (modleTypeDef *modle)
{
	uint8_t temp_buff[3]={0x00, 0xff, 0x00};
	/*iic�˿�����*/
	i2c_GPIO_Config();

	/*PCF��ʼ���ṹ��*/
	DX1_Struct.ADDR = PCF8574T_DX1_ADDR;
	DX2_Struct.ADDR = PCF8574T_DX2_ADDR;
	if (DI16 == *modle) {
		/*PCF��ʼ��ģʽ*/
		DX1_Struct.MODLE = DI;
		DX2_Struct.MODLE = DI;
		/*�򿪴���������*/
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
 * @brief:pcf��ϼ��
 * @param:NULL.
 * @ret:NULL.
 * **************************************************************************/
void DX_DiagCheck (void)
{
	  if (DX1_Struct.DIAG == PCF_ERROR) {
		  printf("DX1����쳣\r\n");
	  }
	  if (DX2_Struct.DIAG == PCF_ERROR) {
		  printf("DX2����쳣\r\n");
	  }
}
/***************************************************************************
 * @brief:�������жϽ���ִ�У�����Ӳ���������ͬ����.
 * @param:�жϱ�־λ��ַ.
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
			  printf("��ʱ��%#X\r\n", tim1.PCF_TEST);
			  printf("����1:%#X\r\n", DX1_Struct.PORTDATA);
			  printf("����2:%#X\r\n", DX2_Struct.PORTDATA);
		  }
	  }
}
