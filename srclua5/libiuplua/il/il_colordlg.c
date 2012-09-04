/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int ColorDlg(lua_State *L)
{
  Ihandle *ih = IupColorDlg();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupcolordlglua_open(lua_State * L)
{
  iuplua_register(L, ColorDlg, "ColorDlg");


#ifdef IUPLUA_USELOH
#include "colordlg.loh"
#else
#ifdef IUPLUA_USELH
#include "colordlg.lh"
#else
  iuplua_dofile(L, "colordlg.lua");
#endif
#endif

  return 0;
}

