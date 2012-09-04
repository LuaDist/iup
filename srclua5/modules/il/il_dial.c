/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iupcontrols.h"
#include "il.h"


static int dial_mousemove_cb(Ihandle *self, double p0)
{
  lua_State *L = iuplua_call_start(self, "mousemove_cb");
  lua_pushnumber(L, p0);
  return iuplua_call(L, 1);
}

static int Dial(lua_State *L)
{
  Ihandle *ih = IupDial((char *)luaL_optstring(L, 1, NULL));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupdiallua_open(lua_State * L)
{
  iuplua_register(L, Dial, "Dial");

  iuplua_register_cb(L, "MOUSEMOVE_CB", (lua_CFunction)dial_mousemove_cb, "dial");

#ifdef IUPLUA_USELOH
#include "dial.loh"
#else
#ifdef IUPLUA_USELH
#include "dial.lh"
#else
  iuplua_dofile(L, "dial.lua");
#endif
#endif

  return 0;
}

