/** \file
 * \brief pre-defined dialogs
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <time.h>

#include "iup.h" 

#include "iup_predialogs.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_strmessage.h"
               

static int CB_button_OK (Ihandle* ih)
{
  iupAttribSetStr(IupGetDialog(ih), "STATUS", "1");
  return IUP_CLOSE;
}

static int CB_button_CANCEL (Ihandle* ih)
{
  iupAttribSetStr(IupGetDialog(ih), "STATUS", "-1");
  return IUP_CLOSE;
}

static int CB_dblclick(Ihandle *ih, int item, char *text)
{
  (void)text;
  iupAttribSetStrf(IupGetDialog(ih), "_IUP_LIST_NUMBER", "%d", item-1);
  iupAttribSetStr(IupGetDialog(ih), "STATUS", "1");
  return IUP_CLOSE;
}

static int CB_list (Ihandle *ih, char *text, int item, int state)
{
  (void)text;
  if (state)
    iupAttribSetStrf(IupGetDialog(ih), "_IUP_LIST_NUMBER", "%d", item-1);
  return IUP_DEFAULT;
}

int IupListDialog (int type, const char *title, int size, const char** list_str,
                   int op, int max_col, int max_lin, int* marks)
{
  Ihandle *lst, *ok, *dlg, *cancel, *dlg_box, *button_box;
  int i, bt;
  char attrib_str[20];
  char *m=NULL;

  if (size > 999)
    size = 999;

  lst = IupList(NULL);

  for (i=0;i<size;i++)
  {
    sprintf(attrib_str,"%d",i+1);
    IupSetAttribute(lst,attrib_str,list_str[i]);
  }
  sprintf(attrib_str,"%d",i+1);
  IupSetAttribute(lst,attrib_str,NULL);
  IupSetAttribute(lst,"EXPAND","YES");

  ok = IupButton(iupStrMessageGet("IUP_OK"), NULL);
  IupSetAttribute(ok,"PADDING" ,"20x5");
  IupSetCallback(ok, "ACTION", (Icallback)CB_button_OK);

  cancel = IupButton(iupStrMessageGet("IUP_CANCEL"), NULL);
  IupSetAttribute(cancel,"PADDING" ,"20x5");
  IupSetCallback(cancel, "ACTION", (Icallback)CB_button_CANCEL);

  button_box = IupHbox(
    IupFill(), 
    ok,
    cancel,
    NULL);
  IupSetAttribute(button_box,"MARGIN","0x0");
  IupSetAttribute(button_box, "NORMALIZESIZE", "HORIZONTAL");

  dlg_box = IupVbox(
    lst,
    button_box,
    NULL);

  IupSetAttribute(dlg_box,"MARGIN","10x10");
  IupSetAttribute(dlg_box,"GAP","10");

  dlg = IupDialog(dlg_box);

  if (type == 1)
  {
    if (op<1 || op>size) op=1;
    iupAttribSetStrf(dlg, "_IUP_LIST_NUMBER", "%d", op-1);
    IupSetfAttribute(lst,"VALUE","%d",op);
    IupSetCallback(lst, "ACTION", (Icallback)CB_list);
    IupSetCallback(lst, "DBLCLICK_CB", (Icallback)CB_dblclick);
  }
  else if ((type == 2) && (marks != NULL))
  {
    m=(char *)marks;
    for (i=0;i<size;i++)
      m[i] = marks[i] ? '+' : '-';
    m[i]='\0';
    IupSetAttribute(lst,"MULTIPLE","YES");
    IupSetAttribute(lst,"VALUE",m);
  }

  if (max_lin < 4) max_lin = 4;
  IupSetfAttribute(lst, "VISIBLELINES", "%d", max_lin);
  IupSetfAttribute(lst, "VISIBLECOLUMNS", "%d", max_col);

  IupSetAttribute(dlg,"TITLE", title);
  IupSetAttribute(dlg,"MINBOX","NO");
  IupSetAttribute(dlg,"MAXBOX","NO");
  IupSetAttributeHandle(dlg,"DEFAULTENTER", ok);
  IupSetAttributeHandle(dlg,"DEFAULTESC", cancel);
  IupSetAttribute(dlg,"PARENTDIALOG", IupGetGlobal("PARENTDIALOG"));
  IupSetAttribute(dlg,"ICON", IupGetGlobal("ICON"));

  IupPopup(dlg,IUP_CENTERPARENT,IUP_CENTERPARENT);

  if ((type == 2) && (marks != NULL))
  {
    m=IupGetAttribute(lst,"VALUE");
    for (i=0;i<size;i++)
      marks[i] = (m[i] == '+');
  }

  bt = IupGetInt(dlg, "STATUS");
  if (type == 1)
  {
    if (bt == 1)
      bt = iupAttribGetInt(dlg, "_IUP_LIST_NUMBER");
    else
      bt = -1;
  }
  else
  {
    if (bt != 1)
      bt = -1;
  }

  IupDestroy(dlg);

  return bt;
}

static int iAlarmButtonAction_CB(Ihandle *ih)
{
  iupAttribSetStr(IupGetDialog(ih), "_IUP_BUTTON_NUMBER", iupAttribGet(ih, "_IUP_BUTTON_NUMBER"));
  return IUP_CLOSE;
}

int IupAlarm(const char *title, const char *msg, const char *b1, const char *b2, const char *b3)
{
  Ihandle  *dlg, *dlg_box, *button_box, *button, *default_esc, *default_enter;
  int bt, len;
  char* padding;

  msg = msg? msg: "";

  if (b1 == NULL)
    return 0;

  len = strlen(b1);
  if (b2)
  {
    int len2 = strlen(b2);
    if (len2 > len)
      len = len2;
  }
  if (b3)
  {
    int len3 = strlen(b3);
    if (len3 > len)
      len = len3;
  }

  if (len > 7)
    padding = "12x2";
  else
    padding = "18x5";

  button_box = IupHbox(NULL);
  IupSetAttribute(button_box, "NORMALIZESIZE", "HORIZONTAL");
  IupSetAttribute(button_box,"MARGIN","0x0");
  IupAppend(button_box, IupFill()); /* to center the buttons */

  button = IupButton(b1, NULL);
  iupAttribSetStr(button, "_IUP_BUTTON_NUMBER", "1");
  IupSetAttribute(button, "PADDING", padding);
  IupAppend(button_box, button);
  IupSetCallback (button, "ACTION", (Icallback)iAlarmButtonAction_CB);
  default_enter = button;
  default_esc = button;

  if (b2 != NULL)
  {
    button = IupButton(b2, NULL);
    iupAttribSetStr(button, "_IUP_BUTTON_NUMBER", "2");
    IupSetAttribute(button, "PADDING", padding);
    IupAppend(button_box, button);
    IupSetCallback (button, "ACTION", (Icallback)iAlarmButtonAction_CB);
    default_esc = button;
  }

  if (b3 != NULL)
  {
    button = IupButton(b3, NULL);
    iupAttribSetStr(button, "_IUP_BUTTON_NUMBER", "3");
    IupSetAttribute(button, "PADDING", padding);
    IupAppend(button_box, button);
    IupSetCallback (button, "ACTION", (Icallback)iAlarmButtonAction_CB);
    default_esc = button;
  }

  IupAppend(button_box, IupFill()); /* to center the buttons */

  dlg_box = IupVbox(
    IupLabel(msg),
    IupSetAttributes(IupLabel(NULL), "SEPARATOR=HORIZONTAL"),
    button_box,
    NULL);

  IupSetAttribute(dlg_box,"MARGIN","10x10");
  IupSetAttribute(dlg_box,"GAP","10");

  dlg = IupDialog(dlg_box);

  IupSetAttribute(dlg,"TITLE", title);
  IupSetAttribute(dlg,"DIALOGFRAME","YES");
  IupSetAttribute(dlg,"DIALOGHINT","YES");
  IupSetAttributeHandle(dlg,"DEFAULTENTER", default_enter);
  IupSetAttributeHandle(dlg,"DEFAULTESC", default_esc);
  IupSetAttribute(dlg,"PARENTDIALOG", IupGetGlobal("PARENTDIALOG"));
  IupSetAttribute(dlg,"ICON", IupGetGlobal("ICON"));

  IupPopup(dlg,IUP_CENTERPARENT,IUP_CENTERPARENT);

  bt = iupAttribGetInt(dlg, "_IUP_BUTTON_NUMBER");

  IupDestroy(dlg);

  return bt;
}

