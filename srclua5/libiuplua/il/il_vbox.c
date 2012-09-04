/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int Vbox(lua_State *L)
{
  Ihandle *ih = IupVbox(NULL);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupvboxlua_open(lua_State * L)
{
  iuplua_register(L, Vbox, "Vbox");


#ifdef IUPLUA_USELOH
#include "vbox.loh"
#else
#ifdef IUPLUA_USELH
#include "vbox.lh"
#else
  iuplua_dofile(L, "vbox.lua");
#endif
#endif

  return 0;
}

