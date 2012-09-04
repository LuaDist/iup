/** \file
* \brief IUP binding for Lua 5.
*
* See Copyright Notice in "iup.h"
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "iupkey.h"

#include <lua.h>
#include <lauxlib.h>

#include "iuplua.h"
#include "il.h"

#include "iup_attrib.h"
#include "iup_globalattrib.h"
#include "iup_object.h"
#include "iup_str.h"


static int SaveImageAsText(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  const char *file_name = luaL_checkstring(L, 2);
  const char *format = luaL_checkstring(L, 3);
  const char *name = luaL_optstring(L, 4, NULL);
  lua_pushboolean(L, IupSaveImageAsText(ih, file_name, format, name));
  return 1;
}

static int Reparent(lua_State *L)
{
  lua_pushinteger(L, IupReparent(iuplua_checkihandle(L,1),
                                 iuplua_checkihandle(L,2),
                                 iuplua_checkihandleornil(L,3)));
  return 1;
}

static int Insert(lua_State *L)
{
  iuplua_pushihandle(L, IupInsert(iuplua_checkihandle(L,1),
                                  iuplua_checkihandleornil(L,2),
                                  iuplua_checkihandle(L,3)));
  return 1;
}

static int Append(lua_State *L)
{
  iuplua_pushihandle(L, IupAppend(iuplua_checkihandle(L,1),
                                  iuplua_checkihandle(L,2)));
  return 1;
}

static int Destroy(lua_State *L)
{
  Ihandle* ih = iuplua_checkihandle(L,1);
  IupDestroy(ih);
  return 0;
}

static int Detach(lua_State *L)
{
  IupDetach(iuplua_checkihandle(L,1));
  return 0;
}

static int Flush(lua_State *L)
{
  (void)L; /* not used */
  IupFlush();
  return 0;
}

static int Version(lua_State *L)
{
  lua_pushstring(L, IupVersion());
  return 1;
}                                                                             

static int GetAttributeData (lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  const char *name = luaL_checkstring(L,2);
  const char *value = IupGetAttribute(ih, name);
  if (!value)
    lua_pushnil(L);
  else
    lua_pushlightuserdata(L, (void*)value);
  return 1;
}

static int GetAttribute (lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  const char *name = luaL_checkstring(L,2);
  const char *value = IupGetAttribute(ih, name);
  if (!value || iupATTRIB_ISINTERNAL(name))
    lua_pushnil(L);
  else
  {
    if (iupAttribIsPointer(ih, name))
    {
      if (ih->handle == (InativeHandle*)value) /* work around for WID, sometimes WID in Windows is not a valid pointer. Why? */
        iuplua_pushihandle(L, ih);
      else if (iupObjectCheck((Ihandle*)value))
        iuplua_pushihandle(L, (Ihandle*)value);
      else
        lua_pushlightuserdata(L, (void*)value);
    }
    else
      lua_pushstring(L,value);
  }
  return 1;
}

static int GetAttributeId(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  const char *name = luaL_checkstring(L,2);
  int id = luaL_checkint(L,3);
  const char *value = IupGetAttributeId(ih, name, id);
  if (!value || iupATTRIB_ISINTERNAL(name))
    lua_pushnil(L);
  else
  {
    if (iupAttribIsPointer(ih, name))
    {
      if (ih->handle == (InativeHandle*)value) /* work around for WID, sometimes WID in Windows is not a valid pointer. Why? */
        iuplua_pushihandle(L, ih);
      else if (iupObjectCheck((Ihandle*)value))
        iuplua_pushihandle(L, (Ihandle*)value);
      else
        lua_pushlightuserdata(L, (void*)value);
    }
    else
      lua_pushstring(L,value);
  }
  return 1;
}

static int GetAttributes(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  const char *value = IupGetAttributes(ih);
  lua_pushstring(L,value);
  return 1;
}

static int GetAllDialogs(lua_State *L)
{
  int n, i, max_n = luaL_optinteger(L, 1, 0);
  char **names;
  if (!max_n) max_n = IupGetAllDialogs(NULL, 0);
  names = (char **) malloc (max_n * sizeof(char *));
  n = IupGetAllDialogs(names, max_n);
  lua_createtable(L, n, 0);
  for (i=0; i<n; i++)
  {
    lua_pushinteger(L,i+1);
    lua_pushstring(L,names[i]);
    lua_settable(L,-3);
  }
  lua_pushinteger(L,n);
  free(names);
  return 2;
}

