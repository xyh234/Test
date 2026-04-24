#ifndef __EEPROM_H
#define __EEPROM_H
#include "newtype.h"

//initial address of EEPROM
//#define FLASH_PAGE_SIZE        ((uint16_t)0x800)
//#define FLASH_WRITE_START_ADDR ((uint32_t)0x08010000)
//#define FLASH_WRITE_END_ADDR   ((uint32_t)0x08018000)

#define EEPROM_ADDR_INI 0x08015000 //0x0007E000

#define EEPROM_ADDR(offset) (EEPROM_ADDR_INI + offset)

//#define EEPROM_ADDR_BEEP                EEPROM_ADDR(0)   //u32
//#define EEPROM_ADDR_MILE                EEPROM_ADDR(4)
#define EEPROM_ADDR_RESISTANCE_MIN      EEPROM_ADDR(0)   //u32
#define EEPROM_ADDR_RESISTANCE_MAX      EEPROM_ADDR(4)
#define EEPROM_ADDR_SLEEP1              EEPROM_ADDR(8)
#define EEPROM_ADDR_UPDATE              EEPROM_ADDR(12)
#define EEPROM_ADDR_RESISTANCE_LEVEL    EEPROM_ADDR(16)
//#define EEPROM_ADDR_AGE                 EEPROM_ADDR(16)
#define EEPROM_ADDR_HistroyNumber       EEPROM_ADDR(20)


#define EEPROM_ADDR_HISTORY_INI         0x08014000//0x0007C000
#define EEPROM_ADDR_HISTORY(offset)     (EEPROM_ADDR_HISTORY_INI + offset)
#define EEPROM_ADDR_HistroyPara(x)      EEPROM_ADDR_HISTORY(x)
#define EEPROM_ADDR_Histroy1            EEPROM_ADDR_HistroyPara(0)
#define EEPROM_ADDR_Histroy2            EEPROM_ADDR_HistroyPara(32)
#define EEPROM_ADDR_Histroy3            EEPROM_ADDR_HistroyPara(64)
#define EEPROM_ADDR_Histroy4            EEPROM_ADDR_HistroyPara(96)
#define EEPROM_ADDR_Histroy5            EEPROM_ADDR_HistroyPara(128)
#define EEPROM_ADDR_Histroy6            EEPROM_ADDR_HistroyPara(160)
#define EEPROM_ADDR_Histroy7            EEPROM_ADDR_HistroyPara(192)
#define EEPROM_ADDR_Histroy8            EEPROM_ADDR_HistroyPara(224)
#define EEPROM_ADDR_Histroy9            EEPROM_ADDR_HistroyPara(256)
#define EEPROM_ADDR_Histroy10           EEPROM_ADDR_HistroyPara(288)
#define EEPROM_ADDR_Histroy11           EEPROM_ADDR_HistroyPara(320)
#define EEPROM_ADDR_Histroy12           EEPROM_ADDR_HistroyPara(352)
#define EEPROM_ADDR_Histroy13           EEPROM_ADDR_HistroyPara(384)
#define EEPROM_ADDR_Histroy14           EEPROM_ADDR_HistroyPara(416)
#define EEPROM_ADDR_Histroy15           EEPROM_ADDR_HistroyPara(448)
#define EEPROM_ADDR_Histroy16           EEPROM_ADDR_HistroyPara(480)
#define EEPROM_ADDR_Histroy17           EEPROM_ADDR_HistroyPara(512)
#define EEPROM_ADDR_Histroy18           EEPROM_ADDR_HistroyPara(544)
#define EEPROM_ADDR_Histroy19           EEPROM_ADDR_HistroyPara(576)
#define EEPROM_ADDR_Histroy20           EEPROM_ADDR_HistroyPara(608)


#define eeprom_rdchar(addr)     (*(__IO uint8_t*)(addr))//(*((__IO uint8_t *)addr))
#define eeprom_read_int(addr)   (*(__IO uint16_t*)(addr))//(*((__IO uint16_t *)addr))
#define eeprom_read_long(addr)  (*(__IO uint32_t*)(addr))//(*((__IO uint32_t *)addr))

void eeprom_wrchar(uint32_t addr, uint8_t ucdata);
void eeprom_write_long(uint32_t addr, uint32_t ucdata);
void eeprom_writeHistory_long(uint32_t addr, History* ucdata);
void eeprom_read(void);
void eeprom_clear(uint32_t addr);

#endif


