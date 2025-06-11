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
 ALIENTEK 探索者STM32F407开发板 实验28
 触摸屏实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

//清空屏幕并在右上角显示"RST"
void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//清屏   
 	POINT_COLOR=BLUE;//设置字体为蓝色 
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//显示清屏区域
	LCD_ShowString(lcddev.width-56,32,200,16,16,"PREDICT");//显示清屏区域
	LCD_ShowString(0,144,200,16,16,"RESULT:");//显示清屏区域
  	POINT_COLOR=RED;//设置画笔蓝色 
	LCD_DrawRectangle(0, 0, 28 *5, 28 *5);
}


float in_buffer[784] = {0};
float out_buffer[10] = {0};


//电阻触摸屏测试函数
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
		if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
		{	
		 	if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
			{	
				if(tp_dev.x[0]>(lcddev.width-24)&&tp_dev.y[0]<16)
				{
					Load_Drow_Dialog();//清除
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
						TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],RED);		//画图
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
			delay_ms(10);	//没有按键按下的时候 
		
		if(key==KEY0_PRES)	//KEY0按下,则执行校准程序
		{
			LCD_Clear(WHITE);	//清屏
		    TP_Adjust();  		//屏幕校准 
			TP_Save_Adjdata();	 
			Load_Drow_Dialog();
		}
		i++;
		if(i%20==0)LED0=!LED0;
	}
}

int main(void)
{ 
	
    HAL_Init();                   	//初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);  	//设置时钟,168Mhz
	delay_init(168);               	//初始化延时函数
	uart_init(115200);             	//初始化USART
	usmart_dev.init(84); 		    //初始化USMART
	LED_Init();						//初始化LED	
	KEY_Init();						//初始化KEY
 	LCD_Init();           			//初始化LCD
	tp_dev.init();				    //触摸屏初始化 
	
  POINT_COLOR=RED;
	
 	Load_Drow_Dialog();	 	
	MX_X_CUBE_AI_Init();
	rtp_test(); 					//电阻屏测试  
}

