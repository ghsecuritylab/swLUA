/*
 * ts.h
 *
 *  Created on: Jan 18, 2019
 *      Author: jacob
 */

#ifndef STLUA_MODULES_TS_H_
#define STLUA_MODULES_TS_H_

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#define STLUA_TS_LIB_NAME "ts"

void stlua_open_lib_ts (lua_State *L);

#endif /* STLUA_MODULES_TS_H_ */
