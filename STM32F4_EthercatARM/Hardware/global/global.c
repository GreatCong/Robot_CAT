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
// @ description:�ⲿ�жϵ���
// @ input:
// @ note: ������wifi,rw_lib_platform.c��copy
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

#define CMD_STATE_NULL 0 //û���յ����ݻ��յ���β
#define CMD_STATE_HEAD 1 //�յ��˰�ͷ
#define CMD_STATE_PROCESS 2 //��ʾ�ڴ��������

#define PACKAGE_HEAD 0x55
#define PACKAGE_TAIL 0xaa

#define VERSION_MAJOR 2 //���汾��
#define VERSION_MINOR 3 //�Ӱ汾��

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

uint8_t Uart_data_rsv[100] = {0};//���ڽ��յ�����

uint8_t Cmd_state = CMD_STATE_NULL;
uint8_t Cmd_type = CMD_NULL;
bool Is_setting_to_flash = false;

//  @ fuction:  
//  @ description:���ڳ�ʼ��  
//  @ input:
//  @ output:
//  @ note: 
void Usart_Init(UART_HandleTypeDef *husart, uint32_t baud)
{
	
	HAL_UART_Receive_IT(husart,Uart_data_rsv,1);//���¿��������ж�
	
}

//  @ fuction:  
//  @ description: ���ڷ��ͺ���(�üĴ���)
//  @ input:
//  @ output:
//  @ note: ����Ϊ��̬����
static int Usart_putc(int ch)
{ 	
  //HAL_UART_Transmit(&huart6,(uint8_t*)&ch,1,2);//���ж��е��û����������ʹ�üĴ�������
	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (uint8_t) ch;      
	return ch;
}

//  @ fuction:  
//  @ description: ���ڽ��ջص� 
//  @ input:
//  @ output:
//  @ note: 
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart == &STDOUT){
		unsigned char c  = Uart_data_rsv[0];
		uint8_t receive_num  = 1;
		uint8_t cmd_SubNum = 0;//�е�ָ����ӱ��
		
		switch(Cmd_state){
			case CMD_STATE_NULL://��ʾҪ�ȴ���ͷ
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
			case CMD_STATE_HEAD://��ʾҪ�ȴ�ָ��
				   Cmd_state = CMD_STATE_PROCESS;
			     Cmd_type = CMD_NULL;
			     receive_num = c+1;//����һ����β
				break;
			case CMD_STATE_PROCESS://��ʾҪ����ָ����
				   Cmd_state = CMD_STATE_NULL;
			     Cmd_type = c;
			     //printf("c=0x%x\r\n",c);
					 switch(Cmd_type){
					 case CMD_WR_FLASH://�Ƚϻ���ʱ��,���յ���β�ٴ���
							Is_setting_to_flash = true;//��������Ϣд�뵽flash��(�Ứ�ѽϳ���ʱ��)
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
								Usart_putc((uint8_t)(settings.limit_goHome_speed>>8));//��λ��ǰ,��λ�ں�
							  Usart_putc((uint8_t)(settings.limit_goHome_speed));
								Usart_putc(PACKAGE_TAIL);
						 break;
					 case CMD_GET_LIMIT_ANGLE:
						    Usart_putc(PACKAGE_HEAD);
								Usart_putc(CMD_GET_LIMIT_ANGLE);
					 
					      cmd_SubNum = Uart_data_rsv[1];
						    if(cmd_SubNum < 3){
								   Usart_putc((uint8_t)(settings.limit_angle[cmd_SubNum]>>8));//��λ��ǰ,��λ�ں�
									 Usart_putc((uint8_t)(settings.limit_angle[cmd_SubNum]));
								}
                else{
								   Usart_putc(0);
									 Usart_putc(0);
								}								
					 
								Usart_putc(PACKAGE_TAIL);
						 break;
					 //set��ָ�����
					 case CMD_SET_ID://��ȡ������ID
								settings.robot_id = Uart_data_rsv[1];//Uart_data_rsv[0]��ʾCMD_type
								break;
					 case CMD_SET_LIMIT_RUN_DIR://��ȡ��λ�˶��ķ���
								settings.limit_run_dir = Uart_data_rsv[1];
								break;
					 case CMD_SET_LIMIT_ENABLE_MASK:
								settings.limit_enable_mask = Uart_data_rsv[1];
							 break;
					 case CMD_SET_LIMIT_MODE:
								settings.limit_mode = Uart_data_rsv[1];
						 break;
					 case CMD_SET_LIMIT_GOHOME_SPEED:
						 settings.limit_goHome_speed = (uint16_t)((Uart_data_rsv[1] << 8) | (Uart_data_rsv[2]));//��λ��ǰ,��λ�ں�
						 break;
					 case CMD_SET_LIMIT_ANGLE:			 
					      cmd_SubNum = Uart_data_rsv[1];
						    if(cmd_SubNum < 3){
									 settings.limit_angle[cmd_SubNum] = (uint16_t)((Uart_data_rsv[2] << 8) | (Uart_data_rsv[3]));//��λ��ǰ,��λ�ں�
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
		
		HAL_UART_Receive_IT(&STDOUT,Uart_data_rsv,receive_num);//���¿��������ж�
	}
}
