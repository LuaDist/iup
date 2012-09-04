/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int item_action(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "action");
  return iuplua_call(L, 0);
}

static int item_highlight_cb(Ihandle *self)
{
  lua_State *L = iuplua_call_start(self, "highlight_cb");
  return iuplua_call(L, 0);
}

static int Item(lua_State *L)
{
  Ihandle *ih = IupItem((char *)luaL_optstring(L, 1, NULL), NULL);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupitemlua_open(lua_State * L)
{
  iuplua_register(L, Item, "Item");

  iuplua_register_cb(L, "ACTION", (lua_CFunction)item_action, "item");
  iuplua_register_cb(L, "HIGHLIGHT_CB", (lua_CFunction)item_highlight_cb, NULL);

#ifdef IUPLUA_USELOH
#include "item.loh"
#else
#ifdef IUPLUA_USELH
#include "item.lh"
#else
  iuplua_dofile(L, "item.lua");
#endif
#endif

  return 0;
}

