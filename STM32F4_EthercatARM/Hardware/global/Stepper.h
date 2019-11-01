#ifndef __STEPPER_H_
#define __STEPPER_H_

#include "stm32f4xx_hal.h"
#include "stdbool.h"
#include "tim.h"

typedef enum{
  Step_Low,
	Step_High
}Step_State_enum;

typedef enum{
  Dir_Low,
	Dir_High
}Dir_State_enum;

#define AXIS_N 6

// Stepper ISR data struct. Contains the running data for the main stepper ISR.
typedef struct {
	uint32_t step_counter[AXIS_N];
	uint32_t step_eventCount;//max steps

	uint16_t step_outbits;  //最大支持16个轴
	uint16_t dir_outbits;
	uint32_t steps[AXIS_N];

	uint32_t step_count;       // Steps remaining in line segment motion
} Stepper_t;

extern Stepper_t Stepper_data;
extern int32_t Sys_position[AXIS_N];
extern uint8_t Stepper_isAutoRun;
extern uint32_t Step_TimerCount;

#define STEP_MAX(a,b) a>b? a:b

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2
#define AXIS_A 3
#define AXIS_B 4
#define AXIS_C 5
#define AXIS_TOOL1 6 //表示工具轴

#define STEP_TIMER htim4
#define STEP_TIMER_CHANNEL TIM_CHANNEL_1
#define MOTOR_TOOL_TIMER htim9
#define MOTOR_TOOL_TIMER_CHANNEL TIM_CHANNEL_1

#define F_TIMER_STEPPER             21000000UL //24Mz
#define MAX_STEP_RATE_HZ 50000 // Hz
#define STEP_TIMER_MIN          (uint16_t)(F_TIMER_STEPPER / MAX_STEP_RATE_HZ)
#define TEST_STEP_RATE_HZ 1000 // 1Hz
#define STEP_TIMER_TEST          (uint16_t)(F_TIMER_STEPPER / TEST_STEP_RATE_HZ)

//为了兼容标准库
#define GPIO_ResetBits(port,pin) HAL_GPIO_WritePin(port,pin,GPIO_PIN_RESET)
#define GPIO_SetBits(port,pin) HAL_GPIO_WritePin(port,pin,GPIO_PIN_SET)
#define GPIO_ReadInputDataBit(port,pin) HAL_GPIO_ReadPin(port,pin) 

void Stepper_Enable(bool isEnable);
void Stepper_ControlStep(uint8_t motor_num,Step_State_enum step_state);
void Stepper_ControlDir(uint8_t motor_num,Dir_State_enum dir_state);

void Stepper_Timer_init(void);
void Stepper_PortResetISR(void);

#endif
