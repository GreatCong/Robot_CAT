#include "Stepper.h"
#include "gpio.h"
#include "tim.h"
#include <string.h>

#include "Home_limit.h"

//  @ fuction:  Stepper_Enable
//  @ description:  
//  @ input:
//  @ output:
//  @ note: 
void Stepper_Enable(bool isEnable){
	if(isEnable){
	  HAL_GPIO_WritePin(GPIO_ENABLE_GPIO_Port,GPIO_ENABLE_Pin,GPIO_PIN_RESET);
	}else{
	  HAL_GPIO_WritePin(GPIO_ENABLE_GPIO_Port,GPIO_ENABLE_Pin,GPIO_PIN_SET);
	}
}

//  @ fuction:  Stepper_ControlStep
//  @ description:  
//  @ input:
//  @ output:
//  @ note: 
void Stepper_ControlStep(uint8_t motor_num,Step_State_enum step_state){
  switch(motor_num){
		case AXIS_X:
			HAL_GPIO_WritePin(GPIO_STEP_X_GPIO_Port,GPIO_STEP_X_Pin,(GPIO_PinState)step_state);
			break;
		case AXIS_Y:
			HAL_GPIO_WritePin(GPIO_STEP_Y_GPIO_Port,GPIO_STEP_Y_Pin,(GPIO_PinState)step_state);
			break;
		case AXIS_Z:
			HAL_GPIO_WritePin(GPIO_STEP_Z_GPIO_Port,GPIO_STEP_Z_Pin,(GPIO_PinState)step_state);
			break;
		case AXIS_A:
			HAL_GPIO_WritePin(GPIO_STEP_A_GPIO_Port,GPIO_STEP_A_Pin,(GPIO_PinState)step_state);
			break;
		case AXIS_B:
			HAL_GPIO_WritePin(GPIO_STEP_B_GPIO_Port,GPIO_STEP_B_Pin,(GPIO_PinState)step_state);
			break;
		case AXIS_C:
			HAL_GPIO_WritePin(GPIO_STEP_C_GPIO_Port,GPIO_STEP_C_Pin,(GPIO_PinState)step_state);
			break;
		default:
			break;
	}
}

//  @ fuction:  Stepper_ControlDir
//  @ description:  
//  @ input:
//  @ output:
//  @ note: 
void Stepper_ControlDir(uint8_t motor_num,Dir_State_enum dir_state){
   switch(motor_num){
		case AXIS_X:
			HAL_GPIO_WritePin(GPIO_DIR_X_GPIO_Port,GPIO_DIR_X_Pin,(GPIO_PinState)dir_state);
			break;
		case AXIS_Y:
			HAL_GPIO_WritePin(GPIO_DIR_Y_GPIO_Port,GPIO_DIR_Y_Pin,(GPIO_PinState)dir_state);
			break;
		case AXIS_Z:
			HAL_GPIO_WritePin(GPIO_DIR_Z_GPIO_Port,GPIO_DIR_Z_Pin,(GPIO_PinState)dir_state);
			break;
		case AXIS_A:
			HAL_GPIO_WritePin(GPIO_DIR_A_GPIO_Port,GPIO_DIR_A_Pin,(GPIO_PinState)dir_state);
			break;
		case AXIS_B:
			HAL_GPIO_WritePin(GPIO_DIR_B_GPIO_Port,GPIO_DIR_B_Pin,(GPIO_PinState)dir_state);
			break;
		case AXIS_C:
			HAL_GPIO_WritePin(GPIO_DIR_C_GPIO_Port,GPIO_DIR_C_Pin,(GPIO_PinState)dir_state);
			break;
		default:
			break;
	}
}

//#define SPINDLE_TIMER htim9
//#define SPINDLE_TIMER_CHANNEL TIM_CHANNEL_1

//__HAL_TIM_SET_AUTORELOAD(&STEP_TIMER,st.exec_segment->cycles_per_tick);
//__HAL_TIM_SET_COMPARE(&STEP_TIMER,STEP_TIMER_CHANNEL,(uint16_t)(st.exec_segment->cycles_per_tick * 0.75));

