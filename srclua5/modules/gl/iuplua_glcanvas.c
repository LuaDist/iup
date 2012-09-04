/** \file
 * \brief iupgl binding for Lua 5.
 *
 * See Copyright Notice in "iup.h"
 */

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iupgl.h"

#include "iuplua.h"
#include "iupluagl.h"
#include "il.h"

#ifdef _WIN32
#include <windows.h>
#endif
#if defined (__APPLE__) || defined (OSX)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

static int GLIsCurrent(lua_State *L)
{  
   lua_pushboolean(L, IupGLIsCurrent(iuplua_checkihandle(L,1)));
   return 1;
}

static int GLMakeCurrent(lua_State *L)
{  
   IupGLMakeCurrent(iuplua_checkihandle(L,1));

   iuplua_get_env(L);
   iuplua_regstring(L, (const char*)glGetString(GL_VENDOR), "GL_VENDOR");
   iuplua_regstring(L, (const char*)glGetString(GL_RENDERER), "GL_RENDERER");
   iuplua_regstring(L, (const char*)glGetString(GL_VERSION), "GL_VERSION");

   return 0;
}

static int GLSwapBuffers(lua_State *L)
{  
   IupGLSwapBuffers(iuplua_checkihandle(L,1));
   return 0;
}

static int GLPalette(lua_State *L)
{
   Ihandle *self = iuplua_checkihandle(L,1);
   int index = luaL_checkint(L,2);
   float r = (float)luaL_checknumber(L,3);
   float g = (float)luaL_checknumber(L,4);
   float b = (float)luaL_checknumber(L,5);
   IupGLPalette(self, index, r, g, b);
   return 0;
}

static int GLUseFont(lua_State *L)
{
   Ihandle *self = iuplua_checkihandle(L,1);
   int first = luaL_checkint(L,2);
   int count = luaL_checkint(L,3);
   int list_base = luaL_checkint(L,4);
   IupGLUseFont(self, first, count, list_base);
   return 0;
}

static int GLWait(lua_State *L)
{
   IupGLWait(luaL_checkint(L,1));
   return 0;
}

void iuplua_glcanvasfuncs_open (lua_State *L)
{
  iuplua_register(L, GLSwapBuffers, "GLSwapBuffers");
  iuplua_register(L, GLIsCurrent, "GLIsCurrent");
  iuplua_register(L, GLMakeCurrent, "GLMakeCurrent");
  iuplua_register(L, GLPalette, "GLPalette");
  iuplua_register(L, GLUseFont, "GLUseFont");
  iuplua_register(L, GLWait, "GLWait");
}
