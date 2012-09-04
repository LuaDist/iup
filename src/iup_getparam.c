/** \file
 * \brief IupGetParam
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "iup.h"

#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_strmessage.h"
#include "iup_drvfont.h"


#define RAD2DEG  57.296f   /* radians to degrees */

enum {IPARAM_TYPE_STR, IPARAM_TYPE_INT, IPARAM_TYPE_FLOAT, IPARAM_TYPE_NONE=-1};


/*******************************************************************************************
                    Internal Callbacks
*******************************************************************************************/

static int iParamButtonOK_CB(Ihandle* self)
{
  Ihandle* dlg = IupGetDialog(self);
  Iparamcb cb = (Iparamcb)IupGetCallback(dlg, "PARAM_CB");
  iupAttribSetStr(dlg, "STATUS", "1");
  if (cb && !cb(dlg, IUP_GETPARAM_OK, (void*)iupAttribGet(dlg, "USER_DATA")))
    return IUP_DEFAULT;
  else
    return IUP_CLOSE;
}

static int iParamButtonCancel_CB(Ihandle* self)
{
  Ihandle* dlg = IupGetDialog(self);
  Iparamcb cb = (Iparamcb)IupGetCallback(dlg, "PARAM_CB");
  iupAttribSetStr(dlg, "STATUS", "0");
  if (cb) cb(dlg, IUP_GETPARAM_CANCEL, (void*)iupAttribGet(dlg, "USER_DATA"));
  return IUP_CLOSE;
}

static int iParamButtonHelp_CB(Ihandle* self)
{
  Ihandle* dlg = IupGetDialog(self);
  Iparamcb cb = (Iparamcb)IupGetCallback(dlg, "PARAM_CB");
  if (cb) cb(dlg, IUP_GETPARAM_HELP, (void*)iupAttribGet(dlg, "USER_DATA"));
  return IUP_DEFAULT;
}

static int iParamToggleAction_CB(Ihandle *self, int v)
{
  Ihandle* param = (Ihandle*)iupAttribGetInherit(self, "_IUPGP_PARAM");
  Ihandle* dlg = IupGetDialog(self);
  Iparamcb cb = (Iparamcb)IupGetCallback(dlg, "PARAM_CB");
  int old_v = iupAttribGetInt(param, "VALUE");

  if (v == 1)
    iupAttribSetStr(param, "VALUE", "1");
  else
    iupAttribSetStr(param, "VALUE", "0");

  if (cb && !cb(dlg, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(dlg, "USER_DATA"))) 
  {
    /* Undo */
    if (old_v == 1)
    {
      iupAttribSetStr(param, "VALUE", "1");
      IupSetAttribute(self, "VALUE", "1");
    }
    else
    {
      iupAttribSetStr(param, "VALUE", "0");
      IupSetAttribute(self, "VALUE", "0");
    }

    /* there is no IUP_IGNORE for IupToggle */
    return IUP_DEFAULT;
  }

  /* update the interface */
  if (v == 1)
    IupStoreAttribute(self, "TITLE", iupAttribGet(param, "_IUPGP_TRUE"));
  else
    IupStoreAttribute(self, "TITLE", iupAttribGet(param, "_IUPGP_FALSE"));

  return IUP_DEFAULT;
}

static int iParamTextAction_CB(Ihandle *self, int c, char *after)
{
  Ihandle* param = (Ihandle*)iupAttribGetInherit(self, "_IUPGP_PARAM");
  Ihandle* dlg = IupGetDialog(self);
  Iparamcb cb = (Iparamcb)IupGetCallback(dlg, "PARAM_CB");
  Ihandle* aux = (Ihandle*)iupAttribGet(param, "AUXCONTROL");
  (void)c;
 
  iupAttribStoreStr(param, "VALUE", after);

  if (cb && !cb(dlg, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(dlg, "USER_DATA"))) 
  {
    /* Undo */
    iupAttribStoreStr(param, "VALUE", IupGetAttribute(self, "VALUE"));
    return IUP_IGNORE;
  }

  if (aux)
  {
    if (iupStrEqual(iupAttribGet(param, "TYPE"), "COLOR"))
      IupStoreAttribute(aux, "BGCOLOR", after);
    else
      IupStoreAttribute(aux, "VALUE", after);
  }

  if (IupGetInt(self, "SPIN"))
  {
    if (iupAttribGet(self, "_IUPGP_SPINREAL"))
    {
      float min = iupAttribGetFloat(param, "MIN");
      float step = iupAttribGetFloat(self, "_IUPGP_INCSTEP");
      float val;
      if (iupStrToFloat(after, &val))
        IupSetfAttribute(self, "SPINVALUE", "%d", (int)((val-min)/step + 0.5));
    }
    else
    {
      int val;
      if (iupStrToInt(after, &val))
        IupSetfAttribute(self, "SPINVALUE", "%d", val);
    }
  }

  return IUP_DEFAULT;
}

static int iParamValAction_CB(Ihandle *self)
{
  Ihandle* param = (Ihandle*)iupAttribGetInherit(self, "_IUPGP_PARAM");
  Ihandle* text = (Ihandle*)iupAttribGetInherit(self, "_IUPGP_TEXT");
  Ihandle* dlg = IupGetDialog(self);
  Iparamcb cb = (Iparamcb)IupGetCallback(dlg, "PARAM_CB");
  float old_value = iupAttribGetFloat(param, "VALUE");
  float val = IupGetFloat(self, "VALUE");

  char* type = iupAttribGet(param, "TYPE");
  if (iupStrEqual(type, "INTEGER"))
  {
    iupAttribSetInt(param, "VALUE", (int)val);
  }
  else
  {
    if (iupAttribGetInt(param, "ANGLE"))
    {
      float old_angle;

      if (val == 0)
      {
        old_angle = iupAttribGetFloat(param, "VALUE");
        iupAttribSetFloat(param, "_IUPGP_OLD_ANGLE", old_angle);
      }
      else
        old_angle = iupAttribGetFloat(param, "_IUPGP_OLD_ANGLE");

      val = old_angle + val*RAD2DEG;

      if (iupAttribGetInt(param, "INTERVAL"))
      {
        float min = iupAttribGetFloat(param, "MIN");
        float max = iupAttribGetFloat(param, "MAX");
        if (val < min)
          val = min;
        if (val > max)
          val = max;
      }
      else if (iupAttribGetInt(param, "PARTIAL"))
      {
        float min = iupAttribGetFloat(param, "MIN");
        if (val < min)
          val = min;
      }
    }

    iupAttribSetFloat(param, "VALUE", val);
  }

  if (cb && !cb(dlg, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(dlg, "USER_DATA"))) 
  {
    /* Undo */
    iupAttribSetFloat(param, "VALUE", old_value);

    if (!iupAttribGetInt(param, "ANGLE"))
      IupSetfAttribute(self, "VALUE", "%g", old_value);

    /* there is no IUP_IGNORE for IupVal */
    return IUP_DEFAULT;
  }

  type = iupAttribGet(param, "TYPE");
  if (iupStrEqual(type, "INTEGER"))
    IupSetfAttribute(text, "VALUE", "%d", (int)val);
  else
    IupSetfAttribute(text, "VALUE", "%g", val);

  if (IupGetInt(text, "SPIN"))
  {
    if (iupAttribGet(text, "_IUPGP_SPINREAL"))
    {
      float min = iupAttribGetFloat(param, "MIN");
      float step = iupAttribGetFloat(text, "_IUPGP_INCSTEP");
      float val = IupGetFloat(text, "VALUE");
      IupSetfAttribute(text, "SPINVALUE", "%d", (int)((val-min)/step + 0.5));
    }
    else
    {
      int val = IupGetInt(text, "VALUE");
      IupSetfAttribute(text, "SPINVALUE", "%d", val);
    }
  }

  return IUP_DEFAULT;
}

