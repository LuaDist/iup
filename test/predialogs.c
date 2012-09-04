#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "iup.h"
#include "iupkey.h"
#include "iupcontrols.h"

//#define USE_GDK

#ifdef USE_GDK
#include <gtk/gtk.h>
static void drawTest(Ihandle *ih)
{
  GtkWidget* widget = (GtkWidget*)IupGetAttribute(ih, "WID");
  GdkGC* gc = widget->style->fg_gc[GTK_WIDGET_STATE(widget)];
  int w = IupGetInt(ih, "PREVIEWWIDTH");
  int h = IupGetInt(ih, "PREVIEWHEIGHT");
  GdkColor color;

  color.red = 65535;  color.green = 0;  color.blue  = 0;
  gdk_gc_set_rgb_fg_color(gc, &color);

  gdk_draw_line(widget->window, gc, 0, 0, w-1, h-1);
  gdk_draw_line(widget->window, gc, 0, h-1, w-1, 0);
}
#else
#ifdef WIN32
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#include <windows.h>
static void drawTest(Ihandle* ih)
{
  RECT rect;
  HDC hDC = (HDC)IupGetAttribute(ih, "PREVIEWDC");
  int w = IupGetInt(ih, "PREVIEWWIDTH");
  int h = IupGetInt(ih, "PREVIEWHEIGHT");

  SetRect(&rect, 0, 0, w, h);
  FillRect(hDC, &rect, GetStockObject(WHITE_BRUSH));

  SelectObject(hDC, GetStockObject(DC_PEN));
  SetDCPenColor(hDC, RGB(255, 0, 0));

  MoveToEx(hDC, 0, 0, NULL);
  LineTo(hDC, w-1, h-1);
  MoveToEx(hDC, 0, h-1, NULL);
  LineTo(hDC, w-1, 0);
}
#else
#include <X11/Xlib.h>

#define xCOLOR8TO16(_x) (_x*257)  /* 65535/255 = 257 */
static unsigned long xGetPixel(Display* dpy, unsigned char cr, unsigned char cg, unsigned char cb)
{
  XColor xc;
  xc.red = xCOLOR8TO16(cr);
  xc.green = xCOLOR8TO16(cg);
  xc.blue = xCOLOR8TO16(cb);
  xc.flags = DoRed | DoGreen | DoBlue;

  XAllocColor(dpy, DefaultColormap(dpy, XDefaultScreen(dpy)), &xc);
  return xc.pixel;
}

static void drawTest(Ihandle* ih)
{
  GC gc = (GC)IupGetAttribute(ih, "PREVIEWDC");
  Display* dpy = (Display*)IupGetAttribute(ih, "XDISPLAY");
  Drawable wnd = (Drawable)IupGetAttribute(ih, "XWINDOW");
  int w = IupGetInt(ih, "PREVIEWWIDTH");
  int h = IupGetInt(ih, "PREVIEWHEIGHT");

  XSetForeground(dpy, gc, xGetPixel(dpy, 255, 255, 255));
  XFillRectangle(dpy, wnd, gc, 0, 0, w, h);

  XSetForeground(dpy, gc, xGetPixel(dpy, 255, 0, 0));

  XDrawLine(dpy, wnd, gc, 0, 0, w-1, h-1);
  XDrawLine(dpy, wnd, gc, 0, h-1, w-1, 0);
}
#endif
#endif

#ifdef USE_OPENGL
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "iupgl.h"

static void drawTestGL(Ihandle* ih)
{
  Ihandle* glcanvas = IupGetAttributeHandle(ih, "PREVIEWGLCANVAS");
  if (glcanvas)
  {
    int w = IupGetInt(ih, "PREVIEWWIDTH");
    int h = IupGetInt(ih, "PREVIEWHEIGHT");

    IupGLMakeCurrent(glcanvas);
    glViewport(0,0,w,h);

    glClearColor(1.0, 0.0, 1.0, 1.f);  /* pink */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(1.0,0.0,0.0);  /* red */
    glBegin(GL_QUADS); 
    glVertex2f(0.9f,0.9f); 
    glVertex2f(0.9f,-0.9f); 
    glVertex2f(-0.9f,-0.9f); 
    glVertex2f(-0.9f,0.9f); 
    glEnd();

    IupGLSwapBuffers(glcanvas);
  }
  else
    drawTest(ih);
}

#endif

static int close_cb(Ihandle *ih)
{
  IupDestroy(ih);
  return IUP_IGNORE;
}

