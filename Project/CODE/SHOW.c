/*******************************************************************************
* @file           : show.c
* @brief          : show.c file.
*                   通用上位机显示解决方案
											 基于串口透传实现
******************************************************************************

/$$                   /$$
| $$                  | $$
| $$        /$$$$$$  /$$$$$$         /$$$$$$/$$$$   /$$$$$$         /$$$$$$$  /$$$$$$   /$$$$$$
| $$       /$$__  $$|_  $$_/        | $$_  $$_  $$ /$$__  $$       /$$_____/ /$$__  $$ /$$__  $$
| $$      | $$$$$$$$  | $$          | $$ \ $$ \ $$| $$$$$$$$      |  $$$$$$ | $$$$$$$$| $$$$$$$$
| $$      | $$_____/  | $$ /$$      | $$ | $$ | $$| $$_____/       \____  $$| $$_____/| $$_____/
| $$$$$$$$|  $$$$$$$  |  $$$$/      | $$ | $$ | $$|  $$$$$$$       /$$$$$$$/|  $$$$$$$|  $$$$$$$
|________/ \_______/   \___/        |__/ |__/ |__/ \_______/      |_______/  \_______/ \_______/


																										____           _______     _______ _    _
																									 |  _ \         / ____\ \   / / ____| |  | |
																									 | |_) |_   _  | (___  \ \_/ / (___ | |  | |
																									 |  _ <| | | |  \___ \  \   / \___ \| |  | |
																									 | |_) | |_| |  ____) |  | |  ____) | |__| |
																									 |____/ \__, | |_____/   |_| |_____/ \____/


******************************************************************************
*/

//////////////////////////////////////////////////////////////////////////////
///																	文件使用说明
///								如果是UART发送，直接调用对应的函数即可
///								如果是TCP发送，每次请固定发送1000个char数据包长度
///								TCP发送分包原因，请根据各发送函数返回的使用字节数在最后补充缺失的字节数
///															否则上位机不进行解析
///
//////////////////////////////////////////////////////////////////////////////
///							变量名称说明
/// Bin_image：储存二值化后图像的二维数组地址
/// 如果send_int发送的变量名称为lspeed，对应显示左边红条表示左轮速度百分比（0-100）
/// rspeed右轮速度，servo舵机打角(默认90)
///
///
///UART示例用法
/*
 while (1)
{
	image_send(Bin_Image);//发送的图像数据
	send_int("hello",1);//发送需要显示的变量，名称随意值随意
	send_int("lspeed",50);//发送左轮速度百分比 0-100
	send_int("rspeed",50);//发送右轮速度百分比 0-100
	send_int("servo",90);//发送舵机打角，0-180
}

*/


///TCP示例用法
/*
  while (1)
{

	length=1000;//数据包长度

	length-=image_send(Width,Length,Bin_Image);//减去发送的数据长度

	send_zero(length);//结尾补零
}
*/
///////////////////////////////////////////////////////////////////////////////






//////////////////////////////////////////////////////////////////////////////

#include "SHOW.h"
#include "globals_ex.h"

char head[2] = {254, 239}; //图像帧头
char tail[2] = {239, 254};
char num[30] = {'N', 'u', 'm'};
char zero = 0;
char uart_buff[15];
long USART0_RX_STA;       //接收状态标记
uint8 UART_Rec_num;//接收计数
uint8 Uart_Rec_Flag;//接收成功标志位
//extern uint8 Stop_Flag;//停车标志位


///输入：发送的变量名称，名称长度，值
///输出：使用的字节数 sned("bianliang1",a)
///"Num5helloi"+Value
int	send_int(char name[], int Value)
{
    int i = 0;
    int	length;
    char *tmp;
    //获取输入名称长度
    while(name[i] != '\0')
        i++;

    length = i;

    tmp = (char *)(&i);

    num[3] = (char)length;
    for(i = 0; i < length; i++)
    {
        num[4 + i] = name[i];
    }
    num[length + 4] = 'i';
    i = Value;
    num[length + 5] = tmp[0];
    num[length + 6] = tmp[1];
    num[length + 7] = tmp[2];
    num[length + 8] = tmp[3];

    My_putbuff(num, length + 9); //发送数据
    return	length + 9;
}


