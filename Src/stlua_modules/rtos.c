/*
 * rtos.c
 *
 *  Created on: Jan 18, 2019
 *      Author: jacob
 */

#include "stlua_modules/rtos.h"

#include "stlua_modules/common.h"

#include "FreeRTOS.h"

static int rtos_yield(lua_State *L) {
	taskYIELD();
	return 0;
}

static int rtos_delay(lua_State *L) {
	const uint32_t tt = luaL_checkinteger(L, 1);

	osDelay(tt);

	return 0;
}

static int rtos_delay_until(lua_State *L) {
	osTim
	osDelayUntil();
	return 0;
}

static const luaL_Reg lib[] = {
	{"yield", rtos_yield},
	{"delay", rtos_delay},


	{"", NULL},

	{NULL, NULL}
};

static void map_constants(lua_State *L) {
	stlua_setfield_integer(L, "GESTURE_NONE", GEST_ID_NO_GESTURE);
	stlua_setfield_integer(L, "GESTURE_MOVE_UP", GEST_ID_MOVE_UP);

}

static int stlua_open_rtos(lua_State *L) {
	luaL_newlib(L, lib); /* new module */
	map_constants(L);
	return 1;
}

void stlua_open_lib_rtos(lua_State *L) {
	luaL_requiref(L, STLUA_RTOS_LIB_NAME, stlua_open_rtos, 1);
	lua_pop(L, 1); /* remove lib */
}