static int iParamListAction_CB(Ihandle *self, char *t, int i, int v)
{
  (void)t;
  if (v == 1)
  {
    Ihandle* param = (Ihandle*)iupAttribGetInherit(self, "_IUPGP_PARAM");
    Ihandle* dlg = IupGetDialog(self);
    Iparamcb cb = (Iparamcb)IupGetCallback(dlg, "PARAM_CB");
    int old_i = iupAttribGetInt(param, "VALUE");

    iupAttribSetInt(param, "VALUE", i-1);

    if (cb && !cb(dlg, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(dlg, "USER_DATA"))) 
    {
      /* Undo */
      iupAttribSetInt(param, "VALUE", old_i);
      IupSetfAttribute(self, "VALUE", "%d", old_i+1);

      /* there is no IUP_IGNORE for IupList */
      return IUP_DEFAULT;
    }
  }

  return IUP_DEFAULT;
}

static int iParamOptionsAction_CB(Ihandle *self, int v)
{
  if (v == 1)
  {
    Ihandle* param = (Ihandle*)iupAttribGetInherit(self, "_IUPGP_PARAM");
    Ihandle* dlg = IupGetDialog(self);
    Iparamcb cb = (Iparamcb)IupGetCallback(dlg, "PARAM_CB");
    int old_v = iupAttribGetInt(param, "VALUE");
    int new_v = iupAttribGetInt(self, "OPT");

    iupAttribSetInt(param, "VALUE", new_v);

    if (cb && !cb(dlg, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(dlg, "USER_DATA"))) 
    {
      /* there is no IUP_IGNORE for IupToggle, manually undo */
      iupAttribSetInt(param, "VALUE", old_v);
      IupSetAttribute(IupGetParent(IupGetParent(self)), "VALUE_HANDLE", (char*)IupGetChild(IupGetParent(self), old_v));
    }
  }

  return IUP_DEFAULT;
}

static int iParamFileButton_CB(Ihandle *self)
{
  Ihandle* param   = (Ihandle*)iupAttribGetInherit(self, "_IUPGP_PARAM");
  Ihandle* textbox = (Ihandle*)iupAttribGetInherit(self, "_IUPGP_TEXT");
  Ihandle* dlg = IupGetDialog(self);
  Iparamcb cb = (Iparamcb)IupGetCallback(dlg, "PARAM_CB");

  Ihandle* fdlg = IupFileDlg();

  IupSetAttributeHandle(fdlg, "PARENTDIALOG", IupGetDialog(self));
  IupSetAttribute(fdlg, "TITLE", iupAttribGet(param, "TITLE"));
  IupSetAttribute(fdlg, "VALUE", iupAttribGet(param, "VALUE"));

  IupSetAttribute(fdlg, "DIALOGTYPE", iupAttribGet(param, "_IUPGP_DIALOGTYPE"));
  IupSetAttribute(fdlg, "FILTER", iupAttribGet(param, "_IUPGP_FILTER"));
  IupSetAttribute(fdlg, "DIRECTORY", iupAttribGet(param, "_IUPGP_DIRECTORY"));
  IupSetAttribute(fdlg, "NOCHANGEDIR", iupAttribGet(param, "_IUPGP_NOCHANGEDIR"));
  IupSetAttribute(fdlg, "NOOVERWRITEPROMPT", iupAttribGet(param, "_IUPGP_NOOVERWRITEPROMPT"));

  IupPopup(fdlg, IUP_CENTER, IUP_CENTER);

  if (!cb || cb(dlg, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(dlg, "USER_DATA"))) 
  {
    if (IupGetInt(fdlg, "STATUS") != -1)
    {
      IupSetAttribute(textbox, "VALUE", iupAttribGet(fdlg, "VALUE"));
      iupAttribStoreStr(param, "VALUE", iupAttribGet(fdlg, "VALUE"));
    }
  }

  IupDestroy(fdlg);

  return IUP_DEFAULT;
}

static int iParamColorButton_CB(Ihandle *self)
{
  Ihandle* param   = (Ihandle*)iupAttribGetInherit(self, "_IUPGP_PARAM");
  Ihandle* textbox = (Ihandle*)iupAttribGetInherit(self, "_IUPGP_TEXT");
  Ihandle* dlg = IupGetDialog(self);
  Iparamcb cb = (Iparamcb)IupGetCallback(dlg, "PARAM_CB");

  Ihandle* color_dlg = IupColorDlg();
  IupSetAttributeHandle(color_dlg, "PARENTDIALOG", IupGetDialog(self));
  IupSetAttribute(color_dlg, "TITLE", iupAttribGet(param, "TITLE"));
  IupSetAttribute(color_dlg, "VALUE", iupAttribGet(param, "VALUE"));

  IupPopup(color_dlg, IUP_CENTER, IUP_CENTER);

  if (!cb || cb(dlg, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(dlg, "USER_DATA"))) 
  {
    if (IupGetInt(color_dlg, "STATUS")==1)
    {
      char* value = IupGetAttribute(color_dlg, "VALUE");
      IupSetAttribute(textbox, "VALUE", value);
      iupAttribStoreStr(param, "VALUE", value);
      IupStoreAttribute(self, "BGCOLOR", value);
    }
  }

  IupDestroy(color_dlg);

  return IUP_DEFAULT;
}

