#ifndef RELAY_MODULE_CONF_H
#define RELAY_MODULE_CONF_H

#include "device_common.h"

#define MAX_SCENE_DEFINITION_COUNT       (200)

#define RELAY_CIRCUIT_CONF_IN_DB_COLUMN       6
#define RELAY_SCENE_CONF_IN_DB_COLUMN         7
#define RELAY_MODULE_CONF_IN_DB_COLUMN        8
#define RELAY_GLOBAL_CONF_IN_DB_COLUMN        9




#define RELAY_GLOBAL_CONF_MODBUS_START_ADDRESS    1000
#define RELAY_CIRCUIT_CONF_MODBUS_START_ADDRESS (10000)
#define RELAY_SCENE_DEF_MODBUS_START_ADDRESS    (20000)

#define RELAY_CIRCUIT_STATISTICS_MODBUS_START_ADDRESS 10
#define RELAY_CIRCUIT_ANALOG_MODBUS_START_ADDRESS 82

#pragma pack(1)
struct relay_module_status_s{
    uint8_t type;
    uint8_t pad;
    uint16_t current_scene;
    uint16_t scene_count;
    uint16_t temperature;
}__attribute__((__packed__));

struct relay_global_conf_s{
    uint8_t  alarm_enable;
    uint8_t  fire_protecttion_mode;
    uint8_t  bad_lamp_detection_enable;
    uint8_t  lamp_switch_random_factor;
    uint16_t lamp_protection_time;
    uint16_t voltage_update_threshold;/*数值*/
    uint16_t current_update_threshold;
    uint16_t current_deadzone;
    uint16_t relay_pulse_interval;
    uint16_t relay_control_interval_random_factor;
    uint16_t relay_control_interval;
}__attribute__((__packed__));

#if 0
struct relay_module_basic_conf_s{
    device_conf_s dev;
    relay_global_conf_s global;
}__attribute__((__packed__));
#endif

struct relay_circuit_statistics_s{
    uint16_t exec_count;
    uint32_t uptime;
    uint16_t alarm;
}__attribute__((__packed__));



struct scene_def_s{
    uint16_t number;
    uint16_t definition;
};

struct relay_circuit_conf_s{
    uint8_t alarm_enable;
    uint8_t pad;   //
    uint16_t voltage_alarm_threshold;
    uint16_t current_alarm_threshold;
    uint16_t update_period;
}__attribute__((__packed__));

struct relay_circuit_analog_s{
    int16_t vol;
    int16_t cur;
    int16_t freq;
    int16_t factor;
    int32_t power;
    int32_t energy_reading;
}__attribute__((__packed__));
#pragma pack()

#endif // RELAY_MODULE_CONF_H
