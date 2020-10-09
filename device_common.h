#ifndef DEVICE_COMMON_H
#define DEVICE_COMMON_H

#include <QStringList>


#define SUBMODULE_MAX_COUNT            247

#define DEV_BASIC_CONF_MODBUS_START_ADDRESS    (0)

#define ADDRESS_CONF_IN_DB_COLUMN  0
#define TYPE_CONF_IN_DB_COLUMN     1
#define DEVICE_NAME_IN_DB_COLUMN   2
#define DEVIVE_DISCR_IN_DB_COLUMN  3
#define BASIC_CONF_IN_DB_COLUMN       4
#define ADVANCED_CONF_IN_DB_COLUMN    5





//const QStringList dev_typeList = { "iTU-11.GW", "iTU-11.6.16", "iTU-11.12.16", "iTU-11.MP.4","iTU-11.6.16D","iTU-11.12.16D" };

enum dev_type{

};

struct rtc_time_s{
    uint16_t year;
    uint16_t month;
    uint16_t day;
    uint16_t hour;
    uint16_t min;
    uint16_t sec;
    uint16_t weekday;
}__attribute__((__packed__));

struct device_conf_s{
    uint16_t dev_addr;
    uint16_t powerbus_baudrate;
    struct rtc_time_s rtc;
    uint16_t update_time; 
}__attribute__((__packed__));

enum alarm_type{
    normal = 0,
    voltage_anomalies = 0x01U,
    current_anomalies = 0x02U,
    open_circuit = 0x04U,
    fire_alarm = 0x8U
};
#endif // DEVICE_COMMON_H
