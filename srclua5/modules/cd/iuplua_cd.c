/** \file
 * \brief IUP Canvas Lua 5 Binding
 *
 * See Copyright Notice in cd.h
 */

#include <stdlib.h>
#include <stdio.h>

#include <cd.h>
#include <cdiup.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"

#include <cdlua.h>
#include <cdluaiup.h>
#include <cdlua5_private.h>


static void *cdiup_checkdata(lua_State *L, int param)
{
  return iuplua_checkihandle(L, param);
}

static cdluaContext cdluaiupctx = 
{
  0,
  "IUP",
  cdContextIup,
  cdiup_checkdata,
  NULL,
  0
};

int cdluaiup_open (lua_State *L)
{
  cdluaLuaState* cdL = cdlua_getstate(L);
  /* leave "cd" table at the top of the stack */
#if LUA_VERSION_NUM > 501
  luaL_openlib(L, "cd", NULL, 0);
#else
  lua_pushstring(L, "cd");
  lua_gettable(L, LUA_GLOBALSINDEX);     
#endif
  cdlua_addcontext(L, cdL, &cdluaiupctx);
  return 1;
}

int luaopen_iupluacd(lua_State* L)
{
  return cdluaiup_open(L);
}

