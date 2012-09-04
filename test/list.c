#include <stdlib.h>
#include <stdio.h>
#include <iup.h>
#include <iupkey.h>

static int button_cb(Ihandle *ih,int but,int pressed,int x,int y,char* status)
{
  printf("BUTTON_CB(but=%c (%d), x=%d, y=%d [%s]) - [pos=%d]\n",(char)but,pressed,x,y, status, IupConvertXYToPos(ih, x, y));
  return IUP_DEFAULT;
}

static int motion_cb(Ihandle *ih,int x,int y,char* status)
{
  printf("MOTION_CB(x=%d, y=%d [%s]) - [pos=%d]\n",x,y, status,IupConvertXYToPos(ih, x, y));
  return IUP_DEFAULT;
}

static int multilist_cb (Ihandle *ih, char *s)
{
  printf("MULTISELECT_CB(%s)\n", s);
  return IUP_DEFAULT;
}

static int list_cb (Ihandle *ih, char *t, int i, int v)
{
  printf("ACTION(%s - %d %d)\n", t, i, v);
  return IUP_DEFAULT;
}

static int dropdown_cb(Ihandle *ih, int state)
{
  printf("DROPDOWN_CB(%d)\n", state);
  return IUP_DEFAULT;
}

static int dblclick_cb(Ihandle *ih, int pos, char *text)
{
  printf("DBLCLICK_CB(%d - %s)\n", pos, text);
  return IUP_DEFAULT;
}

static int edit_cb (Ihandle *ih, int c, char *after)
{
  printf("EDIT_CB(%d - %s)\n", c, after);
  return IUP_DEFAULT;
}

static int caret_cb(Ihandle *ih, int lin, int col)
{
  printf("CARET_CB(%d, %d)\n", lin, col);
  return IUP_DEFAULT;
}

static int btok_cb(Ihandle *ih)
{
  printf("Default Enter\n");
  return IUP_DEFAULT;
}

static int btcancel_cb(Ihandle *ih)
{
  printf("Default Esc\n");
  return IUP_DEFAULT;
}

static int selectedtext_cb(Ihandle *ih)
{
  Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
  Ihandle *text = IupGetDialogChild(ih, "text");
  IupSetAttribute(text, "VALUE", IupGetAttribute(list, "SELECTEDTEXT"));
  return IUP_DEFAULT;
}

static int selection_cb(Ihandle *ih)
{
  Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
  Ihandle *text = IupGetDialogChild(ih, "text");
  IupSetAttribute(text, "VALUE", IupGetAttribute(list, "SELECTION"));
  return IUP_DEFAULT;
}

static int getcaret_cb(Ihandle *ih)
{
  Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
  Ihandle *text = IupGetDialogChild(ih, "text");
  IupSetAttribute(text, "VALUE", IupGetAttribute(list, "CARET"));
  return IUP_DEFAULT;
}

static int getvalue_cb(Ihandle *ih)
{
  Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
  Ihandle *text = IupGetDialogChild(ih, "text");
  IupSetAttribute(text, "VALUE", IupGetAttribute(list, "VALUE"));
  return IUP_DEFAULT;
}

static int getcount_cb(Ihandle *ih)
{
  Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
  printf("COUNT=%s\n", IupGetAttribute(list, "COUNT"));
  return IUP_DEFAULT;
}

static int append_cb(Ihandle *ih)
{
  Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
  Ihandle *text = IupGetDialogChild(ih, "text");
  IupSetAttribute(list, "APPEND", IupGetAttribute(text, "VALUE"));
  return IUP_DEFAULT;
}

static int readonly_cb(Ihandle *ih)
{
  Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
  Ihandle *text = IupGetDialogChild(ih, "text");
  IupSetAttribute(list, "READONLY", IupGetAttribute(text, "VALUE"));
  return IUP_DEFAULT;
}

static int setvalue_cb(Ihandle *ih)
{
  Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
  Ihandle *text = IupGetDialogChild(ih, "text");
  IupSetAttribute(list, "VALUE", IupGetAttribute(text, "VALUE"));
  return IUP_DEFAULT;
}

static int insertitem_cb(Ihandle *ih)
{
  Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
  Ihandle *text = IupGetDialogChild(ih, "text");
  IupSetAttribute(list, "INSERTITEM3", IupGetAttribute(text, "VALUE"));
  return IUP_DEFAULT;
}

