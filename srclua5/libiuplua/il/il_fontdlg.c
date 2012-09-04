/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int FontDlg(lua_State *L)
{
  Ihandle *ih = IupFontDlg();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupfontdlglua_open(lua_State * L)
{
  iuplua_register(L, FontDlg, "FontDlg");


#ifdef IUPLUA_USELOH
#include "fontdlg.loh"
#else
#ifdef IUPLUA_USELH
#include "fontdlg.lh"
#else
  iuplua_dofile(L, "fontdlg.lua");
#endif
#endif

  return 0;
}

