#include <stdlib.h>
#include <stdio.h>
#include "iup.h"          

#define TEST_IMAGE_SIZE 20

static unsigned char image_data_32[16*16*4] = {
  255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0,
  255,  0, 255, 0, 183, 182, 245, 255, 183, 182, 245, 255, 179, 178, 243, 255, 174, 173, 241, 255, 168, 167, 238, 255, 162, 161, 234, 255, 155, 154, 231, 255, 148, 147, 228, 255, 143, 142, 224, 255, 136, 135, 221, 255, 129, 128, 218, 255, 123, 122, 214, 255, 117, 116, 211, 255, 112, 111, 209, 255, 255,  0, 255, 0,
  255,  0, 255, 0, 179, 178, 243, 255, 190, 189, 255, 255, 147, 146, 248, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 246, 248, 251, 255, 75, 88, 190, 255, 89, 88, 176, 255, 89, 88, 176, 255, 255,  0, 255, 0,
  255,  0, 255, 0, 173, 172, 240, 255, 190, 189, 255, 255, 138, 137, 239, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 246, 248, 251, 255, 234, 238, 246, 255, 63, 82, 184, 255, 51, 51, 103, 255, 86, 85, 170, 255, 255,  0, 255, 0,
  255,  0, 255, 0, 167, 166, 237, 255, 190, 189, 255, 255, 129, 128, 230, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 246, 248, 251, 255, 234, 238, 246, 255, 222, 229, 240, 255, 52, 77, 179, 255, 122, 121, 223, 255, 83, 82, 164, 255, 255,  0, 255, 0,
  255,  0, 255, 0, 159, 158, 233, 255, 190, 189, 255, 255, 119, 118, 220, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 246, 248, 251, 255, 234, 238, 246, 255, 222, 229, 240, 255, 210, 219, 234, 255, 40, 71, 173, 255, 114, 113, 215, 255, 80, 79, 159, 255, 255,  0, 255, 0,
  255,  0, 255, 0, 152, 151, 229, 255, 190, 189, 255, 255, 110, 109, 211, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 246, 248, 251, 255, 234, 238, 246, 255, 222, 229, 240, 255, 210, 219, 234, 255, 198, 209, 229, 255, 28, 65, 167, 255, 103, 103, 204, 255, 77, 77, 154, 255, 255,  0, 255, 0,
  255,  0, 255, 0, 146, 145, 226, 255, 190, 189, 255, 255, 103, 102, 204, 255, 255, 255, 255, 255, 255, 255, 255, 255, 246, 248, 251, 255, 234, 238, 246, 255, 222, 229, 240, 255, 210, 219, 234, 255, 198, 209, 229, 255, 189, 202, 225, 255, 17, 59, 161, 255, 92, 93, 194, 255, 74, 74, 148, 255, 255,  0, 255, 0,
  255,  0, 255, 0, 139, 138, 223, 255, 188, 187, 255, 255, 183, 182, 255, 255, 96, 99, 201, 255, 86, 94, 196, 255, 75, 88, 190, 255, 63, 82, 184, 255, 52, 77, 179, 255, 40, 71, 173, 255, 28, 65, 167, 255, 17, 59, 161, 255, 92, 93, 193, 255, 84, 86, 186, 255, 71, 71, 143, 255, 255,  0, 255, 0,
  255,  0, 255, 0, 132, 131, 219, 255, 180, 179, 255, 255, 174, 173, 255, 255, 164, 163, 252, 255, 143, 142, 244, 255, 135, 134, 236, 255, 129, 128, 230, 255, 122, 121, 223, 255, 114, 113, 215, 255, 108, 107, 209, 255, 92, 93, 193, 255, 84, 86, 186, 255, 76, 80, 178, 255, 68, 68, 137, 255, 255,  0, 255, 0,
  255,  0, 255, 0, 124, 123, 215, 255, 170, 169, 255, 255, 160, 159, 251, 255, 148, 147, 245, 255, 75, 91, 113, 255, 75, 91, 113, 255, 75, 91, 113, 255, 75, 91, 113, 255, 82, 98, 118, 255, 91, 106, 125, 255, 84, 86, 186, 255, 76, 79, 178, 255, 68, 73, 170, 255, 65, 65, 131, 255, 255,  0, 255, 0,
  255,  0, 255, 0, 118, 117, 212, 255, 160, 159, 255, 255, 145, 144, 245, 255, 135, 134, 236, 255, 75, 91, 113, 255, 0, 0, 0, 255, 52, 60, 71, 255, 206, 217, 233, 255, 212, 221, 236, 255, 103, 116, 133, 255, 67, 75, 174, 255, 68, 73, 170, 255, 60, 66, 163, 255, 62, 62, 125, 255, 255,  0, 255, 0,
  255,  0, 255, 0, 112, 111, 209, 255, 154, 153, 255, 255, 135, 134, 236, 255, 129, 128, 230, 255, 75, 91, 113, 255, 52, 60, 71, 255, 104, 120, 141, 255, 216, 224, 237, 255, 224, 231, 241, 255, 115, 127, 143, 255, 53, 65, 163, 255, 60, 66, 162, 255, 53, 61, 156, 255, 60, 59, 120, 255, 255,  0, 255, 0,
  255,  0, 255, 0, 108, 107, 207, 255, 143, 142, 243, 255, 129, 128, 230, 255, 36, 68, 170, 255, 33, 50, 71, 255, 171, 180, 195, 255, 179, 187, 198, 255, 188, 193, 202, 255, 196, 200, 206, 255, 72, 77, 86, 255, 51, 62, 158, 255, 54, 61, 156, 255, 49, 57, 152, 255, 57, 57, 114, 255, 255,  0, 255, 0,
  255,  0, 255, 0, 108, 107, 207, 84, 101, 100, 195, 255, 86, 85, 170, 255, 83, 82, 164, 255, 80, 79, 159, 255, 77, 77, 154, 255, 74, 74, 148, 255, 71, 71, 143, 255, 68, 68, 137, 255, 65, 65, 131, 255, 60, 59, 120, 255, 60, 59, 120, 255, 57, 57, 114, 255, 55, 54, 110, 255, 255,  0, 255, 0,
  255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255,  0, 255, 0, 255, 0, 255, 0};

