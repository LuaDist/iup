/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int filedlg_file_cb(Ihandle *self, char * p0, char * p1)
{
  lua_State *L = iuplua_call_start(self, "file_cb");
  lua_pushstring(L, p0);
  lua_pushstring(L, p1);
  return iuplua_call(L, 2);
}

static int FileDlg(lua_State *L)
{
  Ihandle *ih = IupFileDlg();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupfiledlglua_open(lua_State * L)
{
  iuplua_register(L, FileDlg, "FileDlg");

  iuplua_register_cb(L, "FILE_CB", (lua_CFunction)filedlg_file_cb, NULL);

#ifdef IUPLUA_USELOH
#include "filedlg.loh"
#else
#ifdef IUPLUA_USELH
#include "filedlg.lh"
#else
  iuplua_dofile(L, "filedlg.lua");
#endif
#endif

  return 0;
}

