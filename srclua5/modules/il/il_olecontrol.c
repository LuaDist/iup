/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iupole.h"
#include "il.h"


static int OleControl(lua_State *L)
{
  Ihandle *ih = IupOleControl((char *)luaL_checkstring(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupolecontrollua_open(lua_State * L)
{
  iuplua_register(L, OleControl, "OleControl");


#ifdef IUPLUA_USELOH
#include "olecontrol.loh"
#else
#ifdef IUPLUA_USELH
#include "olecontrol.lh"
#else
  iuplua_dofile(L, "olecontrol.lua");
#endif
#endif

  return 0;
}

 
int iupolelua_open(lua_State* L)
{
  if (iuplua_opencall_internal(L))
    IupOleControlOpen();
    
  iuplua_get_env(L);
  iupolecontrollua_open(L);
  return 0;
}

/* obligatory to use require"iupluaole" */
int luaopen_iupluaole(lua_State* L)
{
  return iupolelua_open(L);
}