void Stepper_Timer_init(void){
   //HAL����
	//TIM4ʱ������APB1 84MHz
//  HAL_TIM_Base_Start_IT(&htim4);//��������ж�
  HAL_TIM_OC_Start_IT(&STEP_TIMER,STEP_TIMER_CHANNEL);//��������Ƚ��ж�
	__HAL_TIM_ENABLE_IT(&STEP_TIMER, TIM_IT_UPDATE);//��������ж�
	__HAL_TIM_DISABLE(&STEP_TIMER);
	__HAL_TIM_SET_COUNTER(&STEP_TIMER,0);
	__HAL_TIM_SET_AUTORELOAD(&STEP_TIMER,STEP_TIMER_TEST);//��ʼֵ��1kHz
	__HAL_TIM_SET_COMPARE(&STEP_TIMER,STEP_TIMER_CHANNEL,(uint16_t)(STEP_TIMER_TEST * 3/4));//����ֵ��16λ ϵ��0.75
}



Stepper_t Stepper_data;
int32_t Sys_position[AXIS_N];
uint8_t Stepper_isAutoRun = 0;
uint8_t Stepper_isWaitLimit = 0;
uint32_t Step_TimerCount = STEP_TIMER_TEST;//Ĭ��1000Hz

void Stepper_MainISR(void)
{
	if(Stepper_data.step_count == 0){//�˴��������		
		return;
	}
	
	uint8_t key_limitState_temp = Contol_key_state.key_limit_state;
	if(Contol_key_state.enable_key_limit == 0){//���û��ʹ����λ
	   key_limitState_temp = 0;
	}
	
	__HAL_TIM_SET_AUTORELOAD(&STEP_TIMER,Step_TimerCount);//0.02ms ������50������ 
	__HAL_TIM_SET_COMPARE(&STEP_TIMER,STEP_TIMER_CHANNEL,(uint16_t)(Step_TimerCount * 3/4));//����ֵ��16λ ϵ��0.75
	
//		printf("Stepper_MainISR\r\n");
	// Reset step out bits.
	Stepper_data.step_outbits = 0;

	// Execute step displacement profile by Bresenham line algorithm
	Stepper_data.step_counter[AXIS_X] += Stepper_data.steps[AXIS_X];	
	if(Stepper_data.step_counter[AXIS_X] > Stepper_data.step_eventCount){
		if(!(key_limitState_temp & (1 << LIMIT_X))){//���û����λ,���˶�
		   Stepper_data.step_outbits |= (1<<AXIS_X);
		}
		Stepper_data.step_counter[AXIS_X] -= Stepper_data.step_eventCount;
		if(Stepper_data.dir_outbits &(1<<AXIS_X)){
		  Sys_position[AXIS_X]--;
		}
		else{
		  Sys_position[AXIS_X]++;
		}
	}
	
	// Execute step displacement profile by Bresenham line algorithm
	Stepper_data.step_counter[AXIS_Y] += Stepper_data.steps[AXIS_Y];	
	if(Stepper_data.step_counter[AXIS_Y] > Stepper_data.step_eventCount){
		if(!(key_limitState_temp & (1 << LIMIT_Y))){//���û����λ,���˶�
		   Stepper_data.step_outbits |= (1<<AXIS_Y);
		}
		Stepper_data.step_counter[AXIS_Y] -= Stepper_data.step_eventCount;
		if(Stepper_data.dir_outbits &(1<<AXIS_Y)){
		  Sys_position[AXIS_Y]--;
		}
		else{
		  Sys_position[AXIS_Y]++;
		}
	}
	
	// Execute step displacement profile by Bresenham line algorithm
	Stepper_data.step_counter[AXIS_Z] += Stepper_data.steps[AXIS_Z];	
	if(Stepper_data.step_counter[AXIS_Z] > Stepper_data.step_eventCount){
		if(!(key_limitState_temp & (1 << LIMIT_Z))){//���û����λ,���˶�
		   Stepper_data.step_outbits |= (1<<AXIS_Z);
		}
		Stepper_data.step_counter[AXIS_Z] -= Stepper_data.step_eventCount;
		if(Stepper_data.dir_outbits &(1<<AXIS_Z)){
		  Sys_position[AXIS_Z]--;
		}
		else{
		  Sys_position[AXIS_Z]++;
		}
	}
	
	// Execute step displacement profile by Bresenham line algorithm
	Stepper_data.step_counter[AXIS_A] += Stepper_data.steps[AXIS_A];	
	if(Stepper_data.step_counter[AXIS_A] > Stepper_data.step_eventCount){
		if(!(key_limitState_temp & (1 << LIMIT_A))){//���û����λ,���˶�
		   Stepper_data.step_outbits |= (1<<AXIS_A);
		}
		Stepper_data.step_counter[AXIS_A] -= Stepper_data.step_eventCount;
		if(Stepper_data.dir_outbits &(1<<AXIS_A)){
		  Sys_position[AXIS_A]--;
		}
		else{
		  Sys_position[AXIS_A]++;
		}
	}
	
	// Execute step displacement profile by Bresenham line algorithm
	Stepper_data.step_counter[AXIS_B] += Stepper_data.steps[AXIS_B];	
	if(Stepper_data.step_counter[AXIS_B] > Stepper_data.step_eventCount){
		if(!(key_limitState_temp & (1 << LIMIT_B))){//���û����λ,���˶�
		   Stepper_data.step_outbits |= (1<<AXIS_B);
		}
		Stepper_data.step_counter[AXIS_B] -= Stepper_data.step_eventCount;
		if(Stepper_data.dir_outbits &(1<<AXIS_B)){
		  Sys_position[AXIS_B]--;
		}
		else{
		  Sys_position[AXIS_B]++;
		}
	}
	
	// Execute step displacement profile by Bresenham line algorithm
	Stepper_data.step_counter[AXIS_C] += Stepper_data.steps[AXIS_C];	
	if(Stepper_data.step_counter[AXIS_C] > Stepper_data.step_eventCount){
		if(!(key_limitState_temp & (1 << LIMIT_C))){//���û����λ,���˶�
		   Stepper_data.step_outbits |= (1<<AXIS_C);
		}
		Stepper_data.step_counter[AXIS_C] -= Stepper_data.step_eventCount;
		if(Stepper_data.dir_outbits &(1<<AXIS_C)){
		  Sys_position[AXIS_C]--;
		}
		else{
		  Sys_position[AXIS_C]++;
		}
	}
	
	//���÷���
	Stepper_ControlDir(AXIS_X,(Dir_State_enum)(Stepper_data.dir_outbits &(1<<AXIS_X)));
	Stepper_ControlDir(AXIS_Y,(Dir_State_enum)(Stepper_data.dir_outbits &(1<<AXIS_Y)));
	Stepper_ControlDir(AXIS_Z,(Dir_State_enum)(Stepper_data.dir_outbits &(1<<AXIS_Z)));
	Stepper_ControlDir(AXIS_A,(Dir_State_enum)(Stepper_data.dir_outbits &(1<<AXIS_A)));
	Stepper_ControlDir(AXIS_B,(Dir_State_enum)(Stepper_data.dir_outbits &(1<<AXIS_B)));
	Stepper_ControlDir(AXIS_C,(Dir_State_enum)(Stepper_data.dir_outbits &(1<<AXIS_C)));
	
	//��������
	Stepper_ControlStep(AXIS_X,(Step_State_enum)(Stepper_data.step_outbits &(1<<AXIS_X)));
	Stepper_ControlStep(AXIS_Y,(Step_State_enum)(Stepper_data.step_outbits &(1<<AXIS_Y)));
	Stepper_ControlStep(AXIS_Z,(Step_State_enum)(Stepper_data.step_outbits &(1<<AXIS_Z)));
	Stepper_ControlStep(AXIS_A,(Step_State_enum)(Stepper_data.step_outbits &(1<<AXIS_A)));
	Stepper_ControlStep(AXIS_B,(Step_State_enum)(Stepper_data.step_outbits &(1<<AXIS_B)));
	Stepper_ControlStep(AXIS_C,(Step_State_enum)(Stepper_data.step_outbits &(1<<AXIS_C)));
	
	Stepper_data.step_count--;
	
	if(Stepper_data.step_count==0){
		__HAL_TIM_DISABLE(&STEP_TIMER);
		memset(&Stepper_data,0,sizeof(Stepper_data));
		// X
		Stepper_ControlStep(AXIS_X,Step_Low);
		// Y
		Stepper_ControlStep(AXIS_Y,Step_Low);
		// Z
		Stepper_ControlStep(AXIS_Z,Step_Low);
		// A
		Stepper_ControlStep(AXIS_A,Step_Low);
		// B
		Stepper_ControlStep(AXIS_B,Step_Low);
		// C
		Stepper_ControlStep(AXIS_C,Step_Low);
	}
//	printf("step_count=%d\r\n",Stepper_data.step_count);
//	printf("step_isRun=%d\r\n",Stepper_isAutoRun);
//	printf("%d\r\n",Stepper_data.dir_outbits);
	
}

