#include <stdlib.h>
#include <stdio.h>
#include <iup.h>
#include <iupkey.h>

static void text2multiline (Ihandle* ih, char* attribute)
{
  Ihandle *mltline = IupGetDialogChild(ih, "mltline");
  Ihandle *text = IupGetDialogChild(ih, "text");
  IupSetAttribute (mltline, attribute, IupGetAttribute (text, "VALUE"));
}

static void multiline2text (Ihandle* ih, char* attribute)
{
  Ihandle *mltline = IupGetDialogChild(ih, "mltline");
  Ihandle *text = IupGetDialogChild(ih, "text");
  IupSetAttribute (text, "VALUE", IupGetAttribute(mltline, attribute));
}

static int btn_append_cb (Ihandle* ih)
{
  text2multiline(ih, "APPEND"); 
  return IUP_DEFAULT;
}

static int btn_insert_cb (Ihandle* ih)
{
  text2multiline(ih, "INSERT"); 
  return IUP_DEFAULT;
}

static int btn_clip_cb (Ihandle* ih)
{
  text2multiline(ih, "CLIPBOARD"); 
  return IUP_DEFAULT;
}

int iupKeyNameToCode(const char *name);

static int btn_key_cb (Ihandle* ih)
{
  Ihandle *mltline = IupGetDialogChild(ih, "mltline");
  Ihandle *text = IupGetDialogChild(ih, "text");
  IupSetFocus(mltline);
  IupFlush();
  IupSetfAttribute(NULL, "KEY", "%d", iupKeyNameToCode(IupGetAttribute(text, "VALUE")));
  return IUP_DEFAULT;
}

static int btn_caret_cb (Ihandle* ih)
{
  Ihandle *opt = IupGetHandle("text2multi");
  if (IupGetInt(opt, "VALUE")) 
    text2multiline(ih, "CARET"); 
  else 
    multiline2text(ih, "CARET");
  return IUP_DEFAULT;
}

static int btn_readonly_cb (Ihandle* ih)
{
  Ihandle *opt = IupGetHandle("text2multi");
  if (IupGetInt(opt, "VALUE")) 
    text2multiline(ih, "READONLY"); 
  else 
    multiline2text(ih, "READONLY");
  return IUP_DEFAULT;
}

static int btn_selection_cb (Ihandle* ih)
{
  Ihandle *opt = IupGetHandle ("text2multi");
  if (IupGetInt (opt, "VALUE")) 
    text2multiline (ih, "SELECTION"); 
  else 
    multiline2text (ih, "SELECTION");
  return IUP_DEFAULT;
}

static int btn_selectedtext_cb (Ihandle* ih)
{
  Ihandle *opt = IupGetHandle ("text2multi");
  if (IupGetInt (opt, "VALUE")) 
    text2multiline (ih, "SELECTEDTEXT"); 
  else 
    multiline2text (ih, "SELECTEDTEXT");
  return IUP_DEFAULT;
}

static int btn_overwrite_cb (Ihandle* ih)
{
  Ihandle *opt = IupGetHandle ("text2multi");
  if (IupGetInt (opt, "VALUE")) 
    text2multiline (ih, "OVERWRITE"); 
  else 
    multiline2text (ih, "OVERWRITE");
  return IUP_DEFAULT;
}

static int btn_active_cb(Ihandle *ih)
{
  Ihandle *opt = IupGetHandle ("text2multi");
  if (IupGetInt (opt, "VALUE")) 
    text2multiline (ih, "ACTIVE"); 
  else 
    multiline2text (ih, "ACTIVE");
  return IUP_DEFAULT;
}

static int btn_remformat_cb(Ihandle *ih)
{
  text2multiline (ih, "REMOVEFORMATTING"); 
  return IUP_DEFAULT;
}

static int btn_nc_cb (Ihandle* ih)
{
  Ihandle *opt = IupGetHandle ("text2multi");
  if (IupGetInt (opt, "VALUE")) 
    text2multiline (ih, "NC"); 
  else 
    multiline2text (ih, "NC");
  return IUP_DEFAULT;
}

static int btn_value_cb (Ihandle* ih)
{
  Ihandle *opt = IupGetHandle ("text2multi");
  if (IupGetInt (opt, "VALUE")) 
    text2multiline (ih, "VALUE"); 
  else 
    multiline2text (ih, "VALUE");
  return IUP_DEFAULT;
}

static int btn_tabsize_cb (Ihandle* ih)
{
  Ihandle *opt = IupGetHandle ("text2multi");
  if (IupGetInt (opt, "VALUE")) 
    text2multiline (ih, "TABSIZE"); 
  else 
    multiline2text (ih, "TABSIZE");
  return IUP_DEFAULT;
}

