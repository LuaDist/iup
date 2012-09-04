/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int Hbox(lua_State *L)
{
  Ihandle *ih = IupHbox(NULL);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iuphboxlua_open(lua_State * L)
{
  iuplua_register(L, Hbox, "Hbox");


#ifdef IUPLUA_USELOH
#include "hbox.loh"
#else
#ifdef IUPLUA_USELH
#include "hbox.lh"
#else
  iuplua_dofile(L, "hbox.lua");
#endif
#endif

  return 0;
}

