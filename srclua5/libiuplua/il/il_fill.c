/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int Fill(lua_State *L)
{
  Ihandle *ih = IupFill();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupfilllua_open(lua_State * L)
{
  iuplua_register(L, Fill, "Fill");


#ifdef IUPLUA_USELOH
#include "fill.loh"
#else
#ifdef IUPLUA_USELH
#include "fill.lh"
#else
  iuplua_dofile(L, "fill.lua");
#endif
#endif

  return 0;
}