//TCP协议补零用，串口发送可不管
void send_zero(int num)
{
    for(; num > 0; num--)
        My_putchar(zero);//补零

}

//Vofa显示数据用
void send_float(float value)
{
    char *temp;
    float send = value;
    temp = (char *)&send; //强制类型转换
    My_putbuff(temp, 4); //发送数据
}

void My_putbuff(char *buff, uint32 len)
{
    uint8 temp;
    while(len)
    {
        temp = *buff;
        My_putchar(temp);
        len--;
        buff++;
    }
}



void UART_callback(uint8 buff)
{
    int32 *Rec;
    uint8 temp[4];//高低位转换
    uart_buff[USART0_RX_STA] = buff; //储存接收字节

    USART0_RX_STA++;//计数

    //帧头校验,错误则重新开始
    if(1 == USART0_RX_STA && uart_buff[0] != 'N')
    {
        UART_Rec_num = 0;
        USART0_RX_STA = 0;
    }
    if(2 == USART0_RX_STA && uart_buff[1] != 'u')
    {
        UART_Rec_num = 0;
        USART0_RX_STA = 0;
    }
    if(3 == USART0_RX_STA && uart_buff[2] != 'm')
    {
        UART_Rec_num = 0;
        USART0_RX_STA = 0;
    }

    if(10 == USART0_RX_STA)//理论上接收完3+1+4+2=10个字节,Num+编号+4+\r\n
    {
        USART0_RX_STA = 0;
        Uart_Rec_Flag = 1;
    }

    if(Uart_Rec_Flag)//如果接收成功
    {
        Uart_Rec_Flag = 0; //接收完成后清除标志
        //高低位转换,低位变成高位
        temp[0] = uart_buff[7];
        temp[1] = uart_buff[6];
        temp[2] = uart_buff[5];
        temp[3] = uart_buff[4];

        Rec = (int *)&temp[0];
        Value_Refresh(uart_buff[3], *Rec); //更新数据
    }
}


//更新上位机接收数据
void Value_Refresh(char num, int32 Value)
{
    extern int32 Rec_Int32;
    extern float Rec_Float;
    union Rec_Value //定义一个联合体用于int32和float转换
    {
        int32 int_value;
        float float_value;
    } Rec;

    Rec.int_value = Value; //填入数据,可根据发送数据取出int32或float数据

    switch (num)
    {
    ////////////////////////////
    //前10个为整数数据情况,20到30位浮点数据情况
    ////////////////////////////
    case 0:
        //改为你想赋值给谁
        //Rec_Int32=Value;//进行赋值,sudu2
        //if(Value > 0)
    {
        //Speed_L = Value;
        //Speed_R = Value;
    }
        //else
    {
        //Speed_L = 10;
        //Speed_R = 10;
    }
    break;

    case 1:
        //Send_Value[1]=Value;//改为你想赋值给谁
        break;

    //浮点数
    case 10://左边Rec_Float为想要赋值的对象,右边固定
        // A_Servo = Rec.float_value;//浮点数据转换
        break;

    case 11://左边Rec_Float为想要赋值的对象,右边固定
        // B_Servo = Rec.float_value;
        break;

    case 12://左边Rec_Float为想要赋值的对象,右边固定
        // C_Servo = Rec.float_value;
        break;

    case 13://左边Rec_Float为想要赋值的对象,右边固定
        // kp_servo = Rec.float_value;
        break;

    case 14://左边Rec_Float为想要赋值的对象,右边固定
        // kd_servo = Rec.float_value;
        break;

    case 15://左边Rec_Float为想要赋值的对象,右边固定
        //ki_motor = Rec.float_value;
        break;

    case 16://左边Rec_Float为想要赋值的对象,右边固定
        //kp_motor = Rec.float_value;
        break;


    case 30:
        // Stop_Flag=(short)Value;//停车/发车标志位,1停车
        break;

    default ://匹配失败
        // Stop_Flag = 1;
        break;
    }

}


//待续
