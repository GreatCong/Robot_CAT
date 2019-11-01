#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "stm32f4xx_hal.h"

#define DEBUG_USB 1

//typedef struct{
//	int16_t cnt_tim1;
//	int16_t cnt_tim2;
//	int16_t cnt_tim3;
//	int16_t cnt_tim4;
//	uint8_t dir;
//	
//	uint8_t cmd;
//}PWM_Master_Parameter;

typedef struct{
	uint16_t step_setting;
	uint16_t step_fre;
	uint16_t dir;
	uint16_t tools;
	uint16_t step[7];//6轴+工具轴
}Ethercat_output_cmd;

typedef struct{
	int16_t motor_MSG;
	int16_t error_MSG;
  int16_t data[2];
}Ethercat_input_channel;

typedef struct{
  Ethercat_output_cmd cmd;
	Ethercat_input_channel ch;
}Ethercat_data_parameter;

#define MOTION_ENABLE_BIT 0 //使能
#define MOTION_AUTORUN_START_BIT 1 //不更新
#define MOTION_AUTORUN_STOP_BIT 2 //更新

//extern PWM_Master_Parameter PWM_Master1;
//extern PWM_Master_Parameter PWM_Master2;
extern void MotorTools_init(void);
extern void MotorTools_setPWM(uint16_t value);
void MotorTools_ResetPWM(void);

#endif
