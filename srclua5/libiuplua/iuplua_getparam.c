/** \file
 * \brief IupGetParam bindig to Lua 5.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"


/* Used only by the Lua binding */
int iupGetParamCount(const char *format, int *param_extra);
char iupGetParamType(const char* format, int *line_size);


typedef struct _getparam_data
{
  lua_State *L;
  int has_func;
  int func_ref;
}getparam_data;

static int param_action(Ihandle* dialog, int param_index, void* user_data)
{
  int ret = 1;
  getparam_data* gp = (getparam_data*)user_data;
  if (gp->has_func)
  {
    lua_State *L = gp->L;
    lua_rawgeti(L, LUA_REGISTRYINDEX, gp->func_ref);
    iuplua_plugstate(L, dialog);
    iuplua_pushihandle(L, dialog);
    lua_pushinteger(L, param_index);
    if (iuplua_call_raw(L, 2, 1) != 0)    /* 2 args, 1 return */
    {
      ret = lua_tointeger(L,-1);
      lua_pop(L, 1);
    }
  }
  return ret;
}

static int GetParam(lua_State *L)
{
  getparam_data gp;
  const char* title = luaL_checkstring(L, 1);
  void* user_data = (void*)&gp;
  const char* format = luaL_checkstring(L, 3);
  int param_count, param_extra, i, size, ret,
      line_size = 0, lua_param_start = 4, max_str;
  const char* f = format;
  const char* s;
  void* param_data[50];
  char param_type[50];

  gp.L = L;
  gp.has_func = 0;
  gp.func_ref = 0;

  memset(param_data, 0, sizeof(void*)*50);
  memset(param_type, 0, sizeof(char)*50);

  param_count = iupGetParamCount(format, &param_extra);

  for (i = 0; i < param_count; i++)
  {
    char t = iupGetParamType(f, &line_size);

    switch(t)
    {
    case 'u':
    case 't':
      f += line_size;
      i--; /* compensate next increment */
      continue; /* notice this will go to the next i */
    case 'b':
/*  TO DO: add this code some day:
      if (lua_isboolean(L, lua_param_start))
      {
        param_data[i] = malloc(sizeof(int));
        *(int*)(param_data[i]) = lua_toboolean(L, lua_param_start); lua_param_start++;
        break;
      }  
      else continue and get an integer  */
    case 'i':
    case 'o':
    case 'l':
      param_data[i] = malloc(sizeof(int));
      *(int*)(param_data[i]) = luaL_checkint(L, lua_param_start); lua_param_start++;
      break;
    case 'a':
    case 'r':
      param_data[i] = malloc(sizeof(float));
      *(float*)(param_data[i]) = (float)luaL_checknumber(L, lua_param_start); lua_param_start++;
      break;
    case 'f':
    case 's':
    case 'm':
    case 'n':
    case 'c':
      max_str = 512;
      if (t == 'f')
        max_str = 4096;
      else if (t == 'm')
        max_str = 10240;
      s = luaL_checkstring(L, lua_param_start); lua_param_start++;
      size = strlen(s);
      if (size < max_str)
        param_data[i] = malloc(max_str);
      else
        param_data[i] = malloc(2*size);
      memcpy(param_data[i], s, size+1);
      break;
    }

    param_type[i] = t;
    f += line_size;
  }

  if (lua_isfunction(L, 2))
  {
    lua_pushvalue(L, 2);
    gp.func_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    gp.has_func = 1;
  }

  ret = IupGetParamv(title, param_action, user_data, format, param_count, param_extra, param_data);

  lua_pushboolean(L, ret);

  if (ret)
  {
    for (i = 0; i < param_count; i++)
    {
      switch(param_type[i])
      {
      case 'b':
      case 'i':
      case 'o':
      case 'l':
        lua_pushinteger(L, *(int*)(param_data[i]));
        break;
      case 'a':
      case 'r':
        lua_pushnumber(L, *(float*)(param_data[i]));
        break;
      case 'f':
      case 'n':
      case 'c':
      case 's':
      case 'm':
        lua_pushstring(L, (char*)(param_data[i]));
        break;
      }
    }
  }

  for (i = 0; i < param_count; i++)
  {
    free(param_data[i]);
  }

  if (gp.has_func)
    luaL_unref(L, LUA_REGISTRYINDEX, gp.func_ref);

  if (ret)
    return param_count+1;
  else
    return 1;
}

static int GetParamParam(lua_State *L)
{
  Ihandle *dialog = iuplua_checkihandle(L, 1);
  int param_index = luaL_checkint(L, 2);
  Ihandle* param;
  char param_str[50];
  sprintf(param_str, "PARAM%d", param_index);
  param = (Ihandle*)IupGetAttribute(dialog, param_str);
  iuplua_plugstate(L, param);
  iuplua_pushihandle(L, param);
  return 1;
}

void iupgetparamlua_open(lua_State * L)
{
  iuplua_register(L, GetParam, "GetParam");
  iuplua_register(L, GetParamParam, "GetParamParam");
}
