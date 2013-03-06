/*
 * IupWebBrowser sample
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "iup.h"
#include "iupweb.h"


#ifndef WIN32
static int history_cb(Ihandle* ih)
{
  int i;
  char str[50];
  int back = IupGetInt(ih, "BACKCOUNT");
  int fwrd = IupGetInt(ih, "FORWARDCOUNT");

  printf("HISTORY ITEMS\n");
  for(i = -(back); i < 0; i++)
  {
    sprintf(str, "ITEMHISTORY%d", i);
    printf("Backward %02d: %s\n", i, IupGetAttribute(ih, str));
  }

  sprintf(str, "ITEMHISTORY%d", 0);
  printf("Current  %02d: %s\n", 0, IupGetAttribute(ih, str));

  for(i = 1; i <= fwrd; i++)
  {
    sprintf(str, "ITEMHISTORY%d", i);
    printf("Forward  %02d: %s\n", i, IupGetAttribute(ih, str));
  }

  return IUP_DEFAULT;
}
#endif

static int navigate_cb(Ihandle* self, char* url)
{
  printf("NAVIGATE_CB: %s\n", url);
  (void)self;
  if (strstr(url, "download")!=NULL)
    return IUP_IGNORE;
  return IUP_DEFAULT;
}
                   
static int error_cb(Ihandle* self, char* url)
{
  printf("ERROR_CB: %s\n", url);
  (void)self;
  return IUP_DEFAULT;
}

static int completed_cb(Ihandle* self, char* url)
{
  printf("COMPLETED_CB: %s\n", url);
  (void)self;
  return IUP_DEFAULT;
}

static int newwindow_cb(Ihandle* self, char* url)
{
  printf("NEWWINDOW_CB: %s\n", url);
  IupSetAttribute(self, "VALUE", url);
  return IUP_DEFAULT;
}

static int back_cb(Ihandle* self)
{
  Ihandle* web  = (Ihandle*)IupGetAttribute(self, "MY_WEB");
  IupSetAttribute(web, "BACKFORWARD", "-1");
  return IUP_DEFAULT;
}

static int forward_cb(Ihandle* self)
{
  Ihandle* web  = (Ihandle*)IupGetAttribute(self, "MY_WEB");
  IupSetAttribute(web, "BACKFORWARD", "1");
  return IUP_DEFAULT;
}

static int stop_cb(Ihandle* self)
{
  Ihandle* web  = (Ihandle*)IupGetAttribute(self, "MY_WEB");
  IupSetAttribute(web, "STOP", NULL);
  return IUP_DEFAULT;
}

static int reload_cb(Ihandle* self)
{
  Ihandle* web  = (Ihandle*)IupGetAttribute(self, "MY_WEB");
  IupSetAttribute(web, "RELOAD", NULL);

  //TEST:
//  printf("STATUS=%s\n", IupGetAttribute(web, "STATUS"));
  return IUP_DEFAULT;
}

static int load_cb(Ihandle* self)
{
  Ihandle* txt  = (Ihandle*)IupGetAttribute(self, "MY_TEXT");
  Ihandle* web  = (Ihandle*)IupGetAttribute(self, "MY_WEB");
  IupSetAttribute(web, "VALUE", IupGetAttribute(txt, "VALUE"));

  //TESTS:
//  IupSetAttribute(txt, "VALUE", IupGetAttribute(web, "VALUE"));
//  IupSetAttribute(web, "HTML", "<html><body><b>Hello</b>, World!</body></html>");
//  IupSetAttribute(web, "VALUE", "http://www.microsoft.com");

  return IUP_DEFAULT;
}

void WebBrowserTest(void)
{
  Ihandle *txt, *dlg, *web;
  Ihandle *btLoad, *btReload, *btBack, *btForward, *btStop;
#ifndef WIN32
  Ihandle *history;
#endif

  IupWebBrowserOpen();              

  // Creates an instance of the WebBrowser control
  web = IupWebBrowser();

  // Creates a dialog containing the control
  dlg = IupDialog(IupVbox(IupHbox(btBack = IupButton("Back", NULL),
                                  btForward = IupButton("Forward", NULL),
                                  txt = IupText(""),
                                  btLoad = IupButton("Load", NULL),
                                  btReload = IupButton("Reload", NULL),
                                  btStop = IupButton("Stop", NULL),
#ifndef WIN32
                                  history = IupButton("History", NULL),
#endif
                                  NULL), 
                                  web, NULL));
  IupSetAttribute(dlg, "TITLE", "IupWebBrowser");
  IupSetAttribute(dlg, "MY_TEXT", (char*)txt);
  IupSetAttribute(dlg, "MY_WEB", (char*)web);
  IupSetAttribute(dlg, "RASTERSIZE", "800x600");
  IupSetAttribute(dlg, "MARGIN", "10x10");
  IupSetAttribute(dlg, "GAP", "10");

//   IupSetAttribute(web, "HTML", "<html><body><b>Hello</b>World!</body></html>");
//   IupSetAttribute(txt, "VALUE", "My HTML");
  IupSetAttribute(txt, "VALUE", "http://www.tecgraf.puc-rio.br/iup");
//  IupSetAttribute(txt, "VALUE", "file:///D:/tecgraf/iup/html/index.html");
  IupSetAttribute(web, "VALUE", IupGetAttribute(txt, "VALUE"));
  IupSetAttributeHandle(dlg, "DEFAULTENTER", btLoad);

  IupSetAttribute(txt, "EXPAND", "HORIZONTAL");
  IupSetCallback(btLoad, "ACTION", (Icallback)load_cb);
  IupSetCallback(btReload, "ACTION", (Icallback)reload_cb);
  IupSetCallback(btBack, "ACTION", (Icallback)back_cb);
  IupSetCallback(btForward, "ACTION", (Icallback)forward_cb);
  IupSetCallback(btStop, "ACTION", (Icallback)stop_cb);
#ifndef WIN32
  IupSetCallback(history, "ACTION", (Icallback)history_cb);
#endif

  IupSetCallback(web, "NEWWINDOW_CB", (Icallback)newwindow_cb);
  IupSetCallback(web, "NAVIGATE_CB", (Icallback)navigate_cb);
  IupSetCallback(web, "ERROR_CB", (Icallback)error_cb);
  IupSetCallback(web, "COMPLETED_CB", (Icallback)completed_cb);

  // Shows dialog
  IupShow(dlg);
}

int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  WebBrowserTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
