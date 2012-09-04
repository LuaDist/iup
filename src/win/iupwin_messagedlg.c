/** \file
 * \brief Windows IupMessageDlg pre-defined dialog
 *
 * See Copyright Notice in "iup.h"
 */

#include <windows.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_dialog.h"


static void winMessageDlgHelpCallback(HELPINFO* HelpInfo)
{
  Ihandle* ih = (Ihandle*)HelpInfo->dwContextId;
  Icallback cb = (Icallback)IupGetCallback(ih, "HELP_CB");
  if (cb && cb(ih) == IUP_CLOSE)
  {
    if (iupStrEqualNoCase(iupAttribGetStr(ih, "BUTTONS"), "OK")) /* only one button */
      EndDialog((HWND)HelpInfo->hItemHandle, IDOK);
    else
      EndDialog((HWND)HelpInfo->hItemHandle, IDCANCEL);
  }
}

static int winMessageDlgPopup(Ihandle* ih, int x, int y)
{
  InativeHandle* parent = iupDialogGetNativeParent(ih);
  MSGBOXPARAMS MsgBoxParams;
  int result, num_but = 2;
  DWORD dwStyle = MB_TASKMODAL;
  char *icon, *buttons;
  (void)x;
  (void)y;

  /* if parent is used then it will be modal only relative to it */
  /* if (!parent)
       parent = GetActiveWindow(); */

  icon = iupAttribGetStr(ih, "DIALOGTYPE");
  if (iupStrEqualNoCase(icon, "ERROR"))
    dwStyle |= MB_ICONERROR;
  else if (iupStrEqualNoCase(icon, "WARNING"))
    dwStyle |= MB_ICONWARNING;
  else if (iupStrEqualNoCase(icon, "INFORMATION"))
    dwStyle |= MB_ICONINFORMATION;
  else if (iupStrEqualNoCase(icon, "QUESTION"))
    dwStyle |= MB_ICONQUESTION;

  buttons = iupAttribGetStr(ih, "BUTTONS");
  if (iupStrEqualNoCase(buttons, "OKCANCEL"))
    dwStyle |= MB_OKCANCEL;
  else if (iupStrEqualNoCase(buttons, "YESNO"))
    dwStyle |= MB_YESNO;
  else
  {
    dwStyle |= MB_OK;
    num_but = 1;
  }

  if (IupGetCallback(ih, "HELP_CB"))
    dwStyle |= MB_HELP;

  if (num_but == 2 && iupAttribGetInt(ih, "BUTTONDEFAULT") == 2)
    dwStyle |= MB_DEFBUTTON2;
  else
    dwStyle |= MB_DEFBUTTON1;

  MsgBoxParams.cbSize = sizeof(MSGBOXPARAMS);
  MsgBoxParams.hwndOwner = parent;
  MsgBoxParams.hInstance = NULL;
  MsgBoxParams.lpszText = iupAttribGet(ih, "VALUE");
  MsgBoxParams.lpszCaption = iupAttribGet(ih, "TITLE");
  MsgBoxParams.dwStyle = dwStyle;
  MsgBoxParams.lpszIcon = NULL;
  MsgBoxParams.dwContextHelpId = (DWORD_PTR)ih;
  MsgBoxParams.lpfnMsgBoxCallback = (MSGBOXCALLBACK)winMessageDlgHelpCallback;
  MsgBoxParams.dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);

  result = MessageBoxIndirect(&MsgBoxParams);
  if (result == 0)
  {
    iupAttribSetStr(ih, "BUTTONRESPONSE", NULL);
    return IUP_ERROR;
  }

  if (result == IDNO || result == IDCANCEL)
    iupAttribSetStr(ih, "BUTTONRESPONSE", "2");
  else
    iupAttribSetStr(ih, "BUTTONRESPONSE", "1");

  return IUP_NOERROR;
}

void iupdrvMessageDlgInitClass(Iclass* ic)
{
  ic->DlgPopup = winMessageDlgPopup;
}

/* 
In Windows it will always sound a beep. The beep is different for each dialog type.
*/
