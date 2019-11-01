/**
\addtogroup EL9800Appl EL9800 application
@{
*/

/**
\file el9800appl.c
\author EthercatSSC@beckhoff.com
\brief Implementation

\version 5.11

<br>Changes to version V5.10:<br>
V5.11 ECAT11: create application interface function pointer, add eeprom emulation interface functions<br>
V5.11 EL9800 1: reset outputs on fallback from OP state<br>
<br>Changes to version V5.01:<br>
V5.10 ECAT6: Add "USE_DEFAULT_MAIN" to enable or disable the main function<br>
<br>Changes to version V5.0:<br>
V5.01 EL9800 2: Add TxPdo Parameter object 0x1802<br>
<br>Changes to version V4.30:<br>
V4.50 ECAT2: Create generic application interface functions. Documentation in Application Note ET9300.<br>
V4.50 COE2: Handle invalid PDO assign values.<br>
V4.30 : create file
*/


/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"
//#include "global.h"	

#if EL9800_APPLICATION

/* ECATCHANGE_START(V5.11) ECAT11*/
#include "applInterface.h"
/* ECATCHANGE_END(V5.11) ECAT11*/

//#include "el9800hw.h"

#define _EVALBOARD_
#include "el9800appl.h"
#undef _EVALBOARD_

#if MCI_HW
#include "mcihw.h"
#endif
#if EL9800_HW
#include "el9800hw.h"
#endif

/*** user define   **/
#include "protocol.h"
#include "Stepper.h"
#include "string.h"
#include "Setting.h"
extern Ethercat_data_parameter Ethercat_data;
/*** user define end  **/

/*--------------------------------------------------------------------------------------
------
------    local types and defines
------
--------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    local variables and constants
------
-----------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    application specific functions
------
-----------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    generic functions
------
-----------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    The function is called when an error state was acknowledged by the master

*////////////////////////////////////////////////////////////////////////////////////////

