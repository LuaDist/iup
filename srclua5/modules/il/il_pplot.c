/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iup_pplot.h"
#include "il.h"


static int pplot_edit_cb(Ihandle *self, int p0, int p1, float p2, float p3, float p4, float p5)
{
  lua_State *L = iuplua_call_start(self, "edit_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushnumber(L, p2);
  lua_pushnumber(L, p3);
  lua_pushnumber(L, p4);
  lua_pushnumber(L, p5);
  return iuplua_call(L, 6);
}

static int pplot_deleteend_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "deleteend_cb");
  return iuplua_call(L, 0);
}

static int pplot_selectbegin_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "selectbegin_cb");
  return iuplua_call(L, 0);
}

static int pplot_postdraw_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "postdraw_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int pplot_delete_cb(Ihandle *self, int p0, int p1, float p2, float p3)
{
  lua_State *L = iuplua_call_start(self, "delete_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushnumber(L, p2);
  lua_pushnumber(L, p3);
  return iuplua_call(L, 4);
}

static int pplot_predraw_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "predraw_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int pplot_selectend_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "selectend_cb");
  return iuplua_call(L, 0);
}

static int pplot_select_cb(Ihandle *self, int p0, int p1, float p2, float p3, int p4)
{
  lua_State *L = iuplua_call_start(self, "select_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushnumber(L, p2);
  lua_pushnumber(L, p3);
  lua_pushinteger(L, p4);
  return iuplua_call(L, 5);
}

static int pplot_deletebegin_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "deletebegin_cb");
  return iuplua_call(L, 0);
}

static int pplot_editbegin_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "editbegin_cb");
  return iuplua_call(L, 0);
}

static int pplot_editend_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "editend_cb");
  return iuplua_call(L, 0);
}

static int PPlot(lua_State *L)
{
  Ihandle *ih = IupPPlot();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

void iuplua_pplotfuncs_open(lua_State *L);

int iuppplotlua_open(lua_State * L)
{
  iuplua_register(L, PPlot, "PPlot");

  iuplua_register_cb(L, "EDIT_CB", (lua_CFunction)pplot_edit_cb, "pplot");
  iuplua_register_cb(L, "DELETEEND_CB", (lua_CFunction)pplot_deleteend_cb, NULL);
  iuplua_register_cb(L, "SELECTBEGIN_CB", (lua_CFunction)pplot_selectbegin_cb, NULL);
  iuplua_register_cb(L, "POSTDRAW_CB", (lua_CFunction)pplot_postdraw_cb, NULL);
  iuplua_register_cb(L, "DELETE_CB", (lua_CFunction)pplot_delete_cb, NULL);
  iuplua_register_cb(L, "PREDRAW_CB", (lua_CFunction)pplot_predraw_cb, NULL);
  iuplua_register_cb(L, "SELECTEND_CB", (lua_CFunction)pplot_selectend_cb, NULL);
  iuplua_register_cb(L, "SELECT_CB", (lua_CFunction)pplot_select_cb, NULL);
  iuplua_register_cb(L, "DELETEBEGIN_CB", (lua_CFunction)pplot_deletebegin_cb, NULL);
  iuplua_register_cb(L, "EDITBEGIN_CB", (lua_CFunction)pplot_editbegin_cb, NULL);
  iuplua_register_cb(L, "EDITEND_CB", (lua_CFunction)pplot_editend_cb, NULL);

  iuplua_pplotfuncs_open(L);

#ifdef IUPLUA_USELOH
#include "pplot.loh"
#else
#ifdef IUPLUA_USELH
#include "pplot.lh"
#else
  iuplua_dofile(L, "pplot.lua");
#endif
#endif

  return 0;
}

