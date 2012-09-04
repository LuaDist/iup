/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int User(lua_State *L)
{
  Ihandle *ih = IupUser();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupuserlua_open(lua_State * L)
{
  iuplua_register(L, User, "User");


#ifdef IUPLUA_USELOH
#include "user.loh"
#else
#ifdef IUPLUA_USELH
#include "user.lh"
#else
  iuplua_dofile(L, "user.lua");
#endif
#endif

  return 0;
}