static unsigned char image_data_8 [TEST_IMAGE_SIZE*TEST_IMAGE_SIZE] = 
{
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,0,0,0,5, 
  5,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,0,0,0,5, 
  5,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,0,0,0,5, 
  5,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,0,0,0,5, 
  5,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,0,0,0,5, 
  5,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,0,0,0,5, 
  5,0,0,0,3,3,3,3,3,3,4,4,4,4,4,4,0,0,0,5, 
  5,0,0,0,3,3,3,3,3,3,4,4,4,4,4,4,0,0,0,5, 
  5,0,0,0,3,3,3,3,3,3,4,4,4,4,4,4,0,0,0,5, 
  5,0,0,0,3,3,3,3,3,3,4,4,4,4,4,4,0,0,0,5, 
  5,0,0,0,3,3,3,3,3,3,4,4,4,4,4,4,0,0,0,5, 
  5,0,0,0,3,3,3,3,3,3,4,4,4,4,4,4,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
};

static unsigned char image_data_8_pressed [TEST_IMAGE_SIZE*TEST_IMAGE_SIZE] = 
{
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,5,1,1,1,1,1,2,2,2,2,2,5,0,0,0,5, 
  5,0,0,0,1,5,1,1,1,1,2,2,2,2,5,2,0,0,0,5, 
  5,0,0,0,1,1,5,1,1,1,2,2,2,5,2,2,0,0,0,5, 
  5,0,0,0,1,1,1,5,1,1,2,2,5,2,2,2,0,0,0,5, 
  5,0,0,0,1,1,1,1,5,1,2,5,2,2,2,2,0,0,0,5, 
  5,0,0,0,1,1,1,1,1,5,5,2,2,2,2,2,0,0,0,5, 
  5,0,0,0,3,3,3,3,3,5,5,4,4,4,4,4,0,0,0,5, 
  5,0,0,0,3,3,3,3,5,3,4,5,4,4,4,4,0,0,0,5, 
  5,0,0,0,3,3,3,5,3,3,4,4,5,4,4,4,0,0,0,5, 
  5,0,0,0,3,3,5,3,3,3,4,4,4,5,4,4,0,0,0,5, 
  5,0,0,0,3,5,3,3,3,3,4,4,4,4,5,4,0,0,0,5, 
  5,0,0,0,5,3,3,3,3,3,4,4,4,4,4,5,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
};

static unsigned char image_data_8_inactive [TEST_IMAGE_SIZE*TEST_IMAGE_SIZE] = 
{
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,5,5,1,1,1,1,1,1,1,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,5,5,5,1,5,5,5,5,5,5,0,0,0,5, 
  5,0,0,0,5,5,1,1,1,1,1,1,1,5,5,5,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
};

