/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iup_mglplot.h"
#include "il.h"


static int mglplot_predraw_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "predraw_cb");
  return iuplua_call(L, 0);
}

static int mglplot_postdraw_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "postdraw_cb");
  return iuplua_call(L, 0);
}

static int MglPlot(lua_State *L)
{
  Ihandle *ih = IupMglPlot();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

void iuplua_mglplotfuncs_open(lua_State *L);

int iupmglplotlua_open(lua_State * L)
{
  iuplua_register(L, MglPlot, "MglPlot");

  iuplua_register_cb(L, "PREDRAW_CB", (lua_CFunction)mglplot_predraw_cb, NULL);
  iuplua_register_cb(L, "POSTDRAW_CB", (lua_CFunction)mglplot_postdraw_cb, NULL);

  iuplua_mglplotfuncs_open(L);

#ifdef IUPLUA_USELOH
#include "mglplot.loh"
#else
#ifdef IUPLUA_USELH
#include "mglplot.lh"
#else
  iuplua_dofile(L, "mglplot.lua");
#endif
#endif

  return 0;
}

