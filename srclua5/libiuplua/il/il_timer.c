/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int timer_action_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "action_cb");
  return iuplua_call(L, 0);
}

static int Timer(lua_State *L)
{
  Ihandle *ih = IupTimer();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iuptimerlua_open(lua_State * L)
{
  iuplua_register(L, Timer, "Timer");

  iuplua_register_cb(L, "ACTION_CB", (lua_CFunction)timer_action_cb, "timer");

#ifdef IUPLUA_USELOH
#include "timer.loh"
#else
#ifdef IUPLUA_USELH
#include "timer.lh"
#else
  iuplua_dofile(L, "timer.lua");
#endif
#endif

  return 0;
}

