/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "il.h"


static int spinbox_spin_cb(Ihandle *self, int p0)
{
  lua_State *L = iuplua_call_start(self, "spin_cb");
  lua_pushinteger(L, p0);
  return iuplua_call(L, 1);
}

static int Spinbox(lua_State *L)
{
  Ihandle *ih = IupSpinbox(iuplua_checkihandle(L, 1));
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupspinboxlua_open(lua_State * L)
{
  iuplua_register(L, Spinbox, "Spinbox");

  iuplua_register_cb(L, "SPIN_CB", (lua_CFunction)spinbox_spin_cb, NULL);

#ifdef IUPLUA_USELOH
#include "spinbox.loh"
#else
#ifdef IUPLUA_USELH
#include "spinbox.lh"
#else
  iuplua_dofile(L, "spinbox.lua");
#endif
#endif

  return 0;
}