int  iupDataEntry(int    maxlin,
                  int*   maxcol,
                  int*   maxscr,
                  char*  title,
                  char** text,
                  char** data)
{
  int i, bt;
  Ihandle *ok, *cancel, *dlg, *vb, *hb, **txt, **lbl, *button_box, *dlg_box;

  txt = (Ihandle **)calloc(maxlin, sizeof(Ihandle*));
  if (txt == NULL) return -2;
  lbl = (Ihandle **)calloc(maxlin+1, sizeof(Ihandle*));

  vb = IupVbox(NULL);

  for (i=0; i<maxlin; i++)
  {
    txt[i] = IupText(NULL);
    IupSetAttribute(txt[i],"VALUE",data[i]);
    IupSetfAttribute(txt[i],"VISIBLECOLUMNS","%dx", maxscr[i]);
    IupSetfAttribute(txt[i],"NC", "%d", maxcol[i]);
    IupSetAttribute(txt[i],"EXPAND","HORIZONTAL");

    hb = IupHbox(lbl[i] = IupLabel(text[i]), txt[i], NULL);
    IupSetAttribute(hb,"MARGIN","0x0");
    IupSetAttribute(hb,"ALIGNMENT","ACENTER");
    IupAppend(vb, hb);
  }
  lbl[i] = NULL;
  IupInsert(vb, NULL, IupNormalizerv(lbl));

  ok = IupButton(iupStrMessageGet("IUP_OK"), NULL);
  IupSetAttribute(ok, "PADDING", "20x0");
  IupSetCallback(ok, "ACTION", (Icallback)CB_button_OK);

  cancel = IupButton(iupStrMessageGet("IUP_CANCEL"), NULL);
  IupSetAttribute(cancel, "PADDING", "20x0");
  IupSetCallback(cancel, "ACTION", (Icallback)CB_button_CANCEL);

  button_box = IupHbox(
    IupFill(), 
    ok,
    cancel,
    NULL);
  IupSetAttribute(button_box,"MARGIN","0x0");
  IupSetAttribute(button_box, "NORMALIZESIZE", "HORIZONTAL");

  dlg_box = IupVbox(
    IupFrame(vb),
    button_box,
    NULL);
  IupSetAttribute(dlg_box,"MARGIN","10x10");
  IupSetAttribute(dlg_box,"GAP","5");

  dlg = IupDialog(dlg_box);

  IupSetAttribute(dlg,"TITLE",title);
  IupSetAttribute(dlg,"MINBOX","NO");
  IupSetAttribute(dlg,"MAXBOX","NO");
  IupSetAttributeHandle(dlg,"DEFAULTENTER", ok);
  IupSetAttributeHandle(dlg,"DEFAULTESC", cancel);
  IupSetAttribute(dlg,"PARENTDIALOG",IupGetGlobal("PARENTDIALOG"));
  IupSetAttribute(dlg,"ICON", IupGetGlobal("ICON"));

  IupMap(dlg);

  IupSetfAttribute(dlg,"MAXSIZE", "65535x%d", IupGetInt2(dlg, "RASTERSIZE"));
  IupSetAttribute(dlg,"MINSIZE", IupGetAttribute(dlg, "RASTERSIZE"));

  IupPopup(dlg,IUP_CENTERPARENT,IUP_CENTERPARENT);

  for (i=0; i<maxlin; i++)
  {
    iupStrCopyN(data[i], maxcol[i], IupGetAttribute(txt[i], "VALUE"));
  }

  free(lbl);
  free(txt);

  bt = IupGetInt(dlg, "STATUS");
  IupDestroy(dlg);
  return bt;
}

