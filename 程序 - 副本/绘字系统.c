#include"STC12C5A60S2.h" 
#include<intrins.h> 

sbit     BMA    = P2^2;//���뿪��A����
sbit     BMB    = P2^1;//���뿪��B����
sbit     BMC    = P2^0;//���뿪�� �°�����
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


void v_Lcd12864CheckBusy_f( void )      //æ��⺯��
{
    unsigned int nTimeOut = 0 ;
    SET_INC
    SET_READ
    CLR_EN
    SET_EN
    while( ( io_LCD12864_DATAPORT & 0x80 ) && ( ++nTimeOut != 0 ) ) ; //���λΪ0ʱ������
    CLR_EN
    SET_INC
    SET_READ
}

void v_Lcd12864SendCmd_f( unsigned char byCmd )      //��������
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
void v_Lcd12864SendData_f( unsigned char byData )    //��������
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

void v_DelayMs_f( unsigned int nDelay )              //n������ʱ
{
    unsigned int i ;
    for( ; nDelay > 0 ; nDelay-- )
    {
        for( i = 125 ; i > 0 ; i-- ) ;
    }
}

void v_Lcd12864Init_f( void )                  //��ʼ��
{
    v_Lcd12864SendCmd_f( 0x30 ) ;              //����ָ�  �����趨 8λ���� ������ָ��
    v_DelayMs_f( 50 ) ;
    v_Lcd12864SendCmd_f( 0x01 ) ;              //����
    v_DelayMs_f( 50 ) ;
    v_Lcd12864SendCmd_f( 0x06 ) ;              //�������
    v_DelayMs_f( 50 ) ;
    v_Lcd12864SendCmd_f( 0x0c ) ;              //����ʾ ���α�
}
void v_Lcd12864SetAddress_f( unsigned char x, y )  //��ַת��   x��������y������ ��8��4��
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

//��ʾ����ͼ��ĺ���
void v_Lcd12864DrawPicture_f( unsigned char code *pPicture )
{
    unsigned char i, j, k ;
    for( i = 0 ; i < 2 ; i++ )//����������д
    {
        for( j = 0 ; j < 32 ; j++ )//��
        {
            v_Lcd12864SendCmd_f( 0x80 + j ) ;//дY���꣨�ڼ�����д��
            if( i == 0 )                    //дX����  ���ж�����һ���ϣ�
            {
                v_Lcd12864SendCmd_f( 0x80 ) ;
            }
            else
            {
                v_Lcd12864SendCmd_f( 0x88 ) ;
            }
            for( k = 0 ; k < 16 ; k++ )      //дһ�������ݣ�һ�й�16���ֽڣ��ϰ˸��ַ���
            {
                v_Lcd12864SendData_f( *pPicture++ ) ;
            }
        }
    }
    v_Lcd12864SendCmd_f( 0x30 ) ;//������������
}


void main( void )
{
	while( 1 )
	{
    v_Lcd12864Init_f() ;

    v_Lcd12864PutString_f( 0,0, "���ӹ���ʦ֮��") ;
	v_DelayMs_f( 6000 );
    v_Lcd12864PutString_f( 1,1, "��ӭ����") ;
    v_Lcd12864PutString_f( 2,2, "Ӣ���޷���ʾ") ;
    v_Lcd12864PutString_f( 0,3, "������������") ;
    v_DelayMs_f( 10000 );
	v_Lcd12864SendCmd_f(0x01) ;//3��������ʾ
	v_DelayMs_f( 6000 );
	v_Lcd12864SendCmd_f(0x02) ;//��ַ��λ
	v_Lcd12864PutString_f( 0,0, "������������") ;
	v_DelayMs_f( 6000 );
	v_Lcd12864PutString_f( 1,0, "Ӣ���޷���ʾ") ;
	v_DelayMs_f( 6000 );
	}
}
