#include <stdlib.h>
#include "luaplayer.h"

// if the timer is running:
// measuredTime is the value of the last call to getCurrentMilliseconds
// offset is the value of startTime
//
// if the timer is stopped:
// measuredTime is 0
// offset is the value time() returns on stopped timers

typedef struct
{
	u64 measuredTime;
	u64 offset;
} Timer;

UserdataStubs(Timer, Timer*)

static u64 getCurrentMilliseconds()
{
	return ps2time_time_msec(NULL);
}

static int Timer_new(lua_State *L)
{
	int argc = lua_gettop(L); 
	if (argc != 0 && argc != 1) return luaL_error(L, "Argument error: Timer.new([startTime]) zero or one argument."); 
	
	Timer** luaTimer = pushTimer(L);
	Timer* timer = (Timer*) malloc(sizeof(Timer));
	*luaTimer = timer;
	timer->measuredTime = getCurrentMilliseconds();
	timer->offset = argc == 1 ? luaL_checkint(L, 1) : 0;
	return 1;
}

static int Timer_start(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc > 1) return luaL_error(L, "Argument error: Timer:start() zero argument.");

	Timer* timer = *toTimer(L, 1);
	if (timer->measuredTime) {
		// timer is running
		u64 currentTime = getCurrentMilliseconds();
		lua_pushnumber(L, currentTime - timer->measuredTime + timer->offset);
	} else {
		// timer is stopped
		timer->measuredTime = getCurrentMilliseconds();
		lua_pushnumber(L, timer->offset);
	}
	return 1;
}

static int Timer_time(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc > 1) return luaL_error(L, "Argument error: Timer:time() zero argument.");

	Timer* timer = *toTimer(L, 1);
	if (timer->measuredTime) {
		// timer is running
		u64 currentTime = getCurrentMilliseconds();
		lua_pushnumber(L, currentTime - timer->measuredTime + timer->offset);
	} else {
		// timer is stopped
		lua_pushnumber(L, timer->offset);
	}
	return 1;
}

static int Timer_stop(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc > 1) return luaL_error(L, "Argument error: Timer:stop() zero argument.");

	Timer* timer = *toTimer(L, 1);
	if (timer->measuredTime) {
		// timer is running
		u64 currentTime = getCurrentMilliseconds();
		timer->offset = currentTime - timer->measuredTime + timer->offset;
		timer->measuredTime = 0;
	}
	lua_pushnumber(L, timer->offset);
	return 1;
}


static int Timer_reset(lua_State *L)
{
	int argc = lua_gettop(L);
	if (argc > 2) return luaL_error(L, "Argument error: Timer:reset() zero or one argument.");

	Timer* timer = *toTimer(L, 1);
	if (timer->measuredTime) {
		// timer is running
		u64 currentTime = getCurrentMilliseconds();
		lua_pushnumber(L, currentTime - timer->measuredTime + timer->offset);
	} else {
		// timer is stopped
		lua_pushnumber(L, timer->offset);
	}
	timer->offset = argc == 2 ? luaL_checkint(L, 2) : 0;
	timer->measuredTime = 0;
	return 1;
}


static int Timer_free(lua_State *L)
{
	free(*toTimer(L, 1));
	return 0;
}

static int Timer_tostring (lua_State *L)
{
	Timer* timer = *toTimer(L, 1);
	u64 t;
	if (timer->measuredTime) {
		// timer is running
		u64 currentTime = getCurrentMilliseconds();
		t = currentTime - timer->measuredTime + timer->offset;
	} else {
		// timer is stopped
		t = timer->offset;
	}
	lua_pushfstring(L, "%i", t);
	return 1;
}

static const luaL_reg Timer_methods[] = {
	{"new", Timer_new},
	{"start", Timer_start},
	{"time", Timer_time},
	{"stop", Timer_stop},
	{"reset", Timer_reset},
	{0,0}
};

static const luaL_reg Timer_meta[] = {
	{"__gc", Timer_free},
	{"__tostring", Timer_tostring},
	{0,0}
};

UserdataRegister(Timer, Timer_methods, Timer_meta)

void luaTimer_init(lua_State *L)
{
	Timer_register(L);
}