static int iParamFontButton_CB(Ihandle *self)
{
  Ihandle* param   = (Ihandle*)iupAttribGetInherit(self, "_IUPGP_PARAM");
  Ihandle* textbox = (Ihandle*)iupAttribGetInherit(self, "_IUPGP_TEXT");
  Ihandle* dlg = IupGetDialog(self);
  Iparamcb cb = (Iparamcb)IupGetCallback(dlg, "PARAM_CB");

  Ihandle* font_dlg = IupFontDlg();
  IupSetAttributeHandle(font_dlg, "PARENTDIALOG", IupGetDialog(self));
  IupSetAttribute(font_dlg, "TITLE", iupAttribGet(param, "TITLE"));
  IupSetAttribute(font_dlg, "VALUE", iupAttribGet(param, "VALUE"));

  IupPopup(font_dlg, IUP_CENTER, IUP_CENTER);

  if (!cb || cb(dlg, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(dlg, "USER_DATA"))) 
  {
    if (IupGetInt(font_dlg, "STATUS")==1)
    {
      char* value = IupGetAttribute(font_dlg, "VALUE");
      IupSetAttribute(textbox, "VALUE", value);
      iupAttribStoreStr(param, "VALUE", value);
    }
  }

  IupDestroy(font_dlg);

  return IUP_DEFAULT;
}

static int iParamSpinReal_CB(Ihandle *self, int pos)
{
  Ihandle* param = (Ihandle*)iupAttribGetInherit(self, "_IUPGP_PARAM");
  Ihandle* dlg = IupGetDialog(self);
  Iparamcb cb = (Iparamcb)IupGetCallback(dlg, "PARAM_CB");
  Ihandle* text = (Ihandle*)iupAttribGet(param, "CONTROL");
  float min = iupAttribGetFloat(param, "MIN");
  float max = iupAttribGetFloat(param, "MAX");
  float val, step = iupAttribGetFloat(text, "_IUPGP_INCSTEP");

  /* here spin is always [0-spinmax] converted to [min-max] */
  val = (float)pos*step + min;
  if (val < min)
    val = min;
  if (val > max)
    val = max;

  iupAttribSetFloat(param, "VALUE", val);

  if (cb) 
  {
    int ret;
    iupAttribSetStr(dlg, "SPINNING", "1");
    ret = cb(dlg, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(dlg, "USER_DATA"));
    iupAttribSetStr(dlg, "SPINNING", NULL);
    if (!ret)
      return IUP_IGNORE;
  }

  IupSetfAttribute(text, "VALUE", "%g", (double)val);

  {
    Ihandle* aux = (Ihandle*)iupAttribGet(param, "AUXCONTROL");
    if (aux)
      IupSetfAttribute(aux, "VALUE", "%g", (double)val);
  }

  return IUP_DEFAULT;
}

static int iParamSpinInt_CB(Ihandle *self, int pos)
{
  Ihandle* param = (Ihandle*)iupAttribGetInherit(self, "_IUPGP_PARAM");
  Ihandle* dlg = IupGetDialog(self);
  Iparamcb cb = (Iparamcb)IupGetCallback(dlg, "PARAM_CB");
  Ihandle* text = (Ihandle*)iupAttribGet(param, "CONTROL");

  /* here spin is always [min-max] */

  iupAttribSetInt(param, "VALUE", pos);

  if (cb) 
  {
    int ret;
    iupAttribSetStr(dlg, "SPINNING", "1");
    ret = cb(dlg, iupAttribGetInt(param, "INDEX"), (void*)iupAttribGet(dlg, "USER_DATA"));
    iupAttribSetStr(dlg, "SPINNING", NULL);
    if (!ret)
      return IUP_IGNORE;
  }

  IupSetfAttribute(text, "VALUE", "%g", (double)pos);

  {
    Ihandle* aux = (Ihandle*)iupAttribGet(param, "AUXCONTROL");
    if (aux)
      IupSetfAttribute(aux, "VALUE", "%g", (double)pos);
  }

  return IUP_DEFAULT;
}

/*******************************************************************************************
                    Creates One Parameter Box
*******************************************************************************************/

