#include "ADS1115.h"
#include "main.h"
#include "stm32g0xx.h"

static ADS1115_InitTypeDefine	ADS1115_InitType;//Структура налаштувань
select_channel select_ch;

uint16_t DataRaw[4] = {0};


__STATIC_INLINE void Delay_us (uint32_t __IO us) //Функція затримки в микросекундах
{
us *=(SystemCoreClock/1000000)/5;
while(us--);
}

void ADS1115_Start(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	ADS1115_SDA_OUT();
	
	ADS1115_SDA_SET();
	ADS1115_SCL_SET();
	Delay_us(5);
	ADS1115_SDA_CLR();
	Delay_us(5);
	ADS1115_SCL_CLR();
	
}

void ADS1115_Stop(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	ADS1115_SDA_OUT();
	
	ADS1115_SCL_CLR();
	ADS1115_SDA_CLR();
	Delay_us(5);
	ADS1115_SCL_SET();
	ADS1115_SDA_SET();
	Delay_us(5);
}


void ADS1115_Ack(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	ADS1115_SDA_OUT();
	
	
	ADS1115_SCL_CLR();
	ADS1115_SDA_CLR();
	Delay_us(5);
	ADS1115_SCL_SET();
	Delay_us(5);
	ADS1115_SCL_CLR();	
}


void ADS1115_NAck(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	ADS1115_SDA_OUT();
	
	
	ADS1115_SCL_CLR();
	ADS1115_SDA_SET();
	Delay_us(5);
	ADS1115_SCL_SET();
	Delay_us(5);
	ADS1115_SCL_CLR();
	
}


unsigned char ADS1115_WaitAck(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	unsigned char t = 0;
	
	ADS1115_SDA_IN();
	ADS1115_SDA_SET();
	Delay_us(5);
	ADS1115_SCL_SET();
	Delay_us(5);
	
	while(ADS1115_SDA_GET())
	{
		t++;
		if(t>250)
		{
			ADS1115_Stop();
			return 1;
		}
	}
	ADS1115_SCL_CLR();
	return 0;
}

void ADS1115_SendByte(unsigned char byte)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	unsigned char BitCnt;
	
	ADS1115_SDA_OUT();
	ADS1115_SCL_CLR();
	for(BitCnt = 0; BitCnt < 8; BitCnt++)
	{
		if(byte & 0x80) ADS1115_SDA_SET();
		else ADS1115_SDA_CLR();
		byte <<= 1;
		Delay_us(5);
		ADS1115_SCL_SET();
		Delay_us(5);
		ADS1115_SCL_CLR();
		Delay_us(5);
	}
	
}


unsigned char ADS1115_ReceiveByte(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	unsigned char BitCnt;
	unsigned char retc;
	
	retc = 0;
	
	ADS1115_SDA_IN();
	for(BitCnt = 0; BitCnt < 8; BitCnt++)
	{
		ADS1115_SCL_CLR();
		Delay_us(5);
		ADS1115_SCL_SET();
		retc = retc<<1;
		if(ADS1115_SDA_GET()) retc++;
		Delay_us(5);
	}
	return retc;
}


void Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	ADS1115_SDA_SET();
	ADS1115_SCL_SET();

}
/*
Завантаження налаштувань микросхеми
*/
uint8_t ADS1115_Config(ADS1115_InitTypeDefine	*ADS1115_InitStruct)
{
	uint16_t config;
	uint8_t write_buff[2];
	
	config = ADS1115_InitStruct->OS + ADS1115_InitStruct->MUX + ADS1115_InitStruct->PGA + ADS1115_InitStruct->MODE
	+ ADS1115_InitStruct->DataRate + ADS1115_InitStruct->COMP_MODE + ADS1115_InitStruct->COMP_POLARITY + ADS1115_InitStruct->COMP_LATCH
	+ADS1115_InitStruct->COMP_QUE;
	
	write_buff[0] = (unsigned char) ((config >> 8)& 0xff);
	write_buff[1] = (unsigned char) (config &0xff);
	
	
	ADS1115_Start();
	ADS1115_SendByte(ADS1115_WRITE_ADDR);
	if(ADS1115_WaitAck() == 1)
		return 0;
		
	
	ADS1115_SendByte(ADS1115_REG_CONFIG);
	if(ADS1115_WaitAck() == 1)
		return 0;
		
	
	ADS1115_SendByte(write_buff[0]);
	if(ADS1115_WaitAck() == 1)
		return 0;
		
	
	ADS1115_SendByte(write_buff[1]);
	if(ADS1115_WaitAck() == 1)
		return 0;
		
	
	ADS1115_Stop();
	
	return 1;
	
}
/*
	Одиночне перетворення
*/

