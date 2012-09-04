/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int Label(lua_State *L)
{
  Ihandle *ih = IupLabel((char *)luaL_optstring(L, 1, NULL));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iuplabellua_open(lua_State * L)
{
  iuplua_register(L, Label, "Label");


#ifdef IUPLUA_USELOH
#include "label.loh"
#else
#ifdef IUPLUA_USELH
#include "label.lh"
#else
  iuplua_dofile(L, "label.lua");
#endif
#endif

  return 0;
}

