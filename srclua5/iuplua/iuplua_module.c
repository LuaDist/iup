#include <lua.h>
#include "iup.h"
#include "iuplua.h"

/* obligatory to use require"iuplua" */
int luaopen_iuplua(lua_State* L)
{
  return iuplua_open(L);
}

/* obligatory to use require"iuplua51" */
int luaopen_iuplua51(lua_State* L)
{
  return iuplua_open(L);
}
