#include"STC12C5A60S2.h" 
#include"74HC595.H"
#include"PWM.H"
#include<intrins.h> 

#define u8 unsigned char
#define u16 unsigned int
///////////AD宏定义
#define ADC_POWER 0x80 //AD电源控制
#define ADC_START 0x08 //AD转换控制
#define ADC_FLAG  0x10 //AD转换完成
#define Speed_0   0x00 //540 clk
#define Speed_1   0x20 //360 clk
#define Speed_2   0x40 //180 clk
#define Speed_3   0x60 //90  clk
#define ADC0      0x00 //P1.0
#define ADC1      0x01 //P1.1
#define ADC2      0x02 //P1.2
#define ADC3      0x03 //P1.3
#define ADC4      0x04 //P1.4
#define ADC5      0x05 //P1.5
#define ADC6      0x06 //P1.6
#define ADC7      0x07 //P1.7

#define PWM_level_1 0x00
#define PWM_level_2 32
#define PWM_level_3 172
#define PWM_level_4 0xff

sbit LED0=P4^4;
sbit LED1=P4^5;
sbit KEY0=P3^2;
sbit KEY1=P3^3;
sbit PWMpin= P1^3;
sbit ADpin=P1^0;

//cd4051 数据选择端
sbit m1=P3^5;//a
sbit m2=P3^6;//b
sbit m3=P3^7;//c
//sbit inh1=P3^7;直接接地 
sbit n1=P1^5;//d
sbit n2=P1^6;//e
sbit n3=P1^7;//f
sbit inh2=P1^2;//接地

u8 hang=0;
u8 lie =0;
bit temp1=0;
u8 temp2=0;
u8 temp3=0;
u8 k=0;

u16 add=0;
u8 n=0;

u8 PWM_level[]={PWM_level_1,PWM_level_2,PWM_level_3,PWM_level_4};
u8 pwmvalue= PWM_level_1;
u8 temp4=0;
u8 level=0x00;		//存储亮度等级

