#include "mpu6050_handle.h"
#include "sd_hal_mpu6050.h"

#include "i2c.h"
#include "gpio.h"

//#include <math.h>
#include <stdio.h>
#include "arm_math.h"

#include "home_limit.h"
#include "Setting.h"

#ifndef M_PI
#define M_PI 3.1415926f
#endif

#ifndef PRINT_MSG
#define PRINT_MSG printf
#endif

#define MPU6050_DEBUG 1

/** Global begin **/
SD_MPU6050 mpu1;
Angle_limit_s Angle_limit;
/** Global end  **/

// @ function: 
// @ description:角度限制初始化
// @ input:
// @ note: 
void Angle_Limit_init(void){
   memset(&Angle_limit,0,sizeof(Angle_limit));
	
	 //Angle_limit.limit_data[0] = 89.0f;
	 //Angle_limit.limit_data[1] = 89.0f;
	 //Angle_limit.limit_data[2] = 89.0f;
	
	 Angle_limit.limit_data[0] = (float)settings.limit_angle[0]/10.0f;
	 Angle_limit.limit_data[1] = (float)settings.limit_angle[1]/10.0f;
	 Angle_limit.limit_data[2] = (float)settings.limit_angle[2]/10.0f;
	
	 MPU6050_setAddPin_stateL_all();
}

// @ function: 
// @ description:MPU6050初始化
// @ input:
// @ note: 
int MPU6050_Init(void){
  int ret = 0;
	SD_MPU6050_Result result = SD_MPU6050_Init(&hi2c2,&mpu1,SD_MPU6050_Device_1,SD_MPU6050_Accelerometer_2G,SD_MPU6050_Gyroscope_250s );//AD0接高
	//HAL_Delay(500);
	
	if(result == SD_MPU6050_Result_Ok){
		ret = 0;
	}
	else{
		ret = -1;
	}
	
	return ret;
}

// @ function: 
// @ description:MPU6050设置Add0为高的引脚
// @ input:
// @ note: 
void MPU6050_setAddPin_stateH(MPU6050_pin_enum pin_num){
	MPU6050_setAddPin_stateL_all();
   switch(pin_num){
		 case MPU6050_Pin_X:
			 HAL_GPIO_WritePin(GPIO_SPINDLE_DIR_GPIO_Port,GPIO_SPINDLE_DIR_Pin,GPIO_PIN_SET);
			 break;
		 case MPU6050_Pin_Y:
			 HAL_GPIO_WritePin(GPIO_COOL_MIST_GPIO_Port,GPIO_COOL_MIST_Pin,GPIO_PIN_SET);
			 break;
		 case MPU6050_Pin_Z:
			 HAL_GPIO_WritePin(GPIO_COOL_FLOOD_GPIO_Port,GPIO_COOL_FLOOD_Pin,GPIO_PIN_SET);
			 break;
		 default:
			 break;
	 }
}

// @ function: 
// @ description:MPU6050设置Add0为高的引脚(全部置低)
// @ input:
// @ note: 
void MPU6050_setAddPin_stateL_all(void){
   HAL_GPIO_WritePin(GPIO_SPINDLE_DIR_GPIO_Port,GPIO_SPINDLE_DIR_Pin,GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(GPIO_COOL_MIST_GPIO_Port,GPIO_COOL_MIST_Pin,GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(GPIO_COOL_FLOOD_GPIO_Port,GPIO_COOL_FLOOD_Pin,GPIO_PIN_RESET);
}



// @ function: 
// @ description:MPU6050获取数据信息
// @ input:
// @ note: 
int MPU6050_GetDataProcess(MPU6050_data_s *data_origin){
	  int ret = 0;
	
   	SD_MPU6050_ReadTemperature(&hi2c2,&mpu1);//获取温度
	  data_origin->temper = mpu1.Temperature;
	
	  SD_MPU6050_ReadGyroscope(&hi2c2,&mpu1); //获取陀螺仪数据
	  data_origin->g_data[0] = mpu1.Gyroscope_X;
	  data_origin->g_data[1] = mpu1.Gyroscope_Y;
	  data_origin->g_data[2] = mpu1.Gyroscope_Z;

	  SD_MPU6050_ReadAccelerometer(&hi2c2,&mpu1); //获取加速度数据
	  data_origin->a_data[0] = mpu1.Accelerometer_X;
	  data_origin->a_data[1] = mpu1.Accelerometer_Y;
	  data_origin->a_data[2] = mpu1.Accelerometer_Z;
	
	  return ret;
}

// @ function: 
// @ description:计算X y Z轴的倾角
// @ input:
// @ note: 
void MPU6050_Get_Angle(MPU6050_Angle_s *data,const MPU6050_data_s *data_origin){   
    /* 计算x, y, z 轴倾角，返回弧度值*/
    data->X_Angle = acos(data_origin->a_data[0] / 16384.0);
    data->Y_Angle = acos(data_origin->a_data[1] / 16384.0);
    data->Z_Angle = acos(data_origin->a_data[2] / 16384.0);

    /* 弧度值转换为角度值 */
    data->X_Angle = data->X_Angle * 57.29577f;
    data->Y_Angle = data->Y_Angle * 57.29577f;
    data->Z_Angle = data->Z_Angle * 57.29577f;
	  //data->Z_Angle = arm_sin_f32(M_PI/6);
}

// @ function: 
// @ description: 根据MPU6050的值进行限位处理
// @ input:
// @ note: 
void MPU6050_limit_handle(void){
	bool isMPU6050_work = false;
	//printf("press=%d\n",Contol_key_state.key_press_state);
	for(int i=0;i<3;i++){
		MPU6050_setAddPin_stateH((MPU6050_pin_enum)i);
		if(MPU6050_Init() == 0){
			#if MPU6050_DEBUG
			if(Contol_key_state.key_press_state & (1<< CONTROL_SAFETY_DOOR_BIT)){
			   PRINT_MSG("MPU[%d] WORK FINE\n",i);
		  }	 
			#endif
			 isMPU6050_work = true;
		}
		else{
			#if MPU6050_DEBUG
			if(Contol_key_state.key_press_state & (1<< CONTROL_SAFETY_DOOR_BIT)){
			   PRINT_MSG("MPU[%d] NOT WORKING\n",i);
		  }	
			#endif
			 isMPU6050_work = false;
		}
		if(isMPU6050_work){
			 MPU6050_GetDataProcess(&Angle_limit.data_origin[i]);
			 MPU6050_Get_Angle(&Angle_limit.data_angle[i],&Angle_limit.data_origin[i]);	
			 #if MPU6050_DEBUG 
			 if(Contol_key_state.key_press_state & (1<< CONTROL_SAFETY_DOOR_BIT)){
				 PRINT_MSG("TEMP[%d]: %0.2f\t", i,Angle_limit.data_origin[i].temper / 340.0f + 36.53f);	
				 PRINT_MSG("x=%.2f\ty=%.2f\tz=%.2f\n",Angle_limit.data_angle[i].X_Angle,Angle_limit.data_angle[i].Y_Angle,Angle_limit.data_angle[i].Z_Angle);
		   }	
			 #endif
			
			if(Contol_key_state.mode_mpu6050 == 1){
				if(Angle_limit.data_angle[i].Y_Angle > Angle_limit.limit_data[i]) {
					 Contol_key_state.key_limit_state |= (1<<i);
				}
				else{
					Contol_key_state.key_limit_state &= ~(1<<i);
				}
				
			}//mode_mpu6050
		}//isMPU6050_work
		
	}//for
}
