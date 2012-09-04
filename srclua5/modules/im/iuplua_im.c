/** \file
 * \brief IULUA core - Bindig of iup to Lua 5.
 *
 * See Copyright Notice in "iup.h"
 */

#include <im.h>
#include <im_image.h>
 
#include "iup.h"
#include "iupim.h"

#include <lua.h>
#include <lauxlib.h>

#include "iuplua.h"
#include "iupluaim.h"
#include "imlua.h"
#include "il.h"


static int GetNativeHandleImage(lua_State *L)
{
  void* handle;
  imImage* image;
  luaL_checktype (L, 1, LUA_TLIGHTUSERDATA);
  handle = lua_touserdata(L, 1);
  image = IupGetNativeHandleImage(handle);
  imlua_pushimage(L, image);
  return 1;
}

static int GetImageNativeHandle(lua_State *L)
{
  imImage* image = imlua_checkimage(L, 1);
  void* handle = IupGetImageNativeHandle(image);
  lua_pushlightuserdata(L, handle);
  return 1;
}

static int SaveImage(lua_State *L)
{
  Ihandle *image = iuplua_checkihandle(L,1);
  const char *filename = luaL_checkstring(L, 2);
  const char *format = luaL_checkstring(L, 3);
  lua_pushboolean(L, IupSaveImage(image, filename, format));
  return 1;
}

static int LoadImage(lua_State *L)
{
  const char *filename = luaL_checkstring(L, 1);
  Ihandle* image = IupLoadImage(filename);
  iuplua_plugstate(L, image);
  iuplua_pushihandle(L, image);
  return 1;
}

int iupimlua_open(lua_State *L)
{
  iuplua_get_env(L);
  iuplua_register(L, LoadImage, "LoadImage");
  iuplua_register(L, SaveImage, "SaveImage");
  iuplua_register(L, GetNativeHandleImage, "GetNativeHandleImage");
  iuplua_register(L, GetImageNativeHandle, "GetImageNativeHandle");
  return 0; /* nothing in stack */
}

/* obligatory to use require"iupluaim" */
int luaopen_iupluaim(lua_State* L)
{
  return iupimlua_open(L);
}