static Ihandle* iParamCreateBox(Ihandle* param)
{
  Ihandle *box, *ctrl = NULL, *label;
  char *type = iupAttribGet(param, "TYPE");

  if (iupStrEqual(type, "BUTTONNAMES"))
    return NULL;

  label = IupLabel(iupAttribGet(param, "TITLE"));

  if (iupStrEqual(type, "SEPARATOR"))
  {
    box = IupHbox(label, NULL);
    IupSetAttribute(box,"ALIGNMENT","ACENTER");
  }
  else
  {
    if (iupStrEqual(type, "STRING") && iupAttribGetInt(param, "MULTILINE"))
    {
      Ihandle* hbox1 = IupHbox(IupSetAttributes(IupFill(), "SIZE=5"), label, NULL);
      IupSetAttribute(hbox1,"ALIGNMENT","ACENTER");

      box = IupVbox(hbox1, NULL);
      IupSetAttribute(box,"ALIGNMENT","ALEFT");
    }
    else
    {
      box = IupHbox(IupSetAttributes(IupFill(), "SIZE=5"), label, NULL);
      IupSetAttribute(box,"ALIGNMENT","ACENTER");
    }
  }

  IupSetAttribute(box,"MARGIN","0x0");

  type = iupAttribGet(param, "TYPE");
  if (iupStrEqual(type, "BOOLEAN"))
  {
    int value = iupAttribGetInt(param, "VALUE");
    if (value)
    {
      ctrl = IupToggle(iupAttribGet(param, "_IUPGP_TRUE"), NULL);
      IupSetAttribute(ctrl, "VALUE", "ON");
    }
    else
    {
      ctrl = IupToggle(iupAttribGet(param, "_IUPGP_FALSE"), NULL);
      IupSetAttribute(ctrl, "VALUE", "OFF");
    }
    IupSetCallback(ctrl, "ACTION", (Icallback)iParamToggleAction_CB);

    IupAppend(box, ctrl);
  }
  else if (iupStrEqual(type, "SEPARATOR"))
  {
    ctrl = IupLabel("");
    IupSetAttribute(ctrl, "SEPARATOR", "HORIZONTAL");

    IupAppend(box, ctrl);
  }
  else if (iupStrEqual(type, "LIST"))
  {
    char str[20] = "0";
    int i = 0;
    ctrl = IupList(NULL);
    IupSetCallback(ctrl, "ACTION", (Icallback)iParamListAction_CB);
    IupSetAttribute(ctrl, "DROPDOWN", "YES");
    IupSetfAttribute(ctrl, "VALUE", "%d", iupAttribGetInt(param, "VALUE")+1);

    while (*iupAttribGet(param, str) != 0)
    {
      IupStoreAttributeId(ctrl, "", i+1, iupAttribGet(param, str));
      i++;
      sprintf(str, "%d", i);
    }
    IupStoreAttributeId(ctrl, "", i+1, NULL);

    IupAppend(box, ctrl);
  }
  else if (iupStrEqual(type, "OPTIONS"))
  {
    Ihandle* tgl;
    char str[20] = "0";
    int i = 0;
    ctrl = IupHbox(NULL);
    IupSetAttribute(ctrl, "GAP", "5");

    while (*iupAttribGet(param, str) != 0)
    {
      tgl = IupToggle(iupAttribGet(param, str), NULL);
      IupStoreAttribute(tgl, "OPT", str);
      IupSetCallback(tgl, "ACTION", (Icallback)iParamOptionsAction_CB);

      IupAppend(ctrl, tgl);

      i++;
      sprintf(str, "%d", i);
    }

    IupAppend(box, IupRadio(ctrl));
    tgl = IupGetChild(ctrl, iupAttribGetInt(param, "VALUE"));
    /* RADIO initial value */
    IupSetAttribute(IupGetParent(ctrl), "VALUE_HANDLE", (char*)tgl);
    ctrl = tgl; /* allow TIP to go to the first toggle */
  }
  else if (iupStrEqual(type, "STRING"))
  {
    if (iupAttribGetInt(param, "MULTILINE"))
    {
      Ihandle* hbox;

      ctrl = IupMultiLine(NULL);
      IupSetAttribute(ctrl, "SIZE", "100x50");
      IupSetAttribute(ctrl, "EXPAND", "YES");

      hbox = IupHbox(IupSetAttributes(IupFill(), "SIZE=5"), ctrl, NULL);
      IupSetAttribute(hbox,"ALIGNMENT","ACENTER");

      IupAppend(box, hbox);
    }
    else
    {
      ctrl = IupText(NULL);
      IupSetAttribute(ctrl, "SIZE", "100x");
      IupSetAttribute(ctrl, "EXPAND", "HORIZONTAL");
      IupAppend(box, ctrl);
    }
    IupSetCallback(ctrl, "ACTION", (Icallback)iParamTextAction_CB);
    IupStoreAttribute(ctrl, "VALUE", iupAttribGet(param, "VALUE"));

    {
      char* mask = iupAttribGet(param, "MASK");
      if (mask)
        IupStoreAttribute(ctrl, "MASK", mask);
    }

    iupAttribSetStr(param, "EXPAND", "1");
  }
  else if (iupStrEqual(type, "FILE"))
  {
      Ihandle* aux;

      ctrl = IupText(NULL);
      IupSetAttribute(ctrl, "SIZE", "100x");
      IupSetAttribute(ctrl, "EXPAND", "HORIZONTAL");
      IupAppend(box, ctrl);

      IupSetCallback(ctrl, "ACTION", (Icallback)iParamTextAction_CB);
      IupStoreAttribute(ctrl, "VALUE", iupAttribGet(param, "VALUE"));

      iupAttribSetStr(param, "EXPAND", "1");

      
      aux = IupButton("...", "");
      IupSetAttribute(aux, "EXPAND", "NO");

      IupSetCallback(aux, "ACTION", (Icallback)iParamFileButton_CB);
      iupAttribSetStr(aux, "_IUPGP_PARAM", (char*)param);
      iupAttribSetStr(aux, "_IUPGP_TEXT", (char*)ctrl);
      IupSetAttribute(aux, "EXPAND", "NO");

      IupAppend(box, aux); 
  }
  else if (iupStrEqual(type, "COLOR"))
  {
      Ihandle* aux;

      ctrl = IupText(NULL);
      IupSetAttribute(ctrl, "SIZE", "100x");
      IupSetAttribute(ctrl, "EXPAND", "HORIZONTAL");
      IupAppend(box, ctrl);

      IupSetCallback(ctrl, "ACTION", (Icallback)iParamTextAction_CB);
      IupSetAttribute(ctrl, "MASK", "(/d|/d/d|1/d/d|2(0|1|2|3|4)/d|25(0|1|2|3|4|5)) (/d|/d/d|1/d/d|2(0|1|2|3|4)/d|25(0|1|2|3|4|5)) (/d|/d/d|1/d/d|2(0|1|2|3|4)/d|25(0|1|2|3|4|5)) (/d|/d/d|1/d/d|2(0|1|2|3|4)/d|25(0|1|2|3|4|5))");
      IupStoreAttribute(ctrl, "VALUE", iupAttribGet(param, "VALUE"));

      iupAttribSetStr(param, "EXPAND", "1");
      
      aux = IupButton(NULL, NULL);
      IupSetAttribute(aux, "SIZE", "16x10");
      IupSetAttribute(aux, "EXPAND", "NO");
      IupStoreAttribute(aux, "BGCOLOR", iupAttribGet(param, "VALUE"));

      IupSetCallback(aux, "ACTION", (Icallback)iParamColorButton_CB);
      iupAttribSetStr(param, "AUXCONTROL", (char*)aux);
      iupAttribSetStr(aux, "_IUPGP_PARAM", (char*)param);
      iupAttribSetStr(aux, "_IUPGP_TEXT", (char*)ctrl);
      IupSetAttribute(aux, "EXPAND", "NO");

      IupAppend(box, aux); 
  }
  else if (iupStrEqual(type, "FONT"))
  {
      Ihandle* aux;

      ctrl = IupText(NULL);
      IupSetAttribute(ctrl, "SIZE", "100x");
      IupSetAttribute(ctrl, "EXPAND", "HORIZONTAL");
      IupAppend(box, ctrl);

      IupSetCallback(ctrl, "ACTION", (Icallback)iParamTextAction_CB);
      IupStoreAttribute(ctrl, "VALUE", iupAttribGet(param, "VALUE"));

      iupAttribSetStr(param, "EXPAND", "1");
      
      aux = IupButton("F", NULL);
      IupSetAttribute(aux, "EXPAND", "NO");
      IupStoreAttribute(aux, "FONT", "Times, Bold Italic 12");
      IupSetAttribute(aux, "SIZE", "16x10");

      IupSetCallback(aux, "ACTION", (Icallback)iParamFontButton_CB);
      iupAttribSetStr(param, "AUXCONTROL", (char*)aux);
      iupAttribSetStr(aux, "_IUPGP_PARAM", (char*)param);
      iupAttribSetStr(aux, "_IUPGP_TEXT", (char*)ctrl);
      IupSetAttribute(aux, "EXPAND", "NO");

      IupAppend(box, aux); 
  }
  else /* INTEGER, REAL */
  {
    ctrl = IupText(NULL);
    IupSetCallback(ctrl, "ACTION", (Icallback)iParamTextAction_CB);
    IupStoreAttribute(ctrl, "VALUE", iupAttribGet(param, "VALUE"));

    type = iupAttribGet(param, "TYPE");
    if (iupStrEqual(type, "REAL"))
    {
      if (iupAttribGetInt(param, "INTERVAL"))
      {
        float min = iupAttribGetFloat(param, "MIN");
        float max = iupAttribGetFloat(param, "MAX");
        float step = iupAttribGetFloat(param, "STEP");
        float val = iupAttribGetFloat(param, "VALUE");
        if (step == 0) step = (max-min)/20.0f;
        IupSetfAttribute(ctrl, "MASKFLOAT", "%g:%g", (double)min, (double)max);
                             
        /* here spin is always [0-spinmax] converted to [min-max] */

        IupSetAttribute(ctrl, "SPIN", "YES");   /* spin only for intervals */
        IupSetAttribute(ctrl, "SPINAUTO", "NO");
        IupAppend(box, ctrl);
        IupSetCallback(ctrl, "SPIN_CB", (Icallback)iParamSpinReal_CB);
        /* SPINMIN=0 and SPININC=1 */
        IupSetfAttribute(ctrl, "SPINMAX", "%d", (int)((max-min)/step + 0.5));
        IupSetfAttribute(ctrl, "SPINVALUE", "%d", (int)((val-min)/step + 0.5));

        iupAttribSetFloat(ctrl, "_IUPGP_INCSTEP", step);
        iupAttribSetStr(ctrl, "_IUPGP_SPINREAL", "1");
      }
      else if (iupAttribGetInt(param, "PARTIAL"))
      {
        float min = iupAttribGetFloat(param, "MIN");
        if (min == 0)
          IupSetAttribute(ctrl, "MASK", IUP_MASK_UFLOAT);
        else
          IupSetfAttribute(ctrl, "MASKFLOAT", "%g:%g", (double)min, (double)1.0e10);
        IupAppend(box, ctrl);
      }
      else
      {
        IupSetAttribute(ctrl, "MASK", IUP_MASK_FLOAT);
        IupAppend(box, ctrl);
      }

      IupSetAttribute(ctrl, "SIZE", "50x");
    }
    else /* INTEGER*/
    {
      int val = iupAttribGetInt(param, "VALUE");
      IupSetAttribute(ctrl, "SPIN", "YES");   /* spin always */
      IupSetAttribute(ctrl, "SPINAUTO", "NO");  /* manually update spin so the callback can also updated it */
      IupAppend(box, ctrl);
      IupSetCallback(ctrl, "SPIN_CB", (Icallback)iParamSpinInt_CB);
      iupAttribSetStr(ctrl, "_IUPGP_INCSTEP", "1");
      IupSetfAttribute(ctrl, "SPINVALUE", "%d", val);

      /* here spin is always [min-max] */

      if (iupAttribGetInt(param, "INTERVAL"))
      {
        int min = iupAttribGetInt(param, "MIN");
        int max = iupAttribGetInt(param, "MAX");
        int step = iupAttribGetInt(param, "STEP");
        if (step)
        {
          iupAttribSetInt(ctrl, "_IUPGP_INCSTEP", step);
          IupSetfAttribute(ctrl, "SPININC", "%d", step);
        }
        IupSetfAttribute(ctrl, "SPINMAX", "%d", max);
        IupSetfAttribute(ctrl, "SPINMIN", "%d", min);
        IupSetfAttribute(ctrl, "MASKINT", "%d:%d", min, max);
      }
      else if (iupAttribGetInt(param, "PARTIAL"))
      {
        int min = iupAttribGetInt(param, "MIN");
        if (min == 0)
          IupSetAttribute(ctrl, "MASK", IUP_MASK_UINT);
        else
          IupSetfAttribute(ctrl, "MASKINT", "%d:2147483647", min);
        IupSetfAttribute(ctrl, "SPINMIN", "%d", min);
        IupSetAttribute(ctrl, "SPINMAX", "2147483647");
      }
      else                             
      {
        IupSetAttribute(ctrl, "SPINMIN", "-2147483647");
        IupSetAttribute(ctrl, "SPINMAX", "2147483647");
        IupSetAttribute(ctrl, "MASK", IUP_MASK_INT);
      }

      IupSetAttribute(ctrl, "SIZE", "50x");
    }

    if (iupAttribGetInt(param, "INTERVAL") || iupAttribGetInt(param, "ANGLE"))
    {
      Ihandle* aux;

      if (iupAttribGetInt(param, "ANGLE"))
      {
        aux = IupCreate("dial");  /* Use IupCreate to avoid depending on the IupControls library */
        if (aux)  /* If IupControls library is not available it will fail */
        {
          IupSetfAttribute(aux, "VALUE", "%g", (double)(iupAttribGetFloat(param, "VALUE")/RAD2DEG));
          IupSetAttribute(aux, "SIZE", "50x10");
        }
      }
      else
      {
        char* step;
        aux = IupVal("HORIZONTAL");
        IupStoreAttribute(aux, "MIN", iupAttribGet(param, "MIN"));
        IupStoreAttribute(aux, "MAX", iupAttribGet(param, "MAX"));
        IupStoreAttribute(aux, "VALUE", iupAttribGet(param, "VALUE"));
        IupSetAttribute(aux, "EXPAND", "HORIZONTAL");
        iupAttribSetStr(param, "AUXCONTROL", (char*)aux);
        iupAttribSetStr(param, "EXPAND", "1");
        step = iupAttribGet(param, "STEP");
        if (step)
          IupSetfAttribute(aux, "STEP", "%g", iupAttribGetFloat(param, "STEP")/(iupAttribGetFloat(param, "MAX")-iupAttribGetFloat(param, "MIN")));
        else if (iupStrEqual(type, "INTEGER"))
          IupSetfAttribute(aux, "STEP", "%g", 1.0/(iupAttribGetFloat(param, "MAX")-iupAttribGetFloat(param, "MIN")));
      }

      if (aux)
      {
        IupSetCallback(aux, "VALUECHANGED_CB", (Icallback)iParamValAction_CB);
        iupAttribSetStr(aux, "_IUPGP_PARAM", (char*)param);
        iupAttribSetStr(aux, "_IUPGP_TEXT", (char*)ctrl);

        IupAppend(box, aux);
      }
    }
  }

  if (ctrl) IupStoreAttribute(ctrl, "TIP", iupAttribGet(param, "TIP"));
  iupAttribSetStr(box, "_IUPGP_PARAM", (char*)param);
  iupAttribSetStr(param, "CONTROL", (char*)ctrl);
  iupAttribSetStr(param, "LABEL", (char*)label);
  return box;
}

