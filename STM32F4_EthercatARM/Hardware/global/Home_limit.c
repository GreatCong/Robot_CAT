#include "Home_limit.h"
#include "Stepper.h"
#include "Setting.h"

#include "gpio.h"
#include <string.h>

Control_Key_t Contol_key_state;

//  @ fuction:  
//  @ description: 滴答定时器回调 
//  @ input:
//  @ output:
//  @ note: 
void HAL_SYSTICK_Callback(void){
	
	uint8_t limits = Limits_GetState();
	Limit_PinChangeISR(limits);
//	if(limits) {
//		if(Contol_key_state.debounceCounterLimits == 0) {
//			Contol_key_state.debounceCounterLimits = 5;
//			Limit_PinChangeISR(limits);			
//		}
//				
//	}

	uint8_t controls = System_GetControlState();
	if(controls) {
		// System control
		if(Contol_key_state.debounceCounterControl == 0) {
			Contol_key_state.debounceCounterControl = 10;
			System_PinChangeISR(controls);
		}
	}

//	if(Contol_key_state.debounceCounterLimits && !limits) {
//		Contol_key_state.debounceCounterLimits--;
//		if(Contol_key_state.debounceCounterLimits < 0){
//		  Contol_key_state.debounceCounterLimits = 0;
//		}
//	}
	if(Contol_key_state.debounceCounterControl && !controls) {
		Contol_key_state.debounceCounterControl--;
		if(Contol_key_state.debounceCounterControl < 0){
		  Contol_key_state.debounceCounterControl = 0;
		}
	}
	
	System_LoopHandler();//事件的循环处理
	
}

//  @ fuction:  
//  @ description: 初始化
//  @ input:
//  @ output:
//  @ note: 
void Home_limit_init(void){
    memset(&Contol_key_state,0,sizeof(Contol_key_state));
	
	  Contol_key_state.key_limit_state_mask = (1<<LIMIT_X) | (1<<LIMIT_Y) | (1<<LIMIT_Z) |
	                                          (1<<LIMIT_A) | (1<<LIMIT_B) | (1<<LIMIT_C)  ;
	
	  Contol_key_state.key_press_state_mask = (1<<CONTROL_RESET_BIT) | (1<<CONTROL_FEED_HOLD_BIT) |
	                                          (1<<CONTROL_CYCLE_START_BIT) | (1<<CONTROL_SAFETY_DOOR_BIT);
	
	  Contol_key_state.enable_key_limit = 1;
	  Contol_key_state.enable_key_press = 1;	
}

//  @ fuction:  
//  @ description: 获取限位开关的状态
//  @ input:
//  @ output:
//  @ note: 铣床的限位是正常high,接触low
uint8_t Limits_GetState(void){
	uint8_t limit_state = 0;

	limit_state = (HAL_GPIO_ReadPin(GPIO_LIM_X_GPIO_Port, GPIO_LIM_X_Pin)<<LIMIT_X);
	limit_state |= (HAL_GPIO_ReadPin(GPIO_LIM_Y_GPIO_Port, GPIO_LIM_Y_Pin)<<LIMIT_Y);
	limit_state |= (HAL_GPIO_ReadPin(GPIO_LIM_Z_GPIO_Port, GPIO_LIM_Z_Pin)<<LIMIT_Z);
  limit_state |= (HAL_GPIO_ReadPin(GPIO_LIM_A_GPIO_Port, GPIO_LIM_A_Pin)<<LIMIT_A);
	limit_state |= (HAL_GPIO_ReadPin(GPIO_LIM_B_GPIO_Port, GPIO_LIM_B_Pin)<<LIMIT_B);
	limit_state |= (HAL_GPIO_ReadPin(GPIO_LIM_C_GPIO_Port, GPIO_LIM_C_Pin)<<LIMIT_C);

	//铣床的限位是正常high,接触low,翻转下
	limit_state ^= Contol_key_state.key_limit_state_mask;
	
//	printf("Limits_GetState=%x\r\n",limit_state);
	
	return limit_state;
}

