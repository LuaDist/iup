/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int Clipboard(lua_State *L)
{
  Ihandle *ih = IupClipboard();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupclipboardlua_open(lua_State * L)
{
  iuplua_register(L, Clipboard, "Clipboard");


#ifdef IUPLUA_USELOH
#include "clipboard.loh"
#else
#ifdef IUPLUA_USELH
#include "clipboard.lh"
#else
  iuplua_dofile(L, "clipboard.lua");
#endif
#endif

  return 0;
}

