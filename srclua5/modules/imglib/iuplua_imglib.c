/** \file
 * \brief Iup Image Library Lua 5 Binding
 *
 * See Copyright Notice in cd.h
 */

#include <stdlib.h>
#include <stdio.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iupcontrols.h"
#include "il.h"


static int imagelibopen(lua_State *L)
{
  (void)L;
  IupImageLibOpen();
  return 0;
}

static int iupluaimglib_open (lua_State *L)
{
  IupImageLibOpen();
  
  iuplua_get_env(L);
  iuplua_register(L, imagelibopen, "ImageLibOpen");
  return 0; /* nothing in stack */
}

int luaopen_iupluaimglib(lua_State* L)
{
  return iupluaimglib_open(L);
}

