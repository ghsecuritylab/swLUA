/*
 * lcd.c
 *
 *  Created on: Nov 27, 2018
 *      Author: jacob
 */

#include "stlua_modules/lcd.h"

#include "stlua_modules/common.h"

#include "stm32746g_discovery_lcd.h"

static int get_screen_size(lua_State * L) {
	lua_createtable(L, 0, 2);

	lua_pushinteger(L, BSP_LCD_GetXSize());
	lua_setfield(L, -2, "x");

	lua_pushinteger(L, BSP_LCD_GetYSize());
	lua_setfield(L, -2, "y");

	return 1;
}

//static int set_layer_transparency(lua_State * L) {
//	uint32_t layerIndex = luaL_checkinteger(L, -2);
//	uint8_t  transparency = luaL_checkinteger(L, -1);
//
//	BSP_LCD_SetTransparency(layerIndex, transparency);
//
//	return 0;
//}
//
//static int select_layer(lua_State * L) {
//	uint32_t layerIndex = luaL_checkinteger(L, -1);
//
//	BSP_LCD_SelectLayer(layerIndex);
//
//	return 0;
//}

static int set_text_color(lua_State * L) {
	uint32_t c = luaL_checkinteger(L, -1);
//	Color_t * c = (Color_t *)lua_touserdata(L, 1);
//
//	luaL_argcheck(L, c != NULL, 1, "`Color' expected");
//
//	uint32_t color_value = ColorToBinaryValue(c);
//
//	BSP_LCD_SetTextColor(color_value);
	BSP_LCD_SetTextColor(c);

	return 0;
}

static int get_text_color(lua_State * L) {
	lua_pushinteger(L, BSP_LCD_GetTextColor());

	return 1;
}

static int set_back_color(lua_State * L) {
	uint32_t c = luaL_checkinteger(L, -1);

	BSP_LCD_SetBackColor(c);

	return 0;
}

static int get_back_color(lua_State * L) {
	lua_pushinteger(L, BSP_LCD_GetBackColor());

	return 1;
}

static int read_pixel(lua_State * L) {
	uint16_t x = luaL_checkinteger(L, -2);
	uint16_t y = luaL_checkinteger(L, -1);

	lua_pushinteger(L, BSP_LCD_ReadPixel(x, y));

	return 1;
}

static int draw_pixel(lua_State * L) {
	uint16_t x = luaL_checkinteger(L, -3);
	uint16_t y = luaL_checkinteger(L, -2);
	uint32_t c = luaL_checkinteger(L, -1);

	BSP_LCD_DrawPixel(x, y, c);

	return 0;
}

extern void stackDump(lua_State *);

static int clear(lua_State * L) {
	uint32_t c = luaL_checkinteger(L, -1);

	BSP_LCD_Clear(c);

	return 0;
}

static int clear_string_line(lua_State * L) {
	uint32_t l = luaL_checkinteger(L, -1);

	BSP_LCD_ClearStringLine(l);

	return 0;
}

static int display_string(lua_State * L) {
	if (lua_gettop(L) == 2) {
		uint16_t line = luaL_checkinteger(L, -2);
		const char * s = luaL_checkstring(L, -1);
		BSP_LCD_DisplayStringAtLine(line, (uint8_t*) s);
	} else { // 4 args
		uint16_t x = luaL_checkinteger(L, -4);
		uint16_t y = luaL_checkinteger(L, -3);
		const char * s = luaL_checkstring(L, -2);
		Text_AlignModeTypdef m = (Text_AlignModeTypdef) luaL_checkinteger(L, -1);

		BSP_LCD_DisplayStringAt(x, y, (uint8_t*) s, m);
	}

	return 0;
}

static int display_char(lua_State * L) {
	uint16_t x = luaL_checkinteger(L, -3);
	uint16_t y = luaL_checkinteger(L, -2);
	const char * s = luaL_checkstring(L, -1);

	BSP_LCD_DisplayChar(x, y, (uint8_t) s[0]);

	return 0;
}

static int draw_horizontal_line(lua_State * L) {
	uint16_t x = luaL_checkinteger(L, -3);
	uint16_t y = luaL_checkinteger(L, -2);
	uint16_t l = luaL_checkinteger(L, -1);

	BSP_LCD_DrawHLine(x, y, l);

	return 0;
}

static int draw_vertical_line(lua_State * L) {
	uint16_t x = luaL_checkinteger(L, -3);
	uint16_t y = luaL_checkinteger(L, -2);
	uint16_t l = luaL_checkinteger(L, -1);

	BSP_LCD_DrawVLine(x, y, l);

	return 0;
}

static int draw_line(lua_State * L) {
	uint16_t x1 = luaL_checkinteger(L, -4);
	uint16_t y1 = luaL_checkinteger(L, -3);
	uint16_t x2 = luaL_checkinteger(L, -2);
	uint16_t y2 = luaL_checkinteger(L, -1);

	BSP_LCD_DrawLine(x1, y1, x2, y2);

	return 0;
}

