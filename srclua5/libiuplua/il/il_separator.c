/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int Separator(lua_State *L)
{
  Ihandle *ih = IupSeparator();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupseparatorlua_open(lua_State * L)
{
  iuplua_register(L, Separator, "Separator");


#ifdef IUPLUA_USELOH
#include "separator.loh"
#else
#ifdef IUPLUA_USELH
#include "separator.lh"
#else
  iuplua_dofile(L, "separator.lua");
#endif
#endif

  return 0;
}