static void createimgs(void)
{ 
  Ihandle *image; 

  image = IupImageRGBA(16, 16, image_data_32);
  IupSetHandle ("image2", image); 

  image = IupImage(TEST_IMAGE_SIZE, TEST_IMAGE_SIZE, image_data_8);
  IupSetAttribute(image, "0", "BGCOLOR");
  IupSetAttribute(image, "1", "255 0 0"); 
  IupSetAttribute(image, "2", "0 255 0");
  IupSetAttribute(image, "3", "0 0 255"); 
  IupSetAttribute(image, "4", "255 255 255"); 
  IupSetAttribute(image, "5", "0 0 0"); 
  IupSetHandle ("image1", image); 

  image = IupImage(TEST_IMAGE_SIZE, TEST_IMAGE_SIZE, image_data_8_inactive);
  IupSetAttribute(image, "0", "BGCOLOR");
  IupSetAttribute(image, "1", "255 0 0"); 
  IupSetAttribute(image, "2", "0 255 0");
  IupSetAttribute(image, "3", "0 0 255"); 
  IupSetAttribute(image, "4", "255 255 255"); 
  IupSetAttribute(image, "5", "0 0 0"); 
  IupSetHandle ("image1i", image); 

  image = IupImage(TEST_IMAGE_SIZE, TEST_IMAGE_SIZE, image_data_8_pressed);
  IupSetAttribute(image, "0", "BGCOLOR");
  IupSetAttribute(image, "1", "255 0 0"); 
  IupSetAttribute(image, "2", "0 255 0");
  IupSetAttribute(image, "3", "0 0 255"); 
  IupSetAttribute(image, "4", "255 255 255"); 
  IupSetAttribute(image, "5", "0 0 0"); 
  IupSetHandle ("image1p", image);
}

static int togglecb(Ihandle *self, int v)
{
  if (v == 1) printf("Toggle %s - ON\n", IupGetAttribute(self, "NAME")); 
  else printf("Toggle %s - OFF\n", IupGetAttribute(self, "NAME")); 
  return IUP_DEFAULT;
}

static int toggle9cb(Ihandle *self, int v)
{
  Ihandle* ih = IupGetHandle("radio test");
  if (v == 1) printf("Toggle 9 - ON\n"); 
  else if (v == -1) printf("Toggle 9 - NOTDEF\n"); 
  else printf("Toggle 9 - OFF\n"); 

  if (v == 1)
    IupSetAttribute(ih, "VALUE", "ON");
  else
    IupSetAttribute(ih, "VALUE", "OFF");

  return IUP_DEFAULT;
}

static int toggle7cb(Ihandle *ih, int v)
{
  Ihandle* dlg = IupGetDialog(ih);
  Ihandle* box = IupGetChild(dlg, 0);
  if (v)
    IupSetAttribute(box, "ACTIVE", "NO");
  else
    IupSetAttribute(box, "ACTIVE", "YES");
  IupSetAttribute(ih, "ACTIVE", "YES");
  return IUP_DEFAULT;
}

static int k_any(Ihandle *ih, int c)
{
  printf("K_ANY(%d)\n", c);
  return IUP_DEFAULT;
}

static int getfocus_cb(Ihandle *ih)
{
  printf("GETFOCUS_CB()\n");
  return IUP_DEFAULT;
}

static int help_cb(Ihandle* ih)
{
  printf("HELP_CB()\n");
  return IUP_DEFAULT;
}
     
static int killfocus_cb(Ihandle *ih)
{
  printf("KILLFOCUS_CB()\n");
  return IUP_DEFAULT;
}

static int leavewindow_cb(Ihandle *ih)
{
  printf("LEAVEWINDOW_CB()\n");
  return IUP_DEFAULT;
}

static int enterwindow_cb(Ihandle *ih)
{
  printf("ENTERWINDOW_CB()\n");
  return IUP_DEFAULT;
}