/*******************************************************************************************
                    Creates the Dialog and Normalize Sizes
*******************************************************************************************/

static Ihandle* IupParamDlgP(Ihandle** params)
{
  Ihandle *dlg, *button_ok, *button_cancel, *button_help=NULL, 
          *dlg_box, *button_box, *param_box;
  int i, lbl_width, p, expand;

  button_ok = IupButton(iupStrMessageGet("IUP_OK"), NULL);
  IupSetAttribute(button_ok, "PADDING", "20x0");
  IupSetCallback(button_ok, "ACTION", (Icallback)iParamButtonOK_CB);

  button_cancel = IupButton(iupStrMessageGet("IUP_CANCEL"), NULL);
  IupSetAttribute(button_cancel, "PADDING", "20x0");
  IupSetCallback(button_cancel, "ACTION", (Icallback)iParamButtonCancel_CB);
  
  param_box = IupVbox(NULL);

  i = 0; expand = 0;
  while (params[i] != NULL)
  {
    Ihandle* box = iParamCreateBox(params[i]);
    if (box)
      IupAppend(param_box, box);
    else /* buttonnames */
    {
      char* value = iupAttribGet(params[i], "_IUPGP_OK");
      if (value && *value) IupSetAttribute(button_ok, "TITLE", value);
      value = iupAttribGet(params[i], "_IUPGP_CANCEL");
      if (value && *value) IupSetAttribute(button_cancel, "TITLE", value);
      value = iupAttribGet(params[i], "_IUPGP_HELP");
      if (value && *value) 
      {
        button_help = IupButton(value, NULL);
        IupSetAttribute(button_help, "PADDING", "20x0");
        IupSetCallback(button_help, "ACTION", (Icallback)iParamButtonHelp_CB);
      }
    }

    if (IupGetInt(params[i], "EXPAND"))
      expand = 1;

    i++;
  }

  button_box = IupHbox(
    IupFill(),
    button_ok,
    button_cancel,
    button_help,
    NULL);
  IupSetAttribute(button_box,"MARGIN","0x0");
  IupSetAttribute(button_box, "NORMALIZESIZE", "HORIZONTAL");

  dlg_box = IupVbox(
    IupFrame(param_box),
    button_box,
    NULL);
  IupSetAttribute(dlg_box, "MARGIN", "10x10");
  IupSetAttribute(dlg_box, "GAP", "5");

  dlg = IupDialog(dlg_box);

  if (!expand) 
  {
    IupSetAttribute(dlg, "DIALOGFRAME", "YES");
    IupSetAttribute(dlg,"DIALOGHINT","YES");
  }
  else
  {
    IupSetAttribute(dlg, "MINBOX", "NO");
    IupSetAttribute(dlg, "MAXBOX", "NO");
  }
  IupSetAttributeHandle(dlg, "DEFAULTENTER", button_ok);
  IupSetAttributeHandle(dlg, "DEFAULTESC", button_cancel);
  IupSetAttribute(dlg, "TITLE", "ParamDlg");
  IupSetAttribute(dlg, "PARENTDIALOG", IupGetGlobal("PARENTDIALOG"));
  IupSetAttribute(dlg, "ICON", IupGetGlobal("ICON"));

  IupMap(dlg);

  /* get the largest label size and set INDEX */
  i = 0; lbl_width = 0, p = 0;
  while (params[i] != NULL)
  {
    int w;

    if (IupGetInt(params[i], "DATA_TYPE") != IPARAM_TYPE_NONE)
    {
      char str[20];
      sprintf(str, "PARAM%d", p);
      iupAttribSetStr(dlg, str, (char*)params[i]);
      iupAttribSetInt(params[i], "INDEX", p);
      p++;
    }

    w = IupGetInt((Ihandle*)iupAttribGet(params[i], "LABEL"), "SIZE");
    if (w > lbl_width)
      lbl_width = w;

    i++;
  }

  i = 0;
  while (params[i] != NULL)
  {
    if (IupGetInt(params[i], "DATA_TYPE") != IPARAM_TYPE_NONE)
    {
      char* type = iupAttribGet(params[i], "TYPE");
      if (iupStrEqual(type, "LIST"))
      {
        /* set a minimum size for lists */
        Ihandle* ctrl = (Ihandle*)iupAttribGet(params[i], "CONTROL");
        if (IupGetInt(ctrl, "SIZE") < 50)
          IupSetAttribute(ctrl, "SIZE", "50x");
      }
      else if (iupStrEqual(type, "BOOLEAN"))
      {
        /* reserve enough space for boolean strings */
        Ihandle* ctrl = (Ihandle*)iupAttribGet(params[i], "CONTROL");
        int wf = iupdrvFontGetStringWidth(ctrl, iupAttribGet(params[i], "_IUPGP_FALSE"));
        int wt = iupdrvFontGetStringWidth(ctrl, iupAttribGet(params[i], "_IUPGP_TRUE"));
        int w = IupGetInt(ctrl, "SIZE");
        int v = IupGetInt(ctrl, "VALUE");
        if (v) /* True */
        {
          int box = w - wt;
          wf += box;
          if (wf > w)
            IupSetfAttribute(ctrl, "SIZE", "%dx", wf+8);
        }
        else
        {
          int box = w - wf;
          wt += box;
          if (wt > w)
            IupSetfAttribute(ctrl, "SIZE", "%dx", wt+8);
        }
      }

      IupSetfAttribute((Ihandle*)iupAttribGet(params[i], "LABEL"), "SIZE", "%dx", lbl_width);
    }

    i++;
  }

  IupSetAttribute(dlg, "SIZE", NULL);

  return dlg;
}