static int setitem_cb(Ihandle *ih)
{
  Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
  Ihandle *text = IupGetDialogChild(ih, "text");
  IupSetAttribute(list, "3", IupGetAttribute(text, "VALUE"));
  return IUP_DEFAULT;
}

static int appenditem_cb(Ihandle *ih)
{
  Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
  Ihandle *text = IupGetDialogChild(ih, "text");
  IupSetAttribute(list, "APPENDITEM", IupGetAttribute(text, "VALUE"));
  return IUP_DEFAULT;
}

static int removeitem_cb(Ihandle *ih)
{
  Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
  Ihandle *text = IupGetDialogChild(ih, "text");
  IupSetAttribute(list, "REMOVEITEM", IupGetAttribute(text, "VALUE"));
  return IUP_DEFAULT;
}
                   
static int topitem_cb(Ihandle *ih)
{
  Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
  Ihandle *text = IupGetDialogChild(ih, "text");
  IupSetAttribute(list, "TOPITEM", IupGetAttribute(text, "VALUE"));
  return IUP_DEFAULT;
}

static int showdropdown_cb(Ihandle *ih)
{
  Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
  IupSetAttribute(list, "SHOWDROPDOWN", "YES");
  return IUP_DEFAULT;
}

static void setactivelist(Ihandle* ih)
{
  Ihandle* dialog = IupGetDialog(ih);
  Ihandle* label = (Ihandle*)IupGetAttribute(dialog, "_LABEL");
  IupSetAttribute(dialog, "_ACTIVE_LIST", (char*)ih);
  IupSetAttribute(label, "TITLE", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"));
}

static int getfocus_cb(Ihandle *ih)
{
  setactivelist(ih);
  printf("GETFOCUS_CB(%s)\n", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"));
  return IUP_DEFAULT;
}

static int killfocus_cb(Ihandle *ih)
{
  printf("KILLFOCUS_CB(%s)\n", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"));
  return IUP_DEFAULT;
}

static int leavewindow_cb(Ihandle *ih)
{
  printf("LEAVEWINDOW_CB(%s)\n", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"));
  return IUP_DEFAULT;
}

static int enterwindow_cb(Ihandle *ih)
{
  setactivelist(ih);
  printf("ENTERWINDOW_CB(%s)\n", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"));
  return IUP_DEFAULT;
}

char *iupKeyCodeToName(int code);

static int k_any(Ihandle *ih, int c)
{
  if (iup_isprint(c))
    printf("K_ANY(%s, %d = %s \'%c\')\n", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"), c, iupKeyCodeToName(c), (char)c);
  else
    printf("K_ANY(%s, %d = %s)\n", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"), c, iupKeyCodeToName(c));
  return IUP_CONTINUE;
}

static int help_cb(Ihandle* ih)
{
  printf("HELP_CB(%s)\n", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"));
  return IUP_DEFAULT;
}

static void set_callbacks(Ihandle* list)
{
  IupSetCallback(list, "ACTION", (Icallback)list_cb);
  IupSetCallback(list, "DBLCLICK_CB", (Icallback)dblclick_cb);
  IupSetCallback(list, "DROPDOWN_CB", (Icallback)dropdown_cb);
  IupSetCallback(list, "EDIT_CB", (Icallback)edit_cb);
  IupSetCallback(list, "CARET_CB", (Icallback)caret_cb);
//  IupSetCallback(list, "BUTTON_CB",    (Icallback)button_cb);
//  IupSetCallback(list, "MOTION_CB",    (Icallback)motion_cb);

  IupSetCallback(list, "GETFOCUS_CB", getfocus_cb);
  IupSetCallback(list, "KILLFOCUS_CB", killfocus_cb);

  IupSetCallback(list, "ENTERWINDOW_CB", (Icallback)enterwindow_cb);
  IupSetCallback(list, "LEAVEWINDOW_CB", (Icallback)leavewindow_cb);

  IupSetCallback(list, "K_ANY", (Icallback)k_any);
  IupSetCallback(list, "HELP_CB", (Icallback)help_cb);

//  IupSetAttribute(list, "DROPEXPAND", "NO");
//  IupSetAttribute(list, "AUTOHIDE", "NO");
//  IupSetAttribute(list, "SCROLLBAR", "NO");

  IupSetAttribute(list, "VISIBLE_ITEMS", "20");
//  IupSetAttribute(list, "VISIBLECOLUMNS", "7");
  IupSetAttribute(list, "VISIBLELINES", "4");
}

void ListTest(void)
{
  Ihandle *dlg, *list1, *list2, *list3, *list4, *text, *btok, *btcancel,
          *box, *lists, *buttons1, *buttons2, *buttons3, *label;

  list1 = IupList(NULL);
  list2 = IupList(NULL);
  list3 = IupList(NULL);
  list4 = IupList(NULL);                              

  IupSetAttributes(list1, "1=\"US$ 1000\", 2=\"US$ 2000\", 3=\"US$ 300.000.000\", 4=\"US$ 4000\","
                          "EDITBOX=YES, DROPDOWN=YES, TIP=Edit+Drop, VALUE=\"Edit Here\", NAME=list1");
  IupSetAttributes(list2, "1=\"Banana\", 2=\"Apple\", 3=\"Orange\", 4=\"Strawberry\", 5=\"Grape\","
                          "DROPDOWN=YES, NAME=list2, TIP=Drop, XXX_VALUE=2, XXX_SORT=YES, XXX_BGCOLOR=\"192 64 192\"");
  IupSetAttributes(list3, "1=\"Char A\", 2=\"Char B\", 3=\"Char CCCCC\", 4=\"Char D\", 5=\"Char E\", 6=\"Char F\","
                          "EDITBOX=YES, NAME=list3, TIP=Edit+List, VALUE=\"Edit Here\"");
//  IupSetAttributes(list4, "1=\"Number 1\", 2=\"Number 2\", 3=\"Number 3\", 4=\"Number 4\", 5=\"Number 5\", 6=\"Number 6\", 7=\"Number 7\","
//                          "MULTIPLE=YES, NAME=list4, TIP=List, VALUE=+--++--");
  IupSetAttributes(list4, "1=\"Number 3\", 2=\"Number 4\", 3=\"Number 2\", 4=\"Number 1\", 5=\"Number 6\", 6=\"Number 5\", 7=\"Number 7\","
                          "MULTIPLE=YES, NAME=list4, TIP=List, XXX_SORT=YES, VALUE=+--++--, XXX_CANFOCUS=NO");

  //IupSetAttributes(list1, "5=\"US$ 1000\", 6=\"US$ 2000\", 7=\"US$ 300.000.000\", 8=\"US$ 4000\"");
  //IupSetAttributes(list1, "9=\"US$ 1000\", 10=\"US$ 2000\", 11=\"US$ 300.000.000\", 12=\"US$ 4000\"");
  //IupSetAttributes(list1, "13=\"US$ 1000\", 14=\"US$ 2000\", 15=\"US$ 300.000.000\", 16=\"US$ 4000\"");
  //IupSetAttributes(list1, "17=\"US$ 1000\", 18=\"US$ 2000\", 19=\"US$ 300.000.000\", 20=\"US$ 4000\"");
  //IupSetAttributes(list1, "21=\"US$ 1000\", 22=\"US$ 2000\", 23=\"US$ 300.000.000\", 24=\"US$ 4000\"");

  set_callbacks(list1);
  set_callbacks(list2);
  set_callbacks(list3);
  set_callbacks(list4);

  //IupSetAttributes(list2, "6=\"R$ 1000\", 7=\"R$ 2000\", 8=\"R$ 123456789\", 9=\"R$ 4000\", 10=\"R$ 5000\",");
  //IupSetAttributes(list2, "11=\"R$ 1000\", 12=\"R$ 2000\", 13=\"R$ 123456789\", 14=\"R$ 4000\", 15=\"R$ 5000\",");
  //IupSetAttributes(list2, "16=\"R$ 1000\", 17=\"R$ 2000\", 18=\"R$ 123456789\", 19=\"R$ 4000\", 20=\"R$ 5000\",");
  
  IupSetCallback(list4, "MULTISELECT_CB", (Icallback)multilist_cb);

//  IupSetAttribute(list1, "READONLY", "YES");
//  IupSetAttribute(list3, "READONLY", "YES");
//  IupSetAttribute(list1, "PADDING", "10x10");
//  IupSetAttribute(list3, "PADDING", "10x10");

//  IupSetAttribute(list3, "SPACING", "10");
//  IupSetAttribute(list4, "SPACING", "10");

  printf("COUNT(list1)=%s\n", IupGetAttribute(list1, "COUNT"));
  printf("COUNT(list2)=%s\n", IupGetAttribute(list2, "COUNT"));
  printf("COUNT(list3)=%s\n", IupGetAttribute(list3, "COUNT"));
  printf("COUNT(list4)=%s\n", IupGetAttribute(list4, "COUNT"));

  lists = IupHbox(
    IupVbox(
      IupSetAttributes(IupFrame(IupVbox(list1, NULL)), "TITLE=EDITBOX+DROPDOWN"),
      IupSetAttributes(IupFrame(IupVbox(list2, NULL)), "TITLE=DROPDOWN"),
      NULL), 
    IupSetAttributes(IupFrame(IupVbox(list3, NULL)), "TITLE=EDITBOX"),
    IupSetAttributes(IupFrame(IupVbox(list4, NULL)), "TITLE=MULTIPLE"),
    IupVbox(
      btok = IupSetCallbacks(IupButton("Default Enter", NULL), "ACTION", btok_cb, NULL),
      btcancel = IupSetCallbacks(IupButton("Default Esc", NULL), "ACTION", btcancel_cb, NULL),
      NULL),
    NULL);

  text = IupText(NULL);
  IupSetAttribute(text, "EXPAND", "HORIZONTAL");
  IupSetAttribute(text, "NAME", "text");

  buttons1 = IupHbox(
    IupSetCallbacks(IupButton("Get(VALUE)", NULL), "ACTION", getvalue_cb, NULL),
    IupSetCallbacks(IupButton("Set(VALUE)", NULL), "ACTION", setvalue_cb, NULL),
    IupSetCallbacks(IupButton("Get(COUNT)", NULL), "ACTION", getcount_cb, NULL),
    NULL);
  buttons2 = IupHbox(
    IupSetCallbacks(IupButton("3", NULL), "ACTION", setitem_cb, NULL),
    IupSetCallbacks(IupButton("INSERTITEM3", NULL), "ACTION", insertitem_cb, NULL),
    IupSetCallbacks(IupButton("APPENDITEM", NULL), "ACTION", appenditem_cb, NULL),
    IupSetCallbacks(IupButton("REMOVEITEM", NULL), "ACTION", removeitem_cb, NULL),
    IupSetCallbacks(IupButton("SHOWDROPDOWN", NULL), "ACTION", showdropdown_cb, NULL),
    IupSetCallbacks(IupButton("TOPITEM", NULL), "ACTION", topitem_cb, NULL),
    NULL);
  buttons3 = IupHbox(
    IupSetCallbacks(IupButton("APPEND", NULL), "ACTION", append_cb, NULL),
    IupSetCallbacks(IupButton("Get(CARET)", NULL), "ACTION", getcaret_cb, NULL),
    IupSetCallbacks(IupButton("Set(READONLY)", NULL), "ACTION", readonly_cb, NULL),
    IupSetCallbacks(IupButton("Get(SELECTEDTEXT)", NULL), "ACTION", selectedtext_cb, NULL),
    IupSetCallbacks(IupButton("Get(SELECTION)", NULL), "ACTION", selection_cb, NULL),
    NULL);

  box = IupVbox(lists, IupHbox(IupLabel("Attrib. Value:  "), text, NULL), buttons1, buttons2, buttons3, IupHbox(IupLabel("Current List:  "), label = IupLabel(NULL), NULL), NULL);
  IupSetAttributes(buttons1,"MARGIN=5x5, GAP=5");
  IupSetAttributes(buttons2,"MARGIN=5x5, GAP=5");
  IupSetAttributes(buttons3,"MARGIN=5x5, GAP=5");
  IupSetAttributes(lists,"MARGIN=10x10, GAP=10");
  IupSetAttribute(label,"EXPAND", "HORIZONTAL");

  dlg = IupDialog(box);
  IupSetAttribute(dlg, "TITLE", "IupList Test");
  IupSetAttributeHandle(dlg, "DEFAULTENTER", btok);
  IupSetAttributeHandle(dlg, "DEFAULTESC", btcancel);
  IupSetAttribute(dlg, "_LABEL", (char*)label);

//  IupSetAttribute(box, "BGCOLOR", "92 92 255");
//  IupSetAttribute(dlg, "BGCOLOR", "92 92 255");
//  IupSetAttribute(dlg, "BACKGROUND", "200 10 80");
//  IupSetAttribute(dlg, "BGCOLOR", "173 177 194");  // Motif BGCOLOR for documentation

//  IupSetAttribute(dlg, "FONT", "Helvetica, 24");
//  IupSetAttribute(box, "FGCOLOR", "255 0 0");

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  ListTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