static int GetAllNames(lua_State *L)
{
  int n, i, max_n = luaL_optinteger(L, 1, 0);
  char **names;
  if (!max_n) max_n = IupGetAllNames(NULL, 0);
  names = (char **) malloc (max_n * sizeof(char *));
  n = IupGetAllNames(names,max_n);
  lua_createtable(L, n, 0);
  for (i=0; i<n; i++)
  {
    lua_pushinteger(L,i+1);
    lua_pushstring(L,names[i]);
    lua_settable(L,-3);
  }
  lua_pushinteger(L,n);
  free(names);
  return 2;
}

static int GetAllAttributes(lua_State *L)
{
  int n, i, max_n = luaL_optinteger(L, 1, 0);
  char **names;
  if (!max_n) max_n = IupGetAllAttributes(iuplua_checkihandle(L,1), NULL, 0);
  names = (char **) malloc (max_n * sizeof(char *));
  n = IupGetAllAttributes(iuplua_checkihandle(L,1),names,max_n);
  lua_createtable(L, n, 0);
  for (i=0; i<n; i++)
  {
    lua_pushinteger(L,i+1);
    lua_pushstring(L,names[i]);
    lua_settable(L,-3);
  }
  lua_pushinteger(L,n);
  free(names);
  return 2;
}

static int GetAllClasses(lua_State *L)
{
  int n, i, max_n = luaL_optinteger(L, 1, 0);
  char **names;
  if (!max_n) max_n = IupGetAllClasses(NULL, 0);
  names = (char **) malloc (max_n * sizeof(char *));
  n = IupGetAllClasses(names,max_n);

  if (n == -1)
  {
    lua_pushnil(L);
    return 1;
  }

  lua_createtable(L, n, 0);
  for (i=0; i<n; i++)
  {
    lua_pushinteger(L,i+1);
    lua_pushstring(L,names[i]);
    lua_settable(L,-3);
  }
  lua_pushinteger(L,n);
  free(names);
  return 2;
}

static int GetClassAttributes(lua_State *L)
{
  int n, i, max_n = luaL_optinteger(L, 1, 0);
  char **names;
  if (!max_n) max_n = IupGetClassAttributes(luaL_checkstring(L,1), NULL, 0);
  names = (char **) malloc (max_n * sizeof(char *));
  n = IupGetClassAttributes(luaL_checkstring(L,1),names,max_n);

  if (n == -1)
  {
    lua_pushnil(L);
    return 1;
  }

  lua_createtable(L, n, 0);
  for (i=0; i<n; i++)
  {
    lua_pushinteger(L,i+1);
    lua_pushstring(L,names[i]);
    lua_settable(L,-3);
  }
  lua_pushinteger(L,n);
  free(names);
  return 2;
}

static int GetClassCallbacks(lua_State *L)
{
  int n, i, max_n = luaL_optinteger(L, 1, 0);
  char **names;
  if (!max_n) max_n = IupGetClassCallbacks(luaL_checkstring(L,1), NULL, 0);
  names = (char **) malloc (max_n * sizeof(char *));
  n = IupGetClassCallbacks(luaL_checkstring(L,1),names,max_n);

  if (n == -1)
  {
    lua_pushnil(L);
    return 1;
  }

  lua_createtable(L, n, 0);
  for (i=0; i<n; i++)
  {
    lua_pushinteger(L,i+1);
    lua_pushstring(L,names[i]);
    lua_settable(L,-3);
  }
  lua_pushinteger(L,n);
  free(names);
  return 2;
}

static int SetClassDefaultAttribute(lua_State *L)
{
  IupSetClassDefaultAttribute(luaL_checkstring(L,1), luaL_checkstring(L,2), luaL_optstring(L,3,NULL));
  return 0;
}

static int SaveClassAttributes(lua_State *L)
{
  IupSaveClassAttributes(iuplua_checkihandle(L,1));
  return 0;
}

static int CopyClassAttributes(lua_State *L)
{
  IupCopyClassAttributes(iuplua_checkihandle(L,1), iuplua_checkihandle(L,2));
  return 0;
}

