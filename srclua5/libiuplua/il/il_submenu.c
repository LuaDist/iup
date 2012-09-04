/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int Submenu(lua_State *L)
{
  Ihandle *ih = IupSubmenu((char *)luaL_optstring(L, 1, NULL), iuplua_checkihandleornil(L, 2));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupsubmenulua_open(lua_State * L)
{
  iuplua_register(L, Submenu, "Submenu");


#ifdef IUPLUA_USELOH
#include "submenu.loh"
#else
#ifdef IUPLUA_USELH
#include "submenu.lh"
#else
  iuplua_dofile(L, "submenu.lua");
#endif
#endif

  return 0;
}

