/**
 *************************************************************************************
 * @file 
 * @author
 * @version
 * @date 
 * @brief 
 *************************************************************************************
*/

#include "protocol.h"
//#include "usart.h"
//#include "usbd_cdc_if.h"
//#include "stdio.h"
#include "stepper.h"

/* Private variables ---------------------------------------------------------*/

//uint8_t PWM1_message = 0,PWM2_message = 0;

Ethercat_data_parameter Ethercat_data;
//int16_t encoder_data[8];//Ethercat used
//int16_t cmd[24];//Ethercat used
/* Private variables ---------------------------------------------------------*/

//  @ fuction:  
//  @ description:  
//  @ input:
//  @ output:
//  @ note: 
void MotorTools_init(void){
	HAL_TIM_PWM_Start(&MOTOR_TOOL_TIMER,MOTOR_TOOL_TIMER_CHANNEL);
}

//  @ fuction:  
//  @ description:  
//  @ input:
//  @ output:
//  @ note: 
void MotorTools_setPWM(uint16_t value){
   MOTOR_TOOL_TIMER.Instance->CCR1 = value>2000? 2000:value;//防止值太大
}

//  @ fuction:  
//  @ description:  
//  @ input:
//  @ output:
//  @ note: 
void MotorTools_ResetPWM(void){
   MOTOR_TOOL_TIMER.Instance->CCR1 = 0;
//	HAL_TIM_PWM_Stop(&MOTOR_TOOL_TIMER,MOTOR_TOOL_TIMER_CHANNEL);
}

//  @ fuction:  
//  @ description:  
//  @ input:
//  @ output:
//  @ note: 
void IO_DeviceTools_init(void){
	IO_DeviceTools_Reset();
}

//  @ fuction:  
//  @ description:  设置IO设备的值
//  @ input:
//  @ output:
//  @ note: 
void IO_DeviceTools_setData(uint16_t data){
   if(data & (1<<0)){
		   HAL_GPIO_WritePin(GPIO_SPINDLE_ENA_GPIO_Port,GPIO_SPINDLE_ENA_Pin,GPIO_PIN_SET);	  
		}
		else{
		   HAL_GPIO_WritePin(GPIO_SPINDLE_ENA_GPIO_Port,GPIO_SPINDLE_ENA_Pin,GPIO_PIN_RESET);
		}
}

//  @ fuction:  
//  @ description: 复位IO设备的值
//  @ input:
//  @ output:
//  @ note: 
void IO_DeviceTools_Reset(void){
   HAL_GPIO_WritePin(GPIO_SPINDLE_ENA_GPIO_Port,GPIO_SPINDLE_ENA_Pin,GPIO_PIN_RESET);
}
