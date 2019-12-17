#ifndef _MPU6050_HANDLE_H_
#define _MPU6050_HANDLE_H_

#include <stdint.h>
#include <stdbool.h>

/* 坐标角度结构体 */
typedef struct Angle
{
    float X_Angle;
    float Y_Angle;
    float Z_Angle;
    
} MPU6050_Angle_s;

typedef struct{
	int16_t g_data[3]; //陀螺仪数据
	int16_t a_data[3]; //加速度数据
  float temper; //温度数据
} MPU6050_data_s;

typedef struct{
  MPU6050_Angle_s data_angle[3];
	MPU6050_data_s data_origin[3];
	float limit_data[3];//3个角度
} Angle_limit_s;

typedef enum{
   MPU6050_Pin_X,
	 MPU6050_Pin_Y,
	 MPU6050_Pin_Z
}MPU6050_pin_enum;

extern Angle_limit_s Angle_limit;

extern void Angle_Limit_init(void);
extern int MPU6050_Init(void);
extern int MPU6050_GetDataProcess(MPU6050_data_s *data_origin);
extern void MPU6050_Get_Angle(MPU6050_Angle_s *data, const MPU6050_data_s *data_origin);

extern void MPU6050_setAddPin_stateH(MPU6050_pin_enum pin_num);
extern void MPU6050_setAddPin_stateL_all(void);

extern void MPU6050_limit_handle(void);


#endif