void ADS1115_Settings_1(void)
{
	ADS1115_InitType.COMP_LATCH = ADS1115_COMP_LATCH_0;
	ADS1115_InitType.COMP_MODE = ADS1115_COMP_MODE_0;
	ADS1115_InitType.COMP_POLARITY = ADS1115_COMP_POL_0;
	ADS1115_InitType.DataRate = ADS1115_DATA_RATE_475;
	ADS1115_InitType.MODE = ADS1115_MODE_SINGLE_CONVERTION;
	ADS1115_InitType.MUX = ADS1115_MUX_CHANNEL_0;
	ADS1115_InitType.OS = ADS1115_OS_SINGLE_CONV_START;
	ADS1115_InitType.PGA = ADS1115_PGA_4096;
	
	ADS1115_Config(&ADS1115_InitType);

}
/*
Тривале перетворення
*/
void ADS1115_Settings_2(void)
{
	ADS1115_InitType.COMP_LATCH = ADS1115_COMP_LATCH_0;
	ADS1115_InitType.COMP_MODE = ADS1115_COMP_MODE_0;
	ADS1115_InitType.COMP_POLARITY = ADS1115_COMP_POL_0;
	ADS1115_InitType.DataRate = ADS1115_DATA_RATE_250;
	ADS1115_InitType.MODE = ADS1115_MODE_CONT_CONVERTION;
	ADS1115_InitType.MUX = ADS1115_MUX_CHANNEL_1;
	ADS1115_InitType.OS = ADS1115_OS_SINGLE_CONV_START;
	ADS1115_InitType.PGA = ADS1115_PGA_4096;
	
	ADS1115_Config(&ADS1115_InitType);

}
/*
двоканальне(диференційне) перетворення
*/
void ADS1115_Settings_Differential_Inputs(void)
{
	ADS1115_InitType.COMP_LATCH = ADS1115_COMP_LATCH_0;
	ADS1115_InitType.COMP_MODE = ADS1115_COMP_MODE_0;
	ADS1115_InitType.COMP_POLARITY = ADS1115_COMP_POL_0;
	ADS1115_InitType.DataRate = ADS1115_DATA_RATE_250;
	ADS1115_InitType.MODE = ADS1115_MODE_CONT_CONVERTION;
	ADS1115_InitType.MUX = ADS1115_MUX_DIFF_01;
	ADS1115_InitType.OS = ADS1115_OS_SINGLE_CONV_START;
	ADS1115_InitType.PGA = ADS1115_PGA_256;
	
	ADS1115_Config(&ADS1115_InitType);
	

}

/*
Прочитати вхідні дані поточного каналу
*/
uint8_t ADS1115_ReadRawData(int16_t *data)
{
		unsigned char res[2];
	
	
	ADS1115_Start();
	
	ADS1115_SendByte(ADS1115_WRITE_ADDR);
	
	if(ADS1115_WaitAck() == 1)
		return 0;
	ADS1115_SendByte(ADS1115_REG_CONVERSION);
	if(ADS1115_WaitAck() == 1)
		return 0;
	
	ADS1115_Stop();
	Delay_us(10);
	ADS1115_Start();
	
	ADS1115_SendByte(ADS1115_READ_ADDR);
	if(ADS1115_WaitAck() == 1)
		return 0;
	
	res[0] = ADS1115_ReceiveByte();
	ADS1115_WaitAck();
	res[1] = ADS1115_ReceiveByte();
	ADS1115_NAck();
	ADS1115_Stop();
	
	*data = (int16_t) (((res[0] << 8) & 0xff00) | (res[1] &0xff));
	
	return 1;
}

/*
Вибір каналу
*/