void Stepper_PortResetISR(void)
{
	
//		printf("Stepper_PortResetISR\r\n");
	// Reset stepping pins (leave the direction pins)
	// X
	Stepper_ControlStep(AXIS_X,Step_Low);
	// Y
	Stepper_ControlStep(AXIS_Y,Step_Low);
	// Z
	Stepper_ControlStep(AXIS_Z,Step_Low);
	// A
	Stepper_ControlStep(AXIS_A,Step_Low);
	// B
	Stepper_ControlStep(AXIS_B,Step_Low);
	// C
	Stepper_ControlStep(AXIS_C,Step_Low);
	
//	if(Stepper_data.step_count==0){
//	__HAL_TIM_DISABLE(&STEP_TIMER);
//	}
	
//	__HAL_TIM_DISABLE(&STEP_TIMER);
}

//  @ fuction:  
//  @ description: TIM4 IRQ 
//  @ input:
//  @ output:
//  @ note: ����û�в���HAL��Ļص�
void TIM4_IRQHandler(void){
	/* Capture compare 1 event */
	if(__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_CC1) != RESET)
  {
    if(__HAL_TIM_GET_IT_SOURCE(&htim4, TIM_IT_CC1) !=RESET){ //TIM3 CH1        
		    Stepper_MainISR();// OC			
        __HAL_TIM_CLEAR_IT(&htim4, TIM_IT_CC1);
    }
  }
	/* TIM Update event */
	if(__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_UPDATE) != RESET){
		if(__HAL_TIM_GET_IT_SOURCE(&htim4, TIM_IT_UPDATE) !=RESET){ //��ʱ������ж�				
				Stepper_PortResetISR();// OVF				
				__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);				
		}
	}
	
}


