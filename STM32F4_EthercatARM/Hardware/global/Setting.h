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

#define DEFAULT_ROBOT_ID 0x0 //Ĭ�ϵĻ�����idΪ0
#define DEFAULT_LIMIT_RUN_DIR 0x03 //Ĭ�ϵķ�������Ϊ3,Ҳ����XY����ȡ��
#define DEFAULT_LIMIT_ENABLE_MASK 0x07 //Ĭ��XYZ 3����λ

typedef struct{
   uint8_t robot_id;//�����˵�id
	 uint8_t limit_run_dir;//���Զ���λ��ʱ��,���÷���
	 uint8_t limit_enable_mask;//���Զ���λ��ʱ��,Ҫʹ�ܵ���λ
}Settings_t;

extern Settings_t settings;

void Settings_Init(void);
uint8_t ReadGlobalSettings(void);
void Settings_Restore(uint8_t restore_flag);
void WriteGlobalSettings(void);

#endif
