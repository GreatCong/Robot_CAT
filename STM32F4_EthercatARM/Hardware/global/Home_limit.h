#ifndef __HOME_LIMIT_H_
#define __HOME_LIMIT_H_

#include <stdint.h>
#include <stdbool.h>

#define CONTROL_RESET_BIT			0
#define CONTROL_FEED_HOLD_BIT		1
#define CONTROL_CYCLE_START_BIT		2
#define CONTROL_SAFETY_DOOR_BIT		3

#define LIMIT_X 0
#define LIMIT_Y 1
#define LIMIT_Z 2
#define LIMIT_A 3
#define LIMIT_B 4
#define LIMIT_C 5
#define LIMIT_All 6

//typedef enum{
//   Key_Ctrl_Reset,
//	 Key_Ctrl_FeedHold,
//	 Key_Ctrl_CycleStart,
//	 Key_Ctrl_SafeDoor
//}Control_KeyState;

typedef struct{
  uint8_t key_limit_state;
	uint8_t key_press_state;
	uint8_t key_limit_state_mask;
	uint8_t key_press_state_mask;
	int16_t debounceCounterLimits;
	int16_t debounceCounterControl;
	uint8_t enable_key_limit;
	uint8_t enable_key_press;
	uint8_t mode_ethercat_run;
	uint8_t mode_mpu6050;
}Control_Key_t;

extern Control_Key_t Contol_key_state;
extern bool Is_ethercat_init;

extern void Home_limit_init(void);
extern uint8_t Limits_GetState(void);
extern uint8_t System_GetControlState(void);

extern void Limit_PinChangeISR(uint8_t state);
extern void System_PinChangeISR(uint8_t state);
extern void System_LoopHandler(void);
extern void GoHome(void);

#endif
