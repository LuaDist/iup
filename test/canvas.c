#include <stdlib.h>
#include <stdio.h>
#include "iup.h"
#include "iupkey.h"

static unsigned char matrx_img_cur_excel[15*15] = 
  {
    0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,
    0,0,0,0,1,2,2,2,2,1,1,0,0,0,0,
    0,0,0,0,1,2,2,2,2,1,1,0,0,0,0,
    0,0,0,0,1,2,2,2,2,1,1,0,0,0,0,
    1,1,1,1,1,2,2,2,2,1,1,1,1,1,0,
    1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,
    1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,
    1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,
    1,1,1,1,1,2,2,2,2,1,1,1,1,1,1,
    0,1,1,1,1,2,2,2,2,1,1,1,1,1,1,
    0,0,0,0,1,2,2,2,2,1,1,0,0,0,0,
    0,0,0,0,1,2,2,2,2,1,1,0,0,0,0,
    0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,
    0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  };

static unsigned char pixmap_cursor [ ] =
{
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,1,1,1,2,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,1,1,1,2,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,1,1,1,1,2,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,1,1,1,1,1,2,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,2,1,1,1,1,1,2,2,2,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,2,1,1,1,1,1,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,2,1,1,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
} ;

//#define USE_GDK

/* draw a rectangle that has w=600 always, white background and a red X */
#ifdef USE_GDK
#include <gtk/gtk.h>
static void drawTest(Ihandle *ih, int posx)
{
  GtkWidget* widget = (GtkWidget*)IupGetAttribute(ih, "WID");
  GdkGC* gc = widget->style->fg_gc[GTK_WIDGET_STATE(widget)];
  int w, h;
  GdkColor color;

  IupGetIntInt(ih, "DRAWSIZE", &w, &h);

  /* white background */
  color.red = 65535;  color.green = 65535;  color.blue  = 65535;
  gdk_gc_set_rgb_fg_color(gc, &color);
  gdk_draw_rectangle(widget->window, gc, TRUE, 0, 0, w, h);

  /* red X */
  w = 600; /* virtual size */
  color.red = 65535;  color.green = 0;  color.blue  = 0;
  gdk_gc_set_rgb_fg_color(gc, &color);
  gdk_draw_line(widget->window, gc, -posx, 0, w-posx, h);
  gdk_draw_line(widget->window, gc, -posx, h, w-posx, 0);
}
#else
#ifdef WIN32
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#include <windows.h>
static void drawTest(Ihandle *ih, int posx)
{
  RECT rect;
  int w, h;
  HWND hWnd = (HWND)IupGetAttribute(ih, "HWND");
  HDC hDC = GetDC(hWnd);

  IupGetIntInt(ih, "DRAWSIZE", &w, &h);

  SetRect(&rect, 0, 0, w, h);
  FillRect(hDC, &rect, GetStockObject(WHITE_BRUSH));

  SelectObject(hDC, GetStockObject(DC_PEN));
  SetDCPenColor(hDC, RGB(255, 0, 0));

  w = 600; /* virtual size */
  MoveToEx(hDC, -posx, 0, NULL);
  LineTo(hDC, w-posx, h);
  MoveToEx(hDC, -posx, h, NULL);
  LineTo(hDC, w-posx, 0);

  ReleaseDC(hWnd, hDC);
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

static void drawTest(Ihandle *ih, int posx)
{
  int w, h;
  Display* dpy = (Display*)IupGetAttribute(ih, "XDISPLAY");   /* works for Motif and GTK */
  Drawable wnd = (Drawable)IupGetAttribute(ih, "XWINDOW");
  GC gc = XCreateGC(dpy, wnd, 0, NULL);

  IupGetIntInt(ih, "DRAWSIZE", &w, &h);

  XSetForeground(dpy, gc, xGetPixel(dpy, 255, 255, 255));
  XFillRectangle(dpy, wnd, gc, 0, 0, w, h);

  XSetForeground(dpy, gc, xGetPixel(dpy, 255, 0, 0));

  w = 600; /* virtual size */
  XDrawLine(dpy, wnd, gc, -posx, 0, w-posx, h);
  XDrawLine(dpy, wnd, gc, -posx, h, w-posx, 0);

  XFreeGC(dpy, gc); 
}
#endif
#endif

static int testDropData_cb(Ihandle *self, char* type, void* data, int len, int x, int y)
{
  // Data is a text for this sample, so we can print it
  printf("\n  DROPDATA_CB(type=%s, data=%s, size=%d, x=%d, y=%d)\n\n", type, (char*)data, len, x, y);
  return IUP_DEFAULT;
}

static int map_cb(Ihandle *ih)
{
  printf("MAP_CB()\n");
  return IUP_DEFAULT;
}

static int action(Ihandle *ih, float posx, float posy)
{
  printf("ACTION(posx=%.2f, posy=%.2f)\n", posx, posy);
  drawTest(ih, (int)posx);
  return IUP_DEFAULT;
}

static int scroll_cb(Ihandle *ih, int op, float posx, float posy)
{
  char* op2str[12] = {"SBUP", "SBDN", "SBPGUP", "SBPGDN", "SBPOSV", "SBDRAGV",
                      "SBLEFT", "SBRIGHT", "SBPGLEFT", "SBPGRIGHT", "SBPOSH", "SBDRAGH"};
  printf("SCROLL_CB(%s, posx=%.2f, posy=%.2f)\n", op2str[op], posx, posy);
  IupRedraw(ih, 0);
  return IUP_DEFAULT;
}

static int dropfiles_cb(Ihandle *ih, const char* filename, int num, int x, int y)
{
  printf("DROPFILES_CB(%s, %d, x=%d, y=%d)\n", filename, num, x, y);
  return IUP_DEFAULT;
}

static int resize_cb(Ihandle *ih, int w, int h)
{
  printf("RESIZE_CB(%d, %d) RASTERSIZE=%s CLIENTSIZE=%s\n", w, h, IupGetAttribute(ih, "RASTERSIZE"), IupGetAttribute(ih, "CLIENTSIZE"));

  /* update page size */
  IupSetfAttribute(ih, "DX", "%d", w);

  return IUP_DEFAULT;
}

static int getfocus_cb(Ihandle *ih)
{
  printf("GETFOCUS_CB()\n");
  return IUP_DEFAULT;
}

static int help_cb(Ihandle* ih)
{
  printf("HELP_CB()\n");
  return IUP_DEFAULT;
}
     
static int killfocus_cb(Ihandle *ih)
{
  printf("KILLFOCUS_CB()\n");
  return IUP_DEFAULT;
}

static int focus_cb(Ihandle *ih, int focus)
{
  printf("FOCUS_CB(%d)\n", focus);
  return IUP_DEFAULT;
}

static int leavewindow_cb(Ihandle *ih)
{
  printf("LEAVEWINDOW_CB()\n");
  return IUP_DEFAULT;
}

static int enterwindow_cb(Ihandle *ih)
{
  printf("ENTERWINDOW_CB()\n");
  return IUP_DEFAULT;
}

static int button_cb(Ihandle *ih,int but,int pressed,int x,int y,char* status)
{
  if (but==IUP_BUTTON1 && pressed)
    IupSetAttribute(ih, "CURSOR", "CROSS");
  if (but==IUP_BUTTON1 && !pressed)
    IupSetAttribute(ih, "CURSOR", NULL);
  printf("BUTTON_CB(but=%c (%d), x=%d, y=%d [%s])\n",(char)but,pressed,x,y, status);
  return IUP_DEFAULT;
}

static int wheel_cb(Ihandle *ih,float delta,int x,int y,char* status)
{
  printf("WHEEL_CB(delta=%.2f, x=%d, y=%d [%s])\n",delta,x,y, status);
  return IUP_DEFAULT;
}

static int motion_cb(Ihandle *ih,int x,int y,char* status)
{
  printf("MOTION_CB(x=%d, y=%d [%s])\n",x,y, status);
  return IUP_DEFAULT;
}

char *iupKeyCodeToName(int code);

static int k_any(Ihandle *ih, int c)
{
  //if (c == K_a)
  //  IupSetAttribute(ih, "POSX", "100");

  if (iup_isprint(c))
    printf("K_ANY(%d = %s \'%c\')\n", c, iupKeyCodeToName(c), (char)c);
  else
    printf("K_ANY(%d = %s)\n", c, iupKeyCodeToName(c));

  printf("  MODKEYSTATE(%s)\n", IupGetGlobal("MODKEYSTATE"));
  return IUP_DEFAULT;
}

static int keypress_cb(Ihandle *ih, int c, int pressed)
{
  if (iup_isprint(c))
    printf("KEYPRESS_CB(%d = %s \'%c\' (%d))\n", c, iupKeyCodeToName(c), (char)c, pressed);
  else
    printf("KEYPRESS_CB(%d = %s (%d))\n", c, iupKeyCodeToName(c), pressed);
  return IUP_DEFAULT;
}

void CanvasTest(void)
{
  Ihandle *dlg, *canvas, *box, *image;

  //image = IupImage(32, 32, pixmap_cursor);
  //IupSetAttribute(image, "1", "255 0 0"); 
  //IupSetAttribute(image, "2", "128 0 0"); 
  //IupSetAttribute(image, "HOTSPOT", "21:10");

  image = IupImage(15,15, matrx_img_cur_excel);
  IupSetAttribute (image, "0", "BGCOLOR"); 
  IupSetAttribute (image, "1", "0 0 0"); 
  IupSetAttribute (image, "2", "255 255 255"); 
  IupSetAttribute (image, "HOTSPOT", "7:7");

  box = IupVbox(NULL);
  IupSetAttribute(box, "MARGIN", "5x5");

  canvas = IupCanvas(NULL);
  IupAppend(box, canvas);
  IupSetAttribute(canvas, "RASTERSIZE", "300x200");
  IupSetAttribute(canvas, "TIP", "Canvas Tip");
  IupSetAttribute(canvas, "SCROLLBAR", "YES");
//  IupSetAttribute(canvas, "XAUTOHIDE", "NO");
  IupSetAttribute(canvas, "XMAX", "600");
  IupSetAttribute(canvas, "DX", "300");  /* use a 1x1 scale, this value is updated in RESIZE_CB,
                                           so when the canvas is larger than 600 
                                           it will hide the scrollbar */
  IupSetAttributeHandle(canvas, "CURSOR", image);
  IupSetAttribute(canvas, "DROPTARGET", "YES");
  IupSetAttribute(canvas, "DROPTYPES", "TEXT");
  IupSetCallback (canvas, "DROPDATA_CB", (Icallback)testDropData_cb);

//  IupSetAttribute(canvas, "BORDER", "NO");
//  IupSetAttribute(canvas, "CURSOR", "CROSS");

  IupSetCallback(canvas, "MAP_CB",       (Icallback)map_cb);

//  IupSetCallback(canvas, "KEYPRESS_CB",  (Icallback)keypress_cb);
  IupSetCallback(canvas, "K_ANY",        (Icallback)k_any);
  IupSetCallback(canvas, "HELP_CB",      (Icallback)help_cb);

  IupSetCallback(canvas, "GETFOCUS_CB",  (Icallback)getfocus_cb); 
  IupSetCallback(canvas, "KILLFOCUS_CB", (Icallback)killfocus_cb);
  IupSetCallback(canvas, "FOCUS_CB", (Icallback)focus_cb);
  IupSetCallback(canvas, "ENTERWINDOW_CB", (Icallback)enterwindow_cb);
  IupSetCallback(canvas, "LEAVEWINDOW_CB", (Icallback)leavewindow_cb);

  IupSetCallback(canvas, "BUTTON_CB",    (Icallback)button_cb);
  IupSetCallback(canvas, "MOTION_CB",    (Icallback)motion_cb);
  IupSetCallback(canvas, "SCROLL_CB",  (Icallback)scroll_cb);
//  IupSetCallback(canvas, "WHEEL_CB",    (Icallback)wheel_cb);

  IupSetCallback(canvas, "RESIZE_CB",    (Icallback)resize_cb);
//  IupSetAttribute(canvas, "BGCOLOR", "255 128 128");
  IupSetCallback(canvas, "ACTION",  (Icallback)action);
                   
  // Windows and GTK Only  
  IupSetCallback(canvas, "DROPFILES_CB", (Icallback)dropfiles_cb);

//{
//  Ihandle* cnv = IupCanvas(NULL);
//  IupAppend(box, cnv);
//  IupSetAttribute(cnv, "RASTERSIZE", "300x200");
//  IupSetAttribute(cnv, "CURSOR", "NONE");
//}

  dlg = IupDialog(box);
  IupSetAttribute(dlg, "TITLE", "IupCanvas Test");

printf("IupMap\n");
  IupMap(dlg);
printf("IupShow\n");
  IupShow(dlg);
  IupSetAttribute(canvas, "RASTERSIZE", NULL);  /* release the minimum limitation */
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  CanvasTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
