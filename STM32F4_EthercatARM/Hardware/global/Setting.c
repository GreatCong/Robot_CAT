#include "Setting.h"
#include "eeprom.h"

Settings_t settings;

//  @ fuction:  Settings_Init
//  @ description: ��ʼ��������Ϣ
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
//  @ description: ��ȡ�汾��(������Ծ�����Ϊ��ʼֵ)
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
//  @ description: д��汾�ŵ���Ϣ
//  @ input:
//  @ output:
//  @ note: �˺����ڿ���״̬��ʹ��(дflash�Ứ��ʱ��)
void WriteGlobalSettings(void)
{
	EE_WriteByte(0, SETTINGS_VERSION);
	EE_WriteByteArray(EEPROM_ADDR_GLOBAL, (uint8_t*)&settings, sizeof(Settings_t));

	EE_Program();
}

//  @ fuction:  Settings_Restore
//  @ description: �ָ�Ĭ������
//  @ input:
//  @ output:
//  @ note: 
void Settings_Restore(uint8_t restore_flag) {
	if(restore_flag & SETTINGS_RESTORE_DEFAULTS) {
		settings.robot_id = DEFAULT_ROBOT_ID;//д�������ID
    settings.limit_run_dir = DEFAULT_LIMIT_RUN_DIR;
		settings.limit_enable_mask = DEFAULT_LIMIT_ENABLE_MASK;
		
		WriteGlobalSettings();
	}
}