static int draw_rect(lua_State * L) {
	uint16_t x = luaL_checkinteger(L, -4);
	uint16_t y = luaL_checkinteger(L, -3);
	uint16_t w = luaL_checkinteger(L, -2);
	uint16_t h = luaL_checkinteger(L, -1);

	BSP_LCD_DrawRect(x, y, w, h);

	return 0;
}

static int draw_circle(lua_State * L) {
	uint16_t x = luaL_checkinteger(L, -3);
	uint16_t y = luaL_checkinteger(L, -2);
	uint16_t r = luaL_checkinteger(L, -1);

	BSP_LCD_DrawCircle(x, y, r);

	return 0;
}

static int draw_ellipse(lua_State * L) {
	uint16_t x = luaL_checkinteger(L, -4);
	uint16_t y = luaL_checkinteger(L, -3);
	uint16_t rx = luaL_checkinteger(L, -2);
	uint16_t ry = luaL_checkinteger(L, -1);

	BSP_LCD_DrawEllipse(x, y, rx, ry);

	return 0;
}

static int fill_rect(lua_State * L) {
	uint16_t x = luaL_checkinteger(L, -4);
	uint16_t y = luaL_checkinteger(L, -3);
	uint16_t w = luaL_checkinteger(L, -2);
	uint16_t h = luaL_checkinteger(L, -1);

	BSP_LCD_FillRect(x, y, w, h);

	return 0;
}

static int fill_circle(lua_State * L) {
	uint16_t x = luaL_checkinteger(L, -3);
	uint16_t y = luaL_checkinteger(L, -2);
	uint16_t r = luaL_checkinteger(L, -1);

	BSP_LCD_FillCircle(x, y, r);

	return 0;
}

static int fill_ellipse(lua_State * L) {
	uint16_t x = luaL_checkinteger(L, -4);
	uint16_t y = luaL_checkinteger(L, -3);
	uint16_t rx = luaL_checkinteger(L, -2);
	uint16_t ry = luaL_checkinteger(L, -1);

	BSP_LCD_FillEllipse(x, y, rx, ry);

	return 0;
}

static int swap_buffers(lua_State * L) {

	while (!(LTDC->CDSR & LTDC_CDSR_VSYNCS))
		;

	BSP_LCD_SetLayerVisible(BSP_LCD_GetCurrentLayer(), ENABLE);
	uint32_t newLayerIndex = (BSP_LCD_GetCurrentLayer() + 1) % MAX_LAYER_NUMBER;

	BSP_LCD_SetLayerVisible(newLayerIndex, DISABLE);
	BSP_LCD_SelectLayer(newLayerIndex);

	return 0;
}

static int make_color(lua_State * L) {
	uint32_t a, r, g, b, c;

	r = (uint32_t)luaL_checknumber(L, -4) * 255;
	g = (uint32_t)luaL_checknumber(L, -3) * 255;
	b = (uint32_t)luaL_checknumber(L, -2) * 255;
	a = (uint32_t)luaL_checknumber(L, -1) * 255;

	c = ((a << 24) || (r << 16) || (g << 8) || (b));

	lua_pushinteger(L, c);

	return 1;
}

static const luaL_Reg lib[] = {
	/* Function defnies */
	{ "get_screen_size", get_screen_size },
	//	{"set_layer_transparency", set_layer_transparency},
	//	{"select_layer", select_layer},

	{ "set_text_color", set_text_color },
	{ "get_text_color", get_text_color },
	{ "set_back_color", set_back_color },
	{ "get_back_color", get_back_color },

	{ "read_pixel", read_pixel },
	{ "draw_pixel", draw_pixel },
	{ "clear", clear },
	{ "clear_string_line", clear_string_line },
	{ "display_string", display_string }, // 2 opcje, albo na pozycji, albo na linii
	{ "display_char", display_char },

	{ "draw_horizontal_line", draw_horizontal_line },
	{ "draw_vertical_line", draw_vertical_line },
	{ "draw_line", draw_line },
	{ "draw_rect", draw_rect },
	{ "draw_circle", draw_circle },
//	{"draw_polygon", },
	{ "draw_ellipse", draw_ellipse },
//	{"draw_bitmap", draw_bitmap},
	{ "fill_rect", fill_rect },
	{ "fill_circle", fill_circle },
//	{"fill_polygon", fill_polygon},
	{ "fill_ellipse", fill_ellipse },

	{ "swap_buffers", swap_buffers },

	{ "make_color", make_color },

	/* Placeholders for colors */
	{ "COLOR_BLUE", NULL },
	{ "COLOR_GREEN", NULL },
	{ "COLOR_RED", NULL },
	{ "COLOR_CYAN", NULL },
	{ "COLOR_MAGENTA", NULL },
	{ "COLOR_YELLOW", NULL },
	{ "COLOR_LIGHTBLUE", NULL },
	{ "COLOR_LIGHTGREEN", NULL },
	{ "COLOR_LIGHTRED", NULL },
	{ "COLOR_LIGHTCYAN", NULL },
	{ "COLOR_LIGHTMAGENTA", NULL },
	{ "COLOR_DARKBLUE", NULL },
	{ "COLOR_DARKGREEN", NULL },
	{ "COLOR_DARKRED", NULL },
	{ "COLOR_DARKCYAN", NULL },
	{ "COLOR_DARKMAGENTA", NULL },
	{ "COLOR_DARKYELLOW", NULL },
	{ "COLOR_WHITE", NULL },
	{ "COLOR_LIGHTGRAY", NULL },
	{ "COLOR_GRAY", NULL },
	{ "COLOR_DRKGRAY", NULL },
	{ "COLOR_BLACK", NULL },
	{ "COLOR_BROWN", NULL },
	{ "COLOR_ORANGE", NULL },
	{ "COLOR_TRANSPARENT", NULL },

	{ "CENTER_MODE", NULL },
	{ "RIGHT_MODE", NULL },
	{ "LEFT_MODE", NULL },

	/* ADDITIONAL CONSTANTS */
	{ "LAYERS", NULL },

	{ NULL, NULL }
};

