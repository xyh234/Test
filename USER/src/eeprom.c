#include "n32g43x_flash.h"
#include <declare.h>
#include <eeprom.h>

uint32_t eep_buff[300];

void eeprom_write_long(uint32_t addr, uint32_t ucdata)
{
    uint8_t i;

    eep_buff[0] = eeprom_read_long(EEPROM_ADDR_RESISTANCE_MIN);
    eep_buff[1] = eeprom_read_long(EEPROM_ADDR_RESISTANCE_MAX);
    eep_buff[2] = eeprom_read_long(EEPROM_ADDR_SLEEP1);
    eep_buff[3] = 0;//eeprom_read_long(EEPROM_ADDR_UPDATE);
    eep_buff[4] = eeprom_read_long(EEPROM_ADDR_RESISTANCE_LEVEL);
    eep_buff[5] = eeprom_read_long(EEPROM_ADDR_HistroyNumber);

    eep_buff[(addr - EEPROM_ADDR_INI) / 4] = ucdata;

//		/* Configures the Internal High Speed oscillator */
//		if(FLASH_HSICLOCK_DISABLE == FLASH_ClockInit())
//		{
//				while(1);
//		}
		
   /* Unlocks the FLASH Program Erase Controller */
   FLASH_Unlock();

   /* Erase */
   if (FLASH_COMPL != FLASH_EraseOnePage(EEPROM_ADDR_INI))
   {
		 
   }

    for(i = 0; i < 6; i++)
    {
        FLASH_ProgramWord(EEPROM_ADDR_INI + 4 * i, eep_buff[i]); //EFM_SingleProgram(addr,ucdata);
    }

    /* Locks the FLASH Program Erase Controller */
    FLASH_Lock();
}

#if 1
void eeprom_writeHistory_long(uint32_t addr, History* ucdata)
{
    uint16_t i;
    uint32_t tmp32;

    for(i = 0; i < 300; i++)
    {
        tmp32 = EEPROM_ADDR_HISTORY_INI + 4 * i;
        eep_buff[i] = eeprom_read_long(tmp32);//(EEPROM_ADDR_HISTORY_INI+4*i);
    }		
		
    eep_buff[(addr - EEPROM_ADDR_HISTORY_INI) / 4] = ucdata->HIS_timeFor500m;
    eep_buff[(addr - EEPROM_ADDR_HISTORY_INI) / 4 + 1] = ucdata->HIS_timeFor500m_avg;
    eep_buff[(addr - EEPROM_ADDR_HISTORY_INI) / 4 + 2] = ucdata->HIS_watts;
    eep_buff[(addr - EEPROM_ADDR_HISTORY_INI) / 4 + 3] = ucdata->HIS_watts_avg;
    eep_buff[(addr - EEPROM_ADDR_HISTORY_INI) / 4 + 4] = ucdata->HIS_spm;
    eep_buff[(addr - EEPROM_ADDR_HISTORY_INI) / 4 + 5] = ucdata->HIS_DistFor30min;
    eep_buff[(addr - EEPROM_ADDR_HISTORY_INI) / 4 + 6] = ucdata->HIS_distance;
    eep_buff[(addr - EEPROM_ADDR_HISTORY_INI) / 4 + 7] = ucdata->HIS_calories;
    eep_buff[(addr - EEPROM_ADDR_HISTORY_INI) / 4 + 8] = ucdata->HIS_caloriesPerHour;
    eep_buff[(addr - EEPROM_ADDR_HISTORY_INI) / 4 + 9] = ucdata->HIS_time;
    eep_buff[(addr - EEPROM_ADDR_HISTORY_INI) / 4 + 10] = ucdata->HIS_stroke;
    eep_buff[(addr - EEPROM_ADDR_HISTORY_INI) / 4 + 11] = ucdata->HIS_Drag_coeff;
    eep_buff[(addr - EEPROM_ADDR_HISTORY_INI) / 4 + 12] = ucdata->HIS_hr;	
		
		
		
//		/* Configures the Internal High Speed oscillator */
//		if(FLASH_HSICLOCK_DISABLE == FLASH_ClockInit())
//		{
//				while(1);
//		}
		
   /* Unlocks the FLASH Program Erase Controller */
   FLASH_Unlock();

   /* Erase */
   if (FLASH_COMPL != FLASH_EraseOnePage(EEPROM_ADDR_HISTORY_INI))
   {
		 
   }

    for(i = 0; i < 300; i++)
    {
        FLASH_ProgramWord(EEPROM_ADDR_HISTORY_INI + 4 * i, eep_buff[i]); //EFM_SingleProgram(addr,ucdata);
    }


    /* Locks the FLASH Program Erase Controller */
    FLASH_Lock();
}

#endif

History historyRecord_t[20];

