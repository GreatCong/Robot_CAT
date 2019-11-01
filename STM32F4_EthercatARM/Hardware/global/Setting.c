#include "Setting.h"
#include "eeprom.h"

Settings_t settings;

//  @ fuction:  Settings_Init
//  @ description: 初始化配置信息
//  @ input:
//  @ output:
//  @ note: 
void Settings_Init(void)
{
	EE_Init();

	if(!ReadGlobalSettings()) {
		//Report_StatusMessage(STATUS_SETTING_READ_FAIL);
		Settings_Restore(SETTINGS_RESTORE_ALL); // Force restore all EEPROM data.
		//Report_GrblSettings();
	}
}


//  @ fuction:  ReadGlobalSettings
//  @ description: 读取版本号(如果不对就设置为初始值)
//  @ input:
//  @ output:
//  @ note: 
uint8_t ReadGlobalSettings() {
	// Check version-byte of eeprom
	uint8_t version = EE_ReadByte(0);

	if(version == SETTINGS_VERSION) {
		// Read settings-record and check checksum
		if(!(EE_ReadByteArray((uint8_t*)&settings, EEPROM_ADDR_GLOBAL, sizeof(Settings_t)))) {
			return false;
		}
	}
	else {
		return false;
	}

	return true;
}


//  @ fuction:  WriteGlobalSettings
//  @ description: 写入版本号等信息
//  @ input:
//  @ output:
//  @ note: 此函数在空闲状态下使用(写flash会花费时间)
void WriteGlobalSettings(void)
{
	EE_WriteByte(0, SETTINGS_VERSION);
	EE_WriteByteArray(EEPROM_ADDR_GLOBAL, (uint8_t*)&settings, sizeof(Settings_t));

	EE_Program();
}

//  @ fuction:  Settings_Restore
//  @ description: 恢复默认设置
//  @ input:
//  @ output:
//  @ note: 
void Settings_Restore(uint8_t restore_flag) {
	if(restore_flag & SETTINGS_RESTORE_DEFAULTS) {
		settings.robot_id = DEFAULT_ROBOT_ID;//写入机器人ID
    settings.limit_run_dir = DEFAULT_LIMIT_RUN_DIR;
		settings.limit_enable_mask = DEFAULT_LIMIT_ENABLE_MASK;
		
		WriteGlobalSettings();
	}
}
