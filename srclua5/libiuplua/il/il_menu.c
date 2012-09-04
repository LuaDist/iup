/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int menu_open_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "open_cb");
  return iuplua_call(L, 0);
}

static int menu_menuclose_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "menuclose_cb");
  return iuplua_call(L, 0);
}

static int Menu(lua_State *L)
{
  Ihandle *ih = IupMenu(NULL);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupmenulua_open(lua_State * L)
{
  iuplua_register(L, Menu, "Menu");

  iuplua_register_cb(L, "OPEN_CB", (lua_CFunction)menu_open_cb, NULL);
  iuplua_register_cb(L, "MENUCLOSE_CB", (lua_CFunction)menu_menuclose_cb, NULL);

#ifdef IUPLUA_USELOH
#include "menu.loh"
#else
#ifdef IUPLUA_USELH
#include "menu.lh"
#else
  iuplua_dofile(L, "menu.lua");
#endif
#endif

  return 0;
}