static int help_cb(Ihandle* ih)
{
  (void)ih;
  printf("HELP_CB\n");
  return IUP_DEFAULT; 
}

static int file_cb(Ihandle* ih, const char* filename, const char* status)
{
  (void)ih;
  printf("FILE_CB(%s - %s)\n", status, filename);

  if (strcmp(status, "PAINT")==0)
  {
    printf("  SIZE(%s x %s)\n", IupGetAttribute(ih, "PREVIEWWIDTH"), IupGetAttribute(ih, "PREVIEWHEIGHT"));
#ifdef USE_OPENGL
    drawTestGL(ih);
#else
    drawTest(ih);
#endif
  }
  else if (strcmp(status, "FILTER")==0)
  {
    //IupSetAttribute(ih, "FILE", "test");
    //return IUP_CONTINUE; 
  }

  return IUP_DEFAULT; 
}

static void new_message(char* type, char* buttons)
{
  Ihandle* dlg = IupMessageDlg();

  if (strcmp(type, "ERROR")!=0)
    IupSetAttribute(dlg, "PARENTDIALOG", "_MAIN_DIALOG_TEST_");
  IupSetAttribute(dlg, "DIALOGTYPE", type);
  IupSetAttribute(dlg, "TITLE", "IupMessageDlg Test");
  IupSetAttribute(dlg, "BUTTONS", buttons);
  IupSetAttribute(dlg, "VALUE", "Message Text\nSecond Line");
  if (strcmp(type, "WARNING")==0)
    IupSetAttribute(dlg, "BUTTONDEFAULT", "2");
  if (strcmp(type, "INFORMATION")!=0)
    IupSetCallback(dlg, "HELP_CB", (Icallback)help_cb);

  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  printf("BUTTONRESPONSE(%s)\n", IupGetAttribute(dlg, "BUTTONRESPONSE"));

  IupDestroy(dlg);
}

static void new_color(void)
{
  Ihandle* dlg = IupColorDlg();

  IupSetAttribute(dlg, "PARENTDIALOG", "_MAIN_DIALOG_TEST_");
  IupSetAttribute(dlg, "VALUE", "128 0 255");
  IupSetAttribute(dlg, "ALPHA", "142");
  //IupSetAttribute(dlg, "COLORTABLE", ";;177 29 234;;;0 0 23;253 20 119");
  IupSetAttribute(dlg, "SHOWHEX", "YES");
  IupSetAttribute(dlg, "SHOWCOLORTABLE", "YES");
  //IupSetAttribute(dlg, "SHOWALPHA", "YES");
  IupSetAttribute(dlg, "TITLE", "IupColorDlg Test");
  IupSetCallback(dlg, "HELP_CB", (Icallback)help_cb);

  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (IupGetInt(dlg, "STATUS"))
  {
    printf("OK\n");
    printf("  VALUE(%s)\n", IupGetAttribute(dlg, "VALUE"));
    printf("  COLORTABLE(%s)\n", IupGetAttribute(dlg, "COLORTABLE"));
  }
  else
    printf("CANCEL\n");

  IupDestroy(dlg);
}

static void new_font(void)
{
  Ihandle* dlg = IupFontDlg();

  IupSetAttribute(dlg, "PARENTDIALOG", "_MAIN_DIALOG_TEST_");
  IupSetAttribute(dlg, "COLOR", "128 0 255");
//  IupSetAttribute(dlg, "BGCOLOR", "173 177 194");  // Motif BGCOLOR for documentation
  IupSetAttribute(dlg, "VALUE", "Times, Bold 20");
  IupSetAttribute(dlg, "TITLE", "IupFontDlg Test");
  IupSetCallback(dlg, "HELP_CB", (Icallback)help_cb);

  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

  if (IupGetInt(dlg, "STATUS"))
  {
    printf("OK\n");
    printf("  VALUE(%s)\n", IupGetAttribute(dlg, "VALUE"));
    printf("  COLOR(%s)\n", IupGetAttribute(dlg, "COLOR"));
  }
  else
    printf("CANCEL\n");

  IupDestroy(dlg);
}

