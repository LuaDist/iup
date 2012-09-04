/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int Zbox(lua_State *L)
{
  Ihandle *ih = IupZbox(NULL);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupzboxlua_open(lua_State * L)
{
  iuplua_register(L, Zbox, "Zbox");


#ifdef IUPLUA_USELOH
#include "zbox.loh"
#else
#ifdef IUPLUA_USELH
#include "zbox.lh"
#else
  iuplua_dofile(L, "zbox.lua");
#endif
#endif

  return 0;
}