/*******************************************************************************************
                    Parameter String Parsing
*******************************************************************************************/

static char* iParamGetNextStrItem(char* line, char sep, int *count)
{
  int i = 0;

  while (line[i] != '\n' && line[i] != 0)
  {
    if (line[i] == sep)
    {
      line[i] = 0;
      *count = i+1;
      return line;
    }

    i++;
  }

  /* last item may not have the separator */
  line[i] = 0;
  *count = i;
  return line;
}

static void iParamSetBoolNames(char* extra, Ihandle* param)
{
  char *falsestr = NULL, *truestr = NULL;
  int count;

  if (extra)
  {
    falsestr = iParamGetNextStrItem(extra, ',', &count);  extra += count;
    truestr = iParamGetNextStrItem(extra, ',', &count);
  }

  if (falsestr && truestr)
  {
    iupAttribStoreStr(param, "_IUPGP_TRUE", truestr);
    iupAttribStoreStr(param, "_IUPGP_FALSE", falsestr);
  }
  else
  {
/*    iupAttribStoreStr(param, "_IUPGP_TRUE", iupStrMessageGet("IUP_TRUE"));     */
/*    iupAttribStoreStr(param, "_IUPGP_FALSE", iupStrMessageGet("IUP_FALSE"));   */
    iupAttribStoreStr(param, "_IUPGP_TRUE", "");
    iupAttribStoreStr(param, "_IUPGP_FALSE", "");
  }
}

static void iParamSetInterval(char* extra, Ihandle* param)
{
  char *min, *max, *step;
  int count;

  if (!extra)
    return;

  min = iParamGetNextStrItem(extra, ',', &count);  extra += count;
  max = iParamGetNextStrItem(extra, ',', &count);  extra += count;
  step = iParamGetNextStrItem(extra, ',', &count);  

  if (max[0])
  {
    iupAttribSetStr(param, "INTERVAL", "1");
    iupAttribStoreStr(param, "MIN", min);
    iupAttribStoreStr(param, "MAX", max);
    if (step[0])
      iupAttribStoreStr(param, "STEP", step);
  }
  else
  {
    iupAttribSetStr(param, "PARTIAL", "1");
    iupAttribStoreStr(param, "MIN", min);
  }
}

