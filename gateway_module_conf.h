#ifndef GATEWAY_MODULE_CONF_H
#define GATEWAY_MODULE_CONF_H


#include "device_common.h"
//#include <stdint.h>

#define SCENE_COUNT_PER_SCHEDULE       8
#define SCHEDULE_LIST_COUNT            100

#define GATEWAY_ADVANCED_CONF_MODBUS_START_ADDRESS    10000

#pragma pack(1)
struct module_network_s{
  uint32_t ip;
  uint32_t mask;
  uint32_t gw;
  uint16_t port;
}__attribute__((__packed__));

struct module_mb_comm_s{
  uint32_t baudrate;
  uint16_t parity;
  uint16_t databits;
  uint16_t stopbits;
}__attribute__((__packed__));

struct schedule_item_s{
  uint8_t enable;
  uint8_t hour;
  uint8_t minute;
  uint8_t weekday_mask;
  uint16_t month_mask;
  uint32_t day_mask;
  uint16_t scene_list[SCENE_COUNT_PER_SCHEDULE];
}__attribute__((__packed__));

struct module_discrete_s{
  uint8_t regs[SUBMODULE_MAX_COUNT];//submodule online status.
}__attribute__((__packed__));

struct module_holding_s{
  device_conf_s dev;
  module_network_s net;
  module_mb_comm_s comm;
  uint8_t submodules[32];

//   schedule_item_s schedule_list[SCHEDULE_LIST_COUNT];
}__attribute__((__packed__));

#if 0
struct module_scheduleList_s{
   schedule_item_s list[SCHEDULE_LIST_COUNT];
}__attribute__((__packed__));
#endif

#pragma pack()

#endif // GATEWAY_MODULE_CONF_H
