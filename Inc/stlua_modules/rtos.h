/*
 * rtos.h
 *
 *  Created on: Jan 18, 2019
 *      Author: jacob
 */

#ifndef STLUA_MODULES_RTOS_H_
#define STLUA_MODULES_RTOS_H_

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#define STLUA_RTOS_LIB_NAME "rtos"

void stlua_open_lib_rtos (lua_State *L);

#endif /* STLUA_MODULES_RTOS_H_ */
