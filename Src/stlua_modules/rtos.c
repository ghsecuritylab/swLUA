/*
 * rtos.c
 *
 *  Created on: Jan 18, 2019
 *      Author: jacob
 */

#include "stlua_modules/rtos.h"

#include "stlua_modules/common.h"

#include "cmsis_os.h"

#include "term_io.h"
#include <stdio.h>

static int rtos_yield(lua_State *L) {
	taskYIELD();
	return 0;
}

static int rtos_delay(lua_State *L) {
	const uint32_t tt = luaL_checkinteger(L, 1);

	osDelay(tt);

	return 0;
}

static int rtos_get_milis(lua_State *L) {
	int ms = osKernelSysTick() * portTICK_PERIOD_MS;

	lua_pushinteger(L, ms);

	return 1;
}

static int rtos_get_seconds(lua_State *L) {
	TickType_t ticks = osKernelSysTick();
	double seconds = ((double)ticks  * portTICK_PERIOD_MS / 1000.0);

	lua_pushnumber(L, seconds);

	return 1;
}

static const luaL_Reg lib[] = {
	{"yield", rtos_yield},
	{"delay", rtos_delay},
	{"get_milis", rtos_get_milis},
	{"get_seconds", rtos_get_seconds},

	{NULL, NULL}
};

//static void map_constants(lua_State *L) {
//
//
//}

static int stlua_open_rtos(lua_State *L) {
	luaL_newlib(L, lib); /* new module */
//	map_constants(L);
	return 1;
}

void stlua_open_lib_rtos(lua_State *L) {
	luaL_requiref(L, STLUA_RTOS_LIB_NAME, stlua_open_rtos, 1);
	lua_pop(L, 1); /* remove lib */
}
