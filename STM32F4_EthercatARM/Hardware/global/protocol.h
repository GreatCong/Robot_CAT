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
	uint16_t io_output;
	uint16_t step_setting;
	uint16_t step_fre;
	uint16_t dir;
	uint16_t tools;
	uint32_t step[7];//6��+������
}Ethercat_output_cmd;

typedef struct{
	uint32_t motor_Count;
	int16_t motor_MSG;
	int16_t error_MSG;
	int16_t machine_param;
	int16_t limit_state;
  int16_t data[3];
}Ethercat_input_channel;

typedef struct{
  Ethercat_output_cmd cmd;
	Ethercat_input_channel ch;
}Ethercat_data_parameter;

#define MOTION_ENABLE_BIT 0 //ʹ��
#define MOTION_AUTORUN_START_BIT 1 //������
#define MOTION_AUTORUN_STOP_BIT 2 //����

#define MOTION_WAIT_LIMIT_X 8 //X���˶�ֱ��X��λ
#define MOTION_WAIT_LIMIT_Y 9 //Y���˶�ֱ��Y��λ
#define MOTION_WAIT_LIMIT_Z 10 //Z���˶�ֱ��Z��λ
#define MOTION_WAIT_LIMIT_A 11 //A���˶�ֱ��A��λ
#define MOTION_WAIT_LIMIT_B 12 //B���˶�ֱ��B��λ
#define MOTION_WAIT_LIMIT_C 13 //C���˶�ֱ��C��λ

//extern PWM_Master_Parameter PWM_Master1;
//extern PWM_Master_Parameter PWM_Master2;
extern void MotorTools_init(void);
extern void MotorTools_setPWM(uint16_t value);
extern void MotorTools_ResetPWM(void);

extern void IO_DeviceTools_init(void);
extern void IO_DeviceTools_setData(uint16_t data);
extern void IO_DeviceTools_Reset(void);

#endif
