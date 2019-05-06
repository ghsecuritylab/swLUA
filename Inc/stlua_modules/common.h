/*
 * common.h
 *
 *  Created on: Jan 18, 2019
 *      Author: jacob
 */

#ifndef STLUA_MODULES_COMMON_H_
#define STLUA_MODULES_COMMON_H_

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#define stlua_setfield_integer(L, k, v) \
	do { \
		lua_pushinteger((L), (v)); \
		lua_setfield((L), -2, (k)); \
	} while(0)

#endif /* STLUA_MODULES_COMMON_H_ */