static void map_constants(lua_State *L) {
	stlua_setfield_integer(L, "COLOR_BLUE", LCD_COLOR_BLUE);
	stlua_setfield_integer(L, "COLOR_GREEN", LCD_COLOR_GREEN);
	stlua_setfield_integer(L, "COLOR_RED", LCD_COLOR_RED);
	stlua_setfield_integer(L, "COLOR_CYAN", LCD_COLOR_CYAN);
	stlua_setfield_integer(L, "COLOR_MAGENTA", LCD_COLOR_MAGENTA);
	stlua_setfield_integer(L, "COLOR_YELLOW", LCD_COLOR_YELLOW);

	stlua_setfield_integer(L, "COLOR_LIGHTBLUE", LCD_COLOR_LIGHTBLUE);
	stlua_setfield_integer(L, "COLOR_LIGHTGREEN", LCD_COLOR_LIGHTGREEN);
	stlua_setfield_integer(L, "COLOR_LIGHTRED", LCD_COLOR_LIGHTRED);
	stlua_setfield_integer(L, "COLOR_LIGHTCYAN", LCD_COLOR_LIGHTCYAN);
	stlua_setfield_integer(L, "COLOR_LIGHTMAGENTA", LCD_COLOR_LIGHTMAGENTA);
	stlua_setfield_integer(L, "COLOR_LIGHTYELLOW", LCD_COLOR_LIGHTYELLOW);

	stlua_setfield_integer(L, "COLOR_DARKBLUE", LCD_COLOR_DARKBLUE);
	stlua_setfield_integer(L, "COLOR_DARKGREEN", LCD_COLOR_DARKGREEN);
	stlua_setfield_integer(L, "COLOR_DARKRED", LCD_COLOR_DARKRED);
	stlua_setfield_integer(L, "COLOR_DARKCYAN", LCD_COLOR_DARKCYAN);
	stlua_setfield_integer(L, "COLOR_DARKMAGENTA", LCD_COLOR_DARKMAGENTA);
	stlua_setfield_integer(L, "COLOR_DARKYELLOW", LCD_COLOR_DARKYELLOW);

	stlua_setfield_integer(L, "COLOR_WHITE", LCD_COLOR_WHITE);
	stlua_setfield_integer(L, "COLOR_LIGHTGRAY", LCD_COLOR_LIGHTGRAY);
	stlua_setfield_integer(L, "COLOR_GRAY", LCD_COLOR_GRAY);
	stlua_setfield_integer(L, "COLOR_DARKGRAY", LCD_COLOR_DARKGRAY);
	stlua_setfield_integer(L, "COLOR_BLACK", LCD_COLOR_BLACK);
	stlua_setfield_integer(L, "COLOR_BROWN", LCD_COLOR_BROWN);
	stlua_setfield_integer(L, "COLOR_ORANGE", LCD_COLOR_ORANGE);
	stlua_setfield_integer(L, "COLOR_TRANSPARENT", LCD_COLOR_TRANSPARENT);

	stlua_setfield_integer(L, "CENTER_MODE", CENTER_MODE);
	stlua_setfield_integer(L, "RIGHT_MODE", RIGHT_MODE);
	stlua_setfield_integer(L, "LEFT_MODE", LEFT_MODE);

	stlua_setfield_integer(L, "LAYERS", 2);
}

static int stlua_open_lcd(lua_State *L) {
	luaL_newlib(L, lib); /* new module */
	map_constants(L);
	return 1;
}

void stlua_open_lib_lcd(lua_State *L) {
	luaL_requiref(L, STLUA_LCD_LIB_NAME, stlua_open_lcd, 1);
	lua_pop(L, 1); /* remove lib */
}