euchar fw_update, beep_on, time2sleep, target_age;
void eeprom_read(void)
{
    uint8_t i;
    uint32_t tmp32;
    resistance_min  = eeprom_read_long(EEPROM_ADDR_RESISTANCE_MIN);

    if(resistance_min >= 0xFFFF) resistance_min = 0;

    resistance_max = eeprom_read_long(EEPROM_ADDR_RESISTANCE_MAX);

    if(resistance_max >= 0xFFFF) resistance_max = 0;
	
    resistance_level_max = eeprom_read_long(EEPROM_ADDR_RESISTANCE_LEVEL);

    if(resistance_level_max > 16 || resistance_level_max < 6) resistance_level_max = 10;	

    time2sleep = eeprom_read_long(EEPROM_ADDR_SLEEP1);

    if(time2sleep > 99 || time2sleep == 0) time2sleep = 4;//1;//5;

    tmp32 = eeprom_read_long(EEPROM_ADDR_UPDATE);

    if(tmp32 != 0) eeprom_write_long(EEPROM_ADDR_UPDATE, 0ul);

    fw_update = 0;

    //target_age = eeprom_read_long(EEPROM_ADDR_AGE);
    //if(target_age>120||target_age<10) target_age=25;
	
#if 1
    historyRecordCnt = eeprom_read_long(EEPROM_ADDR_HistroyNumber);

    if(historyRecordCnt >= 20) historyRecordCnt = 0;
		
    for(i = 0; i < 20; i++)
    {
        tmp32 = eeprom_read_long(EEPROM_ADDR_HistroyPara(i * 52));

        if(tmp32 >= 6000)
            historyRecord[i].HIS_timeFor500m = 0;
        else historyRecord[i].HIS_timeFor500m = tmp32;

        tmp32 = eeprom_read_long(EEPROM_ADDR_HistroyPara(i * 52 + 4));

        if(tmp32 > 6000)
            historyRecord[i].HIS_timeFor500m_avg = 0;
        else historyRecord[i].HIS_timeFor500m_avg = tmp32;

        tmp32 = eeprom_read_long(EEPROM_ADDR_HistroyPara(i * 52 + 8));

        if(tmp32 > 9999)
            historyRecord[i].HIS_watts = 0;
        else historyRecord[i].HIS_watts = tmp32;

        tmp32 = eeprom_read_long(EEPROM_ADDR_HistroyPara(i * 52 + 12));

        if(tmp32 > 9999)
            historyRecord[i].HIS_watts_avg = 0;
        else historyRecord[i].HIS_watts_avg = tmp32;

        tmp32 = eeprom_read_long(EEPROM_ADDR_HistroyPara(i * 52 + 16));

        if(tmp32 > 999)
            historyRecord[i].HIS_spm = 0;
        else historyRecord[i].HIS_spm = tmp32;

        tmp32 = eeprom_read_long(EEPROM_ADDR_HistroyPara(i * 52 + 20));

        if(tmp32 > 9999)
            historyRecord[i].HIS_DistFor30min = 0;
        else historyRecord[i].HIS_DistFor30min = tmp32;

        tmp32 = eeprom_read_long(EEPROM_ADDR_HistroyPara(i * 52 + 24));

        if(tmp32 > 19999)
            historyRecord[i].HIS_distance = 0;
        else historyRecord[i].HIS_distance = tmp32;

        tmp32 = eeprom_read_long(EEPROM_ADDR_HistroyPara(i * 52 + 28));

        if(tmp32 > 19999)
            historyRecord[i].HIS_calories = 0;
        else historyRecord[i].HIS_calories = tmp32;
				
        tmp32 = eeprom_read_long(EEPROM_ADDR_HistroyPara(i * 52 + 32));

        if(tmp32 > 9999)
            historyRecord[i].HIS_caloriesPerHour = 0;
        else historyRecord[i].HIS_caloriesPerHour = tmp32;				
				
        tmp32 = eeprom_read_long(EEPROM_ADDR_HistroyPara(i * 52 + 36));

        if(tmp32 > 0x0000ffff)
            historyRecord[i].HIS_time = 0;
        else historyRecord[i].HIS_time = tmp32;	

        tmp32 = eeprom_read_long(EEPROM_ADDR_HistroyPara(i * 52 + 40));

        if(tmp32 > 9999)
            historyRecord[i].HIS_stroke = 0;
        else historyRecord[i].HIS_stroke = tmp32;	
								
        tmp32 = eeprom_read_long(EEPROM_ADDR_HistroyPara(i * 52 + 44));

        if(tmp32 > 999)
            historyRecord[i].HIS_Drag_coeff = 0;
        else historyRecord[i].HIS_Drag_coeff = tmp32;

        tmp32 = eeprom_read_long(EEPROM_ADDR_HistroyPara(i * 52 + 48));

        if(tmp32 > 999)
            historyRecord[i].HIS_hr = 0;
        else historyRecord[i].HIS_hr = tmp32;

								
			
    }
#endif		
}

