/**

  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include <string.h>
#include "bsp_gpio.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "bsp_SysTick.h"
#include "dma.h"
#include "iwdg.h"
#include "SPI.h"
#include "delay.h"
#include "timers.h"
#include "timer4_cap.h"
#include "NVIC_CONFIG.H"

#include "wifi_config.h"
#include "wifi_function.h"
#include "oled.h"
#include "dht11.h"
#include "adc.h"
#include "MQ-2.h"
#include "HC-SR501.h"
#include "HC-SR04.h"
#include "BH1750.h"
#include "RC522.h"

#include "contiki-conf.h"
#include <stdint.h>
#include <stdio.h>
#include <debug-uart.h>
#include <process.h>
#include <procinit.h>
#include <etimer.h>
#include <autostart.h>
#include <clock.h>
#include "contiki_delay.h"
#include "ProcessTask.h"


#define __WIFI_MODULE_ON__          //WIFI模块
#define __OLED_MODULE_ON__          //OLED显示屏
#define __DHT11_MODULE_ON__         //温湿度传感器
#define __MQ02_MODULE_ON__          //烟雾传感器
#define __HCSR501_MODULE_ON__       //红外热释电人体传感器
#define __HCSR04_MODULE_ON__        //超声波测距模块
#define __BH1750_MODULE_ON__        //关照传感器
#define __RC522_MODULE_ON__         //RFID读卡器





unsigned char  DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
unsigned char g_ucTempbuf[20];    
unsigned char MLastSelectedSnr[4];
unsigned char RevBuffer[30];  
unsigned char SerBuffer[20]; 

unsigned char  da1[16] = {0x12,0x34,0x56,0x78,0xED,0xCB,0xA9,0x87,0x12,0x34,0x56,0x78,0x01,0xFE,0x01,0xFE};
unsigned char  da2[4]  = {0x00,0,0,0x01};






void BSP_Config(void)
{
    /* 初始化 */
    delay_init();
    clock_init();
    NVIC_Configuration_Init();
    LED_GPIO_Config();
    USART1_Config(115200);
    USART2_Config(115200);
    
#ifdef __OLED_MODULE_ON__
    OLED_Init(); //初始化OLED模块使用的接口和外设
    OLED_ShowString(0,0,"SPI OLED");
    OLED_ShowString(0,16,"Start OK!");
    OLED_Refresh_Gram();//更新显示
#endif         
    
#ifdef __DHT11_MODULE_ON__
    DHT11_Init(); 
#endif   

#ifdef __MQ02_MODULE_ON__
    MQ02_Init();
#endif
    
#ifdef __HCSR501_MODULE_ON__
    HCSR501_Init();
#endif

#ifdef __HCSR04_MODULE_ON__
	UltrasonicWave_Configuration();               //对超声波模块初始化
#endif

#ifdef __BH1750_MODULE_ON__
	BH1750_Init();               
    BH1750_Start();
#endif

#ifdef __RC522_MODULE_ON__
	RC522_Init();
#endif

#ifdef __WIFI_MODULE_ON__
    WiFi_Config(); 
    ESP8266_STA_TCP_Client();
#endif    
    
}


int main(void)
{	
    BSP_Config();    
    
    IWDG_Start(2);  //wifi模块透传之后开启看门狗
    
    process_init();
    autostart_start(autostart_processes);
    process_start(&red_blink_process,NULL);
    process_start(&green_blink_process,NULL);
    //process_start(&clock_test_process,NULL);
#ifdef __OLED_MODULE_ON__
    process_start(&OLED_Show_Increment_process,NULL);
#endif
    
#ifdef __DHT11_MODULE_ON__
    process_start(&DHT11_Read_Data_process,NULL);
#endif   
    
#ifdef __WIFI_MODULE_ON__     
    process_start(&wifi_send_test_process,NULL);
#endif
    
#ifdef __MQ02_MODULE_ON__     
    process_start(&MQ02_Read_Value_process,NULL);
#endif

#ifdef __HCSR501_MODULE_ON__     
    process_start(&HCSR501_Read_Status_process,NULL);
#endif

#ifdef __HCSR04_MODULE_ON__     
    process_start(&HCSR04_Measure_Distance_process,NULL);
#endif

#ifdef __BH1750_MODULE_ON__     
    process_start(&BH1750_Measure_Lumen_process,NULL);
#endif

#ifdef __RC522_MODULE_ON__     
    process_start(&RC522_Read_Card_process,NULL);
#endif

    while (1)
    {
        do
        {
        }while (process_run()>0);
    }
}





