/** \file
 * \brief iupmask binding for Lua 5.
 *
 * See Copyright Notice in "iup.h"
 */

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iupmask.h"

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"


static int cfMaskRemove (lua_State *L)
{
  iupmaskRemove(iuplua_checkihandle(L,1));
  return 0;
}

static int cfMaskMatRemove(lua_State *L)
{
  iupmaskMatRemove(iuplua_checkihandle(L,1),
                   luaL_checkint(L,2),
                   luaL_checkint(L,3));
  return 0;
}

static int cfMaskSet (lua_State *L)
{
  iupmaskSet(iuplua_checkihandle(L,1),
             (char*)luaL_checkstring(L,2),
             luaL_checkint(L,3),
             luaL_checkint(L,4));
  return 0;
}

static int cfMaskMatSet(lua_State *L)
{
  iupmaskMatSet(iuplua_checkihandle(L,1),
                (char*)luaL_checkstring(L,2),
                luaL_checkint(L,3),
                luaL_checkint(L,4),
                luaL_checkint(L,5),
                luaL_checkint(L,6));
  return 0;
}

static int cfMaskSetInt(lua_State *L)
{
  iupmaskSetInt(iuplua_checkihandle(L,1),
                luaL_checkint(L,2),
                luaL_checkint(L,3),
                luaL_checkint(L,4));
  return 0;
}

static int cfMaskSetFloat(lua_State *L)
{
  iupmaskSetFloat(iuplua_checkihandle(L,1),
                  luaL_checkint(L,2),
                  (float)luaL_checknumber(L,3),
                  (float)luaL_checknumber(L,4));
  return 0;
}

static int cfMaskMatSetInt (lua_State *L)
{
  iupmaskMatSetInt(iuplua_checkihandle(L,1),
                   luaL_checkint(L,2),
                   luaL_checkint(L,3),
                   luaL_checkint(L,4),
                   luaL_checkint(L,5),
                   luaL_checkint(L,6));
  return 0;
}

static int cfMaskMatSetFloat (lua_State *L)
{
  iupmaskMatSetFloat(iuplua_checkihandle(L,1),
                     luaL_checkint(L,2),
                     (float)luaL_checknumber(L,3),
                     (float)luaL_checknumber(L,4),
                     luaL_checkint(L,5),
                     luaL_checkint(L,6));
  return 0;
}

static int cfMaskCheck (lua_State *L)
{
  iupmaskCheck(iuplua_checkihandle(L,1));
  return 0;
}

static int cfMaskMatCheck (lua_State *L)
{
  iupmaskMatCheck(iuplua_checkihandle(L,1),
                  luaL_checkint(L,2),
                  luaL_checkint(L,3));
  return 0;
}

static int match_cb (Ihandle *handle)
{
  lua_State *L = iuplua_call_start(handle, "match_cb");
  return iuplua_call(L, 0);
}

void iupmasklua_open(lua_State *L)
{
  iuplua_regstring(L, IUP_MASK_FLOAT, "MASK_FLOAT");
  iuplua_regstring(L, IUP_MASK_UFLOAT, "MASK_UFLOAT");
  iuplua_regstring(L, IUP_MASK_EFLOAT, "MASK_EFLOAT");
  iuplua_regstring(L, IUP_MASK_INT, "MASK_INT");
  iuplua_regstring(L, IUP_MASK_UINT, "MASK_UINT");

  iuplua_register(L, cfMaskRemove, "maskRemove");
  iuplua_register(L, cfMaskMatRemove, "maskMatRemove");
  iuplua_register(L, cfMaskSet, "maskSet");
  iuplua_register(L, cfMaskMatSet, "maskMatSet");
  iuplua_register(L, cfMaskSetInt, "maskSetInt");
  iuplua_register(L, cfMaskSetFloat, "maskSetFloat");
  iuplua_register(L, cfMaskMatSetInt, "maskMatSetInt");
  iuplua_register(L, cfMaskMatSetFloat, "maskMatSetFloat");
  iuplua_register(L, cfMaskCheck, "maskCheck");
  iuplua_register(L, cfMaskMatCheck, "maskMatCheck");

  iuplua_register_cb(L, "MATCH_CB", (lua_CFunction)match_cb, NULL);
}
