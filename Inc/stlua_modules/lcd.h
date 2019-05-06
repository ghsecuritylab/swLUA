/*
 * lcd.h
 *
 *  Created on: Dec 4, 2018
 *      Author: jacob
 */

#ifndef STLUA_MODULES_LCD_H_
#define STLUA_MODULES_LCD_H_

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#define STLUA_LCD_LIB_NAME "lcd"

void stlua_open_lib_lcd (lua_State *L);

//typedef struct __packed {
//	uint8_t r;
//	uint8_t g;
//	uint8_t b;
//	uint8_t a;
//} Color_t;

#endif /* STLUA_MODULES_LCD_H_ */
