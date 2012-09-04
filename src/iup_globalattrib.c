/** \file
 * \brief global attributes enviroment
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>      
#include <stdio.h>      
#include <string.h>      

#include "iup.h" 

#include "iup_table.h"
#include "iup_globalattrib.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_drvinfo.h"
#include "iup_assert.h"
#include "iup_str.h"
#include "iup_strmessage.h"


static Itable *iglobal_table = NULL;

void iupGlobalAttribInit(void)
{
  iglobal_table = iupTableCreate(IUPTABLE_STRINGINDEXED);
}

void iupGlobalAttribFinish(void)
{
  iupTableDestroy(iglobal_table);
  iglobal_table = NULL;
}

static int iGlobalChangingDefaultColor(const char *name)
{
  if (iupStrEqual(name, "DLGBGCOLOR") ||
      iupStrEqual(name, "DLGFGCOLOR") ||
      iupStrEqual(name, "MENUBGCOLOR") ||
      iupStrEqual(name, "MENUFGCOLOR") ||
      iupStrEqual(name, "TXTBGCOLOR") ||
      iupStrEqual(name, "TXTFGCOLOR"))
  {
    char str[50] = "_IUP_USER_DEFAULT_";
    strcat(str, name);
    iupTableSet(iglobal_table, str, (void*)"1", IUPTABLE_POINTER);  /* mark as changed by the User */
    return 1;
  }
  return 0;
}

int iupGlobalDefaultColorChanged(const char *name)
{
  char str[50] = "_IUP_USER_DEFAULT_";
  strcat(str, name);
  return iupTableGet(iglobal_table, str) != NULL;
}

void iupGlobalSetDefaultColorAttrib(const char* name, int r, int g, int b)
{
  if (!iupGlobalDefaultColorChanged(name))
  {
    char value[50];
    sprintf(value, "%3d %3d %3d", r, g, b);
    iupTableSet(iglobal_table, name, (void*)value, IUPTABLE_STRING);
  }
}

static void iGlobalTableSet(const char *name, const char *value, int store)
{
  if (!value)
    iupTableRemove(iglobal_table, name);
  else if (store)
    iupTableSet(iglobal_table, name, (void*)value, IUPTABLE_STRING);
  else
    iupTableSet(iglobal_table, name, (void*)value, IUPTABLE_POINTER);
}

static void iGlobalSet(const char *name, const char *value, int store)
{
  iupASSERT(name!=NULL);
  if (!name) return;

  if (iupStrEqual(name, "DEFAULTFONTSIZE"))
  {
    iupSetDefaultFontSizeGlobalAttrib(value);
    return;
  }
  if (iupStrEqual(name, "KEYPRESS"))
  {
    int key;
    if (iupStrToInt(value, &key))
      iupdrvSendKey(key, 0x01);
    return;
  }
  if (iupStrEqual(name, "KEYRELEASE"))
  {
    int key;
    if (iupStrToInt(value, &key))
      iupdrvSendKey(key, 0x02);
    return;
  }
  if (iupStrEqual(name, "KEY"))
  {
    int key;
    if (iupStrToInt(value, &key))
      iupdrvSendKey(key, 0x03);
    return;
  }
  if (iupStrEqual(name, "LANGUAGE"))
  {
    iupStrMessageUpdateLanguage(value);
    iGlobalTableSet(name, value, store);
    return;
  }
  if (iupStrEqual(name, "CURSORPOS"))
  {
    int x, y;
    if (iupStrToIntInt(value, &x, &y, 'x') == 2)
      iupdrvWarpPointer(x, y);
    return;
  }
  if (iupStrEqual(name, "MOUSEBUTTON"))
  {
    int x, y, status;
    char bt; 
    if (sscanf(value, "%dx%d %c %d", &x, &y, &bt, &status) == 4)
      iupdrvSendMouse(x, y, bt, status);
    return;
  }

  if (iGlobalChangingDefaultColor(name) || 
      iupdrvSetGlobal(name, value))
    iGlobalTableSet(name, value, store);
}

void IupSetGlobal(const char *name, const char *value)
{
  iGlobalSet(name, value, 0);
}

void IupStoreGlobal(const char *name, const char *value)
{
  iGlobalSet(name, value, 1);
}

char *IupGetGlobal(const char *name)
{
  char* value;
  
  iupASSERT(name!=NULL);
  if (!name) 
    return NULL;

  if (iupStrEqual(name, "DEFAULTFONTSIZE"))
    return iupGetDefaultFontSizeGlobalAttrib();
  if (iupStrEqual(name, "CURSORPOS"))
  {
    char *str = iupStrGetMemory(50);
    int x, y;
    iupdrvGetCursorPos(&x, &y);
    sprintf(str, "%dx%d", (int)x, (int)y);
    return str;
  }
  if (iupStrEqual(name, "SHIFTKEY"))
  {
    char key[5];
    iupdrvGetKeyState(key);
    if (key[0] == 'S')
      return "ON";
    else
      return "OFF";
  }
  if (iupStrEqual(name, "CONTROLKEY"))
  {
    char key[5];
    iupdrvGetKeyState(key);
    if (key[1] == 'C')
      return "ON";
    else
      return "OFF";
  }
  if (iupStrEqual(name, "MODKEYSTATE"))
  {
    char *str = iupStrGetMemory(5);
    iupdrvGetKeyState(str);
    return str;
  }
  if (iupStrEqual(name, "SCREENSIZE"))
  {
    char *str = iupStrGetMemory(50);
    int w, h;
    iupdrvGetScreenSize(&w, &h);
    sprintf(str, "%dx%d", w, h);
    return str;
  }
  if (iupStrEqual(name, "FULLSIZE"))
  {
    char *str = iupStrGetMemory(50);
    int w, h;
    iupdrvGetFullSize(&w, &h);
    sprintf(str, "%dx%d", w, h);
    return str;
  }
  if (iupStrEqual(name, "SCREENDEPTH"))
  {
    char *str = iupStrGetMemory(50);
    int bpp = iupdrvGetScreenDepth();
    sprintf(str, "%d", bpp);
    return str;
  }
  if (iupStrEqual(name, "SCREENDPI"))
  {
    char *str = iupStrGetMemory(50);
    float dpi = iupdrvGetScreenDpi();
    sprintf(str, "%g", dpi);
    return str;
  }
  if (iupStrEqual(name, "SYSTEMLOCALE"))
    return iupdrvLocaleInfo();

  value = iupdrvGetGlobal(name);

  if (!value)
    value = (char*)iupTableGet(iglobal_table, name);

  return value;
}

int iupGlobalIsPointer(const char* name)
{
  static struct {
    const char *name;
  } ptr_table[] = {
#ifndef GTK_MAC
  #ifdef WIN32
    {"HINSTANCE"},
  #else
    {"XDISPLAY"},
    {"XSCREEN"},
    {"APPSHELL"},
  #endif
#endif
  };
#define PTR_TABLE_SIZE ((sizeof ptr_table)/(sizeof ptr_table[0]))

  if (name)
  {
    int i;
    for (i = 0; i < PTR_TABLE_SIZE; i++)
    {
      if (iupStrEqualNoCase(name, ptr_table[i].name))
        return 1;
    }
  }

  return 0;
}
