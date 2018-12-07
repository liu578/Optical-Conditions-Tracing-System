#include"STC12C5A60S2.h" 
#include"74HC595.H"
#include"PWM.H"
#include<intrins.h> 

#define u8 unsigned char
#define u16 unsigned int
///////////AD�궨��
#define ADC_POWER 0x80 //AD��Դ����
#define ADC_START 0x08 //ADת������
#define ADC_FLAG  0x10 //ADת�����
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

//cd4051 ����ѡ���
sbit m1=P3^5;//a
sbit m2=P3^6;//b
sbit m3=P3^7;//c
//sbit inh1=P3^7;ֱ�ӽӵ� 
sbit n1=P1^5;//d
sbit n2=P1^6;//e
sbit n3=P1^7;//f
sbit inh2=P1^2;//�ӵ�

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
u8 level=0x00;		//�洢���ȵȼ�

//�洢ÿһλ��AD�����ֵΪ0��1��0������,1������
u8 result[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//������ʾ�� ���� ����
u8 liema[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f}; //��������ɨ�� ����
//u8 hang[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //����
u8 hangma[]={0x44,0x4e,0xe4,0x5f,0xe4,0x4e,0x44,0x5f};/*"��",0*/

//1ms ��ʱ����
void delay1ms(void)  
{
   u16 a=760;//Ĭ�Ϲ�����12Tģʽ��
   while(a--);
}

//nms ��ʱ����
void delaynms(u16 nms)  //max 65536
{
   while(nms--)
	 {
       delay1ms();
	 }
}
//4051ѡ���г���
void choose_h(u8 num)
{
switch(num)
			{ 
      
      case 0: m3=0;m2=1;m1=1;break; //ѡͨ��һ��
      case 1: m3=0;m2=0;m1=0;break; //ѡͨ�ڶ���
      case 2: m3=0;m2=0;m1=1;break; //ѡͨ������
			case 3: m3=0;m2=1;m1=0;break; //ѡͨ������
      case 4: m3=1;m2=0;m1=1;break; //ѡͨ������
      case 5: m3=1;m2=1;m1=1;break; //ѡͨ������
      case 6: m3=1;m2=1;m1=0;break; //ѡͨ������
			case 7:	m3=1;m2=0;m1=0;break; //ѡͨ�ڰ���	
      default: break;
      }
}
//4051ѡͨ�г���
void choose_l(u8 num)
{
switch(num)
			{ 
      
      case 0: n3=0;n2=0;n1=0;break; //ѡͨ��һ��
      case 1: n3=0;n2=0;n1=1;break; //ѡͨ�ڶ���
      case 2: n3=0;n2=1;n1=0;break; //ѡͨ������
			case 3: n3=0;n2=1;n1=1;break; //ѡͨ������
      case 4: n3=1;n2=0;n1=0;break; //ѡͨ������
      case 5: n3=1;n2=0;n1=1;break; //ѡͨ������
      case 6: n3=1;n2=1;n1=0;break; //ѡͨ������
			case 7:	n3=1;n2=1;n1=1;break; //ѡͨ�ڰ���	
      default: break;
      }
}


//***********************************//
//        ����  �жϳ�ʼ��  			 //
//***********************************//
void Intp_init(void)
{
	
		KEY0=1;
		KEY1=1;
    IT0 = 1;   //�ⲿ�ж�Դ0Ϊ�͵�ƽ����	����0     0:�½��ش���	1:�͵�ƽ����
    EX0 = 1;    //�����ⲿ�ж�
	//	IT1=1;			//�������ж�1Ϊ�͵�ƽ����	����1
//		EX1=1;			//�����ⲿ�ж�
    EA  = 1;    //�����ж�	
}

//ad ��ʼ������
void adinit(void)
{
		AUXR1     = 0x00;                 //ת�������8λ����ADC_RES��,��2λ����ADC_RESL��
    P1ASF     = 0X01;                // ����P1.0��Ϊģ�⹦��A/Dʹ��
    ADC_RES   = 0x00;                   //�������
		//   ADC_RESL  = 0x00;										//����λ�������
		ADC_CONTR = ADC_POWER|Speed_2|ADC0;   //�򿪵�Դ,180CLK����ת��,ѡ��P1.0������
		//POWER SPEED1 SPEED0 ADC_FLAG   ADC_START CHS2 CHS1 CHS0 
		_nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();

}


//ad ת������
u8 getad(void)
{
    ADC_CONTR |= ADC_START;   //��ʼADת��
	
		_nop_();
    _nop_();
		_nop_();
    _nop_();
    _nop_();
	
		while(!(ADC_CONTR&ADC_FLAG));//�ȴ�ADת������
		ADC_CONTR &=~ADC_FLAG	;//ADC_FLAG �����0

		_nop_();
    _nop_();
    _nop_();
		_nop_();
    _nop_();	
		return ADC_RES;

 //  EADC      = 1;                          //����A/Dת���ж�
 //  EA        = 1;                          //�����ж�
}

//adȡƽ������
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

//���벻���жϺ���
bit lianganpanduan(u8 zhi)
{
		if(zhi>0x60)
		return 1;
	else 
		return 0;

}

//�ȼ��жϺ���
u8 levelpanduan(u8 zhi)
{
	if(zhi<0xb0)
		return 1;
	else 
		return 3;

}



//�����а�ɨ�躯��
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
//�ȼ�ɨ�躯��
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
	CCAP0H = mat;   //ģ��0��ʼ��� ռ������Ϊ25%
	CCAP1L = mat;
	CCAP1H = mat;   //ģ��1��ʼ��� ռ������Ϊ25%
	
	if(module==0)
		 {
			switch(mode)
				{ 
		 case 0: CCAPM0 = 0X42;break; //ģ��0����Ϊ8λPWM��������ж�
		 case 1: CCAPM0 = 0X53;break; //ģ��0����Ϊ8λPWM������½��ز����ж�
		 case 2: CCAPM0 = 0X63;break; //ģ��0����Ϊ8λPWM����������ز����ж�
		 case 3: CCAPM0 = 0X73;break; //ģ��0����Ϊ8λPWM����������ز����ж�
		 default: break;
				}
			}
	else
  if(module==1)
     {
      switch(mode)
       { 
   case 0: CCAPM1 = 0X42;break; //ģ��1����Ϊ8λPWM��������ж�
   case 1: CCAPM1 = 0X53;break; //ģ��1����Ϊ8λPWM������½��ز����ж�
   case 2: CCAPM1 = 0X63;break; //ģ��1����Ϊ8λPWM����������ز����ж�
   case 3: CCAPM1 = 0X73;break; //ģ��1����Ϊ8λPWM����������ز����ж�
   default: break;
				}
		}
  else
	if(module==2)
    {
        switch(mode)
			{ 
      case 0: CCAPM0 = CCAPM1 = 0X42;break; //ģ��0��1����Ϊ8λPWM��������ж�
      case 1: CCAPM0 = CCAPM1 = 0X53;break; //ģ��0��1����Ϊ8λPWM������½��ز����ж�
      case 2: CCAPM0 = CCAPM1 = 0X63;break; //ģ��0��1����Ϊ8λPWM����������ز����ж�
      case 3: CCAPM0 = CCAPM1 = 0X73;break; //ģ��0��1����Ϊ8λPWM����������ز����ж�
      default: break;
      }
    }
	CR=1; //PCA��������ʼ����
		
}
	void main()
{

	P4SW=0XFF;			//��P4����Ϊ��ͨIO��
	P4M1=0X01;
	P4M0=0X00;
	
	Intp_init();             //�������ж�0��1��ʼ��
	adinit();								//ad ��ʼ��
	
	inh2=0;					//cd4051 ʹ��
	
	
	TMOD|=0x02; /* timer 0 mode 2: 8-Bit reload */ 
	TH0=0xff;
	TR0=1;     
	PWM_clock(2);      // PCA/PWMʱ��ԴΪ ��ʱ��0�����
	
	PWM_start(pwmvalue,2,0); // ģ��0,����ΪPWM���,���ж�,��ʼռ������Ϊ25%
		
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
//               A/D�жϷ���        		//
//******************************************//
// void AD_Service(void) interrupt 5
// {
//     ADC_CONTR &= !ADC_FLAG; //���־
// 		ADC_CONTR &= !ADC_START;// �ر�ADת��
// 		_nop_();
//     _nop_();
// 		_nop_();
//     _nop_();
// 	
// 		value=ADC_RES*256+ADC_RESL;
//     ADC_RES   = 0x00;
//     ADC_RESL  = 0x00;
// 	if(value>200){
// 		PWM_start(0x40,2,0); // ģ��0,����ΪPWM���,���ж�,��ʼռ������Ϊ25%
//    LED1=!LED1;}
// 	if(value<200){
// 		PWM_start(0x80,2,0); // ģ��0,����ΪPWM���,���ж�,��ʼռ������Ϊ25%
//    LED2=!LED2;}

//     ADC_CONTR = ADC_POWER|Speed_2|ADC_START; //��ʼ��һ��ת��   
//     _nop_();
//     _nop_();
//     _nop_();
//     _nop_();
// 	
// }

//����0 �жϺ���
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
// //����1 �жϺ���
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