void ToggleTest(void)
{
  Ihandle *dlg, *box;
  Ihandle *toggle1, *toggle2, *toggle3, *toggle4, *toggle5, *toggle6, *toggle7, *toggle8, *toggle9;

  createimgs();

  toggle1 = IupToggle(NULL, NULL);
  toggle2 = IupToggle(NULL, NULL);
  toggle3 = IupToggle("Text Toggle", NULL);
//  toggle4 = IupToggle("blue foreground color", NULL);
  toggle4 = IupToggle("Radio &Text", NULL);
  toggle5 = IupToggle("red background color", NULL);
  toggle6 = IupToggle("multiple lines\nsecond line", NULL);
  toggle7 = IupToggle("INACTIVE", NULL);
  toggle8 = IupToggle("Courier 14 Bold font", NULL);
  toggle9 = IupToggle("3 State", NULL);
  
  IupSetHandle("radio test", toggle8);

  IupSetCallback(toggle1, "ACTION", (Icallback)togglecb);
  IupSetCallback(toggle2, "ACTION", (Icallback)togglecb);
  IupSetCallback(toggle3, "ACTION", (Icallback)togglecb);
  IupSetCallback(toggle4, "ACTION", (Icallback)togglecb);
  IupSetCallback(toggle5, "ACTION", (Icallback)togglecb);
  IupSetCallback(toggle6, "ACTION", (Icallback)togglecb);
  IupSetCallback(toggle8, "ACTION", (Icallback)togglecb);
  IupSetCallback(toggle7, "ACTION", (Icallback)toggle7cb);
  IupSetCallback(toggle9, "ACTION", (Icallback)toggle9cb);

  IupSetCallback(toggle3, "K_ANY",        (Icallback)k_any);
  IupSetCallback(toggle3, "HELP_CB",      (Icallback)help_cb);
  IupSetCallback(toggle3, "GETFOCUS_CB",  (Icallback)getfocus_cb); 
  IupSetCallback(toggle3, "KILLFOCUS_CB", (Icallback)killfocus_cb);
  IupSetCallback(toggle3, "ENTERWINDOW_CB", (Icallback)enterwindow_cb);
  IupSetCallback(toggle3, "LEAVEWINDOW_CB", (Icallback)leavewindow_cb);
  
  IupSetAttribute(toggle1, "IMAGE",   "image1");            
  IupSetAttribute(toggle1, "IMPRESS", "image1p");
  IupSetAttribute(toggle1, "IMINACTIVE", "image1i");
//  IupSetAttribute(toggle1, "PADDING",   "10x10");            
//  IupSetAttribute(toggle1, "RASTERSIZE",   "160x160");
//  IupSetAttribute(toggle2, "RASTERSIZE",   "60x60");            
//  IupSetAttribute(toggle1, "ALIGNMENT",   "ARIGHT");
//  IupSetAttribute(toggle2, "ALIGNMENT",  "ARIGHT");            
  IupSetAttribute(toggle2, "IMAGE",   "image2");
  IupSetAttribute(toggle1, "NAME", "1");
  IupSetAttribute(toggle2, "NAME", "2");
//  IupSetAttribute(toggle1, "FLAT", "Yes");
//  IupSetAttribute(toggle2, "FLAT", "Yes");
//  IupSetAttribute(toggle1, "CANFOCUS", "NO");
  IupSetAttribute(toggle2, "CANFOCUS", "NO");
//  IupSetAttribute(toggle3, "CANFOCUS", "NO");
  IupSetAttribute(toggle3, "NAME", "3");
  IupSetAttribute(toggle4, "NAME", "4");
  IupSetAttribute(toggle5, "NAME", "5");
  IupSetAttribute(toggle6, "NAME", "6");
  IupSetAttribute(toggle8, "NAME", "8");
//  IupSetAttribute(toggle4, "FGCOLOR", "0 0 255");         
  IupSetAttribute(toggle5, "BGCOLOR", "255 0 0");         
  IupSetAttribute(toggle8, "FONT",    "COURIER_BOLD_14"); 
  IupSetAttribute(toggle9, "3STATE",  "YES");             
  IupSetAttribute(toggle6, "TIP", "Toggle Tip");
  IupSetAttribute(toggle7, "RIGHTBUTTON", "YES");
  IupSetAttribute(toggle5, "RIGHTBUTTON", "YES");

  /* Creating box that contains the toggles */
  box = IupHbox(
    IupVbox(
      toggle1, 
      toggle2, 
      toggle3, 
      toggle7, 
      toggle9, 
      NULL), 
    IupFrame(IupRadio(IupVbox(
      toggle4, 
      toggle5, 
      toggle6, 
      toggle8, 
      NULL))),
    NULL);

  IupSetAttribute(IupGetChild(box, 1), "TITLE", "Radio in a Frame");

//  IupSetAttribute(box, "BGCOLOR", "255 0 0");         

  /* Dialog */
  dlg = IupDialog(box);
  IupSetAttribute (dlg, "TITLE", "IupToggle Test"); 
  IupSetAttributes(dlg, "MARGIN=5x5, GAP=5"); 

  /* Associates a menu to the dlg */
  IupShowXY(dlg, IUP_CENTER, IUP_CENTER); 
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  ToggleTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
