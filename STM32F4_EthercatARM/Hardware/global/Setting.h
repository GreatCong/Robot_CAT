#ifndef __SETTING_H_
#define __SETTING_H_

#include <stdint.h>
#include <stdbool.h>

#define EEPROM_ADDR_GLOBAL         			1U
#define EEPROM_ADDR_PARAMETERS     			512U
#define EEPROM_ADDR_STARTUP_BLOCK  			768U
#define EEPROM_ADDR_BUILD_INFO     			942U

#define SETTINGS_VERSION 					2  // NOTE: Check settings_reset() when moving to next version.

#ifndef SETTINGS_RESTORE_ALL
	#define SETTINGS_RESTORE_ALL 			0xFF // All bitflags
#endif


// Bit field and masking macros
#define BIT(n) 						(1 << n)
#define BIT_TRUE_ATOMIC(x,mask) 	BIT_TRUE(x,mask)
#define BIT_FALSE_ATOMIC(x,mask) 	BIT_FALSE(x,mask)
#define BIT_TOGGLE_ATOMIC(x,mask) 	(x) ^= (mask)
#define BIT_TRUE(x,mask) 			(x) |= (mask)
#define BIT_FALSE(x,mask) 			(x) &= ~(mask)
#define BIT_IS_TRUE(x,mask) 		((x & mask) != 0)
#define BIT_IS_FALSE(x,mask) 		((x & mask) == 0)

// Define settings restore bitflags.
#define SETTINGS_RESTORE_DEFAULTS 			BIT(0)
#define SETTINGS_RESTORE_PARAMETERS 		BIT(1)
#define SETTINGS_RESTORE_STARTUP_LINES 		BIT(2)
#define SETTINGS_RESTORE_BUILD_INFO 		BIT(3)

#define DEFAULT_ROBOT_ID 0x0 //默认的机器人id为0
#define DEFAULT_LIMIT_RUN_DIR 0x03 //默认的方向设置为3,也就是XY方向取反
#define DEFAULT_LIMIT_ENABLE_MASK 0x07 //默认XYZ 3个限位
#define DEFAULT_LIMIT_MODE 0x0 //默认是用限位开关
#define DEFAULT_LIMIT_GOHOME_SPEED 1500 //默认是限位速度为1500
#define DEFAULT_LIMIT_ANGLE_X 890 //限位角度X 89.0度
#define DEFAULT_LIMIT_ANGLE_Y 890 //限位角度Y 89.0度
#define DEFAULT_LIMIT_ANGLE_Z 890 //限位角度Z 89.0度

typedef struct{
   uint8_t robot_id;//机器人的id
	 uint8_t limit_run_dir;//在自动归位的时候,设置方向
	 uint8_t limit_enable_mask;//在自动归位的时候,要使能的限位
	 uint8_t limit_mode;//选择是限位开关还是角度传感器
	 uint16_t limit_goHome_speed;//限位归位的时候的速度
	 uint16_t limit_angle[3];//用MPU6050限位的角度(放大了10倍,0-180)
}Settings_t;

extern Settings_t settings;

void Settings_Init(void);
uint8_t ReadGlobalSettings(void);
void Settings_Restore(uint8_t restore_flag);
void WriteGlobalSettings(void);

#endif