//存储每一位的AD结果，值为0或1，0代表不亮,1代表亮
u8 result[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//点阵显示的 行码 列码
u8 liema[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f}; //点阵逐行扫描 列码
//u8 hang[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //行码
u8 hangma[]={0x44,0x4e,0xe4,0x5f,0xe4,0x4e,0x44,0x5f};/*"桂",0*/

//1ms 延时程序
void delay1ms(void)  
{
   u16 a=760;//默认工作在12T模式下
   while(a--);
}

//nms 延时程序
void delaynms(u16 nms)  //max 65536
{
   while(nms--)
	 {
       delay1ms();
	 }
}
//4051选择行程序
void choose_h(u8 num)
{
switch(num)
			{ 
      
      case 0: m3=0;m2=1;m1=1;break; //选通第一行
      case 1: m3=0;m2=0;m1=0;break; //选通第二行
      case 2: m3=0;m2=0;m1=1;break; //选通第三行
			case 3: m3=0;m2=1;m1=0;break; //选通第四行
      case 4: m3=1;m2=0;m1=1;break; //选通第五行
      case 5: m3=1;m2=1;m1=1;break; //选通第六行
      case 6: m3=1;m2=1;m1=0;break; //选通第七行
			case 7:	m3=1;m2=0;m1=0;break; //选通第八行	
      default: break;
      }
}
//4051选通列程序
void choose_l(u8 num)
{
switch(num)
			{ 
      
      case 0: n3=0;n2=0;n1=0;break; //选通第一列
      case 1: n3=0;n2=0;n1=1;break; //选通第二列
      case 2: n3=0;n2=1;n1=0;break; //选通第三列
			case 3: n3=0;n2=1;n1=1;break; //选通第四列
      case 4: n3=1;n2=0;n1=0;break; //选通第五列
      case 5: n3=1;n2=0;n1=1;break; //选通第六列
      case 6: n3=1;n2=1;n1=0;break; //选通第七列
			case 7:	n3=1;n2=1;n1=1;break; //选通第八列	
      default: break;
      }
}


//***********************************//
//        按键  中断初始化  			 //
//***********************************//
void Intp_init(void)
{
	
		KEY0=1;
		KEY1=1;
    IT0 = 1;   //外部中断源0为低电平触发	按键0     0:下降沿触发	1:低电平触发
    EX0 = 1;    //允许外部中断
	//	IT1=1;			//允许外中断1为低电平触发	按键1
//		EX1=1;			//允许外部中断
    EA  = 1;    //开总中断	
}

//ad 初始化函数
void adinit(void)
{
		AUXR1     = 0x00;                 //转换结果高8位放在ADC_RES中,低2位放在ADC_RESL中
    P1ASF     = 0X01;                // 仅将P1.0作为模拟功能A/D使用
    ADC_RES   = 0x00;                   //结果清零
		//   ADC_RESL  = 0x00;										//低两位结果清零
		ADC_CONTR = ADC_POWER|Speed_2|ADC0;   //打开电源,180CLK周期转换,选择P1.0作输入
		//POWER SPEED1 SPEED0 ADC_FLAG   ADC_START CHS2 CHS1 CHS0 
		_nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();

}


//ad 转换程序
u8 getad(void)
{
    ADC_CONTR |= ADC_START;   //开始AD转换
	
		_nop_();
    _nop_();
		_nop_();
    _nop_();
    _nop_();
	
		while(!(ADC_CONTR&ADC_FLAG));//等待AD转换结束
		ADC_CONTR &=~ADC_FLAG	;//ADC_FLAG 软件置0

		_nop_();
    _nop_();
    _nop_();
		_nop_();
    _nop_();	
		return ADC_RES;

 //  EADC      = 1;                          //允许A/D转换中断
 //  EA        = 1;                          //开总中断
}

//ad取平均函数
u8 adaverage(void)
{
		u8 u=0;
		u16 adp=0x00;
	for(;u<10;u++)
	{
	adp=adp+getad();
	}
	return adp/10;
}

//亮与不亮判断函数
bit lianganpanduan(u8 zhi)
{
		if(zhi>0x60)
		return 1;
	else 
		return 0;

}

//等级判断函数
u8 levelpanduan(u8 zhi)
{
	if(zhi<0xb0)
		return 1;
	else 
		return 3;

}



//光阵列版扫描函数
void saomiao(void)
{
	hang=0;
	lie=0;
		for(;hang<=7;hang++)
	{
		lie=0;
		choose_h(hang);
		for(;lie<=7;lie++)
		{
			choose_l(lie);
			temp1=lianganpanduan(adaverage());
			temp2=(u8)temp1;
			temp3|=temp2<<(7-lie);
		}	
		result[hang]=temp3;
				temp3=0x00;
	
	}
}
//等级扫描函数
u8 levelsaomiao(void)
{
	hang=0;
	lie=0;
	n=0;
	add=0;
		for(;hang<=7;hang++)
	{
		lie=0;
		choose_h(hang);
		for(;lie<=7;lie++)
		{
			choose_l(lie);
			temp4=adaverage();
			if(lianganpanduan(temp4)==1)
			{n++;
			add+=temp4;
			}
			
		}	
	return add/n;	
	}	

}


void PWM_clock(u8 clock)
{ 
CMOD |= (clock<<1);
CL = 0x00;
CH = 0x00;
}

void PWM_start(u8 mat,u8 module,u8 mode) 
{
	CCAP0L = mat;
	CCAP0H = mat;   //模块0初始输出 占空因数为25%
	CCAP1L = mat;
	CCAP1H = mat;   //模块1初始输出 占空因数为25%
	
	if(module==0)
		 {
			switch(mode)
				{ 
		 case 0: CCAPM0 = 0X42;break; //模块0设置为8位PWM输出，无中断
		 case 1: CCAPM0 = 0X53;break; //模块0设置为8位PWM输出，下降沿产生中断
		 case 2: CCAPM0 = 0X63;break; //模块0设置为8位PWM输出，上升沿产生中断
		 case 3: CCAPM0 = 0X73;break; //模块0设置为8位PWM输出，跳变沿产生中断
		 default: break;
				}
			}
	else
  if(module==1)
     {
      switch(mode)
       { 
   case 0: CCAPM1 = 0X42;break; //模块1设置为8位PWM输出，无中断
   case 1: CCAPM1 = 0X53;break; //模块1设置为8位PWM输出，下降沿产生中断
   case 2: CCAPM1 = 0X63;break; //模块1设置为8位PWM输出，上升沿产生中断
   case 3: CCAPM1 = 0X73;break; //模块1设置为8位PWM输出，跳变沿产生中断
   default: break;
				}
		}
  else
	if(module==2)
    {
        switch(mode)
			{ 
      case 0: CCAPM0 = CCAPM1 = 0X42;break; //模块0和1设置为8位PWM输出，无中断
      case 1: CCAPM0 = CCAPM1 = 0X53;break; //模块0和1设置为8位PWM输出，下降沿产生中断
      case 2: CCAPM0 = CCAPM1 = 0X63;break; //模块0和1设置为8位PWM输出，上升沿产生中断
      case 3: CCAPM0 = CCAPM1 = 0X73;break; //模块0和1设置为8位PWM输出，跳变沿产生中断
      default: break;
      }
    }
	CR=1; //PCA计数器开始计数
		
}
	void main()
{

	P4SW=0XFF;			//把P4设置为普通IO口
	P4M1=0X01;
	P4M0=0X00;
	
	Intp_init();             //按键外中断0、1初始化
	adinit();								//ad 初始化
	
	inh2=0;					//cd4051 使能
	
	
	TMOD|=0x02; /* timer 0 mode 2: 8-Bit reload */ 
	TH0=0xff;
	TR0=1;     
	PWM_clock(2);      // PCA/PWM时钟源为 定时器0的溢出
	
	PWM_start(pwmvalue,2,0); // 模块0,设置为PWM输出,无中断,初始占空因素为25%
		
	while(1)
	{
	
	Ser_IN(liema[k]);
	Ser_IN(result[k]);	
	Par_OUT();
		
		k++;	
		if(k==8)
		k=0;		
	}
}

//******************************************//
//               A/D中断服务        		//
//******************************************//
// void AD_Service(void) interrupt 5
// {
//     ADC_CONTR &= !ADC_FLAG; //清标志
// 		ADC_CONTR &= !ADC_START;// 关闭AD转换
// 		_nop_();
//     _nop_();
// 		_nop_();
//     _nop_();
// 	
// 		value=ADC_RES*256+ADC_RESL;
//     ADC_RES   = 0x00;
//     ADC_RESL  = 0x00;
// 	if(value>200){
// 		PWM_start(0x40,2,0); // 模块0,设置为PWM输出,无中断,初始占空因素为25%
//    LED1=!LED1;}
// 	if(value<200){
// 		PWM_start(0x80,2,0); // 模块0,设置为PWM输出,无中断,初始占空因素为25%
//    LED2=!LED2;}

//     ADC_CONTR = ADC_POWER|Speed_2|ADC_START; //开始下一次转换   
//     _nop_();
//     _nop_();
//     _nop_();
//     _nop_();
// 	
// }

//按键0 中断函数
void INT0_Service(void) interrupt 0
{
	EX0=0;
  while(!KEY1);
	LED0=0;
	saomiao();
	level=levelpanduan(levelsaomiao());
	pwmvalue=PWM_level[level];
	PWM_start(pwmvalue,2,0);
	LED0=1;
	EX0=1;
	
    
}
// //按键1 中断函数
// void INT1_Service(void) interrupt 2
// {
// 	
// 	u8 lp=level;
// 	u8 kk=0;
// 	delaynms(30);
// 	EX1=0;
// //LED1=0;
// 	while(!KEY0)
// 	{
// 			Ser_IN(liema[kk]);
// 			switch(lp)
// 			{
// 				 case 0: Ser_IN(d0[kk]);	break; 
// 				 case 1: Ser_IN(d1[kk]);	break; 
// 				 case 2: Ser_IN(d2[kk]);	break; 
// 				 case 3: Ser_IN(d3[kk]);	break; 
// 				 case 4: Ser_IN(d4[kk]);	break; 
// 				 default: break;
// 			}
// 			Par_OUT();
// 			kk++;	
// 			if(kk==8)
// 			kk=0;	
// 			LED1=0;
// 	}
// 	EX1=1;
// //	LED1=1;
// }
