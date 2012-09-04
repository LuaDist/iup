/** \file
 * \brief iuptree binding for Lua 5.
 *
 * See Copyright Notice in "iup.h"
 */

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iupcontrols.h"

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"

/* 
   The REGISTRY is used to store references to the associated Lua objects.
   Given an ID, to retreive a Lua object is quite simple.

   But given a user_id to obtain the ID is more complicated.
   The iup.TREEREFTABLE is used to do this mapping.
   We use the object as the index to this table.
*/

/* iup.TREEREFTABLE[object at pos] = ref */
static void tree_settableref(lua_State *L, int pos, int ref)
{
  lua_getglobal(L, "iup");
  lua_pushstring(L, "TREEREFTABLE");
  lua_gettable(L, -2);
  lua_remove(L, -2); /* remove "iup" from stack */
  lua_pushvalue(L, pos);
  if(ref == LUA_NOREF)
    lua_pushnil(L);
  else
    lua_pushinteger(L, ref);
  lua_settable(L, -3);
  lua_pop(L, 1);
}

/* ref = iup.TREEREFTABLE[object at pos] */
static int tree_gettableref(lua_State *L, int pos)
{
  lua_getglobal(L, "iup");
  lua_pushstring(L, "TREEREFTABLE");
  lua_gettable(L, -2);
  lua_remove(L, -2); /* remove "iup" from stack */
  lua_pushvalue(L, pos);
  lua_gettable(L, -2);
  if (lua_isnil(L, -1))
  {
    lua_pop(L, 1);
    return LUA_NOREF;
  }
  else
  {
    int ref = lua_tointeger(L, -1);
    lua_pop(L, 1);
    return ref;
  }
}

static void tree_push_userid(lua_State *L, void* userid)
{
  int ref = (int)userid;
  if (ref == 0) /* userid is actually NULL */
    lua_pushnil(L);
  else 
  {
    if (ref > 0) ref--; /* only positive references are shifted */
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
  }
}

/*****************************************************************************
 * Userdata/Table <-> id functions 
 ****************************************************************************/

static int TreeGetId(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  int ref = tree_gettableref(L, 2);
  if (ref == LUA_NOREF)
    lua_pushnil(L);
  else
  {
    int id;
    if (ref >= 0) ref++;  /* only positive references are shifted */
    id = IupTreeGetId(ih, (void*)ref);
    if (id == -1)
      lua_pushnil(L);
    else
      lua_pushinteger(L, id);
  }
  return 1;        
}

static int TreeGetUserId(lua_State *L)
{  
  Ihandle *ih = iuplua_checkihandle(L,1);
  int id = luaL_checkint(L,2);
  tree_push_userid(L, IupTreeGetUserId(ih, id));
  return 1;
}

static int TreeSetUserId(lua_State *L)
{  
  Ihandle *ih = iuplua_checkihandle(L,1);
  int id = luaL_checkint(L,2);
  int ref = (int)IupTreeGetUserId(ih, id);
  if (ref != 0) /* userid is not NULL */
  {
    if (ref > 0) ref--; /* only positive references are shifted */

    /* release the previous object referenced there */
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    tree_settableref(L, 4, LUA_NOREF);
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
    lua_pop(L, 1);
  }

  if (lua_isnil(L, 3))
    IupTreeSetUserId(ih, id, NULL);
  else
  {
    /* add a new reference */
    lua_pushvalue(L, 3);
    ref = luaL_ref(L, LUA_REGISTRYINDEX);
    tree_settableref(L, 3, ref);

    if (ref >= 0) ref++;  /* only positive references are shifted */
    IupTreeSetUserId(ih, id, (void*)ref);
  }

  return 0;
}

static int tree_multiselection_cb(Ihandle *ih, int* ids, int p1)
{
  int i;
  lua_State *L = iuplua_call_start(ih, "multiselection_cb");
  lua_createtable(L, p1, 0);
  for (i = 0; i < p1; i++)
  {
    lua_pushinteger(L,i+1);
    lua_pushinteger(L,ids[i]);
    lua_settable(L,-3);
  }
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int tree_multiunselection_cb(Ihandle *ih, int* ids, int p1)
{
  int i;
  lua_State *L = iuplua_call_start(ih, "multiunselection_cb");
  lua_createtable(L, p1, 0);
  for (i = 0; i < p1; i++)
  {
    lua_pushinteger(L,i+1);
    lua_pushinteger(L,ids[i]);
    lua_settable(L,-3);
  }
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int tree_noderemoved_cb(Ihandle *ih, void* p1)
{
  lua_State *L = iuplua_call_start(ih, "noderemoved_cb");
  tree_push_userid(L, p1);
  return iuplua_call(L, 1);
}

void iuplua_treefuncs_open (lua_State *L)
{
  iuplua_register_cb(L, "MULTISELECTION_CB", (lua_CFunction)tree_multiselection_cb, NULL);
  iuplua_register_cb(L, "MULTIUNSELECTION_CB", (lua_CFunction)tree_multiunselection_cb, NULL);
  iuplua_register_cb(L, "NODEREMOVED_CB", (lua_CFunction)tree_noderemoved_cb, NULL);

/* In Lua 5:
  TreeSetTableId = TreeSetUserId
  TreeGetTable   = TreeGetUserId
  TreeGetTableId = TreeGetId
*/

  /* Userdata <-> id */
  iuplua_register(L, TreeGetId, "TreeGetId");
  iuplua_register(L, TreeGetUserId, "TreeGetUserId");
  iuplua_register(L, TreeSetUserId, "TreeSetUserId");

  /* Table <-> id */
  iuplua_register(L, TreeGetId, "TreeGetTableId");
  iuplua_register(L, TreeGetUserId, "TreeGetTable");
  iuplua_register(L, TreeSetUserId, "TreeSetTableId");
}
