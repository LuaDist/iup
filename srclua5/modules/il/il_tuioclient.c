/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iuptuio.h"
#include "il.h"


static int TuioClient(lua_State *L)
{
  Ihandle *ih = IupTuioClient(luaL_optinteger(L, 1, 0));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iuptuioclientlua_open(lua_State * L)
{
  iuplua_register(L, TuioClient, "TuioClient");


#ifdef IUPLUA_USELOH
#include "tuioclient.loh"
#else
#ifdef IUPLUA_USELH
#include "tuioclient.lh"
#else
  iuplua_dofile(L, "tuioclient.lua");
#endif
#endif

  return 0;
}

 

int iuptuiolua_open(lua_State* L)
{
  if (iuplua_opencall_internal(L))
    IupTuioOpen();
    
  iuplua_get_env(L);
  iuptuioclientlua_open(L);
  
  return 0;
}

/* obligatory to use require"iupluatuio" */
int luaopen_iupluatuio(lua_State* L)
{
  return iuptuiolua_open(L);
}

