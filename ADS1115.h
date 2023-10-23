#ifndef ADS1115_H_
#define ADS1115_H_

#include "stdint.h"
#include "stm32g0xx_hal.h"


/*             ______________
							|   _          |
			 addr---| 1|_|      10 |---scl
							|              |
	alert/rdy---| 2          9 |---sda
							|              |
			  gnd---| 3          8 |---vdd
							|              |
				in0---| 4          7 |---in3
							|              |
				in1---|	5					 6 |---in2
							|______________|


*/
/*================Device I2C address===============*/ 

#define ADS1115_I2C_ADDR_GND 			0x48<<1 
#define ADS1115_I2C_ADDR_VCC 			0x49<<1 
#define ADS1115_I2C_ADDR_SDA 			0x4A<<1 
#define ADS1115_I2C_ADDR_SCL 			0x4B<<1 


#define ADS1115_ADDRESS						ADS1115_I2C_ADDR_GND
#define ADS1115_WRITE_ADDR				ADS1115_ADDRESS|0x00
#define ADS1115_READ_ADDR					ADS1115_ADDRESS|0x01

/*================Pointer Register===============*/

#define ADS1115_REG_CONVERSION		0x00
#define ADS1115_REG_CONFIG				0x01
#define ADS1115_REG_LO_THRESH			0x02
#define ADS1115_REG_HI_THRESH			0x03

/*================Config Register===============*/
/*
Цей біт визначає робочий стан пристрою. ОС може бути записана лише у вимкненому стані та не має ефекту, коли триває перетворення.

When writing:
0 : No effect
1 : Start a single conversion (when in power-down state)
When reading:
0 : Device is currently performing a conversion
1 : Device is not currently performing a conversion
*/
/*Bit[15]*/
#define ADS1115_OS_OPERATIONAL_STATUS						0x0000	//No effect
#define ADS1115_OS_SINGLE_CONV_START						0x8000	//begin single conversion

//Input multiplexer configuration (ADS1115 only)
/*Bit[14:12]*/
#define ADS1115_MUX_DIFF_01						0x0000	//AINp-AIN0,AINn-AIN1(default)
#define ADS1115_MUX_DIFF_03						0x1000	//AINp-AIN0,AINn-AIN3
#define ADS1115_MUX_DIFF_13						0x2000	//AINp-AIN1,AINn-AIN3
#define ADS1115_MUX_DIFF_23						0x3000	//AINp-AIN2,AINn-AIN3
#define ADS1115_MUX_CHANNEL_0					0x4000	//AINp-AIN0,AINn-GND
#define ADS1115_MUX_CHANNEL_1					0x5000	//AINp-AIN1,AINn-GND
#define ADS1115_MUX_CHANNEL_2					0x6000	//AINp-AIN2,AINn-GND
#define ADS1115_MUX_CHANNEL_3					0x7000	//AINp-AIN3,AINn-GND
/*Bit[11:9]*/
#define ADS1115_PGA_6144						0x0000	//FSR = ±6.144 V
#define ADS1115_PGA_4096						0x0200	//FSR = ±4.096 V
#define ADS1115_PGA_2048						0x0400	//FSR = ±2.048 V (default)
#define ADS1115_PGA_1024						0x0600	//FSR = ±1.024 V
#define ADS1115_PGA_512							0x0800	//FSR = ±0.512 V
#define ADS1115_PGA_256							0x0A00	//FSR = ±0.256 V
/*Bit[8]*/
#define ADS1115_MODE_CONT_CONVERTION			0x0000 //Continuous-conversion mode
#define ADS1115_MODE_SINGLE_CONVERTION			0x0100 // Single-shot mode or power-down state (default)
/*Bit[7:5]*/
#define ADS1115_DATA_RATE_8						0x0000	//8 SPS
#define ADS1115_DATA_RATE_16					0x0020	//16 SPS
#define ADS1115_DATA_RATE_32					0x0040	//32 SPS
#define ADS1115_DATA_RATE_64					0x0060	//64 SPS
#define ADS1115_DATA_RATE_128					0x0080	//128 SPS (default)
#define ADS1115_DATA_RATE_250					0x00A0	//250 SPS
#define ADS1115_DATA_RATE_475					0x00C0	//475 SPS
#define ADS1115_DATA_RATE_860					0x00E0	//860 SPS

//Comparator mode (ADS1114 and ADS1115 only)
/*Bit[4]*/
#define ADS1115_COMP_MODE_0						0x0000	//Traditional comparator (default)
#define ADS1115_COMP_MODE_1						0x0010	//Window comparator

//Comparator polarity (ADS1114 and ADS1115 only)
/*Bit[3]*/
#define ADS1115_COMP_POL_0						0x0000	// Active low (default)
#define ADS1115_COMP_POL_1						0x0008	// Active high

