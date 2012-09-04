/******************************************************************************
 * Automatically generated file. Please don't change anything.                *
 *****************************************************************************/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "iup.h"
#include "iuplua.h"
#include "iupcontrols.h"
#include "il.h"


static int Gauge(lua_State *L)
{
  Ihandle *ih = IupGauge();
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

int iupgaugelua_open(lua_State * L)
{
  iuplua_register(L, Gauge, "Gauge");


#ifdef IUPLUA_USELOH
#include "gauge.loh"
#else
#ifdef IUPLUA_USELH
#include "gauge.lh"
#else
  iuplua_dofile(L, "gauge.lua");
#endif
#endif

  return 0;
}

