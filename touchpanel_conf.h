#ifndef TOUCHPANEL_CONF_H
#define TOUCHPANEL_CONF_H
#include "device_common.h"



#define IIC1_START_ADDRESS    3000
#define IIC2_START_ADDRESS    2000
#define ADC_START_ADDRESS    8000
#define SPI_START_ADDRESS    4000
#define RS485_START_ADDRESS    7000
#define DI_START_ADDRESS    9000


#define IIC1_CONF_IN_DB_COLUMN    6
#define IIC2_CONF_IN_DB_COLUMN    7
#define ADC_CONF_IN_DB_COLUMN     8
#define SPI_CONF_IN_DB_COLUMN     9
#define RS485_CONF_IN_DB_COLUMN   10
#define DI_CONF_IN_DB_COLUMN      11


#pragma pack(1)
struct TouchPanelConfigure{
    uint8_t TouchMode1;
    uint8_t TouchDataNumber1;
    uint16_t TouchButton1Scene1;
    uint16_t TouchButton1Scene2;
    uint32_t NC;
    uint32_t NC1;
    uint8_t TouchMode2;
    uint8_t TouchDataNumber2;
    uint16_t TouchButton2Scene1;
    uint16_t TouchButton2Scene2;
    uint32_t NC2;
    uint32_t NC3;
    uint8_t TouchMode3;
    uint8_t TouchDataNumber3;
    uint16_t TouchButton3Scene1;
    uint16_t TouchButton3Scene2;
    uint32_t NC4;
    uint32_t NC5;
    uint8_t TouchMode4;
    uint8_t TouchDataNumber4;
    uint16_t TouchButton4Scene1;
    uint16_t TouchButton4Scene2;
    uint32_t NC6;
    uint32_t NC7;
}__attribute__((__packed__));

struct TouchAdcConfigure{
    uint8_t Adc1Mode;
    uint8_t Adc1DataNumber;
    int Range1H;
    int Range1L;
    uint16_t Adc1Scene1;
    uint16_t Adc1Scene2;
    uint8_t Adc2Mode;
    uint8_t Adc2DataNumber;
    int Range2H;
    int Range2L;
    uint16_t Adc2Scene1;
    uint16_t Adc2Scene2;
    uint8_t Adc3Mode;
    uint8_t Adc3DataNumber;
    int Range3H;
    int Range3L;
    uint16_t Adc3Scene1;
    uint16_t Adc3Scene2;
    uint8_t Adc4Mode;
    uint8_t Adc4DataNumber;
    int Range4H;
    int Range4L;
    uint16_t Adc4Scene1;
    uint16_t Adc4Scene2;
}__attribute__((__packed__));

#pragma pack()
#endif // TOUCHPANEL_CONF_H
