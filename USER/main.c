#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "touch.h"
#include "app_x-cube-ai.h"

/************************************************
 ALIENTEK ̽����STM32F407������ ʵ��28
 ������ʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

//�����Ļ�������Ͻ���ʾ"RST"
void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//����   
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//��ʾ��������
	LCD_ShowString(lcddev.width-56,32,200,16,16,"PREDICT");//��ʾ��������
	LCD_ShowString(0,144,200,16,16,"RESULT:");//��ʾ��������
  	POINT_COLOR=RED;//���û�����ɫ 
	LCD_DrawRectangle(0, 0, 28 *5, 28 *5);
}


float in_buffer[784] = {0};
float out_buffer[10] = {0};


//���败�������Ժ���
void rtp_test(void)
{
	u8 key;
	u8 i=0;	  
	float max_value = 0;
	uint32_t max_index = 0;
	LCD_DrawRectangle(0, 0, 28 *5, 28 *5);
	while(1)
	{
	 	key=KEY_Scan(0);
		tp_dev.scan(0); 		 
		if(tp_dev.sta&TP_PRES_DOWN)			//������������
		{	
		 	if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
			{	
				if(tp_dev.x[0]>(lcddev.width-24)&&tp_dev.y[0]<16)
				{
					Load_Drow_Dialog();//���
					for(int i=0;i<784;i++)
					{
						in_buffer[i] = 0.0;
					}
					for(int i=0;i<10;i++)
					{
						out_buffer[i] = 0.0;
					}
				}
				else if(tp_dev.x[0]>(lcddev.width-56)&&tp_dev.y[0]>32&&tp_dev.y[0]<48)
				{
					/*
					printf("\n");
					for (int i = 0; i < 28; i++) 
					{
						for (int j = 0; j < 28; j++) {
							if(in_buffer[i * 28 + j] == 0.0f)
								printf("  ");
							else
								printf("1 ");
						}
						printf("\n");
					}
					*/
					MX_X_CUBE_AI_Process();
					max_value = 0;
					max_index = 0;
					for (int i = 0; i < 10; i++) {
							if (out_buffer[i] > max_value) {
									max_value = out_buffer[i];
									max_index = i;
							}
					}
					LCD_ShowNum(16,160,max_index,2,24);
				}
				else 
				{
					if(tp_dev.x[0]<28*5&&tp_dev.y[0]<28*5)
					{
						TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],RED);		//��ͼ
						int in_buffer_x,in_buffer_y;
						in_buffer_y =  tp_dev.y[0]/5;
						in_buffer_x =  tp_dev.x[0]/5;
						in_buffer[in_buffer_y*28 + in_buffer_x] = 1;
						in_buffer[in_buffer_y*28 + in_buffer_x+1] = 1;
						in_buffer[(in_buffer_y+1)*28 + in_buffer_x] = 1;
						in_buffer[(in_buffer_y+1)*28 + in_buffer_x+1] = 1;
					}
				}					
			}
		}
		else 
			delay_ms(10);	//û�а������µ�ʱ�� 
		
		if(key==KEY0_PRES)	//KEY0����,��ִ��У׼����
		{
			LCD_Clear(WHITE);	//����
		    TP_Adjust();  		//��ĻУ׼ 
			TP_Save_Adjdata();	 
			Load_Drow_Dialog();
		}
		i++;
		if(i%20==0)LED0=!LED0;
	}
}

int main(void)
{ 
	
    HAL_Init();                   	//��ʼ��HAL��    
    Stm32_Clock_Init(336,8,2,7);  	//����ʱ��,168Mhz
	delay_init(168);               	//��ʼ����ʱ����
	uart_init(115200);             	//��ʼ��USART
	usmart_dev.init(84); 		    //��ʼ��USMART
	LED_Init();						//��ʼ��LED	
	KEY_Init();						//��ʼ��KEY
 	LCD_Init();           			//��ʼ��LCD
	tp_dev.init();				    //��������ʼ�� 
	
  POINT_COLOR=RED;
	
 	Load_Drow_Dialog();	 	
	MX_X_CUBE_AI_Init();
	rtp_test(); 					//����������  
}

