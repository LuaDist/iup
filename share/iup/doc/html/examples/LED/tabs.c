/*IupTabs Example using LED 
Creates several tabs, each containing diverse contents, including another IupTabs element.

To run this example, the C code that will include the LED code must be compiled. The LED code is in a file called “iuptabs.led”.*/

#include <stdlib.h>

#include "iup.h"
#include "iupcontrols.h"

/*
 * Callback of the exit button
 */

int exit_cb(Ihandle * self)
{
  return IUP_CLOSE;
}


/*
 * Callback for changing tabs in example 4
 */

int tabchange_ignore_cb(Ihandle * self, Ihandle* new_tab, Ihandle* old_tab)
{
  char *texto = IupGetAttribute(IupGetHandle("texto_a_preencher"), "VALUE");

  if(texto != NULL)
    if(strlen(texto) > 0)
      return IUP_DEFAULT;

  IupMessage("Erro", "Primeiro preencha campo,\ndepois avance");

  return IUP_IGNORE;
}


/*
 * Callback for changing tabs in example 3 
 */

int tabchange_cb(Ihandle * self, Ihandle* new_tab, Ihandle* old_tab)
{
  IupSetAttribute(IupGetHandle("tab_atual"),
    "VALUE",IupGetAttribute(new_tab, ICTL_TABTITLE));

  IupSetAttribute(IupGetHandle("tab_anterior"),
    "VALUE",IupGetAttribute(old_tab, ICTL_TABTITLE));
  
  return IUP_DEFAULT;
}


/*
 * Creates a simple Tabs using only C
 */

int tabs_em_c_cb(Ihandle* self)
{
  Ihandle *dlg = NULL, *tabs = NULL, *label = NULL, *button = NULL, *text = NULL,
     *vbox = NULL, *frame = NULL;

  /*
   * Creates a dialog with Tabs using C
   */

  label = IupLabel("Exemplo de Utilização de Tabs\nTab 1");
  IupSetAttribute(label, ICTL_TABTITLE, "Primeiro Tab");

  button = IupButton("Tab 2", NULL);
  IupSetAttribute(button, ICTL_TABTITLE, "Segundo Tab");

  text = IupText("Tab 3");
  IupSetAttribute(text, ICTL_TABTITLE, "Terceiro Tab");

  frame = IupFrame(IupCanvas(NULL));
  vbox = IupVbox(frame, NULL);
  IupSetAttribute(vbox, ICTL_TABTITLE, "Quarto Tab");
  IupSetAttribute(frame, "TITLE", "Frame");

  /* Creates tabs */
  tabs = IupTabs(label, button, text, vbox, NULL);

  dlg = IupDialog
    (
     IupVbox
     (
      IupFill(),
      tabs,
      IupFill(),
      IupHbox
      (
       IupFill(),
       IupButton("Sair", "exit_cb"),
       IupFill(),
       NULL
       ),
      IupFill(),
      NULL
      )
    );

  IupSetAttribute(dlg, "TITLE", "Exemplo 1 de Utilizacao do Tabs");
  
  IupPopup(dlg, IUP_CENTER, IUP_CENTER);
  IupDestroy(dlg);

  return IUP_DEFAULT;
}


void main(int argc, char **argv)
{
  char *error = NULL;
 
  /* Initialization of IUP and its controls */
  IupOpen(&argc, &argv);
  IupControlsOpen() ;

  if ((error = IupLoad("IupTabs.led")) != NULL)
  {
    IupMessage("LED error", error);
    exit(1);
  }

  /*
   * Sets callbacks
   */
  IupSetFunction("exit_cb", (Icallback) exit_cb);
  IupSetFunction("tabs_em_c", (Icallback) tabs_em_c_cb);
  IupSetFunction("tabchange_cb", (Icallback) tabchange_cb);
  IupSetFunction("tabchange_ignore_cb", (Icallback) tabchange_ignore_cb);


  /* 
   * Shows dialog and starts iteration
   */
  IupShow(IupGetHandle("dlg_led"));
  IupMainLoop();

  IupDestroy(IupGetHandle("dlg_led"));
  IupControlsClose() ;
  IupClose();
}