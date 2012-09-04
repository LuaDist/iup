/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iupcontrols.h"
#include "il.h"


static int colorbar_switch_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "switch_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int colorbar_select_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "select_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static char * colorbar_cell_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "cell_cb");
  lua_pushinteger(L, p0);
  return iuplua_call_rs(L, 1);
}

static int colorbar_extended_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "extended_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int Colorbar(lua_State *L)
{
  Ihandle *ih = IupColorbar();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupcolorbarlua_open(lua_State * L)
{
  iuplua_register(L, Colorbar, "Colorbar");

  iuplua_register_cb(L, "SWITCH_CB", (lua_CFunction)colorbar_switch_cb, NULL);
  iuplua_register_cb(L, "SELECT_CB", (lua_CFunction)colorbar_select_cb, NULL);
  iuplua_register_cb(L, "CELL_CB", (lua_CFunction)colorbar_cell_cb, NULL);
  iuplua_register_cb(L, "EXTENDED_CB", (lua_CFunction)colorbar_extended_cb, NULL);

#ifdef IUPLUA_USELOH
#include "colorbar.loh"
#else
#ifdef IUPLUA_USELH
#include "colorbar.lh"
#else
  iuplua_dofile(L, "colorbar.lua");
#endif
#endif

  return 0;
}