void ADS1115_Select_channel(select_channel select_ch)
{
	switch (select_ch)
  {
  	case Single_channel_0:
			ADS1115_InitType.MUX = ADS1115_MUX_CHANNEL_0;
  		break;
  	case Single_channel_1:
			ADS1115_InitType.MUX = ADS1115_MUX_CHANNEL_1;
  		break;
		case Single_channel_2:
			ADS1115_InitType.MUX = ADS1115_MUX_CHANNEL_2;
  		break;
		case Single_channel_3:
			ADS1115_InitType.MUX = ADS1115_MUX_CHANNEL_3;
  		break;
		case Differential_channel_01:
			ADS1115_InitType.MUX = ADS1115_MUX_DIFF_01;
  		break;
		case Differential_channel_03:
			ADS1115_InitType.MUX = ADS1115_MUX_DIFF_03;
  		break;
		case Differential_channel_13:
			ADS1115_InitType.MUX = ADS1115_MUX_DIFF_13;
  		break;
		case Differential_channel_23:
			ADS1115_InitType.MUX = ADS1115_MUX_DIFF_23;
  		break;
  	default:
  		break;
  }
	
	ADS1115_Config(&ADS1115_InitType);
	
}

/*
Перетворює необроблені дані вибірки в дані напруги,
розраховані на основі інформації про підсилення,
що міститься в структурі ADS1115_InitType
*/
float ADS1115_Raw_Data_to_Volt(int16_t data)
{
	float volt;
	
	switch (ADS1115_InitType.PGA)
  {
  	case ADS1115_PGA_256:
			volt = data * 0.0078125;
  		break;
  	case ADS1115_PGA_512:
			volt = data * 0.015625;
  		break;
		case ADS1115_PGA_1024:
			volt = data * 0.03125;
  		break;
		case ADS1115_PGA_2048:
			volt = data * 0.0625;
  		break;
		case ADS1115_PGA_4096:
			volt = data * 0.125;
  		break;
		case ADS1115_PGA_6144:
			volt = data * 0.1875;
  		break;
  	default:
			volt = 0;
  		break;
  }
	
	return volt;
}


/*
Отримати значення вибірки напруги поточного каналу
*/

float ADS1115_Get_Volt(void)
{
	int16_t data;
	ADS1115_ReadRawData(&data);
	return ADS1115_Raw_Data_to_Volt(data);
}

/*
Обчислює середнє значення напруги
...num: кількість середніх значень для розрахунку
*/
float ADS1115_Get_Avarage_Volt(uint16_t num)
{
	int32_t sum = 0;
	int16_t data;
	
	if(num == 0)
	{
		return ADS1115_Get_Volt();
	}
	
	for(uint16_t i = 0;i < num;i++)
	{
		ADS1115_ReadRawData(&data);
		sum += data;
	}
	
	return ADS1115_Raw_Data_to_Volt(sum/num);
	
}

/*
Оновити дані вибірки всіх каналів

Оскільки каналу потрібно чекати тривалий час,перш ніж дані стануть стабільними після перемикання каналу.
Під час виконання багатоканального заміру,затримка блокування та очікування завершення перемикання після перемикання каналів,займе занадто багато системного часу. Тому необхідно опитувати та збирати дані через таймер.
Після завершення кожного збору даних,перемикайте на наступній канал.

Виклик цієї функції оновлює значення DataRaw[4].

Цю функцію слід періодично викликати у обробнику переривання оновлення таймера

Наприклад:
Таймер оновлюється 10 раз на секунду
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM17)
	{
		ADS1115_Refresh_All_Channel();
	}
}

*/

void ADS1115_Refresh_All_Channel(void)
{
	static uint8_t ch = 0;
	int16_t adcDataTemp = 0;

	//під час перемикання каналів,можуть бути прочитані не вірні дані(сміття),тому перші два зчитування проводимо в холосту і відкидаємо
	ADS1115_ReadRawData(&adcDataTemp);
	ADS1115_ReadRawData(&adcDataTemp);
	
	//зчитуємо дані перетворення
	if(ADS1115_ReadRawData(&adcDataTemp) != 0)
	{
		DataRaw[ch] = adcDataTemp;
	}
	
	//у нас 4 канали
	ch++;
	
	if(ch > 4-1)
	{
		ch = 0;
	}
	//перемикаємося на наступний канал
	ADS1115_Select_channel(ch);
}


