int IupGetFile(char* filename)
{
  Ihandle *dlg = 0;
  int ret;
  char filter[4096] = "*.*";
  static char dir[4096] = "";  /* static will make the dir persist from one call to another if not defined */

  if (!filename) return -1;

  dlg = IupFileDlg();

  iupStrFileNameSplit(filename, dir, filter);

  IupSetAttribute(dlg, "FILTER", filter);
  IupSetAttribute(dlg, "DIRECTORY", dir);
  IupSetAttribute(dlg, "ALLOWNEW", "YES");
  IupSetAttribute(dlg, "NOCHANGEDIR", "YES");
  IupSetAttribute(dlg, "PARENTDIALOG", IupGetGlobal("PARENTDIALOG"));
  IupSetAttribute(dlg, "ICON", IupGetGlobal("ICON"));

  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  ret = IupGetInt(dlg, "STATUS");
  if (ret != -1)
  {
    char* value = IupGetAttribute(dlg, "VALUE");
    if (value) 
    {
      iupStrCopyN(filename, 4096, value);
      iupStrFileNameSplit(filename, dir, NULL);
    }
  }

  IupDestroy(dlg);

  return ret;
}

int IupGetText(const char* title, char* text)
{
  Ihandle *ok, *cancel, *multi_text, *button_box, *dlg_box, *dlg;
  int bt;

  multi_text = IupMultiLine(NULL);
  IupSetAttribute(multi_text,"EXPAND", "YES");
  IupSetAttribute(multi_text,"VALUE", text);
  IupSetAttribute(multi_text,"FONT", "Courier, 12");
  IupSetAttribute(multi_text, "VISIBLELINES", "10");
  IupSetAttribute(multi_text, "VISIBLECOLUMNS", "50");

  ok = IupButton(iupStrMessageGet("IUP_OK"), NULL);
  IupSetAttribute(ok, "PADDING", "20x5");
  IupSetCallback(ok, "ACTION", (Icallback)CB_button_OK);

  cancel  = IupButton(iupStrMessageGet("IUP_CANCEL"), NULL);
  IupSetAttribute(cancel, "PADDING", "20x5");
  IupSetCallback(cancel, "ACTION", (Icallback)CB_button_CANCEL);

  button_box = IupHbox(
    IupFill(),
    ok,
    cancel,
    NULL);
  IupSetAttribute(button_box,"MARGIN","0x0");
  IupSetAttribute(button_box, "NORMALIZESIZE", "HORIZONTAL");

  dlg_box = IupVbox(
    multi_text,
    button_box,
    NULL);

  IupSetAttribute(dlg_box,"MARGIN","10x10");
  IupSetAttribute(dlg_box,"GAP","10");

  dlg = IupDialog (dlg_box);

  IupSetAttribute(dlg,"TITLE", title);
  IupSetAttribute(dlg,"MINBOX","NO");
  IupSetAttribute(dlg,"MAXBOX","NO");
  IupSetAttributeHandle(dlg,"DEFAULTENTER", ok);
  IupSetAttributeHandle(dlg,"DEFAULTESC", cancel);
  IupSetAttribute(dlg,"PARENTDIALOG", IupGetGlobal("PARENTDIALOG"));
  IupSetAttribute(dlg,"ICON", IupGetGlobal("ICON"));

  IupMap(dlg);

  IupSetAttribute(multi_text, "VISIBLELINES", NULL);
  IupSetAttribute(multi_text, "VISIBLECOLUMNS", NULL);

  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  bt = IupGetInt(dlg, "STATUS");
  if (bt==1)
    iupStrCopyN(text, 10240, IupGetAttribute(multi_text, "VALUE"));
  else
    bt = 0; /* return 0 instead of -1 */

  IupDestroy(dlg);
  return bt;
}

