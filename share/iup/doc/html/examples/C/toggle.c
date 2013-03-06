/*IupToggle Example in C 
*/

#include <stdlib.h>
#include <stdio.h>
#include "iup.h"

static unsigned char img_1[] = 
{
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,
  1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,
  1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,
  1,1,1,2,2,2,2,2,2,2,2,2,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

static unsigned char img_2[] = 
{
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,2,2,2,2,2,2,1,1,1,1,1,1,
  1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,2,2,2,2,2,2,2,2,2,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};


void createimgs(void)
{ 
  Ihandle *img1, *img2;
  img1 = IupImage(16,16, img_1);
  img2 = IupImage(16,16, img_2);
  IupSetHandle ("img1", img1); 
  IupSetHandle ("img2", img2);
  IupSetAttribute (img1, "1", "255 255 255"); 
  IupSetAttribute (img1, "2", "0 192 0"); 
  IupSetAttribute (img2, "1", "255 255 255"); 
  IupSetAttribute (img2, "2", "0 192 0"); 
}

int toggle_cb(Ihandle* ih, int state)
{
  int pos = IupGetChildPos(IupGetParent(ih), ih);
  printf("toggle%d_cb(state=%d)\n", pos+1, state);
  return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Ihandle *toggles, *dlg, *box;
  Ihandle *toggle1, *toggle2, *toggle3, *toggle4, *toggle5, *toggle6;

  IupOpen(&argc, &argv);

  createimgs();

  toggle1 = IupToggle("toggle with image", NULL);
  toggle2 = IupToggle("deactivated toggle with image", NULL);
  toggle3 = IupToggle("regular toggle", NULL);
  toggle4 = IupToggle("blue foreground color", NULL);
  toggle5 = IupToggle("deactivated toggle", NULL);
  toggle6 = IupToggle("toggle with Courier 14 Bold font", NULL);

  IupSetCallback(toggle1, "ACTION", (Icallback) toggle_cb);
  IupSetCallback(toggle3, "ACTION", (Icallback) toggle_cb);
  IupSetCallback(toggle4, "ACTION", (Icallback) toggle_cb);
  IupSetCallback(toggle6, "ACTION", (Icallback) toggle_cb);
  
  IupSetAttribute(toggle1, "IMAGE",   "img1");              /* Toggle 1 uses image                 */
  IupSetAttribute(toggle2, "IMAGE",   "img2");              /* Toggle 2 uses image                 */
  IupSetAttribute(toggle2, "ACTIVE",  "NO");              /* Toggle 2 inactive                   */
  IupSetAttribute(toggle4, "FGCOLOR", "0 0 255");           /* Toggle 4 has blue foreground color  */
  IupSetAttribute(toggle5, "ACTIVE",  "NO");              /* Toggle 6 inactive                   */
  IupSetAttribute(toggle6, "FONT",    "Courier, Bold 14"); /* Toggle 8 has Courier 14 Bold font   */

  /* Creating box that contains the toggles */
  box = 
    IupVbox (
      toggle1, 
      toggle2, 
      toggle3, 
      toggle4, 
      toggle5, 
      toggle6, 
      NULL  );

  toggles = IupRadio( box ); /* Grouping toggles */
//  toggles = box;  /* No grouping */

  /* Dialog */
  dlg = IupDialog(toggles);
  IupSetAttribute (dlg, "TITLE", "IupToggle"); 
  IupSetAttributes(dlg, "MARGIN=5x5, GAP=5, RESIZE=NO"); 

  /* Associates a menu to the dialog */
  IupShowXY(dlg, IUP_CENTER, IUP_CENTER); 
  IupMainLoop();
  IupClose();

  return EXIT_SUCCESS;
}
