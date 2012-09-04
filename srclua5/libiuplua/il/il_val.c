/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int val_button_press_cb(Ihandle *self, double p0)
{
  lua_State *L = iuplua_call_start(self, "button_press_cb");
  lua_pushnumber(L, p0);
  return iuplua_call(L, 1);
}

static int val_mousemove_cb(Ihandle *self, double p0)
{
  lua_State *L = iuplua_call_start(self, "mousemove_cb");
  lua_pushnumber(L, p0);
  return iuplua_call(L, 1);
}

static int val_button_release_cb(Ihandle *self, double p0)
{
  lua_State *L = iuplua_call_start(self, "button_release_cb");
  lua_pushnumber(L, p0);
  return iuplua_call(L, 1);
}

static int Val(lua_State *L)
{
  Ihandle *ih = IupVal((char *)luaL_optstring(L, 1, NULL));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupvallua_open(lua_State * L)
{
  iuplua_register(L, Val, "Val");

  iuplua_register_cb(L, "BUTTON_PRESS_CB", (lua_CFunction)val_button_press_cb, NULL);
  iuplua_register_cb(L, "MOUSEMOVE_CB", (lua_CFunction)val_mousemove_cb, "val");
  iuplua_register_cb(L, "BUTTON_RELEASE_CB", (lua_CFunction)val_button_release_cb, NULL);

#ifdef IUPLUA_USELOH
#include "val.loh"
#else
#ifdef IUPLUA_USELH
#include "val.lh"
#else
  iuplua_dofile(L, "val.lua");
#endif
#endif

  return 0;
}

