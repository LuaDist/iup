/** \file
 * \brief Bindig of iupcontrols to Lua 5.
 *
 * See Copyright Notice in "iup.h"
 */
 
#include <lua.h>
#include <lualib.h>

#include "iup.h"
#include "iupcontrols.h"

#include "iuplua.h"
#include "iupluacontrols.h"
#include "il.h"
#include "il_controls.h"

int iupcontrolslua_open(lua_State * L)
{
  if (iuplua_opencall_internal(L))
    IupControlsOpen();

  iuplua_get_env(L);

  iupgaugelua_open(L);
  iupmatrixlua_open(L);
  iupmasklua_open(L);
  iupdiallua_open(L);
  iupcolorbrowserlua_open(L);
  iupcellslua_open(L);
  iupcolorbarlua_open(L);

  return 0;
}

int iupcontrolslua_close(lua_State * L)
{
  if (iuplua_opencall_internal(L))
    IupControlsClose();
  return 0;
}

/* obligatory to use require"iupluacontrols" */
int luaopen_iupluacontrols(lua_State* L)
{
  return iupcontrolslua_open(L);
}