static void new_file(char* dialogtype, int preview)
{
  Ihandle *dlg = IupFileDlg(); 

  IupSetAttribute(dlg, "PARENTDIALOG", "_MAIN_DIALOG_TEST_");
  IupSetAttribute(dlg, "DIALOGTYPE", dialogtype);
  IupSetAttribute(dlg, "TITLE", "IupFileDlg Test");
  IupSetAttribute(dlg, "DIRECTORY", "/tecgraf/iup");
  if (strcmp(dialogtype, "DIR")!=0)
  {
//    IupSetAttributes(dlg, "FILTER = \"*.bmp\", FILTERINFO = \"Bitmap Files\"");
//    IupSetAttribute(dlg, "FILTER", "*.jpg;*.jpeg;*.bmp;*.gif;*.tif;*.tiff;*.png");
//    IupSetAttribute(dlg, "EXTFILTER", "Text files|*.txt;*.doc|Image files|*.jpg;*.bmp;*.gif|");
//    IupSetAttribute(dlg, "FILE", "/tecgraf/im/test.bmp");
//    IupSetAttribute(dlg, "FILE", "test.bmp");

//    IupSetAttribute(dlg, "FILTER", "*.txt"); 
//    IupSetAttribute(dlg, "FILTER", "*.bmp;*.jpg"); 
//    IupSetAttribute(dlg, "EXTFILTER", "TEXT|*.txt|");
//    IupSetAttribute(dlg, "EXTFILTER", "BMP FILES|*.bmp|JPEG FILES|*.jpg|");
    IupSetAttribute(dlg, "EXTFILTER", "IMAGEFILES|*.bmp;*.jpg|TEXT|*.txt|");
  }
  IupSetCallback(dlg, "HELP_CB", (Icallback)help_cb);
  IupSetAttribute(dlg, "FILE", "test.bmp");
//  IupSetAttributes(dlg, "FILE = \"\\tecgraf\\iup\\test.bmp\""); // OK
//  IupSetAttributes(dlg, "FILE = \"/tecgraf/iup/test.bmp\""); // OK
//  IupSetAttributes(dlg, "FILE = \"test.bmp\", DIRECTORY = \"/tecgraf/iup\"");   // OK
//  IupSetAttributes(dlg, "FILE = \"test.bmp\", DIRECTORY = \"\\tecgraf\\iup\"");  // OK
//  IupSetAttribute(dlg, "NOCHANGEDIR", "NO");
//  IupSetAttribute(dlg, "MULTIPLEFILES", "YES");
//  IupSetAttribute(dlg, "RASTERSIZE", "800x600");
  IupSetCallback(dlg, "FILE_CB", (Icallback)file_cb);

  if (preview)
  {
    IupSetAttribute(dlg, "SHOWPREVIEW", "YES");

#ifdef USE_OPENGL
    if (preview==2)
    {
      Ihandle* glcanvas = IupGLCanvas(NULL);
      IupSetAttribute(glcanvas, "BUFFER", "DOUBLE");
      IupSetAttributeHandle(dlg, "PREVIEWGLCANVAS", glcanvas);
    }
#endif
  }
  
  IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT); 

  switch(IupGetInt(dlg, "STATUS"))
  {
    case 1: 
      printf("OK\n");
      printf("  New file - VALUE(%s)\n", IupGetAttribute(dlg, "VALUE")); 
      break; 
    case 0 : 
      printf("OK\n");
      printf("  File exists - VALUE(%s)\n", IupGetAttribute(dlg, "VALUE"));
      break; 
    case -1 : 
      printf("CANCEL\n");
      break; 
  }

  IupDestroy(dlg);
}

static void new_alarm(void)
{
  int ret;
  IupSetGlobal("PARENTDIALOG", "_MAIN_DIALOG_TEST_");
  ret = IupAlarm ("IupAlarm Test", "Message Text\nSecond Line", "But 1", "Button 2", "B3");
  IupSetGlobal("PARENTDIALOG", NULL);
  //int ret = IupAlarm ("IupAlarm Test", "Message Text\nSecond Line\nVery long long long long long long long long long long long long text", "But 1", "Button 2", "B3");
  printf("Button(%d)\n", ret);
}

static void new_gettext(void)
{
  int ret;
  char text[1024] = "text first line\nsecond line";
  IupSetGlobal("PARENTDIALOG", "_MAIN_DIALOG_TEST_");
  ret = IupGetText("IupGetText Text", text);
  IupSetGlobal("PARENTDIALOG", NULL);
  if (ret)
  {
    printf("OK\n");
    printf("Text(%s)\n", text);
  }
  else
    printf("CANCEL\n");
}

static void new_getfile(void)
{
  int ret;
  char filename[1024] = "*.*";
  IupSetGlobal("PARENTDIALOG", "_MAIN_DIALOG_TEST_");
  ret = IupGetFile(filename);
  IupSetGlobal("PARENTDIALOG", NULL);
  if (ret!=-1)
  {
    printf("OK\n");
    if (ret == 0)
      printf("File(%s)\n", filename);
    else
      printf("New File(%s)\n", filename);
  }
  else
    printf("CANCEL\n");
}

