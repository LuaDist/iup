/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int Normalizer(lua_State *L)
{
  Ihandle *ih = IupNormalizer(NULL);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupnormalizerlua_open(lua_State * L)
{
  iuplua_register(L, Normalizer, "Normalizer");


#ifdef IUPLUA_USELOH
#include "normalizer.loh"
#else
#ifdef IUPLUA_USELH
#include "normalizer.lh"
#else
  iuplua_dofile(L, "normalizer.lua");
#endif
#endif

  return 0;
}