//  @ fuction:  
//  @ description: 获取按键开关的状态
//  @ input:
//  @ output:
//  @ note: 上拉了,正常low,接触high
uint8_t System_GetControlState(void) {
	uint8_t control_state = 0;//默认引脚均要下拉
	uint8_t pin = ((HAL_GPIO_ReadPin(GPIO_CTRL_RST_GPIO_Port, GPIO_CTRL_RST_Pin)<<CONTROL_RESET_BIT) |
					(HAL_GPIO_ReadPin(GPIO_CTRL_FEED_GPIO_Port, GPIO_CTRL_FEED_Pin)<<CONTROL_FEED_HOLD_BIT) |
					(HAL_GPIO_ReadPin(GPIO_CTRL_START_GPIO_Port, GPIO_CTRL_START_Pin)<<CONTROL_CYCLE_START_BIT) |
					(HAL_GPIO_ReadPin(GPIO_DOOR_GPIO_Port, GPIO_DOOR_Pin)<<CONTROL_SAFETY_DOOR_BIT));
  
	//printf("System_GetControlState=%x\r\n",control_state);
	
	//不需要翻转
	//control_state ^= Contol_key_state.key_press_state_mask;
	//printf("System_GetControlState_mask=%x\r\n",control_state);
	
	if(pin){
	   if(pin & (1 << CONTROL_RESET_BIT)){
		    control_state |= (1<<CONTROL_RESET_BIT);
		 }
		
		 if(pin & (1 << CONTROL_FEED_HOLD_BIT)){
		    control_state |= (1<<CONTROL_FEED_HOLD_BIT);
		 }
		 
		 if(pin & (1 << CONTROL_CYCLE_START_BIT)){
		    control_state |= (1<<CONTROL_CYCLE_START_BIT);
		 }
		 
		 if(pin & (1 << CONTROL_SAFETY_DOOR_BIT)){
		    control_state |= (1<<CONTROL_SAFETY_DOOR_BIT);
		 }
	}

	return control_state;
}

//  @ fuction:  
//  @ description: 
//  @ input:
//  @ output:
//  @ note: 
void Limit_PinChangeISR(uint8_t state){
  Contol_key_state.key_limit_state = state;//获取限位开关的状态
}

//  @ fuction:  
//  @ description: 
//  @ input:
//  @ output:
//  @ note: 
void System_PinChangeISR(uint8_t state){
	 Contol_key_state.key_press_state = state;//获取按键开关的状态
}

//  @ fuction:  
//  @ description: 
//  @ input:
//  @ output:
//  @ note: 
void System_LoopHandler(void){
		if(Contol_key_state.key_press_state){//这种读法类似于自锁开关
		   if(Contol_key_state.key_press_state & (1<<CONTROL_RESET_BIT)){//如果是设置回原点
				 if(Contol_key_state.mode_ethercat_run == 1){ //ethecat应用运行的时候,取消自动归位
				    Contol_key_state.key_press_state &= ~(1<<CONTROL_RESET_BIT);
				 }
				 else{
				   GoHome();
				 }				 
			     
			 }
			 else{
			    //其他按键暂时屏蔽掉
				 Contol_key_state.key_press_state = 0;
			 }
		}
}

//  @ fuction:  
//  @ description: 自动归位
//  @ input:
//  @ output:
//  @ note: 
void GoHome(void){
	uint8_t mask = settings.limit_enable_mask;//默认XYZ 3个轴0x07
	
  if(Stepper_data.step_count == 0){
			Stepper_t stepper_param;
			memset(&stepper_param,0,sizeof(stepper_param));
		 if(!(Contol_key_state.key_limit_state & (1<<LIMIT_X))){
				stepper_param.steps[AXIS_X] = 6000;
		 }
		 
		 if(!(Contol_key_state.key_limit_state & (1<<LIMIT_Y))){
				stepper_param.steps[AXIS_Y] = 6000;
		 }
		 
		 if(!(Contol_key_state.key_limit_state & (1<<LIMIT_Z))){
				stepper_param.steps[AXIS_Z] = 6000;
		 }
		 
		 if(!(Contol_key_state.key_limit_state & (1<<LIMIT_A))){
				stepper_param.steps[AXIS_A] = 6000;
		 }
		 
		 if(!(Contol_key_state.key_limit_state & (1<<LIMIT_B))){
				stepper_param.steps[AXIS_B] = 6000;
		 }
		 
		 if(!(Contol_key_state.key_limit_state & (1<<LIMIT_C))){
				stepper_param.steps[AXIS_C] = 6000;
		 }
		 
			stepper_param.dir_outbits = settings.limit_run_dir;//X,Y方向取反0x3
		 
			Stepper_setParam(stepper_param,1500);//姑且设定速度为1500
			Stepper_Enable(true);//使能电机
			Contol_key_state.enable_key_limit = 1;
		 HAL_GPIO_TogglePin(LED_BOOT_GPIO_Port,LED_BOOT_Pin);
	 }
	 else{
			if(Contol_key_state.key_limit_state == (mask)){
				 Contol_key_state.key_press_state &= ~(1<<CONTROL_RESET_BIT);
				 Contol_key_state.key_limit_state = 0;
				
				 memset(&Stepper_data,0,sizeof(Stepper_data));
				 __HAL_TIM_DISABLE(&STEP_TIMER);	
				 //HAL_GPIO_TogglePin(LED_STATE_GPIO_Port,LED_STATE_Pin);
				 Stepper_Enable(false);//失能电机			
			}
	 }
}