static int GetDialog(lua_State *L)
{
  iuplua_pushihandle(L, IupGetDialog(iuplua_checkihandle(L,1)));
  return 1;
}

static int GetFile (lua_State *L)
{
  const char *fname = luaL_checkstring(L,1);
  char returned_fname[4096];
  int ret;
  iupStrCopyN(returned_fname, 4096, fname);
  ret = IupGetFile(returned_fname);
  lua_pushstring(L, returned_fname);
  lua_pushinteger(L, ret);
  return 2;
}

static int GetFocus(lua_State *L)
{
  iuplua_pushihandle(L, IupGetFocus());
  return 1;
}

static int GetClassName(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  lua_pushstring(L, IupGetClassName(ih));
  return 1;
}

static int GetClassType(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  lua_pushstring(L, IupGetClassType(ih));
  return 1;
}

static int ClassMatch(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L, 1);
  lua_pushboolean(L, IupClassMatch(ih, luaL_checkstring(L, 2)));
  return 1;
}

static int GetGlobal(lua_State *L)
{
  const char *a = luaL_checkstring(L,1);
  const char *v = IupGetGlobal(a);
  if (iupGlobalIsPointer(a))
    lua_pushlightuserdata(L, (void*)v);
  else
    lua_pushstring(L,v);
  return 1;
}

static int GetHandle(lua_State *L)
{
  const char *name = luaL_checkstring(L,1);
  Ihandle *ih = IupGetHandle(name);
  iuplua_pushihandle(L,ih);
  return 1;
}

static int GetLanguage (lua_State *L)
{
  char * value = IupGetLanguage();
  lua_pushstring(L,value);
  return 1;
}

static int GetName(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  char * name = IupGetName(ih);
  lua_pushstring(L,name);
  return 1;
}

static int Help(lua_State *L)
{
  const char *url = luaL_checkstring(L,1);
  IupHelp(url);
  return 0;
}

static int Hide(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  lua_pushinteger(L, IupHide(ih));
  return 1;
}

static int PlayInput(lua_State *L)
{
  lua_pushinteger(L, IupPlayInput(luaL_optstring(L,1,NULL)));
  return 1;
}

static int RecordInput(lua_State *L)
{
  lua_pushinteger(L, IupRecordInput(luaL_optstring(L,1,NULL), luaL_optinteger(L, 2, 0)));
  return 1;
}

static int Load(lua_State *L)
{
  const char *s = luaL_checkstring(L,1);
  const char *r = IupLoad(s);
  lua_pushstring(L,r);
  return 1;
}

static int LoadBuffer(lua_State *L)
{
  const char *s = luaL_checkstring(L,1);
  const char *r = IupLoadBuffer(s);
  lua_pushstring(L,r);
  return 1;
}

static int LoopStep(lua_State *L)
{
  lua_pushinteger(L,IupLoopStep());
  return 1;
}

static int LoopStepWait(lua_State *L)
{
  lua_pushinteger(L,IupLoopStepWait());
  return 1;
}

static int ExitLoop(lua_State *L)
{
  (void)L;
  IupExitLoop();
  return 0;
}

static int MainLoop(lua_State *L)
{
  lua_pushinteger(L,IupMainLoop());
  return 1;
}

static int MainLoopLevel(lua_State *L)
{
  lua_pushinteger(L,IupMainLoopLevel());
  return 1;
}

static int Map(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  lua_pushinteger(L, IupMap(ih));
  return 1;
}

static int Unmap(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  IupUnmap(ih);
  return 0;
}

static int MapFont(lua_State *L)
{
  const char *font = luaL_checkstring(L,1);
  const char *nfont = IupMapFont(font);
  lua_pushstring(L, nfont);
  return 1;
}

static int Message(lua_State *L)
{
  const char *title = luaL_checkstring(L,1);
  const char *message = luaL_checkstring(L,2);
  IupMessage(title, message);
  return 0;
}

static int Alarm(lua_State *L)
{
  int n = IupAlarm(luaL_checkstring(L, 1), 
                   luaL_checkstring(L, 2), 
                   luaL_checkstring(L, 3), 
                   luaL_optstring(L, 4, NULL), 
                   luaL_optstring(L, 5, NULL));
  lua_pushinteger(L, n);
  return 1;
}

