/*
 * mqtt.h
 *
 *  Created on: May 7, 2019
 *      Author: jacob
 */

#ifndef STLUA_MODULES_MQTT_H_
#define STLUA_MODULES_MQTT_H_

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#define STLUA_MQTT_LIB_NAME "mqtt"

void stlua_open_lib_mqtt (lua_State *L);

#endif /* STLUA_MODULES_MQTT_H_ */
