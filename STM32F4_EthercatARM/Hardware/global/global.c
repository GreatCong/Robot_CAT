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
#define VERSION_MINOR 3 //子版本号

typedef enum{
	CMD_NULL = 0x7f,
  CMD_GET_ID,//0x80
	CMD_SET_ID,//0x81
	CMD_WR_FLASH, //0x82
	CMD_GET_VERSION, //0x83
	CMD_GET_LIMIT_RUN_DIR,//0x84
	CMD_SET_LIMIT_RUN_DIR,//0x85
	CMD_GET_LIMIT_ENABLE_MASK,//0x86
	CMD_SET_LIMIT_ENABLE_MASK,//0x87
	CMD_GET_LIMIT_MODE,//0x88
	CMD_SET_LIMIT_MODE,//0x89
	CMD_GET_LIMIT_GOHOME_SPEED,//0x8a
	CMD_SET_LIMIT_GOHOME_SPEED, //0x8b
	CMD_GET_LIMIT_ANGLE,//0x8c
	CMD_SET_LIMIT_ANGLE //0x8d
}CMD_TYPE_E;

uint8_t Uart_data_rsv[100] = {0};//串口接收的数据

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
	
	HAL_UART_Receive_IT(husart,Uart_data_rsv,1);//重新开启串口中断
	
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
		unsigned char c  = Uart_data_rsv[0];
		uint8_t receive_num  = 1;
		uint8_t cmd_SubNum = 0;//有的指令还有子编号
		
		switch(Cmd_state){
			case CMD_STATE_NULL://表示要等待包头
				if(c == PACKAGE_HEAD){
			     Cmd_state = CMD_STATE_HEAD;
			     Cmd_type = CMD_NULL;
					 receive_num = 1;
			  }
				else{
				   Cmd_state = CMD_STATE_NULL;
			     Cmd_type = CMD_NULL;
					 receive_num = 1;
				}
				break;
			case CMD_STATE_HEAD://表示要等待指令
				   Cmd_state = CMD_STATE_PROCESS;
			     Cmd_type = CMD_NULL;
			     receive_num = c+1;//包含一个包尾
				break;
			case CMD_STATE_PROCESS://表示要处理指令了
				   Cmd_state = CMD_STATE_NULL;
			     Cmd_type = c;
			     //printf("c=0x%x\r\n",c);
					 switch(Cmd_type){
					 case CMD_WR_FLASH://比较花费时间,接收到包尾再处理
							Is_setting_to_flash = true;//将配置信息写入到flash中(会花费较长的时间)
						 break;
					 case CMD_GET_VERSION:
							Usart_putc(PACKAGE_HEAD);
							Usart_putc(CMD_GET_VERSION);
							Usart_putc(VERSION_MAJOR);
							Usart_putc(VERSION_MINOR);
							Usart_putc(PACKAGE_TAIL);
						 break;
					 case CMD_GET_ID:
								Usart_putc(PACKAGE_HEAD);
								Usart_putc(CMD_GET_ID);
								Usart_putc(settings.robot_id);
								Usart_putc(PACKAGE_TAIL);
								break;
					 case CMD_GET_LIMIT_RUN_DIR:
								Usart_putc(PACKAGE_HEAD);
								Usart_putc(CMD_GET_LIMIT_RUN_DIR);
								Usart_putc(settings.limit_run_dir);
								Usart_putc(PACKAGE_TAIL);
								break;
					 case CMD_GET_LIMIT_ENABLE_MASK:
								Usart_putc(PACKAGE_HEAD);
								Usart_putc(CMD_GET_LIMIT_ENABLE_MASK);
								Usart_putc(settings.limit_enable_mask);
								Usart_putc(PACKAGE_TAIL);
								break; 
					 case CMD_GET_LIMIT_MODE:
								Usart_putc(PACKAGE_HEAD);
								Usart_putc(CMD_GET_LIMIT_MODE);
								Usart_putc(settings.limit_mode);
								Usart_putc(PACKAGE_TAIL);
						 break;
					 case CMD_GET_LIMIT_GOHOME_SPEED:
						    Usart_putc(PACKAGE_HEAD);
								Usart_putc(CMD_GET_LIMIT_MODE);
								Usart_putc((uint8_t)(settings.limit_goHome_speed>>8));//高位在前,低位在后
							  Usart_putc((uint8_t)(settings.limit_goHome_speed));
								Usart_putc(PACKAGE_TAIL);
						 break;
					 case CMD_GET_LIMIT_ANGLE:
						    Usart_putc(PACKAGE_HEAD);
								Usart_putc(CMD_GET_LIMIT_ANGLE);
					 
					      cmd_SubNum = Uart_data_rsv[1];
						    if(cmd_SubNum < 3){
								   Usart_putc((uint8_t)(settings.limit_angle[cmd_SubNum]>>8));//高位在前,低位在后
									 Usart_putc((uint8_t)(settings.limit_angle[cmd_SubNum]));
								}
                else{
								   Usart_putc(0);
									 Usart_putc(0);
								}								
					 
								Usart_putc(PACKAGE_TAIL);
						 break;
					 //set的指令解析
					 case CMD_SET_ID://获取机器人ID
								settings.robot_id = Uart_data_rsv[1];//Uart_data_rsv[0]表示CMD_type
								break;
					 case CMD_SET_LIMIT_RUN_DIR://获取限位运动的方向
								settings.limit_run_dir = Uart_data_rsv[1];
								break;
					 case CMD_SET_LIMIT_ENABLE_MASK:
								settings.limit_enable_mask = Uart_data_rsv[1];
							 break;
					 case CMD_SET_LIMIT_MODE:
								settings.limit_mode = Uart_data_rsv[1];
						 break;
					 case CMD_SET_LIMIT_GOHOME_SPEED:
						 settings.limit_goHome_speed = (uint16_t)((Uart_data_rsv[1] << 8) | (Uart_data_rsv[2]));//高位在前,低位在后
						 break;
					 case CMD_SET_LIMIT_ANGLE:			 
					      cmd_SubNum = Uart_data_rsv[1];
						    if(cmd_SubNum < 3){
									 settings.limit_angle[cmd_SubNum] = (uint16_t)((Uart_data_rsv[2] << 8) | (Uart_data_rsv[3]));//高位在前,低位在后
								}                				
						 break;
					 default:
								break;
				 }
			     receive_num = 1;
				break;
			default:
				   Cmd_state = CMD_STATE_NULL;
			     Cmd_type = CMD_NULL;
			     receive_num = 1;
				break;
		}
		
		HAL_UART_Receive_IT(&STDOUT,Uart_data_rsv,receive_num);//重新开启串口中断
	}
}
