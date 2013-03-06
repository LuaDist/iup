/*
 * IupOleControl sample using C++
 */

#include <stdlib.h>
#include <stdio.h>

#include <windows.h>
#include <exdisp.h>

#include "iup.h"
#include "iupole.h"


// Needed to use the WebBrowser OLE control if not using the "expdisp.h" header
//#import "progid:Shell.Explorer.2" no_namespace named_guids

static WCHAR* Char2Wide(char* str)
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

static int load_cb(Ihandle* self)
{
  Ihandle* txt = (Ihandle*)IupGetAttribute(self, "MY_TEXT");
  IWebBrowser2 *pweb = (IWebBrowser2*)IupGetAttribute(self, "MY_WEB");
  WCHAR* url = Char2Wide(IupGetAttribute(txt, "VALUE"));

  // Uses the navigate method of the control
  pweb->Navigate(url, NULL, NULL, NULL, NULL);

  free(url);
  return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Ihandle* txt, *bt;
  IupOpen(&argc, &argv);
  IupOleControlOpen();

  // Creates an instance of the WebBrowser control
  Ihandle* control = IupOleControl("Shell.Explorer.2");

  // Sets production mode
  IupSetAttribute(control, "DESIGNMODE", "NO");

  // Creates a dialog containing the OLE control
  Ihandle* dlg = IupDialog(IupVbox(IupHbox(txt = IupText(""), bt = IupButton("Load", NULL), NULL), control, NULL));
  IupSetAttribute(dlg, "TITLE", "IupOle");
  IupSetAttribute(dlg, "MY_TEXT", (char*)txt);
  IupSetAttribute(txt, "EXPAND", "HORIZONTAL");
  IupSetCallback(bt, "ACTION", (Icallback)load_cb);

  // Maps the dialog to force the creation of the control
  IupMap(dlg);

  // Gathers the IUnknown pointer to access the control's interface

  IUnknown* punk = (IUnknown*) IupGetAttribute(control, "IUNKNOWN");
  IWebBrowser2 *pweb = NULL;
  punk->QueryInterface(IID_IWebBrowser2, (void **)&pweb);
  punk->Release();
  IupSetAttribute(dlg, "MY_WEB", (char*)pweb);

  // Shows dialog
  IupShow(dlg);

  IupMainLoop();

  // Releases the control interface
  pweb->Release();

  IupClose();

  return EXIT_SUCCESS;

}
