/** \file
 * \brief matrix binding for Lua 5.
 *
 * See Copyright Notice in "iup.h"
 */

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iupcontrols.h"
#include <cd.h>
#include <cdlua.h>

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"

#include "iup_attrib.h"
#include "iup_object.h"


static int matrix_draw_cb(Ihandle *self, int p0, int p1, int p2, int p3, int p4, int p5, cdCanvas* cnv)
{
  lua_State *L = iuplua_call_start(self, "draw_cb");
  lua_pushnumber(L, p0);
  lua_pushnumber(L, p1);
  lua_pushnumber(L, p2);
  lua_pushnumber(L, p3);
  lua_pushnumber(L, p4);
  lua_pushnumber(L, p5);
  cdlua_pushcanvas(L, cnv);
  return iuplua_call(L, 7);
}

static int matrix_bgcolor_cb(Ihandle *self, int p0, int p1, unsigned int *p2, unsigned int *p3, unsigned int *p4)
{
  int ret;
  lua_State *L = iuplua_call_start(self, "bgcolor_cb");
  lua_pushnumber(L, p0);
  lua_pushnumber(L, p1);
  ret = iuplua_call_raw(L, 2+2, LUA_MULTRET);   /* 2 args + 2 args(errormsg, handle), variable number of returns */
  if (ret || lua_isnil(L, -1))
    return IUP_DEFAULT;
  ret = lua_tointeger(L,-1);

  if (ret == IUP_IGNORE) 
  {
    lua_pop(L, 1);
    return IUP_IGNORE;
  }

  *p2 = (unsigned int)lua_tointeger(L, -4); 
  *p3 = (unsigned int)lua_tointeger(L, -3); 
  *p4 = (unsigned int)lua_tointeger(L, -2); 
  lua_pop(L, 4);
  return IUP_DEFAULT;
}

static int matrix_fgcolor_cb(Ihandle *self, int p0, int p1, unsigned int *p2, unsigned int *p3, unsigned int *p4)
{
  int ret;
  lua_State *L = iuplua_call_start(self, "fgcolor_cb");
  lua_pushnumber(L, p0);
  lua_pushnumber(L, p1);
  ret = iuplua_call_raw(L, 2+2, LUA_MULTRET);   /* 2 args + 2 args(errormsg, handle), variable number of returns */
  if (ret || lua_isnil(L, -1))
    return IUP_DEFAULT;
  ret = lua_tointeger(L,-1);

  if (ret == IUP_IGNORE) 
  {
    lua_pop(L, 1);
    return IUP_IGNORE;
  }

  *p2 = (unsigned int)lua_tointeger(L, -4); 
  *p3 = (unsigned int)lua_tointeger(L, -3); 
  *p4 = (unsigned int)lua_tointeger(L, -2); 
  lua_pop(L, 4);
  return IUP_DEFAULT;
}

static int MatGetAttribute(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  const char *name = luaL_checkstring(L,2);
  int lin = luaL_checkint(L,3);
  int col = luaL_checkint(L,4);
  const char *value = IupMatGetAttribute(ih, name, lin, col);
  if (!value || iupATTRIB_ISINTERNAL(name))
    lua_pushnil(L);
  else
  {
    if (iupAttribIsPointer(ih, name))
    {
      if (iupObjectCheck((Ihandle*)value))
        iuplua_pushihandle(L, (Ihandle*)value);
      else
        lua_pushlightuserdata(L, (void*)value);
    }
    else
      lua_pushstring(L,value);
  }
  return 1;
}

static int MatStoreAttribute(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  const char *a = luaL_checkstring(L,2);
  int lin = luaL_checkint(L,3);
  int col = luaL_checkint(L,4);

  if (lua_isnil(L,5)) 
    IupMatSetAttribute(ih,a,lin, col,NULL);
  else 
  {
    const char *v;
    if(lua_isuserdata(L,5)) 
    {
      v = lua_touserdata(L,5);
      IupMatSetAttribute(ih,a,lin, col,v);
    }
    else 
    {
      v = luaL_checkstring(L,5);
      IupMatStoreAttribute(ih,a,lin, col,v);
    }
  }
  return 0;
}

void iuplua_matrixfuncs_open (lua_State *L)
{
  iuplua_register(L, MatGetAttribute, "MatGetAttribute");
  iuplua_register(L, MatStoreAttribute, "MatStoreAttribute");
  iuplua_register(L, MatStoreAttribute, "MatSetAttribute");

  iuplua_register_cb(L, "BGCOLOR_CB", (lua_CFunction)matrix_bgcolor_cb, NULL);
  iuplua_register_cb(L, "FGCOLOR_CB", (lua_CFunction)matrix_fgcolor_cb, NULL);
  iuplua_register_cb(L, "DRAW_CB", (lua_CFunction)matrix_draw_cb, NULL);
}
