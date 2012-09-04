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


static char * matrix_font_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "font_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call_rs(L, 2);
}

static int matrix_draw_cb(Ihandle *self, int p0, int p1, int p2, int p3, int p4, int p5, int p6)
{
  lua_State *L = iuplua_call_start(self, "draw_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  lua_pushinteger(L, p3);
  lua_pushinteger(L, p4);
  lua_pushinteger(L, p5);
  lua_pushinteger(L, p6);
  return iuplua_call(L, 7);
}

static int matrix_edition_cb(Ihandle *self, int p0, int p1, int p2, int p3)
{
  lua_State *L = iuplua_call_start(self, "edition_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  lua_pushinteger(L, p3);
  return iuplua_call(L, 4);
}

static int matrix_mark_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "mark_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int matrix_markedit_cb(Ihandle *self, int p0, int p1, int p2)
{
  lua_State *L = iuplua_call_start(self, "markedit_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  return iuplua_call(L, 3);
}

static int matrix_bgcolor_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "bgcolor_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static char * matrix_value_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "value_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call_rs(L, 2);
}

static int matrix_dropselect_cb(Ihandle *self, int p0, int p1, Ihandle * p2, char * p3, int p4, int p5)
{
  lua_State *L = iuplua_call_start(self, "dropselect_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  iuplua_pushihandle(L, p2);
  lua_pushstring(L, p3);
  lua_pushinteger(L, p4);
  lua_pushinteger(L, p5);
  return iuplua_call(L, 6);
}

static int matrix_drop_cb(Ihandle *self, Ihandle * p0, int p1, int p2)
{
  lua_State *L = iuplua_call_start(self, "drop_cb");
  iuplua_pushihandle(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  return iuplua_call(L, 3);
}

static int matrix_dropcheck_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "dropcheck_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int matrix_menudrop_cb(Ihandle *self, Ihandle * p0, int p1, int p2)
{
  lua_State *L = iuplua_call_start(self, "menudrop_cb");
  iuplua_pushihandle(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  return iuplua_call(L, 3);
}

static int matrix_action_cb(Ihandle *self, int p0, int p1, int p2, int p3, char * p4)
{
  lua_State *L = iuplua_call_start(self, "action_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushinteger(L, p2);
  lua_pushinteger(L, p3);
  lua_pushstring(L, p4);
  return iuplua_call(L, 5);
}

static int matrix_enteritem_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "enteritem_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int matrix_leaveitem_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "leaveitem_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int matrix_scrolltop_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "scrolltop_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int matrix_fgcolor_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "fgcolor_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int matrix_mousemove_cb(Ihandle *self, int p0, int p1)
{
  lua_State *L = iuplua_call_start(self, "mousemove_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  return iuplua_call(L, 2);
}

static int matrix_release_cb(Ihandle *self, int p0, int p1, char * p2)
{
  lua_State *L = iuplua_call_start(self, "release_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushstring(L, p2);
  return iuplua_call(L, 3);
}

static int matrix_value_edit_cb(Ihandle *self, int p0, int p1, char * p2)
{
  lua_State *L = iuplua_call_start(self, "value_edit_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushstring(L, p2);
  return iuplua_call(L, 3);
}

static int matrix_click_cb(Ihandle *self, int p0, int p1, char * p2)
{
  lua_State *L = iuplua_call_start(self, "click_cb");
  lua_pushinteger(L, p0);
  lua_pushinteger(L, p1);
  lua_pushstring(L, p2);
  return iuplua_call(L, 3);
}

static int Matrix(lua_State *L)
{
  Ihandle *ih = IupMatrix(NULL);
  iuplua_plugstate(L, ih);
  iuplua_pushihandle_raw(L, ih);
  return 1;
}

void iuplua_matrixfuncs_open(lua_State *L);

int iupmatrixlua_open(lua_State * L)
{
  iuplua_register(L, Matrix, "Matrix");

  iuplua_register_cb(L, "FONT_CB", (lua_CFunction)matrix_font_cb, NULL);
  iuplua_register_cb(L, "DRAW_CB", (lua_CFunction)matrix_draw_cb, NULL);
  iuplua_register_cb(L, "EDITION_CB", (lua_CFunction)matrix_edition_cb, NULL);
  iuplua_register_cb(L, "MARK_CB", (lua_CFunction)matrix_mark_cb, NULL);
  iuplua_register_cb(L, "MARKEDIT_CB", (lua_CFunction)matrix_markedit_cb, NULL);
  iuplua_register_cb(L, "BGCOLOR_CB", (lua_CFunction)matrix_bgcolor_cb, NULL);
  iuplua_register_cb(L, "VALUE_CB", (lua_CFunction)matrix_value_cb, NULL);
  iuplua_register_cb(L, "DROPSELECT_CB", (lua_CFunction)matrix_dropselect_cb, NULL);
  iuplua_register_cb(L, "DROP_CB", (lua_CFunction)matrix_drop_cb, NULL);
  iuplua_register_cb(L, "DROPCHECK_CB", (lua_CFunction)matrix_dropcheck_cb, NULL);
  iuplua_register_cb(L, "MENUDROP_CB", (lua_CFunction)matrix_menudrop_cb, NULL);
  iuplua_register_cb(L, "ACTION_CB", (lua_CFunction)matrix_action_cb, "matrix");
  iuplua_register_cb(L, "ENTERITEM_CB", (lua_CFunction)matrix_enteritem_cb, NULL);
  iuplua_register_cb(L, "LEAVEITEM_CB", (lua_CFunction)matrix_leaveitem_cb, NULL);
  iuplua_register_cb(L, "SCROLLTOP_CB", (lua_CFunction)matrix_scrolltop_cb, NULL);
  iuplua_register_cb(L, "FGCOLOR_CB", (lua_CFunction)matrix_fgcolor_cb, NULL);
  iuplua_register_cb(L, "MOUSEMOVE_CB", (lua_CFunction)matrix_mousemove_cb, "matrix");
  iuplua_register_cb(L, "RELEASE_CB", (lua_CFunction)matrix_release_cb, NULL);
  iuplua_register_cb(L, "VALUE_EDIT_CB", (lua_CFunction)matrix_value_edit_cb, NULL);
  iuplua_register_cb(L, "CLICK_CB", (lua_CFunction)matrix_click_cb, NULL);

  iuplua_matrixfuncs_open(L);

#ifdef IUPLUA_USELOH
#include "matrix.loh"
#else
#ifdef IUPLUA_USELH
#include "matrix.lh"
#else
  iuplua_dofile(L, "matrix.lua");
#endif
#endif

  return 0;
}

