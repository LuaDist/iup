/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int toggle_action(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "action");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int Toggle(lua_State *L)
{
  Ihandle *ih = IupToggle((char *)luaL_optstring(L, 1, NULL), NULL);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iuptogglelua_open(lua_State * L)
{
  iuplua_register(L, Toggle, "Toggle");

  iuplua_register_cb(L, "ACTION", (lua_CFunction)toggle_action, "toggle");

#ifdef IUPLUA_USELOH
#include "toggle.loh"
#else
#ifdef IUPLUA_USELH
#include "toggle.lh"
#else
  iuplua_dofile(L, "toggle.lua");
#endif
#endif

  return 0;
}

