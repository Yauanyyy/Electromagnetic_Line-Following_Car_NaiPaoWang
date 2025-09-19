
#include "zf_uart.h"
#include "ano_dt.h"

//使用的UART口
extern UARTN_enum UART_Using;

//数据拆分宏定义，在发送大于1字节的数据类型时，比如int16、float等，需要把数据拆分成单独字节进行发送
#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)	  ) )     //!< uint32_t 数据拆分 byte0  
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )     //!< uint32_t 数据拆分 byte1  
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )     //!< uint32_t 数据拆分 byte2  
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )     //!< uint32_t 数据拆分 byte3  

//发送数据缓存
unsigned char data_to_send[50];

//向上位机发送发送8个int16_t数据
void ano_send (short data1, short data2, short data3, short data4, short data5, short data6, short data7, short data8)
{
	unsigned char  _cnt = 0;
	unsigned char  sum = 0, i = 0;

	data_to_send[_cnt++] = 0xAA;      //匿名协议帧头  0xAAAA
	data_to_send[_cnt++] = 0xAA;
	data_to_send[_cnt++] = 0xF1;      //使用用户协议帧0xF1
	data_to_send[_cnt++] = 16;        //8个int16_t 长度 16个字节

	data_to_send[_cnt++] = BYTE0 (data1);
	data_to_send[_cnt++] = BYTE1 (data1);

	data_to_send[_cnt++] = BYTE0 (data2);
	data_to_send[_cnt++] = BYTE1 (data2);

	data_to_send[_cnt++] = BYTE0 (data3);
	data_to_send[_cnt++] = BYTE1 (data3);

	data_to_send[_cnt++] = BYTE0 (data4);
	data_to_send[_cnt++] = BYTE1 (data4);

	data_to_send[_cnt++] = BYTE0 (data5);
	data_to_send[_cnt++] = BYTE1 (data5);

	data_to_send[_cnt++] = BYTE0 (data6);
	data_to_send[_cnt++] = BYTE1 (data6);

	data_to_send[_cnt++] = BYTE0 (data7);
	data_to_send[_cnt++] = BYTE1 (data7);

	data_to_send[_cnt++] = BYTE0 (data8);
	data_to_send[_cnt++] = BYTE1 (data8);

	sum = 0;
	for (i = 0; i < _cnt; i++)
	{
		sum += data_to_send[i];
	}

	data_to_send[_cnt++] = sum;

	for (i = 0; i < 21; i++)
	{
		uart_putchar (UART_Using, data_to_send[i]);
	}

}