static int ListDialog(lua_State *L)
{
  int type = luaL_checkint(L,1);
  int size = luaL_checkint(L,3);
  char** list = iuplua_checkstring_array(L, 4, size);
  int* marks = lua_isnoneornil(L, 8)? NULL: iuplua_checkint_array(L,8,0);
  int i, ret;

  if (!marks && type==2)
    luaL_error(L, "invalid marks, must not be nil.");
  if (marks && type==2 && size != iuplua_getn(L, 8))
    luaL_error(L, "invalid number of elements in the marks.");

  ret = IupListDialog(type, luaL_checkstring(L, 2), 
                            size, 
                            list, 
                            luaL_checkint(L, 5), 
                            luaL_checkint(L, 6), 
                            luaL_checkint(L, 7), 
                            marks);

  if (marks && type==2 && ret!=-1)
  {
    for (i=0; i<size; i++)
    {
      lua_pushinteger(L, i+1);
      lua_pushinteger(L, marks[i]);
      lua_settable(L, 8);
    }
  }

  lua_pushinteger(L, ret);
    
  if (marks) free(marks);
  free(list);

  return 1;
}

static int GetText(lua_State *L)
{
  char buffer[10240];
  const char *title = luaL_checkstring(L,1);
  const char *text = luaL_checkstring(L,2);
  iupStrCopyN(buffer, 10240, text);
  if (IupGetText(title, buffer))
  {
    lua_pushstring(L, buffer);
    return 1;
  }
  return 0;
}

static int NextField(lua_State *L)
{
  Ihandle *h1 = iuplua_checkihandle(L,1);
  Ihandle *h2 = IupNextField(h1);
  iuplua_pushihandle(L,h2);
  return 1;
}

static int PreviousField(lua_State *L)
{
  Ihandle *h1 = iuplua_checkihandle(L,1);
  Ihandle *h2 = IupNextField(h1);
  iuplua_pushihandle(L,h2);
  return 1;
}

static int Popup(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  int x = luaL_optint(L,2, IUP_CURRENT);
  int y = luaL_optint(L,3, IUP_CURRENT);
  lua_pushinteger(L,IupPopup(ih,x,y));
  return 1;
}

static int cf_isprint(lua_State *L)
{
  int value = luaL_checkint(L, 1);
  lua_pushinteger(L, iup_isprint(value));
  return 1;
}

static int cf_xCODE(lua_State *L)
{
  int value = luaL_checkint(L, 1);
  lua_pushinteger(L, IUPxCODE(value));
  return 1;
}

static int cf_isxkey(lua_State *L)
{
  int value = luaL_checkint(L, 1);
  lua_pushboolean(L, iup_isXkey(value));
  return 1;
}

static int cf_isShiftXkey(lua_State *L)
{
  int value = luaL_checkint(L, 1);
  lua_pushboolean(L, iup_isShiftXkey(value));
  return 1;
}

static int cf_isCtrlXkey(lua_State *L)
{
  int value = luaL_checkint(L, 1);
  lua_pushboolean(L, iup_isCtrlXkey(value));
  return 1;
}

static int cf_isAltXkey(lua_State *L)
{
  int value = luaL_checkint(L, 1);
  lua_pushboolean(L, iup_isAltXkey(value));
  return 1;
}

static int cf_isSysXkey(lua_State *L)
{
  int value = luaL_checkint(L, 1);
  lua_pushboolean(L, iup_isSysXkey(value));
  return 1;
}

static int cf_isbutton1(lua_State *L)
{
  const char *value = luaL_checkstring(L, 1);
  lua_pushboolean(L, iup_isbutton1(value));
  return 1;
}

static int cf_isbutton2(lua_State *L)
{
  const char *value = luaL_checkstring(L, 1);
  lua_pushboolean(L, iup_isbutton2(value));
  return 1;
}

static int cf_isbutton3(lua_State *L)
{
  const char *value = luaL_checkstring(L, 1);
  lua_pushboolean(L, iup_isbutton3(value));
  return 1;
}

static int cf_isshift(lua_State *L)
{
  const char *value = luaL_checkstring(L, 1);
  lua_pushboolean(L, iup_isshift(value));
  return 1;
}

static int cf_iscontrol(lua_State *L)
{
  const char *value = luaL_checkstring(L, 1);
  lua_pushboolean(L, iup_iscontrol(value));
  return 1;
}

