/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


 
static int ImageRGBA(lua_State *L)
{
  int w = luaL_checkint(L, 1);
  int h = luaL_checkint(L, 2);
  unsigned char *pixels = iuplua_checkuchar_array(L, 3, w*h*4);
  Ihandle *ih = IupImageRGBA(w, h, pixels);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  free(pixels);
  return 1;
}
 
int iupimagergbalua_open(lua_State * L)
{
  iuplua_register(L, ImageRGBA, "ImageRGBA");


#ifdef IUPLUA_USELOH
#include "imagergba.loh"
#else
#ifdef IUPLUA_USELH
#include "imagergba.lh"
#else
  iuplua_dofile(L, "imagergba.lua");
#endif
#endif

  return 0;
}

