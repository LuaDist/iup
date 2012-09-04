#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "iupcontrols.h"

static Ihandle *label_red, *label_green, *label_blue, *label_color;

static void clrbrwsr_update_text(unsigned char r, unsigned char g, unsigned char b, char* hsi)
{
  float h, s, i;
  sscanf(hsi, "%f %f %f", &h, &s, &i);
  IupSetfAttribute(label_red, "TITLE", "R:%3d  H:%3.0f", (int)r, (double)h);
  IupSetfAttribute(label_green, "TITLE", "G:%3d  S:%1.2f", (int)g, (double)s);
  IupSetfAttribute(label_blue, "TITLE", "B:%3d  I:%1.2f", (int)b, (double)i);
  IupSetfAttribute(label_color, "BGCOLOR", "%d %d %d", (int)r, (int)g, (int)b);
}

static int clrbrwsr_drag_cb(Ihandle* ih, unsigned char r, unsigned char g, unsigned char b)
{
  (void)ih;
  printf("DRAG_CB(%d, %d, %d)\n", (int)r, (int)g, (int)b);
  clrbrwsr_update_text(r, g, b, IupGetAttribute(ih, "HSI"));
  return IUP_DEFAULT;
}

static int clrbrwsr_change_cb(Ihandle* ih, unsigned char r, unsigned char g, unsigned char b)
{
  (void)ih;
  printf("CHANGE_CB(%d, %d, %d)\n", (int)r, (int)g, (int)b);
  clrbrwsr_update_text(r, g, b, IupGetAttribute(ih, "HSI"));
  return IUP_DEFAULT;
}

void ColorBrowserTest(void)
{
  Ihandle *dlg, *hbox_final, *vbox, *clrbrwsr;
  
  label_red   = IupLabel(NULL);
  IupSetAttribute(label_red, "SIZE", "100x10");
  IupSetAttribute(label_red, "FONT", "Courier, 12");
  label_green = IupLabel(NULL);
  IupSetAttribute(label_green, "SIZE", "100x10");
  IupSetAttribute(label_green, "FONT", "Courier, 12");
  label_blue  = IupLabel(NULL);
  IupSetAttribute(label_blue, "SIZE", "100x10");
  IupSetAttribute(label_blue, "FONT", "Courier, 12");

  label_color  = IupLabel(NULL);
  IupSetAttribute(label_color, "RASTERSIZE", "50x50");

  clrbrwsr = IupColorBrowser();
//  IupSetAttribute(clrbrwsr, "ACTIVE", "NO");
//  IupSetAttribute(clrbrwsr, "BGCOLOR", "128 128 255");
  IupSetAttribute(clrbrwsr, "EXPAND", "YES");
  IupSetAttribute(clrbrwsr, "RGB", "128 0 128");
  clrbrwsr_update_text(128, 0, 128, IupGetAttribute(clrbrwsr, "HSI"));

  IupSetCallback(clrbrwsr, "CHANGE_CB", (Icallback)clrbrwsr_change_cb);
  IupSetCallback(clrbrwsr, "DRAG_CB", (Icallback)clrbrwsr_drag_cb);

  vbox = IupVbox(
           label_red,
           label_green,
           label_blue,
           label_color,
           NULL);

  hbox_final = IupHbox (clrbrwsr, vbox, NULL);
  IupSetAttribute(hbox_final, "MARGIN", "10x10");

  dlg = IupDialog (hbox_final);
  IupSetAttribute(dlg, "TITLE", "IupColorBrowser Test");
  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);
  IupControlsOpen();

  ColorBrowserTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