void    APPL_AckErrorInd(UINT16 stateTrans)
{ 
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from INIT to PREOP when
           all general settings were checked to start the mailbox handler. This function
           informs the application about the state transition, the application can refuse
           the state transition when returning an AL Status error code.
           The return code NOERROR_INWORK can be used, if the application cannot confirm
           the state transition immediately, in that case the application need to be complete 
           the transition by calling ECAT_StateChange.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartMailboxHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from PREEOP to INIT
             to stop the mailbox handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopMailboxHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    pIntMask    pointer to the AL Event Mask which will be written to the AL event Mask
                        register (0x204) when this function is succeeded. The event mask can be adapted
                        in this function
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from PREOP to SAFEOP when
             all general settings were checked to start the input handler. This function
             informs the application about the state transition, the application can refuse
             the state transition when returning an AL Status error code.
            The return code NOERROR_INWORK can be used, if the application cannot confirm
            the state transition immediately, in that case the application need to be complete 
            the transition by calling ECAT_StateChange.
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartInputHandler(UINT16 *pIntMask)
{
	  Stepper_Enable(true);//使能电机
//	MotorTools_setPWM(2000);
	memset(&Stepper_data,0,sizeof(Stepper_data));
	Ethercat_data.cmd.step_setting = 0;
		STEP_SETTING_Obj7000 = 0;
		Stepper_isAutoRun = 0;
//	__HAL_TIM_ENABLE(&STEP_TIMER);
	//Ethercat_data.ch.data[0] = settings.robot_id;//先将一个AD通道当作id的输入(测试用)
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from SAFEOP to PREEOP
             to stop the input handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopInputHandler(void)
{
	  Stepper_Enable(false);//失能电机
	MotorTools_ResetPWM();
	  memset(&Stepper_data,0,sizeof(Stepper_data));
		Ethercat_data.cmd.step_setting = 0;
	Stepper_isAutoRun = 0;
	STEP_SETTING_Obj7000 = 0;//缓冲，否则会自动运行
//	__HAL_TIM_DISABLE(&STEP_TIMER);//添加只能走一次step中断
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from SAFEOP to OP when
             all general settings were checked to start the output handler. This function
             informs the application about the state transition, the application can refuse
             the state transition when returning an AL Status error code.
           The return code NOERROR_INWORK can be used, if the application cannot confirm
           the state transition immediately, in that case the application need to be complete 
           the transition by calling ECAT_StateChange.
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartOutputHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from OP to SAFEOP
             to stop the output handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopOutputHandler(void)
{
//	data[0] = 0;
//	data[1] = 0;
//	data[2] = 0;
//	data[3] = 0;
//	cmd[0] = 0;
//	cmd[1] = 0;
//	cmd[2] = 0;
//	cmd[3] = 0;
	
	return ALSTATUSCODE_NOERROR;//add by lcj
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\return     0(ALSTATUSCODE_NOERROR), NOERROR_INWORK
\param      pInputSize  pointer to save the input process data length
\param      pOutputSize  pointer to save the output process data length

\brief    This function calculates the process data sizes from the actual SM-PDO-Assign
            and PDO mapping
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 APPL_GenerateMapping(UINT16* pInputSize,UINT16* pOutputSize)
{
#if COE_SUPPORTED
    UINT16 result = ALSTATUSCODE_NOERROR;
    UINT16 PDOAssignEntryCnt = 0;
    OBJCONST TOBJECT OBJMEM * pPDO = NULL;
    UINT16 PDOSubindex0 = 0;
    UINT32 *pPDOEntry = NULL;
    UINT16 PDOEntryCnt = 0;
    UINT16 InputSize = 0;
    UINT16 OutputSize = 0;

    /*Scan object 0x1C12 RXPDO assign*/
    for(PDOAssignEntryCnt = 0; PDOAssignEntryCnt < sRxPDOassign.u16SubIndex0; PDOAssignEntryCnt++)
    {
        pPDO = OBJ_GetObjectHandle(sRxPDOassign.aEntries[PDOAssignEntryCnt]);
        if(pPDO != NULL)
        {
            PDOSubindex0 = *((UINT16 *)pPDO->pVarPtr);
            for(PDOEntryCnt = 0; PDOEntryCnt < PDOSubindex0; PDOEntryCnt++)
            {
			//				result = *(UINT8 *)pPDO->pVarPtr;
			//				result=(OBJ_GetEntryOffset((PDOEntryCnt+1),pPDO)>>3);
                pPDOEntry = (UINT32 *)((UINT8 *)pPDO->pVarPtr + (OBJ_GetEntryOffset((PDOEntryCnt+1),pPDO)>>3));    //goto PDO entry
                // we increment the expected output size depending on the mapped Entry
                OutputSize += (UINT16) ((*pPDOEntry) & 0xFF);
            }
        }
        else
        {
            /*assigned PDO was not found in object dictionary. return invalid mapping*/
            OutputSize = 0;
            result = ALSTATUSCODE_INVALIDOUTPUTMAPPING;
            break;
        }
    }

    OutputSize = (OutputSize + 7) >> 3;

    if(result == 0)
    {
        /*Scan Object 0x1C13 TXPDO assign*/
        for(PDOAssignEntryCnt = 0; PDOAssignEntryCnt < sTxPDOassign.u16SubIndex0; PDOAssignEntryCnt++)
        {
            pPDO = OBJ_GetObjectHandle(sTxPDOassign.aEntries[PDOAssignEntryCnt]);
            if(pPDO != NULL)
            {
                PDOSubindex0 = *((UINT16 *)pPDO->pVarPtr);
                for(PDOEntryCnt = 0; PDOEntryCnt < PDOSubindex0; PDOEntryCnt++)
                {
                    pPDOEntry = (UINT32 *)((UINT8 *)pPDO->pVarPtr + (OBJ_GetEntryOffset((PDOEntryCnt+1),pPDO)>>3));    //goto PDO entry
                    // we increment the expected output size depending on the mapped Entry
                    InputSize += (UINT16) ((*pPDOEntry) & 0xFF);
                }
            }
            else
            {
                /*assigned PDO was not found in object dictionary. return invalid mapping*/
                InputSize = 0;
                result = ALSTATUSCODE_INVALIDINPUTMAPPING;
                break;
            }
        }
    }
    InputSize = (InputSize + 7) >> 3;

    *pInputSize = InputSize;
    *pOutputSize = OutputSize;
    return result;
#else
    *pInputSize = 6;
    *pOutputSize = 6;
    return ALSTATUSCODE_NOERROR;
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////
/**
\param      pData  pointer to input process data
\brief      This function will copies the inputs from the local memory to the ESC memory
            to the hardware
*////////////////////////////////////////////////////////////////////////////////////////
#if _STM32_IO4 && AL_EVENT_ENABLED
/* the pragma interrupt_level is used to tell the compiler that these functions will not
   be called at the same time from the main function and the interrupt routine */
#pragma interrupt_level 1
#endif


void APPL_InputMapping(UINT16* pData)
{
    UINT16 j = 0;
    UINT16 *pTmpData = (UINT16 *)pData;

    /* we go through all entries of the TxPDO Assign object to get the assigned TxPDOs */
   for (j = 0; j < sTxPDOassign.u16SubIndex0; j++)
   {
      switch (sTxPDOassign.aEntries[j])
      {
      /* TxPDO 1 */
      case 0x1A00:
         *pTmpData++ = MOTOR_MESSAGE_Obj6000;
			   *pTmpData++ = ERROR_MSG_Obj6010;
			   *pTmpData++ = AD_CH1_Obj6020;
			   *pTmpData++ = AD_CH2_Obj6030;
         break;
      }
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\param      pData  pointer to output process data

\brief    This function will copies the outputs from the ESC memory to the local memory
            to the hardware
*////////////////////////////////////////////////////////////////////////////////////////
#if _STM32_IO4  && AL_EVENT_ENABLED
/* the pragma interrupt_level is used to tell the compiler that these functions will not
   be called at the same time from the main function and the interrupt routine */
#pragma interrupt_level 1
#endif
void APPL_OutputMapping(UINT16* pData)
{
    UINT16 j = 0;
    UINT16 *pTmpData = (UINT16 *)pData;

    /* we go through all entries of the RxPDO Assign object to get the assigned RxPDOs */
    for (j = 0; j < sRxPDOassign.u16SubIndex0; j++)
    {
        switch (sRxPDOassign.aEntries[j])
        {
        /* RxPDO 2 */
        case 0x1600:
					  STEP_SETTING_Obj7000 = SWAPWORD(*pTmpData++);
				    STEP_FRE_Obj7010 = SWAPWORD(*pTmpData++);
            MOTOR_DIR_Obj7020 = SWAPWORD(*pTmpData++);
				
						//MOTOR1_STEP_Obj7030 = SWAPDWORD(*pTmpData++);
//				printf("step1=%x",*pTmpData++);//0xaabbccdd中0xccdd
//				printf("step2=%x",*pTmpData++);//0xaabbccdd中0xaabb
				    MOTOR1_STEP_Obj7030 = SWAPDWORD(*pTmpData)|(SWAPDWORD(*(pTmpData+1))<<16);
				    pTmpData+=2;
						MOTOR2_STEP_Obj7040 = SWAPDWORD(*pTmpData)|(SWAPDWORD(*(pTmpData+1))<<16);
						pTmpData+=2;
						MOTOR3_STEP_Obj7050 = SWAPDWORD(*pTmpData)|(SWAPDWORD(*(pTmpData+1))<<16);
						pTmpData+=2;
						MOTOR4_STEP_Obj7060 = SWAPDWORD(*pTmpData)|(SWAPDWORD(*(pTmpData+1))<<16);
						pTmpData+=2;
						MOTOR5_STEP_Obj7070 = SWAPDWORD(*pTmpData)|(SWAPDWORD(*(pTmpData+1))<<16);
				    pTmpData+=2;
						MOTOR6_STEP_Obj7080 = SWAPDWORD(*pTmpData)|(SWAPDWORD(*(pTmpData+1))<<16);
						pTmpData+=2;
						MOTOR7_STEP_Obj7090 = SWAPDWORD(*pTmpData)|(SWAPDWORD(*(pTmpData+1))<<16);
				    
//				    printf("step1=%lx\r\n",MOTOR1_STEP_Obj7030);
//						printf("step2=%lx\r\n",MOTOR2_STEP_Obj7040);
//						printf("step3=%lx\r\n",MOTOR3_STEP_Obj7050);
//						printf("step4=%lx\r\n",MOTOR4_STEP_Obj7060);
//						printf("step5=%lx\r\n",MOTOR5_STEP_Obj7070);				
            break;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\brief    This function will called from the synchronisation ISR 
            or from the mainloop if no synchronisation is supported
*////////////////////////////////////////////////////////////////////////////////////////

void APPL_Application(void)
{
	Ethercat_data.ch.data[0] = settings.robot_id;//先将一个AD通道当作id的输入(测试用)
	
	MOTOR_MESSAGE_Obj6000 = Ethercat_data.ch.motor_MSG;
	ERROR_MSG_Obj6010 = Ethercat_data.ch.error_MSG;
	AD_CH1_Obj6020 = Ethercat_data.ch.data[0];
	AD_CH2_Obj6030 = Ethercat_data.ch.data[1];
	
	Ethercat_data.cmd.step_setting = STEP_SETTING_Obj7000;
	Ethercat_data.cmd.step_fre = STEP_FRE_Obj7010;
	Ethercat_data.cmd.dir = MOTOR_DIR_Obj7020;
	Ethercat_data.cmd.step[AXIS_X] = MOTOR1_STEP_Obj7030;
	Ethercat_data.cmd.step[AXIS_Y] = MOTOR2_STEP_Obj7040;
	Ethercat_data.cmd.step[AXIS_Z] = MOTOR3_STEP_Obj7050;
	Ethercat_data.cmd.step[AXIS_A] = MOTOR4_STEP_Obj7060;
	Ethercat_data.cmd.step[AXIS_B] = MOTOR5_STEP_Obj7070;
	Ethercat_data.cmd.step[AXIS_C] = MOTOR6_STEP_Obj7080;
	Ethercat_data.cmd.step[AXIS_TOOL1] = MOTOR7_STEP_Obj7090;
  
//	//控制步进电机
//  Stepper_ControlDir(AXIS_X,Ethercat_data.cmd.dir[0]);
//	Stepper_ControlStep(AXIS_X,Ethercat_data.cmd.step[0]);
//	
//	Stepper_ControlDir(AXIS_Y,Ethercat_data.cmd.dir[1]);
//	Stepper_ControlStep(AXIS_Y,Ethercat_data.cmd.step[1]);
//	
//	Stepper_ControlDir(AXIS_Z,Ethercat_data.cmd.dir[2]);
//	Stepper_ControlStep(AXIS_Z,Ethercat_data.cmd.step[2]);
	
//	memeset(&Stepper_data,0,sizeof(Stepper_data));

				
//	if(Stepper_data.is_AutoRun){
//		if(Stepper_data.step_count==0){
//		  Stepper_data.is_AutoRun = 0;
//		}
//	}
	//调试
	MOTOR_MESSAGE_Obj6000 = Stepper_data.step_count;
	ERROR_MSG_Obj6010 = Stepper_isAutoRun;
//	AD_CH1_Obj6020 = MOTOR_TOOL_TIMER.Instance->CCR1;
	
  if(Ethercat_data.cmd.step_setting &(1<<MOTION_ENABLE_BIT)){//加一个使能，否则在扫描的时候就会因为初值而运动
//		printf("step_setting=%x\r\n",Ethercat_data.cmd.step_setting);
		if(Stepper_isAutoRun){
		   if(Ethercat_data.cmd.step_setting &(1<<MOTION_AUTORUN_STOP_BIT)){//如果设置手动更新，停止自动更新
			    Stepper_isAutoRun = 0;
				 ERROR_MSG_Obj6010 = 2;//直接赋值,防止后面有start变成1，保证有一个脉冲
			 }
		}
		
		//吸盘的处理
//		MotorTools_setPWM(Ethercat_data.cmd.tools);
			MotorTools_setPWM(Ethercat_data.cmd.step[AXIS_TOOL1]);
		
	  if(Stepper_isAutoRun==0){ //不是自动运行就更新数据
			memset(&Stepper_data,0,sizeof(Stepper_data));
			//获取最大步长
			Stepper_data.step_eventCount = STEP_MAX(Ethercat_data.cmd.step[AXIS_X],Ethercat_data.cmd.step[AXIS_Y]);
			Stepper_data.step_eventCount = STEP_MAX(Stepper_data.step_eventCount,Ethercat_data.cmd.step[AXIS_Z]);
			
			//设置Stepper参数
			if(Stepper_data.step_eventCount == 1){//正常1/2=0.5,但是整形中1/2=0,此时不会运动
				Stepper_data.step_counter[AXIS_X] = Stepper_data.step_counter[AXIS_Y] = Stepper_data.step_counter[AXIS_Z] = 1;
				Stepper_data.step_counter[AXIS_A] = Stepper_data.step_counter[AXIS_B] = Stepper_data.step_counter[AXIS_C] = 1;
			}
			else{
				 Stepper_data.step_counter[AXIS_X] = Stepper_data.step_counter[AXIS_Y] = Stepper_data.step_counter[AXIS_Z] = (Stepper_data.step_eventCount >> 1);
				 Stepper_data.step_counter[AXIS_A] = Stepper_data.step_counter[AXIS_B] = Stepper_data.step_counter[AXIS_C] = (Stepper_data.step_eventCount >> 1);
			}
			Stepper_data.step_count = Stepper_data.step_eventCount;//设置结束标志
			//步长
			Stepper_data.steps[AXIS_X] = Ethercat_data.cmd.step[AXIS_X];
			Stepper_data.steps[AXIS_Y] = Ethercat_data.cmd.step[AXIS_Y];
			Stepper_data.steps[AXIS_Z] = Ethercat_data.cmd.step[AXIS_Z];
			Stepper_data.steps[AXIS_A] = Ethercat_data.cmd.step[AXIS_A];
			Stepper_data.steps[AXIS_B] = Ethercat_data.cmd.step[AXIS_B];
			Stepper_data.steps[AXIS_C] = Ethercat_data.cmd.step[AXIS_C];
			//方向
	//		Stepper_data.dir_outbits = 0;	
	//		if(Ethercat_data.cmd.dir[AXIS_X]>0){
	//			Stepper_data.dir_outbits |= (1<<AXIS_X);
	//		}
	//		if(Ethercat_data.cmd.dir[AXIS_Y]>0){
	//			Stepper_data.dir_outbits |= (1<<AXIS_Y);
	//		}	
	//		if(Ethercat_data.cmd.dir[AXIS_Z]>0){
	//			Stepper_data.dir_outbits |= (1<<AXIS_Z);
	//		}
			Stepper_data.dir_outbits = Ethercat_data.cmd.dir;	

	//    Stepper_data.dir_outbits = Ethercat_data.cmd.dir[AXIS_X] | 
	//		                           Ethercat_data.cmd.dir[AXIS_Y] | 
	//															 Ethercat_data.cmd.dir[AXIS_Z];
		
	}
		//设置脉冲延时
		Step_TimerCount = (uint32_t)(F_TIMER_STEPPER / Ethercat_data.cmd.step_fre);
		if(Ethercat_data.cmd.step_fre > MAX_STEP_RATE_HZ)
			Step_TimerCount = STEP_TIMER_MIN;//防止太快出现尖锐的响声
		
//		__HAL_TIM_SET_AUTORELOAD(&STEP_TIMER,Step_TimerCount);//0.02ms 最大接收50个脉冲
//		__HAL_TIM_SET_COMPARE(&STEP_TIMER,STEP_TIMER_CHANNEL,(uint16_t)(Step_TimerCount * 0.75));
//		__HAL_TIM_SET_COUNTER(&STEP_TIMER,0);
		
			if(Ethercat_data.cmd.step_setting &(1<<MOTION_AUTORUN_START_BIT)){//自动运行
		    Stepper_isAutoRun = 1;//当count==0时，Stepper_data.is_AutoRun = 0
	   }

	//最大频率的脉冲有尖锐的响声
	//	__HAL_TIM_SET_AUTORELOAD(&STEP_TIMER,STEP_TIMER_MIN);//0.02ms 最大接收50个脉冲
	//	__HAL_TIM_SET_COMPARE(&STEP_TIMER,STEP_TIMER_CHANNEL,(uint16_t)(STEP_TIMER_MIN * 0.75));
		__HAL_TIM_ENABLE(&STEP_TIMER);
//		printf("xx=%d\r\n",Ethercat_data.cmd.dir[3]);
		
		//设置sys_position反馈(暂时不考虑)
	}
  else{
		memset(&Stepper_data,0,sizeof(Stepper_data));
	  __HAL_TIM_DISABLE(&STEP_TIMER);
		MotorTools_ResetPWM();
	}	
	Ethercat_data.cmd.step_setting &= ~(1<<MOTION_ENABLE_BIT);


	
	
	
}


#if EXPLICIT_DEVICE_ID
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    The Explicit Device ID of the EtherCAT slave

 \brief     Calculate the Explicit Device ID
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 APPL_GetDeviceID()
{
    UINT16 Value = (UINT16)(SWITCH_8<<7)|(SWITCH_7<<6)|(SWITCH_6<<5)|(SWITCH_5<<4)|(SWITCH_4<<3)|(SWITCH_3<<2)|(SWITCH_2<<1)|(SWITCH_1);
    return Value;
}
#endif

#if USE_DEFAULT_MAIN
/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This is the main function

*////////////////////////////////////////////////////////////////////////////////////////

#endif //#if USE_DEFAULT_MAIN
#endif //#if EL9800_APPLICATION

/** @} */