//  @ fuction:  
//  @ description: �����趨stepper�Ĳ���
//  @ input:
//  @ output:
//  @ note: 
void Stepper_setParam(Stepper_t stepper_param,uint32_t stepper_fre){
	memset(&Stepper_data,0,sizeof(Stepper_data));
	//��ȡ��󲽳�
	Stepper_data.step_eventCount = STEP_MAX(stepper_param.steps[AXIS_X],stepper_param.steps[AXIS_Y]);
	Stepper_data.step_eventCount = STEP_MAX(Stepper_data.step_eventCount,stepper_param.steps[AXIS_Z]);
	Stepper_data.step_eventCount = STEP_MAX(Stepper_data.step_eventCount,stepper_param.steps[AXIS_A]);
	Stepper_data.step_eventCount = STEP_MAX(Stepper_data.step_eventCount,stepper_param.steps[AXIS_B]);
	Stepper_data.step_eventCount = STEP_MAX(Stepper_data.step_eventCount,stepper_param.steps[AXIS_C]);
			
	//����Stepper����
	if(Stepper_data.step_eventCount == 1){//����1/2=0.5,����������1/2=0,��ʱ�����˶�
		Stepper_data.step_counter[AXIS_X] = Stepper_data.step_counter[AXIS_Y] = Stepper_data.step_counter[AXIS_Z] = 1;
		Stepper_data.step_counter[AXIS_A] = Stepper_data.step_counter[AXIS_B] = Stepper_data.step_counter[AXIS_C] = 1;
	}
	else{
		 Stepper_data.step_counter[AXIS_X] = Stepper_data.step_counter[AXIS_Y] = Stepper_data.step_counter[AXIS_Z] = (Stepper_data.step_eventCount >> 1);
		 Stepper_data.step_counter[AXIS_A] = Stepper_data.step_counter[AXIS_B] = Stepper_data.step_counter[AXIS_C] = (Stepper_data.step_eventCount >> 1);
	}
	Stepper_data.step_count = Stepper_data.step_eventCount;//���ý�����־
	//����
	Stepper_data.steps[AXIS_X] = stepper_param.steps[AXIS_X];
	Stepper_data.steps[AXIS_Y] = stepper_param.steps[AXIS_Y];
	Stepper_data.steps[AXIS_Z] = stepper_param.steps[AXIS_Z];
	Stepper_data.steps[AXIS_A] = stepper_param.steps[AXIS_A];
	Stepper_data.steps[AXIS_B] = stepper_param.steps[AXIS_B];
	Stepper_data.steps[AXIS_C] = stepper_param.steps[AXIS_C];
	
	Stepper_data.dir_outbits = stepper_param.dir_outbits;
	//����������ʱ
	Step_TimerCount = (uint32_t)(F_TIMER_STEPPER / stepper_fre);
	if(stepper_fre > MAX_STEP_RATE_HZ){
			Step_TimerCount = STEP_TIMER_MIN;//��ֹ̫����ּ��������
	}

		__HAL_TIM_ENABLE(&STEP_TIMER);
}
