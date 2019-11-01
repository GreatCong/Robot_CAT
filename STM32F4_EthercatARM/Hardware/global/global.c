#include "global.h"	
//#include "stm32f4xx.h"

void LED_Twinkle(void)
{
//		GPIO_ToggleBits(GPIOA, GPIO_Pin_2);
//		delay_ms(60);
	HAL_GPIO_TogglePin(LED_STATE_GPIO_Port,LED_STATE_Pin);
	HAL_Delay(60);
}


#include "ecatappl.h"
#include "tim.h"

void Ethercat_irq_handle(void){
   PDI_Isr();
}
void Ethercat_sync0_handle(void){
	NVIC_DisableIRQ(ETHERCAT_IRQ_EXTI_IRQn);
   Sync0_Isr();
	NVIC_EnableIRQ(ETHERCAT_IRQ_EXTI_IRQn);
}

void Ethercat_sync1_handle(void){
	NVIC_DisableIRQ(ETHERCAT_IRQ_EXTI_IRQn);
   Sync1_Isr();
	NVIC_EnableIRQ(ETHERCAT_IRQ_EXTI_IRQn);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if(htim == &htim5){
	   ECAT_CheckTimer();
	}
}

// @ function: 
// @ description:外部中断调用
// @ input:
// @ note: 由于无wifi,rw_lib_platform.c中copy
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
//  if (GPIO_Pin == WIFI_INT_Pin)
//  {
//		DRIVER_INT_HANDLE();
//  }
	
	switch(GPIO_Pin){
//	  case AD_BUSY_Pin: //AD7606 Handle
//				  AD7606_handle();
//			break;
		case ETHERCAT_IRQ_Pin://Ethercat IRQ
			    Ethercat_irq_handle();
			break;
		case SYNC0_Pin: //Ethercat SYNC0
				  Ethercat_sync0_handle();
			break;
		case SYNC1_Pin: //Ethercat SYNC1
				  Ethercat_sync1_handle();
			break;
		default:
			break;
	}
 
}

#include "usart.h"
#include "Setting.h"

#define CMD_STATE_NULL 0 //没有收到数据或收到包尾
#define CMD_STATE_HEAD 1 //收到了包头
#define CMD_STATE_PROCESS 2 //表示在处理过程中

#define PACKAGE_HEAD 0x55
#define PACKAGE_TAIL 0xaa

#define VERSION_MAJOR 2 //主版本号
#define VERSION_MINOR 1 //子版本号

typedef enum{
	CMD_NULL = 0x7f,
  CMD_GET_ID,//0x80
	CMD_SET_ID,//0x81
	CMD_WR_FLASH, //0x82
	CMD_GET_VERSION //0x83
}CMD_TYPE_E;

uint8_t Uart_data_rsv = 0;//串口接收的数据

uint8_t Cmd_state = CMD_STATE_NULL;
uint8_t Cmd_type = CMD_NULL;
bool Is_setting_to_flash = false;

//  @ fuction:  
//  @ description:串口初始化  
//  @ input:
//  @ output:
//  @ note: 
void Usart_Init(UART_HandleTypeDef *husart, uint32_t baud)
{
	
	HAL_UART_Receive_IT(husart,&Uart_data_rsv,1);//重新开启串口中断
	
}

//  @ fuction:  
//  @ description: 串口发送函数(用寄存器)
//  @ input:
//  @ output:
//  @ note: 设置为静态调用
static int Usart_putc(int ch)
{ 	
  //HAL_UART_Transmit(&huart6,(uint8_t*)&ch,1,2);//在中断中调用会造成阻塞，使用寄存器不会
	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (uint8_t) ch;      
	return ch;
}

//  @ fuction:  
//  @ description: 串口接收回调 
//  @ input:
//  @ output:
//  @ note: 
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart == &STDOUT){
		unsigned char c  = Uart_data_rsv;
		
		switch(c){
			case PACKAGE_HEAD:
				Cmd_state = CMD_STATE_HEAD;
			  Cmd_type = CMD_NULL;
				break;
			case PACKAGE_TAIL:
				Cmd_state = CMD_STATE_NULL;
			  //比较花费时间,接收到包尾再处理
			  if(Cmd_type == CMD_WR_FLASH){//将配置信息写入到flash中(会花费较长的时间)
					  
					  Is_setting_to_flash = true;
				}
				else if(Cmd_type == CMD_GET_VERSION){
				    Usart_putc(PACKAGE_HEAD);
						Usart_putc(CMD_GET_VERSION);
						Usart_putc(VERSION_MAJOR);
					  Usart_putc(VERSION_MINOR);
						Usart_putc(PACKAGE_TAIL);
				}
				Cmd_type = CMD_NULL;
				break;
			default:
				if(Cmd_state == CMD_STATE_HEAD){
				   Cmd_type = c;
					 Cmd_state = CMD_STATE_PROCESS;
					if(Cmd_type == CMD_GET_ID){
					   Usart_putc(PACKAGE_HEAD);
						 Usart_putc(CMD_GET_ID);
						 Usart_putc(settings.robot_id);
						 Usart_putc(PACKAGE_TAIL);
					}
				}
				else{
					switch(Cmd_type){//对指令解析
						case CMD_SET_ID://获取机器人ID
							settings.robot_id = c;
							break;
						default:
							break;
					}
				}
				break;
		}
		
		HAL_UART_Receive_IT(&STDOUT,&Uart_data_rsv,1);//重新开启串口中断
	}
}