int IupGetColor(int x, int y, unsigned char *r, unsigned char *g, unsigned char *b)
{
  int ret;
  Ihandle* dlg = IupColorDlg();

  IupSetAttribute(dlg, "TITLE",  iupStrMessageGet("IUP_GETCOLOR"));
  IupSetfAttribute(dlg, "VALUE", "%d %d %d", *r, *g, *b);
  IupSetAttribute(dlg, "PARENTDIALOG", IupGetGlobal("PARENTDIALOG"));
  IupSetAttribute(dlg, "ICON", IupGetGlobal("ICON"));

  IupPopup(dlg, x, y);

  ret = IupGetInt(dlg, "STATUS");
  if (ret)
    iupStrToRGB(IupGetAttribute(dlg, "VALUE"), r, g, b);

  IupDestroy(dlg);

  return ret;
}

void iupVersionDlg(void)
{
  Ihandle* dlg;

  dlg = IupDialog(IupVbox(IupFrame(IupVbox(
                      IupLabel(IupVersion()),
                      IupLabel(IUP_VERSION_DATE),
                      IupLabel(IUP_COPYRIGHT),
                      NULL)), 
                    IupButton(iupStrMessageGet("IUP_OK"), NULL),
                    NULL));

  IupSetAttribute(dlg,"TITLE","IUP Version");
  IupSetAttribute(dlg,"DIALOGFRAME","YES");
  IupSetAttribute(dlg,"DIALOGHINT","YES");
  IupSetAttribute(dlg,"GAP","10");
  IupSetAttribute(dlg,"MARGIN","10x10");

  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);
  IupDestroy(dlg);
}
