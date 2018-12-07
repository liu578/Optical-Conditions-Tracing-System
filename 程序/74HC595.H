//Note: 74HC595驱动

//      __                   __
//Note: MR 主复位接电源正极, OE 使能端,输出有效接电源负极


/*--------------------------------------------*/
#ifndef __74HC595_H__
#define __74HC595_H__



/*--------------------------------------------*/
sbit SD	   = P2^0;	//串行数据输入
sbit ST_CK = P2^2;	//存储寄存器时钟输入
sbit SH_CK = P2^1;	//移位寄存器时钟输入

// sbit SCLK=P2^1;
// sbit RCLK=P2^2;
// sbit DATA=P2^0;


/*--------------------------------------------*/
//数码管断码和位码的定义
//unsigned char code seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e,0x00,0xff};
                          //0   1    2    3    4    5    6    7    8    9    a    b    c     d    e     f    8   无
//unsigned char code pos[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
                          //1    2    3   4    5    6     7   8


/*--------------------------------------------*/
//函数声明
void Ser_IN(unsigned char Data);	//串行数据输入
void Par_OUT(void);					//串行数据输出
//void Ser_Par(unsigned char Data);	//串行输入,并行输出



/*--------------------------------------------*/
//串行数据输入
void Ser_IN(unsigned char Data)
{
	unsigned char i;
	for(i = 0; i < 8; i++)
	{
		SH_CK = 0;			//CLOCK_MAX=100MHz
		SD = Data & 0x80;
		Data <<= 1;
		SH_CK = 1;
	}
}



/*--------------------------------------------*/
//并行数据输出
void Par_OUT(void)
{
	ST_CK = 0;
	ST_CK = 1;
}



/*--------------------------------------------*/
//串行输入,并行输出
/*void Ser_Par(unsigned char Data)
{
	Ser_IN(Data);
	Par_OUT();
}
*/


/*--------------------------------------------*/
#endif 