static int cf_isdouble(lua_State *L)
{
  const char *value = luaL_checkstring(L, 1);
  lua_pushboolean(L, iup_isdouble(value));
  return 1;
}

static int cf_isalt(lua_State *L)
{
  const char *value = luaL_checkstring(L, 1);
  lua_pushboolean(L, iup_isalt(value));
  return 1;
}

static int cf_issys(lua_State *L)
{
  const char *value = luaL_checkstring(L, 1);
  lua_pushboolean(L, iup_issys(value));
  return 1;
}

static int cf_isbutton4(lua_State *L)
{
  const char *value = luaL_checkstring(L, 1);
  lua_pushboolean(L, iup_isbutton4(value));
  return 1;
}

static int cf_isbutton5(lua_State *L)
{
  const char *value = luaL_checkstring(L, 1);
  lua_pushboolean(L, iup_isbutton5(value));
  return 1;
}

static int GetParent(lua_State *L)
{
  Ihandle * ih = iuplua_checkihandle(L,1);
  Ihandle * parent = IupGetParent(ih);
  iuplua_pushihandle(L, parent);
  return 1;
}

static int VersionNumber(lua_State *L)
{
  lua_pushinteger(L, IupVersionNumber());
  return 1;
}

static int GetNextChild(lua_State *L)
{
  Ihandle* parent = iuplua_checkihandle(L,1);
  Ihandle* next = iuplua_checkihandleornil(L,2);
  Ihandle* nextchild = IupGetNextChild(parent, next);
  iuplua_pushihandle(L, nextchild);
  return 1;
}

static int GetChild(lua_State *L)
{
  Ihandle* ih = iuplua_checkihandle(L,1);
  int pos = luaL_checkint(L,2);
  Ihandle* child = IupGetChild(ih, pos);
  iuplua_pushihandle(L, child);
  return 1;
}

static int GetChildPos(lua_State *L)
{
  Ihandle* ih = iuplua_checkihandle(L,1);
  Ihandle* child = iuplua_checkihandle(L,2);
  lua_pushinteger(L, IupGetChildPos(ih, child));
  return 1;
}

static int GetBrother(lua_State *L)
{
  Ihandle * ih = iuplua_checkihandle(L,1);
  Ihandle * brother = IupGetBrother(ih);
  iuplua_pushihandle(L, brother);
  return 1;
}

static int GetDialogChild(lua_State *L)
{
  Ihandle * ih = iuplua_checkihandle(L,1);
  const char* name = luaL_checkstring(L,2);
  Ihandle * child = IupGetDialogChild(ih, name);
  iuplua_pushihandle(L, child);
  return 1;
}

static int LayoutDialog(lua_State *L)
{
  iuplua_pushihandle(L,IupLayoutDialog(iuplua_checkihandleornil(L,1)));
  return 1;
}

static int ElementPropertiesDialog(lua_State *L)
{
  iuplua_pushihandle(L,IupElementPropertiesDialog(iuplua_checkihandle(L,1)));
  return 1;
}


static int ConvertXYToPos(lua_State *L)
{
  lua_pushinteger(L, IupConvertXYToPos(iuplua_checkihandle(L,1), luaL_checkint(L, 2), luaL_checkint(L, 3)));
  return 1;
}

static int TextConvertLinColToPos(lua_State *L)
{
  int pos;
  IupTextConvertLinColToPos(iuplua_checkihandle(L,1), luaL_checkint(L, 2), luaL_checkint(L, 3), &pos);
  lua_pushinteger(L, pos);
  return 1;
}

static int TextConvertPosToLinCol(lua_State *L)
{
  int lin, col;
  IupTextConvertPosToLinCol(iuplua_checkihandle(L,1), luaL_checkint(L, 2), &lin, &col);
  lua_pushinteger(L, lin);
  lua_pushinteger(L, col);
  return 2;
}

static int SetAttributes(lua_State *L)
{
  Ihandle * ih = iuplua_checkihandle(L,1);
  const char *attributes = luaL_checkstring(L,2);
  IupSetAttributes(ih, attributes);
  iuplua_pushihandle(L,ih);
  return 1;
}

static int SetFocus(lua_State *L)
{
  Ihandle *h1 = iuplua_checkihandle(L,1);
  Ihandle *h2 = IupSetFocus(h1);
  iuplua_pushihandle(L,h2);
  return 1;
}