static void iParamSetFileOptions(char* extra, Ihandle* param)
{
  char *type, *filter, *directory, *nochangedir, *nooverwriteprompt;
  int count;

  if (!extra)
    return;

  type = iParamGetNextStrItem(extra, '|', &count);  extra += count;
  filter = iParamGetNextStrItem(extra, '|', &count);  extra += count;
  directory = iParamGetNextStrItem(extra, '|', &count);  extra += count;
  nochangedir = iParamGetNextStrItem(extra, '|', &count);  extra += count;
  nooverwriteprompt = iParamGetNextStrItem(extra, '|', &count);  extra += count;

  iupAttribStoreStr(param, "_IUPGP_DIALOGTYPE", type);
  iupAttribStoreStr(param, "_IUPGP_FILTER", filter);
  iupAttribStoreStr(param, "_IUPGP_DIRECTORY", directory);
  iupAttribStoreStr(param, "_IUPGP_NOCHANGEDIR", nochangedir);
  iupAttribStoreStr(param, "_IUPGP_NOOVERWRITEPROMPT", nooverwriteprompt);
}

static void iParamSetButtonNames(char* extra, Ihandle* param)
{
  char *ok, *cancel, *help;
  int count;

  if (!extra)
    return;

  ok = iParamGetNextStrItem(extra, ',', &count);  extra += count;
  cancel = iParamGetNextStrItem(extra, ',', &count);  extra += count;
  help = iParamGetNextStrItem(extra, ',', &count);  extra += count;

  iupAttribStoreStr(param, "_IUPGP_OK", ok);
  iupAttribStoreStr(param, "_IUPGP_CANCEL", cancel);
  iupAttribStoreStr(param, "_IUPGP_HELP", help);
}

static void iParamSetListItems(char* extra, Ihandle* param)
{
  int i = 0, count;
  char str[20], *item;

  if (!extra)
    return;

  item = iParamGetNextStrItem(extra, '|', &count);  extra += count;
  while (*item)
  {
    sprintf(str, "%i", i);
    iupAttribStoreStr(param, str, item);

    item = iParamGetNextStrItem(extra, '|', &count);  extra += count;
    i++;
  }

  sprintf(str, "%i", i);
  iupAttribSetStr(param, str, "");
}

static char* iParamGetStrExtra(char* line, char start, char end, int *count)
{
  int i = 0, end_pos = -1;

  if (*line != start)
  {
    *count = 0;
    return NULL;
  }
  line++; /* skip start */

  while (line[i] != '\n' && line[i] != 0)
  {
    if (line[i] == end)
      end_pos = i;

    i++;
  }

  if (end_pos != -1)
  {
    line[end_pos] = 0;
    *count = 1+end_pos+1;
    return line;
  }
  else
  {
    *count = 0;
    return NULL;
  }
}

static int iParamCopyStrLine(char* line, const char* format)
{
  int i = 0;
  while (format[i] != '\n' && format[i] != 0)
  {
    line[i] = format[i];
    i++;
    if (i > 4094)   /* to avoid being bigger than the local array */
      break;
  }
  line[i] = '\n';
  line[i+1] = 0;
  return i+1; 
}

static char* iParmGetType(const char* format)
{
  char* type = strchr(format, '%');
  while (type && *(type+1)=='%')
    type = strchr(type+2, '%');
  return type;
}

/* Used in IupLua */
char iupGetParamType(const char* format, int *line_size)
{
  char* type = iParmGetType(format);
  char* line_end = strchr(format, '\n');
  if (line_end)
    *line_size = line_end-format+1;  /* include line separator */
  if (type)
    return *(type+1);  /* skip separator */
  else
    return 0;
}

static char* iParamGetTitle(char* line_ptr, int count)
{
  int i, n;
  char* title = line_ptr;
  title[count] = 0;
  n = -1;
  for (i=0; i<count; i++)
  {
    if (title[i] != '%')
      n++;
    else /* if (title[i+1] == '%')  if equal to %, next is a %, because we are inside title only */
    {
      n++;
      i++;  /* skip second % */
    }

    if (i != n)  /* copy only if there is a second % */
      title[n] = title[i];
  }
  return title;
}

static Ihandle *IupParamf(const char* format, int *line_size)
{
  Ihandle* param;
  char line[4096];
  char* line_ptr = &line[0], *title, *type, *tip, *extra, *mask;
  int count;

  *line_size = iParamCopyStrLine(line, format);

  type = iParmGetType(line_ptr);
  if (!type)
    return NULL;

  count = type-line_ptr;
  title = iParamGetTitle(line_ptr, count);
  line_ptr += count;

  type++; /* skip separator */
  line_ptr += 2;

  param = IupUser();
  iupAttribStoreStr(param, "TITLE", title);
  
/**********************************************************************************
  REMEMBER: if a new parameter type is added
            then IupLua must be also updated.
 **********************************************************************************/

  switch(*type)
  {
  case 'b':
    iupAttribSetStr(param, "TYPE", "BOOLEAN");
    iupAttribSetInt(param, "DATA_TYPE", IPARAM_TYPE_INT);
    extra = iParamGetStrExtra(line_ptr, '[', ']', &count);  line_ptr += count;
    iParamSetBoolNames(extra, param);
    break;
  case 'l':
    iupAttribSetStr(param, "TYPE", "LIST");
    iupAttribSetInt(param, "DATA_TYPE", IPARAM_TYPE_INT);
    extra = iParamGetStrExtra(line_ptr, '|', '|', &count);  line_ptr += count;
    iParamSetListItems(extra, param);
    break;
  case 'o':
    iupAttribSetStr(param, "TYPE", "OPTIONS");
    iupAttribSetInt(param, "DATA_TYPE", IPARAM_TYPE_INT);
    extra = iParamGetStrExtra(line_ptr, '|', '|', &count);  line_ptr += count;
    iParamSetListItems(extra, param);
    break;
  case 'a':
    iupAttribSetStr(param, "TYPE", "REAL");
    iupAttribSetInt(param, "DATA_TYPE", IPARAM_TYPE_FLOAT);
    iupAttribSetStr(param, "ANGLE", "1");
    extra = iParamGetStrExtra(line_ptr, '[', ']', &count);  line_ptr += count;
    iParamSetInterval(extra, param);
    break;
  case 'm':
    iupAttribSetStr(param, "MULTILINE", "1");
    /* continue */
  case 's':
    iupAttribSetStr(param, "TYPE", "STRING");
    iupAttribSetInt(param, "DATA_TYPE", IPARAM_TYPE_STR);
    mask = iParamGetNextStrItem(line_ptr, '{', &count);  
    if (*mask) 
      iupAttribStoreStr(param, "MASK", mask);
    line_ptr += count-1; /* ignore the fake separator */
    line_ptr[0] = '{';   /* restore possible separator */
    break;
  case 'i':
    iupAttribSetStr(param, "TYPE", "INTEGER");
    iupAttribSetInt(param, "DATA_TYPE", IPARAM_TYPE_INT);
    extra = iParamGetStrExtra(line_ptr, '[', ']', &count);  line_ptr += count;
    iParamSetInterval(extra, param);
    break;
  case 'r':
    iupAttribSetStr(param, "TYPE", "REAL");
    iupAttribSetInt(param, "DATA_TYPE", IPARAM_TYPE_FLOAT);
    extra = iParamGetStrExtra(line_ptr, '[', ']', &count);  line_ptr += count;
    iParamSetInterval(extra, param);
    break;
  case 'f':
    iupAttribSetStr(param, "TYPE", "FILE");
    iupAttribSetInt(param, "DATA_TYPE", IPARAM_TYPE_STR);
    extra = iParamGetStrExtra(line_ptr, '[', ']', &count);  line_ptr += count;
    iParamSetFileOptions(extra, param);
    break;
  case 'n':
    iupAttribSetStr(param, "TYPE", "FONT");
    iupAttribSetInt(param, "DATA_TYPE", IPARAM_TYPE_STR);
    break;
  case 'c':
    iupAttribSetStr(param, "TYPE", "COLOR");
    iupAttribSetInt(param, "DATA_TYPE", IPARAM_TYPE_STR);
    break;
  case 't':
    iupAttribSetStr(param, "TYPE", "SEPARATOR");
    iupAttribSetInt(param, "DATA_TYPE", IPARAM_TYPE_NONE);
    break;
  case 'u':
    iupAttribSetStr(param, "TYPE", "BUTTONNAMES");
    iupAttribSetInt(param, "DATA_TYPE", IPARAM_TYPE_NONE);
    extra = iParamGetStrExtra(line_ptr, '[', ']', &count);  line_ptr += count;
    iParamSetButtonNames(extra, param);
    break;
  default:
    IupDestroy(param);
    return NULL;
  }

  tip = iParamGetStrExtra(line_ptr, '{', '}', &count);
  if (tip)
    iupAttribStoreStr(param, "TIP", tip);

  return param;
}

