/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int multiline_action(Ihandle *self, int p0, char * p1)
{
  lua_State *L = iuplua_call_start(self, "action");
  lua_pushinteger(L, p0);
  lua_pushstring(L, p1);
  return iuplua_call(L, 2);
}

static int MultiLine(lua_State *L)
{
  Ihandle *ih = IupMultiLine(NULL);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupmultilinelua_open(lua_State * L)
{
  iuplua_register(L, MultiLine, "MultiLine");

  iuplua_register_cb(L, "ACTION", (lua_CFunction)multiline_action, "multiline");

#ifdef IUPLUA_USELOH
#include "multiline.loh"
#else
#ifdef IUPLUA_USELH
#include "multiline.lh"
#else
  iuplua_dofile(L, "multiline.lua");
#endif
#endif

  return 0;
}

