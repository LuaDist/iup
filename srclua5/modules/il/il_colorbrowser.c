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


static int colorbrowser_drag_cb(Ihandle *self, unsigned char  p0, unsigned char  p1, unsigned char  p2)
{
  lua_State *L = iuplua_call_start(self, "drag_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  return iuplua_call(L, 3);
}

static int colorbrowser_change_cb(Ihandle *self, unsigned char  p0, unsigned char  p1, unsigned char  p2)
{
  lua_State *L = iuplua_call_start(self, "change_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  return iuplua_call(L, 3);
}

static int ColorBrowser(lua_State *L)
{
  Ihandle *ih = IupColorBrowser();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupcolorbrowserlua_open(lua_State * L)
{
  iuplua_register(L, ColorBrowser, "ColorBrowser");

  iuplua_register_cb(L, "DRAG_CB", (lua_CFunction)colorbrowser_drag_cb, NULL);
  iuplua_register_cb(L, "CHANGE_CB", (lua_CFunction)colorbrowser_change_cb, NULL);

#ifdef IUPLUA_USELOH
#include "colorbrowser.loh"
#else
#ifdef IUPLUA_USELH
#include "colorbrowser.lh"
#else
  iuplua_dofile(L, "colorbrowser.lua");
#endif
#endif

  return 0;
}