/*******************************************************************************************
                    Exported Functions
*******************************************************************************************/

/* Used in IupLua also */
int iupGetParamCount(const char *format, int *param_extra)
{
  int param_count = 0, extra = 0;
  const char* s = format;

  *param_extra = 0;
  while(*s)
  {
    if (*s == '%' && *(s+1) == 't')  /* do not count separator lines */
    {
      extra = 1;
      (*param_extra)++;
    }

    if (*s == '%' && *(s+1) == 'u')  /* do not count button names lines */
    {
      extra = 1;
      (*param_extra)++;
    }

    if (*s == '\n')
    {
      if (extra)
        extra = 0;
      else
        param_count++;
    }

    s++;
  }

  return param_count;
}

static void iParamDestroyAll(Ihandle **params)
{
  int i = 0;
  while (params[i] != NULL)
  {
    IupDestroy(params[i]);
    i++;
  }
}

int IupGetParamv(const char* title, Iparamcb action, void* user_data, const char* format, int param_count, int param_extra, void** param_data)
{
  Ihandle *dlg, *params[50];
  int i, line_size, p;

  assert(title && format);
  if (!title || !format)
    return 0;

  for (i = 0, p = 0; i < param_count+param_extra; i++)
  {
    int data_type;

    params[i] = IupParamf(format, &line_size);
    assert(params[i]);
    if (!params[i])
      return 0;

    data_type = IupGetInt(params[i], "DATA_TYPE");
    if (data_type == IPARAM_TYPE_FLOAT)
    {
      float *data_float = (float*)(param_data[p]);
      if (!data_float) return 0;
      iupAttribSetFloat(params[i], "VALUE", *data_float);
      p++;
    }
    else if (data_type == IPARAM_TYPE_INT)
    {
      int *data_int = (int*)(param_data[p]);
      if (!data_int) return 0;
      iupAttribSetInt(params[i], "VALUE", *data_int);
      p++;
    }
    else if (data_type == IPARAM_TYPE_STR)
    {
      char *data_str = (char*)(param_data[p]);
      if (!data_str) return 0;
      iupAttribStoreStr(params[i], "VALUE", data_str);
      p++;
    }

    format += line_size; 
  }
  params[i] = NULL;

  dlg = IupParamDlgP(params);
  IupSetAttribute(dlg, "TITLE", (char*)title);
  IupSetCallback(dlg, "PARAM_CB", (Icallback)action);
  iupAttribSetStr(dlg, "USER_DATA", (char*)user_data);

  if (action) 
    action(dlg, IUP_GETPARAM_INIT, user_data);

  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (!IupGetInt(dlg, "STATUS"))
  {
    iParamDestroyAll(params);
    IupDestroy(dlg);
    return 0;
  }
  else
  {
    for (i = 0, p = 0; i < param_count+param_extra; i++)
    {
      Ihandle* param = params[i];
      int data_type = iupAttribGetInt(param, "DATA_TYPE");
      if (data_type == IPARAM_TYPE_INT)
      {
        int *data_int = (int*)(param_data[p]);
        *data_int = iupAttribGetInt(param, "VALUE");
        p++;
      }
      else if (data_type == IPARAM_TYPE_FLOAT)
      {
        float *data_float = (float*)(param_data[p]);
        *data_float = iupAttribGetFloat(param, "VALUE");
        p++;
      }
      else if (data_type == IPARAM_TYPE_STR)
      {
        char *data_str = (char*)(param_data[p]);
        int max_str = iupAttribGetInt(param, "MAXSTR");
        if (!max_str)
        {
          max_str = 512;
          if (iupStrEqual(iupAttribGet(param, "TYPE"), "FILE"))
            max_str = 4096;
          else if (iupStrEqual(iupAttribGet(param, "TYPE"), "STRING") && iupAttribGetInt(param, "MULTILINE"))
            max_str = 10240;
        }
        iupStrCopyN(data_str, max_str, iupAttribGet(param, "VALUE"));
        p++;
      }
    }

    iParamDestroyAll(params);
    IupDestroy(dlg);
    return 1;
  }
}

int IupGetParam(const char* title, Iparamcb action, void* user_data, const char* format,...)
{
  int param_count, param_extra, i;
  void* param_data[50];
  va_list arg;

  param_count = iupGetParamCount(format, &param_extra);

  va_start(arg, format);
  for (i = 0; i < param_count; i++)
  {
    param_data[i] = (void*)(va_arg(arg, void*));
  }
  va_end(arg);

  return IupGetParamv(title, action, user_data, format, param_count, param_extra, param_data);
}

