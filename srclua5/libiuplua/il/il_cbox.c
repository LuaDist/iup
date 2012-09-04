/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int Cbox(lua_State *L)
{
  Ihandle *ih = IupCbox(NULL);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupcboxlua_open(lua_State * L)
{
  iuplua_register(L, Cbox, "Cbox");


#ifdef IUPLUA_USELOH
#include "cbox.loh"
#else
#ifdef IUPLUA_USELH
#include "cbox.lh"
#else
  iuplua_dofile(L, "cbox.lua");
#endif
#endif

  return 0;
}

