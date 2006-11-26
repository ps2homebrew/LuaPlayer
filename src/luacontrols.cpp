#include <stdlib.h>
#include "luaplayer.h"


// The "Controls" userdata object.
// ------------------------------
UserdataStubs(Controls, Controls);

static int Controls_tostring (lua_State *L)
{
	char buff[32];
	sprintf(buff, "%d", toControls(L, 1)->Buttons);
	lua_pushfstring(L, "Controls (%s)", buff);
	return 1;
}

static int Controls_read(lua_State *L)
{
	int argc = lua_gettop(L);
	if(argc) return luaL_error(L, "Argument error: Controls.read() cannot be called from an instance.");
	Controls* pad = pushControls(L);
	padProcessEvents(0, 0, pad);
	return 1;
}

const char* g_errorMessage = "Argument error: The Controls functions take no arguments (and also, must be called with a colon from an instance: e g mycontrols:left().";

#define CHECK_CTRL(NAME, BIT) \
static int NAME(lua_State *L) \
{ \
	if (lua_gettop(L) != 1) return luaL_error(L, g_errorMessage); \
	Controls *a = toControls(L, 1);\
	lua_pushboolean(L, (a->Buttons & BIT) == BIT); \
	return 1; \
}

CHECK_CTRL(Controls_select, PAD_SELECT)
CHECK_CTRL(Controls_start, PAD_START)
CHECK_CTRL(Controls_up, PAD_UP)
CHECK_CTRL(Controls_right, PAD_RIGHT)
CHECK_CTRL(Controls_down, PAD_DOWN)
CHECK_CTRL(Controls_left, PAD_LEFT)
CHECK_CTRL(Controls_triangle, PAD_TRIANGLE)
CHECK_CTRL(Controls_circle, PAD_CIRCLE)
CHECK_CTRL(Controls_cross, PAD_CROSS)
CHECK_CTRL(Controls_square, PAD_SQUARE)
CHECK_CTRL(Controls_l1, PAD_L1)
CHECK_CTRL(Controls_r1, PAD_R1)
CHECK_CTRL(Controls_l2, PAD_L2)
CHECK_CTRL(Controls_r2, PAD_R2)
CHECK_CTRL(Controls_l3, PAD_L3)
CHECK_CTRL(Controls_r3, PAD_R3)



static int Controls_buttons(lua_State *L)
{
	if (lua_gettop(L) != 1) return luaL_error(L, g_errorMessage);
	Controls *a = toControls(L, 1);
	lua_pushnumber(L, a->Buttons);
	return 1;
}

static int Controls_analogX(lua_State *L) 
{ 
	if (lua_gettop(L) != 1) return luaL_error(L, "Argument error: The Controls functions take no arguments."); 
	Controls *a = toControls(L, 1);
	lua_pushnumber(L, a->Lx -128); 
	return 1; 
}
static int Controls_analogY(lua_State *L) 
{ 
	if (lua_gettop(L) != 1) return luaL_error(L, "Argument error: The Controls functions take no arguments."); 
	Controls *a = toControls(L, 1);
	lua_pushnumber(L, a->Ly -128); 
	return 1; 
}

static int Controls_equal(lua_State *L) {
	Controls* a = toControls(L, 1);
	Controls* b = toControls(L, 2);
	lua_pushboolean(L, a->Buttons == b->Buttons );
	return 1;
}

	
static const luaL_reg Controls_methods[] = {
	{"read",  Controls_read},
	{"select", Controls_select },
	{"start", Controls_start },
	{"up", Controls_up },
	{"right", Controls_right },
	{"down", Controls_down },
	{"left", Controls_left },
	// this is kept for compatibility with the PSP
	{"l", Controls_l1 },
	{"r", Controls_r1 },
	// then "regular" triggers names are defined
	{"l1", Controls_l1 },
	{"r1", Controls_r1 },
	{"l2", Controls_l2 },
	{"r2", Controls_r2 },
	{"l3", Controls_l3 },
	{"r3", Controls_r3 },
	{"triangle", Controls_triangle },
	{"circle", Controls_circle },
	{"cross", Controls_cross },
	{"square", Controls_square },
	{"analogX", Controls_analogX},
	{"analogY", Controls_analogY},
	{"buttons", Controls_buttons },
  {0, 0}
};
static const luaL_reg Controls_meta[] = {
  {"__tostring", Controls_tostring},
  {"__eq", Controls_equal},
  {0, 0}
};

UserdataRegister(Controls, Controls_methods, Controls_meta)

void setTableValue(lua_State *L, char* name, int value)
{
	lua_pushstring(L, name);
	lua_pushnumber(L, value);
	lua_settable(L, -3);
}

void luaControls_init(lua_State *L) {
	
	Controls_register(L);

	lua_pushstring(L, "Controls");
	lua_gettable(L, LUA_GLOBALSINDEX); \
	setTableValue(L, "selectMask", PAD_SELECT);
	setTableValue(L, "startMask", PAD_START);
	setTableValue(L, "upMask", PAD_UP);
	setTableValue(L, "rightMask", PAD_RIGHT);
	setTableValue(L, "downMask", PAD_DOWN);
	setTableValue(L, "leftMask", PAD_LEFT);
	setTableValue(L, "l1triggerMask", PAD_L1);
	setTableValue(L, "r1triggerMask", PAD_R1);
	setTableValue(L, "l2triggerMask", PAD_L2);
	setTableValue(L, "r2triggerMask", PAD_R2);
	setTableValue(L, "l3triggerMask", PAD_L3);
	setTableValue(L, "r3triggerMask", PAD_R3);
	setTableValue(L, "triangleMask", PAD_TRIANGLE);
	setTableValue(L, "circleMask", PAD_CIRCLE);
	setTableValue(L, "crossMask", PAD_CROSS);
	setTableValue(L, "squareMask", PAD_SQUARE);
}
