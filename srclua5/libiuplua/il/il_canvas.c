/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int canvas_action(Ihandle *self, float p0, float p1)
{
  lua_State *L = iuplua_call_start(self, "action");
  lua_pushnumber(L, p0);
  lua_pushnumber(L, p1);
  return iuplua_call(L, 2);
}

static int canvas_leavewindow_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "leavewindow_cb");
  return iuplua_call(L, 0);
}

static int canvas_resize_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "resize_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int canvas_touch_cb(Ihandle *self, int p0, int p1, int p2, char * p3)
{
  lua_State *L = iuplua_call_start(self, "touch_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  lua_pushstring(L, p3);
  return iuplua_call(L, 4);
}

static int canvas_wheel_cb(Ihandle *self, float p0, int p1, int p2, char * p3)
{
  lua_State *L = iuplua_call_start(self, "wheel_cb");
  lua_pushnumber(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  lua_pushstring(L, p3);
  return iuplua_call(L, 4);
}

static int canvas_mdiactivate_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "mdiactivate_cb");
  return iuplua_call(L, 0);
}

static int canvas_button_cb(Ihandle *self, int p0, int p1, int p2, int p3, char * p4)
{
  lua_State *L = iuplua_call_start(self, "button_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  lua_pushinteger(L, p3);
  lua_pushstring(L, p4);
  return iuplua_call(L, 5);
}

static int canvas_focus_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "focus_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int canvas_wom_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "wom_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int canvas_keypress_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "keypress_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int canvas_motion_cb(Ihandle *self, int p0, int p1, char * p2)
{
  lua_State *L = iuplua_call_start(self, "motion_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushstring(L, p2);
  return iuplua_call(L, 3);
}

static int canvas_enterwindow_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "enterwindow_cb");
  return iuplua_call(L, 0);
}

static int canvas_scroll_cb(Ihandle *self, int p0, float p1, float p2)
{
  lua_State *L = iuplua_call_start(self, "scroll_cb");
  lua_pushinteger(L, p0);
  lua_pushnumber(L, p1);
  lua_pushnumber(L, p2);
  return iuplua_call(L, 3);
}

static int Canvas(lua_State *L)
{
  Ihandle *ih = IupCanvas(NULL);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupcanvaslua_open(lua_State * L)
{
  iuplua_register(L, Canvas, "Canvas");

  iuplua_register_cb(L, "ACTION", (lua_CFunction)canvas_action, "canvas");
  iuplua_register_cb(L, "LEAVEWINDOW_CB", (lua_CFunction)canvas_leavewindow_cb, NULL);
  iuplua_register_cb(L, "RESIZE_CB", (lua_CFunction)canvas_resize_cb, NULL);
  iuplua_register_cb(L, "TOUCH_CB", (lua_CFunction)canvas_touch_cb, NULL);
  iuplua_register_cb(L, "WHEEL_CB", (lua_CFunction)canvas_wheel_cb, NULL);
  iuplua_register_cb(L, "MDIACTIVATE_CB", (lua_CFunction)canvas_mdiactivate_cb, NULL);
  iuplua_register_cb(L, "BUTTON_CB", (lua_CFunction)canvas_button_cb, NULL);
  iuplua_register_cb(L, "FOCUS_CB", (lua_CFunction)canvas_focus_cb, NULL);
  iuplua_register_cb(L, "WOM_CB", (lua_CFunction)canvas_wom_cb, NULL);
  iuplua_register_cb(L, "KEYPRESS_CB", (lua_CFunction)canvas_keypress_cb, NULL);
  iuplua_register_cb(L, "MOTION_CB", (lua_CFunction)canvas_motion_cb, NULL);
  iuplua_register_cb(L, "ENTERWINDOW_CB", (lua_CFunction)canvas_enterwindow_cb, NULL);
  iuplua_register_cb(L, "SCROLL_CB", (lua_CFunction)canvas_scroll_cb, NULL);

#ifdef IUPLUA_USELOH
#include "canvas.loh"
#else
#ifdef IUPLUA_USELH
#include "canvas.lh"
#else
  iuplua_dofile(L, "canvas.lua");
#endif
#endif

  return 0;
}

