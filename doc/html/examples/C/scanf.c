/* IupScanf: Example in C 
   Shows a dialog with three fields to be filled. 
   One receives a string, the other receives a real number and 
   the last receives an integer number.
*/

#include <stdlib.h>
#include <stdio.h>
#include "iup.h"

int main(int argc, char **argv)
{
  int ret;
  int integer = 12;
  float real = 1e-3f;
  char text[300]="This is a vector of characters";
  char *fmt =
  {
   "IupScanf Example\n"
   "Text: %300.5%s\n"
   "Real: %20.10%g\n"
   "Integer: %20.10%d\n"
  };
  
  IupOpen(&argc, &argv);

  IupSetLanguage("ENGLISH");

  ret = IupScanf(fmt, text, &real, &integer);
  if (ret == -1)
    IupMessage("IupScanf","Operation canceled");
  else
    IupMessagef("IupScanf","Text: %s\nReal: %f\nInteger: %d\nFields read successfully: %d",text,real,integer,ret);

  IupClose();

  return EXIT_SUCCESS;

}