//Latching comparator (ADS1114 and ADS1115 only)
/*
Цей біт контролює, чи контакт ALERT/RDY фіксується після встановлення чи очищається після перетворення в межах верхнього та нижнього порогових значень. 
Цей біт не виконує функції на ADS1113.
*/
/*Bit[2]*/
#define ADS1115_COMP_LATCH_0					0x0000	//: Nonlatching comparator . Вивід ALERT/RDY не фіксується, коли встановлено (за замовчуванням).
/*
ALERT/RDY залишається заблокованим, доки дані перетворення не будуть прочитані або відповідна відповідь SMBus попередження не буде надіслана хостом.
Пристрій відповідає своєю адресою, і це найнижча адреса, яка на даний момент підтримує лінію шини ALERT/RDY.
*/
#define ADS1115_COMP_LATCH_1					0x0004	//Latching comparator. 

//Comparator queue and disable (ADS1114 and ADS1115 only)
/*
Ці біти виконують дві функції. 
Якщо встановлено значення 11, компаратор вимикається, а пін ALERT/RDY встановлюється у стан високого опору. 
Якщо встановлено будь-яке інше значення, пін ALERT/RDY і функція порівняння вмикаються, і встановлене значення визначає кількість послідовних перетворень, 
що перевищують верхнє або нижнє порогове значення, необхідне перед встановленням піна ALERT/RDY. Ці біти не функціонують на ADS1113.
*/
/*Bit[1:0]*/
#define ADS1115_COMP_QUE_0						0x0000	//Assert after one conversion
#define ADS1115_COMP_QUE_1						0x0001	//Assert after two conversions
#define ADS1115_COMP_QUE_2						0x0002	//Assert after four conversions
#define ADS1115_COMP_QUE_3						0x0003	//Disable comparator and set ALERT/RDY pin to high-impedance (default)

typedef enum{

	Single_channel_0,
	Single_channel_1,
	Single_channel_2,
	Single_channel_3,
	Differential_channel_01,
	Differential_channel_03,
	Differential_channel_13,
	Differential_channel_23,


}select_channel;




typedef struct{
	
	uint16_t OS;
	uint16_t MUX;
	uint16_t PGA;
	uint16_t MODE;
	uint16_t DataRate;
	uint16_t COMP_MODE;
	uint16_t COMP_POLARITY;
	uint16_t COMP_LATCH;
	uint16_t COMP_QUE;
	
}ADS1115_InitTypeDefine;

extern uint16_t DataRaw[4];


#define ADS1115_SCL_CLR() HAL_GPIO_WritePin(SOFT_SCL_GPIO_Port, SOFT_SCL_Pin, GPIO_PIN_RESET)
#define ADS1115_SCL_SET() HAL_GPIO_WritePin(SOFT_SCL_GPIO_Port, SOFT_SCL_Pin, GPIO_PIN_SET)

#define ADS1115_SDA_IN() GPIO_InitStruct.Pin = SOFT_SDA_Pin; GPIO_InitStruct.Mode = GPIO_MODE_INPUT; GPIO_InitStruct.Pull = GPIO_PULLUP; GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; HAL_GPIO_Init(SOFT_SDA_GPIO_Port,&GPIO_InitStruct)
#define ADS1115_SDA_GET() HAL_GPIO_ReadPin(SOFT_SDA_GPIO_Port, SOFT_SDA_Pin)

#define ADS1115_SDA_OUT() GPIO_InitStruct.Pin = SOFT_SDA_Pin; GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; GPIO_InitStruct.Pull = GPIO_PULLUP; GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; HAL_GPIO_Init(SOFT_SDA_GPIO_Port,&GPIO_InitStruct)
#define ADS1115_SDA_CLR() HAL_GPIO_WritePin(SOFT_SDA_GPIO_Port, SOFT_SDA_Pin, GPIO_PIN_RESET)
#define ADS1115_SDA_SET() HAL_GPIO_WritePin(SOFT_SDA_GPIO_Port, SOFT_SDA_Pin, GPIO_PIN_SET)



void Init(void);
uint8_t ADS1115_Config(ADS1115_InitTypeDefine	*ADS1115_InitStruct);

void ADS1115_Settings_1(void);
void ADS1115_Settings_2(void);
void ADS1115_Settings_Differential_Inputs(void);

uint8_t ADS1115_ReadRawData(int16_t *data);
void ADS1115_Select_channel(select_channel select_ch);
float ADS1115_Raw_Data_to_Volt(int16_t data);

float ADS1115_Get_Volt(void);
float ADS1115_Get_Avarage_Volt(uint16_t num);

void ADS1115_Refresh_All_Channel(void);

#endif
