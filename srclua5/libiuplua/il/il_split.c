/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int Split(lua_State *L)
{
  Ihandle *ih = IupSplit(iuplua_checkihandleornil(L, 1), iuplua_checkihandleornil(L, 2));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupsplitlua_open(lua_State * L)
{
  iuplua_register(L, Split, "Split");


#ifdef IUPLUA_USELOH
#include "split.loh"
#else
#ifdef IUPLUA_USELH
#include "split.lh"
#else
  iuplua_dofile(L, "split.lua");
#endif
#endif

  return 0;
}

