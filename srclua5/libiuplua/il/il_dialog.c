/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int dialog_show_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "show_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int dialog_tips_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "tips_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int dialog_unmap_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "unmap_cb");
  return iuplua_call(L, 0);
}

static int dialog_copydata_cb(Ihandle *self, char * p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "copydata_cb");
  lua_pushstring(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int dialog_destroy_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "destroy_cb");
  return iuplua_call(L, 0);
}

static int dialog_dropdata_cb(Ihandle *self, char * p0, void* p1, int p2, int p3, int p4)
{
  lua_State *L = iuplua_call_start(self, "dropdata_cb");
  lua_pushstring(L, p0);
  lua_pushlightuserdata(L, p1);
  lua_pushinteger(L, p2);
  lua_pushinteger(L, p3);
  lua_pushinteger(L, p4);
  return iuplua_call(L, 5);
}

static int dialog_close_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "close_cb");
  return iuplua_call(L, 0);
}

static int dialog_trayclick_cb(Ihandle *self, int p0, int p1, int p2)
{
  lua_State *L = iuplua_call_start(self, "trayclick_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  return iuplua_call(L, 3);
}

static int dialog_dragend_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "dragend_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int dialog_map_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "map_cb");
  return iuplua_call(L, 0);
}

static int dialog_dropfiles_cb(Ihandle *self, char * p0, int p1, int p2, int p3)
{
  lua_State *L = iuplua_call_start(self, "dropfiles_cb");
  lua_pushstring(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  lua_pushinteger(L, p3);
  return iuplua_call(L, 4);
}

static int dialog_move_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "move_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int dialog_dragdata_cb(Ihandle *self, char * p0, void* p1, int p2)
{
  lua_State *L = iuplua_call_start(self, "dragdata_cb");
  lua_pushstring(L, p0);
  lua_pushlightuserdata(L, p1);
  lua_pushinteger(L, p2);
  return iuplua_call(L, 3);
}

static int dialog_dragdatasize_cb(Ihandle *self, char * p0)
{
  lua_State *L = iuplua_call_start(self, "dragdatasize_cb");
  lua_pushstring(L, p0);
  return iuplua_call(L, 1);
}

static int dialog_dragbegin_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "dragbegin_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int Dialog(lua_State *L)
{
  Ihandle *ih = IupDialog(iuplua_checkihandleornil(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupdialoglua_open(lua_State * L)
{
  iuplua_register(L, Dialog, "Dialog");

  iuplua_register_cb(L, "SHOW_CB", (lua_CFunction)dialog_show_cb, NULL);
  iuplua_register_cb(L, "TIPS_CB", (lua_CFunction)dialog_tips_cb, NULL);
  iuplua_register_cb(L, "UNMAP_CB", (lua_CFunction)dialog_unmap_cb, NULL);
  iuplua_register_cb(L, "COPYDATA_CB", (lua_CFunction)dialog_copydata_cb, NULL);
  iuplua_register_cb(L, "DESTROY_CB", (lua_CFunction)dialog_destroy_cb, NULL);
  iuplua_register_cb(L, "DROPDATA_CB", (lua_CFunction)dialog_dropdata_cb, NULL);
  iuplua_register_cb(L, "CLOSE_CB", (lua_CFunction)dialog_close_cb, NULL);
  iuplua_register_cb(L, "TRAYCLICK_CB", (lua_CFunction)dialog_trayclick_cb, NULL);
  iuplua_register_cb(L, "DRAGEND_CB", (lua_CFunction)dialog_dragend_cb, NULL);
  iuplua_register_cb(L, "MAP_CB", (lua_CFunction)dialog_map_cb, NULL);
  iuplua_register_cb(L, "DROPFILES_CB", (lua_CFunction)dialog_dropfiles_cb, NULL);
  iuplua_register_cb(L, "MOVE_CB", (lua_CFunction)dialog_move_cb, NULL);
  iuplua_register_cb(L, "DRAGDATA_CB", (lua_CFunction)dialog_dragdata_cb, NULL);
  iuplua_register_cb(L, "DRAGDATASIZE_CB", (lua_CFunction)dialog_dragdatasize_cb, NULL);
  iuplua_register_cb(L, "DRAGBEGIN_CB", (lua_CFunction)dialog_dragbegin_cb, NULL);

#ifdef IUPLUA_USELOH
#include "dialog.loh"
#else
#ifdef IUPLUA_USELH
#include "dialog.lh"
#else
  iuplua_dofile(L, "dialog.lua");
#endif
#endif

  return 0;
}

