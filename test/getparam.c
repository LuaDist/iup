/* IupGetParam Example in C 
   Shows a dialog with all the possible fields. 
*/
#include <stdlib.h>
#include <stdio.h>

#include <iup.h>
#include <iupcontrols.h>

static int param_action(Ihandle* dialog, int param_index, void* user_data)
{                
  switch(param_index)
  {
  case IUP_GETPARAM_OK:
    printf("IupGetParam - OK\n");
    break;
  case IUP_GETPARAM_INIT:
    {
      Ihandle* param = (Ihandle*)IupGetAttributeId(dialog, "PARAM", param_index);
      IupSetfAttribute(param, "MAXSTR", "%d", 30);

      printf("IupGetParam - Map\n");
      break;
    }
  case IUP_GETPARAM_CANCEL:
    printf("IupGetParam - Cancel\n");
    break;
  case IUP_GETPARAM_HELP:
    printf("IupGetParam - Help\n");
    break;
//  case 1:
//    return 0;
  default:
    {
      Ihandle* param = (Ihandle*)IupGetAttributeId(dialog, "PARAM", param_index);
      printf("PARAM%d = %s\n", param_index, IupGetAttribute(param, "VALUE"));
      break;
    }
  }
  return 1;
}
                            
void GetParamTest(void)
{
  int pboolean = 1;
  int pinteger = 3456;
  float preal = 3.543f;
  int pinteger2 = 192;
  float preal2 = 0.5f;
  float pangle = 90;
  char pstring[100] = "string text";
  char pfont[100] = "Courier, 24";
  char pcolor[100] = "255 0 128";
  int plist = 2, poptions = 1;
  char pstring2[200] = "second text\nsecond line";
  char file_name[500] = "test.jpg";
  
  if (!IupGetParam("Title", param_action, 0,
                   "Bt %u[, MyCancel, Help!]\n"
                   "Boolean 100%%: %b[No,Yes]\n"
                   "Integer: %i\n"
                   "Real 1: %r\n"
                   "Sep1 %t\n"
                   "Integer: %i[0,255]\n"
                   "Real 2: %r[-1.5,1.5,0.05]\n"
                   "Sep2 %t\n"
                   "Angle: %a[0,360]\n"
                   "String: %s\n"
                   "Options: %o|item0|item1|item2|\n" 
                   "List: %l|item0|item1|item2|item3|item4|item5|item6|\n" 
                   "File: %f[OPEN|*.bmp;*.jpg|CURRENT|NO|NO]\n"
                   "Color: %c{Color Tip}\n"
                   "Font: %n\n"
                   "Sep3 %t\n"
                   "Multiline: %m\n",
                   &pboolean, &pinteger, &preal, &pinteger2, &preal2, &pangle, pstring, &poptions, &plist, file_name, pcolor, pfont, pstring2, NULL))
    return;
  
  IupMessagef("IupGetParam",
              "Boolean Value: %d\n"
              "Integer: %d\n"
              "Real 1: %g\n"
              "Integer: %d\n"
              "Real 2: %g\n"
              "Angle: %g\n"
              "String: %s\n"
              "Options Index: %d\n" 
              "List Index: %d\n" 
              "FileName: %s\n"
              "Color: %s\n"
              "Font: %s\n"
              "Multiline: %s",
              pboolean, pinteger, (double)preal, pinteger2, (double)preal2, (double)pangle, pstring, poptions, plist, file_name, pcolor, pfont, pstring2);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  GetParamTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