static int SetGlobal(lua_State *L)
{
  const char *a = luaL_checkstring(L,1);
  const char *v = luaL_checkstring(L,2);
  IupSetGlobal(a,v);
  return 0;
}

static int SetHandle(lua_State *L)
{
  const char *name = luaL_checkstring(L,1);
  Ihandle *ih = iuplua_checkihandle(L,2);
  Ihandle *last = IupSetHandle(name, ih);
  iuplua_pushihandle(L, last);
  return 1;
}

static int SetLanguage(lua_State *L)
{
  IupSetLanguage(luaL_checkstring(L,1));
  return 0;
}

static int GetChildCount (lua_State *L)
{
  lua_pushinteger(L, IupGetChildCount(iuplua_checkihandle(L,1)));
  return 1;
}

static int Show (lua_State *L)
{
  lua_pushinteger(L, IupShow(iuplua_checkihandle(L,1)));
  return 1;
}

static int Refresh (lua_State *L)
{
  IupRefresh(iuplua_checkihandle(L,1));
  return 0;
}

static int RefreshChildren (lua_State *L)
{
  IupRefreshChildren(iuplua_checkihandle(L,1));
  return 0;
}

static int Update (lua_State *L)
{
  IupUpdate(iuplua_checkihandle(L,1));
  return 0;
}

static int UpdateChildren (lua_State *L)
{
  IupUpdateChildren(iuplua_checkihandle(L,1));
  return 0;
}

static int Redraw(lua_State *L)
{
  IupRedraw(iuplua_checkihandle(L,1), luaL_checkint(L, 2));
  return 0;
}

static int ShowXY(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  int x = luaL_optint(L,2, IUP_CURRENT);
  int y = luaL_optint(L,3, IUP_CURRENT);
  lua_pushinteger(L, IupShowXY(ih,x,y));
  return 1;
}

static int ResetAttribute(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  const char *a = luaL_checkstring(L,2);
  IupResetAttribute(ih, a);
  return 0;
}

static int StoreAttribute(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  const char *a = luaL_checkstring(L,2);

  if (lua_isnil(L,3)) 
    IupSetAttribute(ih,a,NULL);
  else 
  {
    const char *v;
    if(lua_isuserdata(L,3)) 
    {
      v = lua_touserdata(L,3);
      IupSetAttribute(ih,a,v);
    }
    else 
    {
      v = luaL_checkstring(L,3);
      IupStoreAttribute(ih,a,v);
    }
  }
  return 0;
}

static int StoreAttributeId(lua_State *L)
{
  Ihandle *ih = iuplua_checkihandle(L,1);
  const char *a = luaL_checkstring(L,2);
  int id = luaL_checkint(L,3);

  if (lua_isnil(L,4)) 
    IupSetAttributeId(ih,a,id,NULL);
  else 
  {
    const char *v;
    if(lua_isuserdata(L,4)) 
    {
      v = lua_touserdata(L,4);
      IupSetAttributeId(ih,a,id,v);
    }
    else 
    {
      v = luaL_checkstring(L,4);
      IupStoreAttributeId(ih,a,id,v);
    }
  }
  return 0;
}

static int StoreGlobal(lua_State *L)
{
  const char *a = luaL_checkstring(L,1);
  const char *v = luaL_checkstring(L,2);
  IupStoreGlobal(a,v);
  return 0;
}

static int UnMapFont (lua_State *L)
{
  const char *s = luaL_checkstring(L,1);
  const char *n = IupUnMapFont(s);
  lua_pushstring(L,n);
  return 1;
}


/*****************************************************************************
* iupluaapi_open                                                               *
****************************************************************************/


