/* IupGetParam: Example in C 
   Shows a dialog with all the possible fields. 
*/

#include <stdlib.h>
#include <stdio.h>

#include <iup.h>
#include <iupcontrols.h>

int param_action(Ihandle* dialog, int param_index, void* user_data)
{                
  switch(param_index)
  {
  case IUP_GETPARAM_OK:
    printf("OK\n");
    break;
  case IUP_GETPARAM_INIT:
    printf("Map\n");
    break;
  case IUP_GETPARAM_CANCEL:
    printf("Cancel\n");
    break;
  case IUP_GETPARAM_HELP:
    printf("Help\n");
    break;
  default:
    {
      Ihandle* param;
      char param_str[50];
      sprintf(param_str, "PARAM%d", param_index);
      param = (Ihandle*)IupGetAttribute(dialog, param_str);
      printf("%s = %s\n", param_str, IupGetAttribute(param, "VALUE"));
      break;
    }
  }
  return 1;
}

int main(int argc, char **argv)
{
  int pboolean = 1;
  int pinteger = 3456;
  float preal = 3.543f;
  int pinteger2 = 192;
  float preal2 = 0.5f;
  float pangle = 90;
  char pstring[100] = "string text";
  char pcolor[100] = "255 0 128";
  int plist = 2;
  char pstring2[200] = "second text\nsecond line";
  char pfile_name[500] = "test.jpg";
  
  IupOpen(&argc, &argv);
  IupControlsOpen();   /* so we can show the IupDial, but can be tested without it */
  IupSetLanguage("ENGLISH");

  if (!IupGetParam("Title", param_action, 0,
                   "Boolean: %b[No,Yes]{Boolean Tip}\n"
                   "Integer: %i{Integer Tip}\n"
                   "Real 1: %r{Real Tip}\n"
                   "Sep1 %t\n"
                   "Integer: %i[0,255]{Integer Tip 2}\n"
                   "Real 2: %r[-1.5,1.5]{Real Tip 2}\n"
                   "Sep2 %t\n"
                   "Angle: %a[0,360]{Angle Tip}\n"
                   "String: %s{String Tip}\n"
                   "List: %l|item1|item2|item3|{List Tip}\n" 
                   "File: %f[OPEN|*.bmp;*.jpg|CURRENT|NO|NO]{File Tip}\n"
                   "Color: %c{Color Tip}\n"
                   "Sep3 %t\n"
                   "Multiline: %m{Multiline Tip}\n",
                   &pboolean, &pinteger, &preal, &pinteger2, &preal2, &pangle, pstring, &plist, pfile_name, pcolor, pstring2, NULL))
    return IUP_DEFAULT;
  
  IupMessagef("IupGetParam",
              "Boolean Value: %d\n"
              "Integer: %d\n"
              "Real 1: %g\n"
              "Integer: %d\n"
              "Real 2: %g\n"
              "Angle: %g\n"
              "String: %s\n"
              "List Index: %d\n" 
              "FileName: %s\n"
              "Color: %s\n"
              "Multiline: %s",
              pboolean, pinteger, (double)preal, pinteger2, (double)preal2, (double)pangle, pstring, plist, pfile_name, pcolor, pstring2);

  IupClose();

  return EXIT_SUCCESS;

}
