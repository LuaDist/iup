/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int MessageDlg(lua_State *L)
{
  Ihandle *ih = IupMessageDlg();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupmessagedlglua_open(lua_State * L)
{
  iuplua_register(L, MessageDlg, "MessageDlg");


#ifdef IUPLUA_USELOH
#include "messagedlg.loh"
#else
#ifdef IUPLUA_USELH
#include "messagedlg.lh"
#else
  iuplua_dofile(L, "messagedlg.lua");
#endif
#endif

  return 0;
}