static int k_f2(void)
{
  printf("K_F2\n");
  return IUP_DEFAULT;
}

static int file_open(void)
{
  char filename[100] = "";
  IupGetFile(filename);  // test key after dlg in multiline
  printf(filename);
  return IUP_DEFAULT;
}

char *iupKeyCodeToName(int code);

static int k_any(Ihandle *ih, int c)
{
  if (iup_isprint(c))
    printf("K_ANY(%d = %s \'%c\')\n", c, iupKeyCodeToName(c), (char)c);
  else
    printf("K_ANY(%d = %s)\n", c, iupKeyCodeToName(c));
  printf("  CARET(%s)\n", IupGetAttribute(ih, "CARET"));
  if (c == K_cA)
    return IUP_IGNORE;   // Sound a beep in Windows
  if (c == K_cP)
  {
    file_open();
    return IUP_IGNORE;   // Sound a beep in Windows
  }
  return IUP_CONTINUE;
}

static int action(Ihandle *ih, int c, char* after)
{
  if (iup_isprint(c))
    printf("ACTION(%d = %s \'%c\', %s)\n", c, iupKeyCodeToName(c), (char)c, after);
  else
    printf("ACTION(%d = %s, %s)\n", c, iupKeyCodeToName(c), after);
  if (c == K_i)
    return IUP_IGNORE;   // OK
  if (c == K_cD)
    return IUP_IGNORE;   // Sound a beep in Windows
  if (c == K_h)
    return K_j;
  return IUP_DEFAULT;
}

static int caret_cb(Ihandle *ih, int lin, int col, int pos)
{
  printf("CARET_CB(%d, %d - %d)\n", lin, col, pos);
  printf("  CARET(%s - %s)\n", IupGetAttribute(ih, "CARET"), IupGetAttribute(ih, "CARETPOS"));
  return IUP_DEFAULT;
}

static int getfocus_cb(void)
{
  printf("GETFOCUS_CB()\n");
  return IUP_DEFAULT;
}

static int help_cb(void)
{
  printf("HELP_CB()\n");
  return IUP_DEFAULT;
}
     
static int killfocus_cb(void)
{
  printf("KILLFOCUS_CB()\n");
  return IUP_DEFAULT;
}

static int leavewindow_cb(void)
{
  printf("LEAVEWINDOW_CB()\n");
  return IUP_DEFAULT;
}

static int enterwindow_cb(void)
{
  printf("ENTERWINDOW_CB()\n");
  return IUP_DEFAULT;
}

static int btn_def_esc_cb(void)
{
  printf("DEFAULTESC\n");
  return IUP_DEFAULT;
}

static int btn_def_enter_cb(void)
{
  printf("DEFAULTENTER\n");
  return IUP_DEFAULT;
}

static int dropfiles_cb(Ihandle *ih, const char* filename, int num, int x, int y)
{
  printf("DROPFILES_CB(%s, %d, x=%d, y=%d)\n", filename, num, x, y);
  return IUP_DEFAULT;
}

static int button_cb(Ihandle *ih,int but,int pressed,int x,int y,char* status)
{
  int lin, col, pos;
  printf("BUTTON_CB(but=%c (%d), x=%d, y=%d [%s])\n",(char)but,pressed,x,y, status);
  pos = IupConvertXYToPos(ih, x, y);
  IupTextConvertPosToLinCol(ih, pos, &lin, &col);
  printf("         (lin=%d, col=%d, pos=%d)\n", lin, col, pos);
  return IUP_DEFAULT;
}

static int motion_cb(Ihandle *ih,int x,int y,char* status)
{
  int lin, col, pos;
  printf("MOTION_CB(x=%d, y=%d [%s])\n",x,y, status);
  pos = IupConvertXYToPos(ih, x, y);
  IupTextConvertPosToLinCol(ih, pos, &lin, &col);
  printf("         (lin=%d, col=%d, pos=%d)\n", lin, col, pos);
  return IUP_DEFAULT;
}

