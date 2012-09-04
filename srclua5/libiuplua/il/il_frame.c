/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int Frame(lua_State *L)
{
  Ihandle *ih = IupFrame(iuplua_checkihandleornil(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupframelua_open(lua_State * L)
{
  iuplua_register(L, Frame, "Frame");


#ifdef IUPLUA_USELOH
#include "frame.loh"
#else
#ifdef IUPLUA_USELH
#include "frame.lh"
#else
  iuplua_dofile(L, "frame.lua");
#endif
#endif

  return 0;
}

