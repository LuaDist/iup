#include <stdlib.h>
#include <stdio.h>

#include <windows.h>
#include <exdisp.h>

#include <iup.h>
#include <iupole.h>


#ifndef BIG_TEST
static Ihandle *IupOleCreateBrowser(void)
{
  Ihandle *browser=IupOleControl("Shell.Explorer.2");
  IupSetAttribute(browser,"DESIGNMODE", "NO");
  return browser;
}

static WCHAR* Char2Wide(const char* str)
{
  if (str)
  {
    int n = strlen(str)+1;
    WCHAR* wstr = (WCHAR*)malloc(n * sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, n);
    return wstr;
  }
  return NULL;
}

static IWebBrowser2 *IupOleGetWebBrowserInterface(Ihandle *browser)
{
  IWebBrowser2 *pweb = (IWebBrowser2*)IupGetAttribute(browser, "IUP_IWebBrowser2");
  if (!pweb)
  {
    IUnknown *punk = (IUnknown*)IupGetAttribute(browser,"IUNKNOWN");
    punk->QueryInterface(IID_IWebBrowser2, (void **) &pweb);
    punk->Release();
    IupSetAttribute(browser, "IUP_IWebBrowser2", (char*)pweb);
  }
  return pweb;
}

static void IupOleNavigate(Ihandle *browser,char *fname)
{
  IWebBrowser2 *pweb = IupOleGetWebBrowserInterface(browser);
  WCHAR* url = Char2Wide(fname);
  pweb->Navigate(url, NULL, NULL, NULL, NULL);
  free(url);
}

static void IupOleClose(Ihandle* browser)
{
  IWebBrowser2 *pweb = IupOleGetWebBrowserInterface(browser);
  pweb->Release();
} 

static int load_cb(Ihandle* ih)
{
  Ihandle* txt = (Ihandle*)IupGetAttribute(ih, "MY_TEXT");
  Ihandle* browser = (Ihandle*)IupGetAttribute(ih, "MY_BROWSER");
  IupOleNavigate(browser, IupGetAttribute(txt, "VALUE"));
  return IUP_DEFAULT;
}

static int close_cb(Ihandle* ih)
{
  Ihandle* browser = (Ihandle*)IupGetAttribute(ih, "MY_BROWSER");
  IupOleClose(browser);
  return IUP_DEFAULT;
}

void OleTest(void)
{
  Ihandle* txt, *bt;
  Ihandle * browser = IupOleCreateBrowser();

  // Creates a dlg containing the OLE control
  Ihandle* dlg = IupDialog(IupVbox(IupHbox(txt = IupText(""), bt = IupButton("Load", NULL), NULL), browser, NULL));
  IupSetAttribute(dlg, "MARGIN", "10x10");
  IupSetAttribute(dlg, "GAP", "10");
  IupSetAttribute(dlg, "TITLE", "IupOle");
  IupSetAttribute(dlg, "MY_TEXT", (char*)txt);
  IupSetAttribute(dlg, "MY_BROWSER", (char*)browser);
  IupSetAttribute(txt, "EXPAND", "HORIZONTAL");
//  IupSetAttribute(txt, "VALUE", "d:/test.doc");
  IupSetCallback(bt, "ACTION", (Icallback)load_cb);
  IupSetCallback(dlg, "CLOSE_CB", (Icallback)close_cb);
  IupSetAttributeHandle(dlg, "DEFAULTENTER", bt);

  // Shows dlg
  IupShow(dlg);
}

int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);
  IupOleControlOpen();

  OleTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