static void new_list(void)
{
  int ret;   
  int size = 8 ;
  int marks[8] = { 0,0,0,0,1,1,0,0 };
  const char *options[] = {
    "Blue"   ,
    "Red"    ,
    "Green"  ,
    "Yellow" ,
    "Black"  ,
    "White"  ,
    "Gray"   ,
    "Brown"  } ;
    
  IupSetGlobal("PARENTDIALOG", "_MAIN_DIALOG_TEST_");
  ret = IupListDialog(2,"IupListDialog Test",size,options,0,8,5,marks);
  IupSetGlobal("PARENTDIALOG", NULL);

  if (ret == -1)
  {
    printf("CANCEL\n");
  }
  else
  {
    int i;
    char selection[80] = "";
    printf("OK\n");
    
    for(i = 0 ; i < size ; i++)
    {
      if(marks[i])
      {
        char temp[10];        
        sprintf(temp,"%s\n",options[i]);      
        strcat(selection,temp);
      }
    }
    
    printf("  Options (%s)\n", selection);    
  }
}

static int k_any(Ihandle *ih, int c)
{
  switch(c)
  {
  case K_m:
    IupSetGlobal("PARENTDIALOG", "_MAIN_DIALOG_TEST_");
    IupMessage("IupMessage Test", "Message Text\nSecond Line.");
    IupSetGlobal("PARENTDIALOG", NULL);
    break;
  case K_e:
    new_message("ERROR", NULL);
    break;
  case K_i:
    new_message("INFORMATION", NULL);
    break;
  case K_w:
    new_message("WARNING", "OKCANCEL");
    break;
  case K_q:
    new_message("QUESTION", "YESNO");
    break;
  case K_c:
    new_color();
    break;
  case K_f:
    new_font();
    break;
  case K_o:
    new_file("OPEN", 0);
    break;
  case K_O:
    new_file("OPEN", 1);
    break;
  case K_G:
    new_file("OPEN", 2);
    break;
  case K_s:
    new_file("SAVE", 0);
    break;
  case K_d:
    new_file("DIR", 0);
    break;
  case K_a:
    new_alarm();
    break;
  case K_y:
    IupShow(IupLayoutDialog(NULL));
    break;
  case K_g:
    new_getfile();
    break;
  case K_t:
    new_gettext();
    break;
  case K_l:
    new_list();
    break;
  case K_ESC:
    IupDestroy(ih);
    return IUP_IGNORE;
  }
  return IUP_DEFAULT;
}

void PreDialogsTest(void)
{
  char* msg = "Press a key for a pre-defined dialog:\n"
              "e = IupMessageDlg(ERROR)\n"
              "i = IupMessageDlg(INFORMATION)\n"
              "w = IupMessageDlg(WARNING)\n"
              "q = IupMessageDlg(QUESTION)\n"
              "--------------------\n"
              "o = IupFileDlg(OPEN)\n"
              "O = IupFileDlg(OPEN+PREVIEW)\n"
              "G = IupFileDlg(OPEN+PREVIEW+OPENGL)\n"
              "s = IupFileDlg(SAVE)\n"
              "d = IupFileDlg(DIR)\n"
              "--------------------\n"
              "c = IupColorDlg\n"
              "f = IupFontDlg\n"
              "--------------------\n"
              "m = IupMessage\n"
              "a = IupAlarm\n"
              "t = IupGetText\n"
              "g = IupGetFile\n"
              "l = IupListDialog\n"
              "y = IupLayoutDialog\n"
              "--------------------\n"
              "Esc = quit";
  Ihandle *dlg = IupDialog(IupVbox(IupLabel(msg), NULL));

#ifdef USE_OPENGL
  IupGLCanvasOpen();
#endif

  IupSetHandle("_MAIN_DIALOG_TEST_", dlg);

  IupSetAttribute(dlg, "TITLE", "Pre-defined Dialogs Test");
  IupSetAttribute(dlg, "MARGIN", "10x10");

  IupSetCallback(dlg, "K_ANY",    (Icallback)k_any);
  IupSetCallback(dlg, "CLOSE_CB", (Icallback)close_cb);

  IupShow(dlg);
}


#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);
  IupControlsOpen();

#ifdef USE_OPENGL
  IupGLCanvasOpen();
#endif

  PreDialogsTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