void TextTest(void)
{
  int formatting = 0;
  Ihandle *dlg, *mltline, *text, *opt, *btn_def_enter, *btn_def_esc, *btn_active, *btn_overwrite,
          *btn_append, *btn_insert, *btn_caret, *btn_clip, *btn_key, *btn_readonly, *btn_tabsize,
          *btn_selection, *btn_selectedtext, *btn_nc, *btn_value, *lbl, *formattag, *btn_remformat;

//  IupSetGlobal("UTF8AUTOCONVERT", "NO");

  text = IupText (NULL);
  IupSetAttribute(text, "EXPAND", "HORIZONTAL");
//  IupSetAttribute(text, "VALUE", "Single Line Text");
  IupSetAttribute(text, "CUEBANNER", "Enter Attribute Value Here");
  IupSetAttribute(text, "NAME", "text");
  IupSetAttribute(text, "TIP", "Attribute Value");

  opt = IupToggle("Set/Get", NULL);
  IupSetAttribute (opt, "VALUE", "ON");
  IupSetHandle ("text2multi", opt);

  mltline = IupMultiLine(NULL);  
//  mltline = IupText(NULL);  
//  IupSetAttribute(mltline, "MULTILINE", "YES");
  IupSetAttribute(mltline, "NAME", "mltline");

  IupSetCallback(mltline, "DROPFILES_CB", (Icallback)dropfiles_cb);
  IupSetCallback(mltline, "BUTTON_CB",    (Icallback)button_cb);
//  IupSetCallback(mltline, "MOTION_CB",    (Icallback)motion_cb);
  IupSetCallback(mltline, "HELP_CB",      (Icallback)help_cb);
  IupSetCallback(mltline, "GETFOCUS_CB",  (Icallback)getfocus_cb); 
  IupSetCallback(mltline, "KILLFOCUS_CB", (Icallback)killfocus_cb);
  IupSetCallback(mltline, "ENTERWINDOW_CB", (Icallback)enterwindow_cb);
  IupSetCallback(mltline, "LEAVEWINDOW_CB", (Icallback)leavewindow_cb);
  //IupSetCallback(mltline, "ACTION", (Icallback)action);
  IupSetCallback(mltline, "K_ANY", (Icallback)k_any);
  IupSetCallback(mltline, "K_F2", (Icallback)k_f2);
  IupSetCallback(mltline, "CARET_CB", (Icallback)caret_cb);
//  IupSetAttribute(mltline, "WORDWRAP", "YES");
//  IupSetAttribute(mltline, "BORDER", "NO");
//  IupSetAttribute(mltline, "AUTOHIDE", "YES");
//  IupSetAttribute(mltline, "BGCOLOR", "255 0 128");
//  IupSetAttribute(mltline, "FGCOLOR", "0 128 192");
//  IupSetAttribute(mltline, "PADDING", "15x15");
//  IupSetAttribute(mltline, "VALUE", "First Line\nSecond Line Big Big Big\nThird Line\nmore\nmore\nΓ§Γ£ΓµΓ΅Γ³Γ©"); // UTF-8
  IupSetAttribute(mltline, "VALUE", "First Line\nSecond Line Big Big Big\nThird Line\nmore\nmore\nηγυασι"); // Windows-1252
  IupSetAttribute(mltline, "TIP", "First Line\nSecond Line\nThird Line");
//  IupSetAttribute(mltline, "FONT", "Helvetica, 14");
//  IupSetAttribute(mltline, "MASK", IUP_MASK_FLOAT);
//  IupSetAttribute(mltline, "FILTER", "UPPERCASE");
//  IupSetAttribute(mltline, "ALIGNMENT", "ACENTER");
//  IupSetAttribute(mltline, "CANFOCUS", "NO");

  /* Turns on multiline expand and text horizontal expand */
  IupSetAttribute(mltline, "SIZE", "80x40");
  IupSetAttribute(mltline, "EXPAND", "YES");

//  IupSetAttribute(mltline, "FONT", "Courier, 16");
//  IupSetAttribute(mltline, "FONT", "Arial, 12");
//    IupSetAttribute(mltline, "FORMATTING", "YES");

  formatting = 0;
  if (formatting)          /* just to make easier to comment this section */
  {
    /* formatting before Map */
    IupSetAttribute(mltline, "FORMATTING", "YES");

    formattag = IupUser();
    IupSetAttribute(formattag, "ALIGNMENT", "CENTER");
    IupSetAttribute(formattag, "SPACEAFTER", "10");
    IupSetAttribute(formattag, "FONTSIZE", "24");
    IupSetAttribute(formattag, "SELECTION", "3,1:3,50");
    IupSetAttribute(mltline, "ADDFORMATTAG_HANDLE", (char*)formattag);

    formattag = IupUser();
    IupSetAttribute(formattag, "BGCOLOR", "255 128 64");
    IupSetAttribute(formattag, "UNDERLINE", "SINGLE");
    IupSetAttribute(formattag, "WEIGHT", "BOLD");
    IupSetAttribute(formattag, "SELECTION", "3,7:3,11");
    IupSetAttribute(mltline, "ADDFORMATTAG_HANDLE", (char*)formattag);
  }

  /* Creates buttons */
//  btn_append = IupButton ("APPEND ηγυασι", NULL);   // Windows-1252
//  btn_append = IupButton ("APPEND Γ§Γ£ΓµΓ΅Γ³Γ©", NULL);  // UTF-8
  btn_append = IupButton ("&APPEND", NULL);
  btn_insert = IupButton ("INSERT", NULL);
  btn_caret = IupButton ("CARET", NULL);
  btn_readonly = IupButton ("READONLY", NULL);
  btn_selection = IupButton ("SELECTION", NULL);
  btn_selectedtext = IupButton ("SELECTEDTEXT", NULL);
  btn_nc = IupButton ("NC", NULL);
  btn_value = IupButton ("VALUE", NULL);
  btn_tabsize = IupButton ("TABSIZE", NULL);
  btn_clip = IupButton ("CLIPBOARD", NULL);
  btn_key = IupButton ("KEY", NULL);
  btn_def_enter = IupButton ("Default Enter", NULL);
  btn_def_esc = IupButton ("Default Esc", NULL);
  btn_active = IupButton("ACTIVE", NULL);
  btn_remformat = IupButton("REMOVEFORMATTING", NULL);
  btn_overwrite = IupButton("OVERWRITE", NULL);

  IupSetAttribute(btn_append, "TIP", "First Line\nSecond Line\nThird Line");

  /* Registers callbacks */
  IupSetCallback(btn_append, "ACTION", (Icallback) btn_append_cb);
  IupSetCallback(btn_insert, "ACTION", (Icallback) btn_insert_cb);
  IupSetCallback(btn_caret, "ACTION", (Icallback) btn_caret_cb);
  IupSetCallback(btn_readonly, "ACTION", (Icallback) btn_readonly_cb);
  IupSetCallback(btn_selection, "ACTION", (Icallback) btn_selection_cb);
  IupSetCallback(btn_selectedtext, "ACTION", (Icallback) btn_selectedtext_cb);
  IupSetCallback(btn_nc, "ACTION", (Icallback) btn_nc_cb);
  IupSetCallback(btn_value, "ACTION", (Icallback) btn_value_cb);
  IupSetCallback(btn_tabsize, "ACTION", (Icallback) btn_tabsize_cb);
  IupSetCallback(btn_clip, "ACTION", (Icallback) btn_clip_cb);
  IupSetCallback(btn_key, "ACTION", (Icallback) btn_key_cb);
  IupSetCallback(btn_def_enter, "ACTION", (Icallback) btn_def_enter_cb);
  IupSetCallback(btn_def_esc, "ACTION", (Icallback) btn_def_esc_cb);
  IupSetCallback(btn_active, "ACTION", (Icallback) btn_active_cb);
  IupSetCallback(btn_remformat, "ACTION", (Icallback) btn_remformat_cb);
  IupSetCallback(btn_overwrite, "ACTION", (Icallback) btn_overwrite_cb);

  lbl = IupLabel("&Multiline:");
  IupSetAttribute(lbl, "PADDING", "2x2");

  /* Creates dlg */
  dlg = IupDialog(IupVbox(lbl,
                          mltline, 
                          IupHbox (text, opt, NULL),
                          IupHbox (btn_append, btn_insert, btn_caret, btn_readonly, btn_selection, NULL),
                          IupHbox (btn_selectedtext, btn_nc, btn_value, btn_tabsize, btn_clip, btn_key, NULL), 
                          IupHbox (btn_def_enter, btn_def_esc, btn_active, btn_remformat, btn_overwrite, NULL), 
                          NULL));
  IupSetCallback(dlg, "K_cO", (Icallback)file_open);
  IupSetAttribute(dlg, "TITLE", "IupText Test");
  IupSetAttribute(dlg, "MARGIN", "10x10");
  IupSetAttribute(dlg, "GAP", "5");
  IupSetAttributeHandle(dlg, "DEFAULTENTER", btn_def_enter);
  IupSetAttributeHandle(dlg, "DEFAULTESC", btn_def_esc);
  IupSetAttribute(dlg, "SHRINK", "YES");

  if (formatting)          /* just to make easier to comment this section */
  {
    IupMap(dlg);
    /* formatting after Map */

    formattag = IupUser();
    IupSetAttribute(formattag, "ITALIC", "YES");
    IupSetAttribute(formattag, "STRIKEOUT", "YES");
    IupSetAttribute(formattag, "SELECTION", "2,1:2,12");
    IupSetAttribute(mltline, "ADDFORMATTAG_HANDLE", (char*)formattag);
  }

  /* Shows dlg in the center of the screen */
  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
  IupSetFocus(mltline);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  TextTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
