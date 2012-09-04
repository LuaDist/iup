/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int ProgressBar(lua_State *L)
{
  Ihandle *ih = IupProgressBar();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupprogressbarlua_open(lua_State * L)
{
  iuplua_register(L, ProgressBar, "ProgressBar");


#ifdef IUPLUA_USELOH
#include "progressbar.loh"
#else
#ifdef IUPLUA_USELH
#include "progressbar.lh"
#else
  iuplua_dofile(L, "progressbar.lua");
#endif
#endif

  return 0;
}