void iupluaapi_open(lua_State * L)
{
  struct luaL_Reg funcs[] = {
    {"Append", Append},
    {"Insert", Insert},
    {"Reparent", Reparent},
    {"Destroy", Destroy},
    {"Detach", Detach},
    {"Flush", Flush},
    {"Version", Version},
    {"GetAttribute", GetAttribute},
    {"GetAttributeData", GetAttributeData},
    {"GetAttributes", GetAttributes},
    {"GetAllAttributes", GetAllAttributes},
    {"SetClassDefaultAttribute", SetClassDefaultAttribute},
    {"SaveClassAttributes", SaveClassAttributes},
    {"CopyClassAttributes", CopyClassAttributes},
    {"GetAllClasses", GetAllClasses},
    {"GetClassAttributes", GetClassAttributes},
    {"GetClassCallbacks", GetClassCallbacks},
    {"GetAllDialogs", GetAllDialogs},
    {"GetAllNames", GetAllNames},
    {"GetDialog", GetDialog},
    {"GetFile", GetFile},
    {"GetFocus", GetFocus},
    {"GetClassName", GetClassName},
    {"GetClassType", GetClassType},
    {"ClassMatch", ClassMatch},
    {"GetGlobal", GetGlobal},
    {"GetHandle", GetHandle},
    {"GetLanguage", GetLanguage},
    {"GetName", GetName},
    {"Help", Help},
    {"Hide", Hide},
    {"Load", Load},
    {"LoadBuffer", LoadBuffer},
    {"PlayInput", PlayInput},
    {"RecordInput", RecordInput},
    {"LoopStep", LoopStep},
    {"LoopStepWait", LoopStepWait},
    {"ExitLoop", ExitLoop},
    {"MainLoop", MainLoop},
    {"MainLoopLevel", MainLoopLevel},
    {"Map", Map},
    {"Unmap", Unmap},
    {"MapFont", MapFont},
    {"Message", Message},
    {"Alarm", Alarm},  
    {"ListDialog", ListDialog},
    {"GetText", GetText},
    {"NextField", NextField},
    {"Popup", Popup},
    {"PreviousField", PreviousField},
    {"ResetAttribute", ResetAttribute},
    {"SetAttribute", StoreAttribute},
    {"SetAttributes", SetAttributes},
    {"isbutton1", cf_isbutton1},
    {"isbutton2", cf_isbutton2},
    {"isbutton3", cf_isbutton3},
    {"isshift", cf_isshift},
    {"iscontrol", cf_iscontrol},
    {"isdouble", cf_isdouble},
    {"isalt", cf_isalt},
    {"issys", cf_issys},
    {"isbutton4", cf_isbutton4},
    {"isbutton5", cf_isbutton5},
    {"GetParent", GetParent},
    {"GetNextChild", GetNextChild},
    {"GetChild", GetChild},
    {"GetChildPos", GetChildPos},
    {"VersionNumber", VersionNumber},
    {"GetBrother", GetBrother},
    {"GetDialogChild", GetDialogChild},
    {"LayoutDialog", LayoutDialog},
    {"ElementPropertiesDialog", ElementPropertiesDialog},
    {"SetFocus", SetFocus},
    {"SetGlobal", SetGlobal},
    {"SetHandle", SetHandle},
    {"SetLanguage", SetLanguage},
    {"Show", Show},
    {"GetChildCount", GetChildCount},
    {"Refresh", Refresh},
    {"RefreshChildren", RefreshChildren},
    {"Update", Update},
    {"UpdateChildren", UpdateChildren},
    {"SaveImageAsText", SaveImageAsText},
    {"Redraw", Redraw},
    {"ShowXY", ShowXY},
    {"StoreAttribute", StoreAttribute},
    {"StoreGlobal", StoreGlobal},
    {"UnMapFont", UnMapFont},
    {"Scanf", iupluaScanf},
    {"isprint", cf_isprint},
    {"IUPxCODE", cf_xCODE},
    {"isxkey", cf_isxkey},
    {"isXkey", cf_isxkey},
    {"isShiftXkey", cf_isShiftXkey},
    {"isCtrlXkey", cf_isCtrlXkey},
    {"isAltXkey", cf_isAltXkey},
    {"isSysXkey", cf_isSysXkey},
    {"TextConvertLinColToPos", TextConvertLinColToPos},
    {"TextConvertPosToLinCol", TextConvertPosToLinCol},
    {"ConvertXYToPos", ConvertXYToPos},
    {"StoreAttributeId", StoreAttributeId},
    {"GetAttributeId", GetAttributeId},
    {"SetAttributeId", StoreAttributeId},
    {NULL, NULL},
  };

  /* "iup" table is at the top of the stack */
  luaL_register(L, NULL, funcs);
}


Ihandle*  IupGetChild     (Ihandle* ih, int pos);
