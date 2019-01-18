/*
 * ts.c
 *
 *  Created on: Jan 18, 2019
 *      Author: jacob
 */

#include "stlua_modules/ts.h"

#include "stlua_modules/common.h"

#include "stm32746g_discovery_ts.h"

static int get_status(lua_State *L) {
	TS_StateTypeDef tsState;
	uint32_t gestureId = GEST_ID_NO_GESTURE;

#if (TS_MULTI_TOUCH_SUPPORTED == 1)
	int fieldsNum = 2;
#else
	int fieldsNum = 1;
#endif

	if (BSP_TS_GetState(&tsState) != TS_OK) {
		return luaL_error(L, "Failed to get TouchScreen state.");
	}

	lua_createtable(L, 0, fieldsNum); // TS_StateTypeDef mapping

	lua_createtable(L, tsState.touchDetected, 0); // push
	{
		for(int i = 0; i < tsState.touchDetected; ++i) {
			lua_createtable(L, 0, 2); // push

			lua_pushinteger(L, tsState.touchX[i]); // push
			lua_setfield(L, -2, "x"); // push & 2x pop

			lua_pushinteger(L, tsState.touchY[i]); // push
			lua_setfield(L, -2, "y"); // push & 2x pop

			lua_rawseti(L, -2, i + 1); // pop
		}
	}
	lua_setfield(L, -2, "touches"); // push & 2x pop

#if (TS_MULTI_TOUCH_SUPPORTED == 1)
	gestureId = tsState.gestureId;
#endif

	lua_pushinteger(L, gestureId); // push
	lua_setfield(L, -2, "gesture"); // push & 2x pop

	return 1;
}

static const luaL_Reg lib[] = {
	{"get_status", get_status},

	{"GESTURE_NONE", NULL},
	{"GESTURE_MOVE_UP", NULL},
	{"GESTURE_MOVE_RIGHT", NULL},
	{"GESTURE_MOVE_DOWN", NULL},
	{"GESTURE_MOVE_LEFT", NULL},
	{"GESTURE_ZOOM_IN", NULL},
	{"GESTURE_ZOOM_OUT", NULL},

	{NULL, NULL}
};

static void map_constants(lua_State *L) {
	stlua_setfield_integer(L, "GESTURE_NONE", GEST_ID_NO_GESTURE);
	stlua_setfield_integer(L, "GESTURE_MOVE_UP", GEST_ID_MOVE_UP);
	stlua_setfield_integer(L, "GESTURE_MOVE_RIGHT", GEST_ID_MOVE_RIGHT);
	stlua_setfield_integer(L, "GESTURE_MOVE_DOWN", GEST_ID_MOVE_DOWN);
	stlua_setfield_integer(L, "GESTURE_MOVE_LEFT", GEST_ID_MOVE_LEFT);
	stlua_setfield_integer(L, "GESTURE_ZOOM_IN", GEST_ID_ZOOM_IN);
	stlua_setfield_integer(L, "GESTURE_ZOOM_OUT", GEST_ID_ZOOM_OUT);
}

static int stlua_open_ts(lua_State *L) {
	luaL_newlib(L, lib); /* new module */
	map_constants(L);
	return 1;
}

void stlua_open_lib_ts(lua_State *L) {
	luaL_requiref(L, STLUA_TS_LIB_NAME, stlua_open_ts, 1);
	lua_pop(L, 1); /* remove lib */
}
