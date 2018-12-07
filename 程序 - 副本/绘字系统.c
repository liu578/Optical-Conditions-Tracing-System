#include"STC12C5A60S2.h" 
#include<intrins.h> 

sbit     BMA    = P2^2;//编码开关A引脚
sbit     BMB    = P2^1;//编码开关B引脚
sbit     BMC    = P2^0;//编码开关 下按引脚
sbit     LED    = P3^5;
sbit io_LCD12864_RS = P2^5 ;
sbit io_LCD12864_RW = P2^6 ;
sbit io_LCD12864_EN = P2^7 ;

#define io_LCD12864_DATAPORT P0
#define SET_DATA  io_LCD12864_RS = 1 ;
#define SET_INC  io_LCD12864_RS = 0 ;
#define SET_READ  io_LCD12864_RW = 1 ;
#define SET_WRITE io_LCD12864_RW = 0 ;
#define SET_EN    io_LCD12864_EN = 1 ;
#define CLR_EN    io_LCD12864_EN = 0 ;


void v_Lcd12864CheckBusy_f( void )      //忙检测函数
{
    unsigned int nTimeOut = 0 ;
    SET_INC
    SET_READ
    CLR_EN
    SET_EN
    while( ( io_LCD12864_DATAPORT & 0x80 ) && ( ++nTimeOut != 0 ) ) ; //最高位为0时表允许
    CLR_EN
    SET_INC
    SET_READ
}

void v_Lcd12864SendCmd_f( unsigned char byCmd )      //发送命令
{
    v_Lcd12864CheckBusy_f() ;
    SET_INC
    SET_WRITE
    CLR_EN
    io_LCD12864_DATAPORT = byCmd ;
    _nop_();
    _nop_();
    SET_EN  
    _nop_();
    _nop_();
    CLR_EN
    SET_READ
    SET_INC
}
void v_Lcd12864SendData_f( unsigned char byData )    //发送数据
{
    v_Lcd12864CheckBusy_f() ;
    SET_DATA
    SET_WRITE
    CLR_EN
    io_LCD12864_DATAPORT = byData ;
    _nop_();
    _nop_();
    SET_EN  
    _nop_();
    _nop_();
    CLR_EN
    SET_READ
    SET_INC
}

void v_DelayMs_f( unsigned int nDelay )              //n毫秒延时
{
    unsigned int i ;
    for( ; nDelay > 0 ; nDelay-- )
    {
        for( i = 125 ; i > 0 ; i-- ) ;
    }
}

void v_Lcd12864Init_f( void )                  //初始化
{
    v_Lcd12864SendCmd_f( 0x30 ) ;              //基本指令集  功能设定 8位数据 无扩充指令
    v_DelayMs_f( 50 ) ;
    v_Lcd12864SendCmd_f( 0x01 ) ;              //清屏
    v_DelayMs_f( 50 ) ;
    v_Lcd12864SendCmd_f( 0x06 ) ;              //光标右移
    v_DelayMs_f( 50 ) ;
    v_Lcd12864SendCmd_f( 0x0c ) ;              //开显示 关游标
}
void v_Lcd12864SetAddress_f( unsigned char x, y )  //地址转换   x是列数，y是行数 共8列4行
{
    unsigned char byAddress ;
    switch( y )
    {
        case 0 :    byAddress = 0x80 + x ;
            break;
        case 1 :    byAddress = 0x90 + x ;
            break ;
        case 2 :    byAddress = 0x88 + x ;
            break ;
        case 3 :    byAddress = 0x98 + x ;
            break ;
        default :
            break ;
    }
    v_Lcd12864SendCmd_f( byAddress ) ;
}
void v_Lcd12864PutString_f( unsigned char x, unsigned char y, unsigned char *pData )
{
    v_Lcd12864SetAddress_f( x, y ) ;
    while( *pData != '\0' )
    {
        v_Lcd12864SendData_f( *pData++ ) ;
    }            

}

//显示整屏图像的函数
void v_Lcd12864DrawPicture_f( unsigned char code *pPicture )
{
    unsigned char i, j, k ;
    for( i = 0 ; i < 2 ; i++ )//分上下两屏写
    {
        for( j = 0 ; j < 32 ; j++ )//行
        {
            v_Lcd12864SendCmd_f( 0x80 + j ) ;//写Y坐标（第几行上写）
            if( i == 0 )                    //写X坐标  （判断在那一屏上）
            {
                v_Lcd12864SendCmd_f( 0x80 ) ;
            }
            else
            {
                v_Lcd12864SendCmd_f( 0x88 ) ;
            }
            for( k = 0 ; k < 16 ; k++ )      //写一整行数据（一行共16个字节，合八个字符）
            {
                v_Lcd12864SendData_f( *pPicture++ ) ;
            }
        }
    }
    v_Lcd12864SendCmd_f( 0x30 ) ;//基本功能设置
}


void main( void )
{
	while( 1 )
	{
    v_Lcd12864Init_f() ;

    v_Lcd12864PutString_f( 0,0, "电子工程师之家") ;
	v_DelayMs_f( 6000 );
    v_Lcd12864PutString_f( 1,1, "欢迎光临") ;
    v_Lcd12864PutString_f( 2,2, "英文无法显示") ;
    v_Lcd12864PutString_f( 0,3, "★○◇◆※☆■△") ;
    v_DelayMs_f( 10000 );
	v_Lcd12864SendCmd_f(0x01) ;//3秒后清除显示
	v_DelayMs_f( 6000 );
	v_Lcd12864SendCmd_f(0x02) ;//地址归位
	v_Lcd12864PutString_f( 0,0, "★○◇◆※☆■△") ;
	v_DelayMs_f( 6000 );
	v_Lcd12864PutString_f( 1,0, "英文无法显示") ;
	v_DelayMs_f( 6000 );
	}
}
