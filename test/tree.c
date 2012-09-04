/*IupTree Example in C 
Creates a tree with some branches and leaves. 
Two callbacks are registered: one deletes marked nodes when the Del key is pressed, 
and the other, called when the right mouse button is pressed, opens a menu with options. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "iupkey.h"


static Ihandle* load_image_LogoTecgraf(void)
{
  unsigned char imgdata[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 108, 120, 143, 125, 132, 148, 178, 173, 133, 149, 178, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 110, 130, 48, 130, 147, 177, 254, 124, 139, 167, 254, 131, 147, 176, 137, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 115, 128, 153, 134, 142, 159, 191, 194, 47, 52, 61, 110, 114, 128, 154, 222, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 143, 172, 192, 140, 156, 188, 99, 65, 69, 76, 16, 97, 109, 131, 251, 129, 144, 172, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 131, 147, 175, 232, 140, 157, 188, 43, 0, 0, 0, 0, 100, 112, 134, 211, 126, 141, 169, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 72, 78, 88, 26, 48, 52, 57, 60, 135, 150, 178, 254, 108, 121, 145, 83, 105, 118, 142, 76, 106, 119, 143, 201, 118, 133, 159, 122, 117, 129, 152, 25, 168, 176, 190, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    118, 128, 145, 3, 104, 117, 140, 92, 114, 127, 152, 180, 131, 147, 177, 237, 133, 149, 178, 249, 38, 42, 50, 222, 137, 152, 180, 249, 126, 142, 170, 182, 114, 128, 154, 182, 104, 117, 140, 227, 95, 107, 128, 238, 83, 93, 112, 248, 84, 95, 113, 239, 104, 117, 141, 180, 115, 129, 155, 93, 127, 140, 165, 4,
    98, 109, 130, 153, 109, 123, 147, 254, 145, 163, 195, 153, 138, 154, 182, 56, 115, 123, 138, 5, 92, 99, 109, 35, 134, 149, 177, 230, 0, 0, 0, 0, 0, 0, 0, 0, 120, 133, 159, 143, 135, 151, 181, 115, 86, 89, 93, 5, 41, 45, 51, 54, 40, 45, 53, 150, 107, 120, 144, 254, 122, 137, 164, 154,
    51, 57, 66, 147, 83, 93, 112, 255, 108, 121, 145, 159, 113, 126, 151, 62, 123, 136, 159, 8, 87, 93, 103, 35, 125, 141, 169, 230, 0, 0, 0, 0, 0, 0, 0, 0, 129, 143, 169, 143, 140, 156, 184, 115, 134, 147, 172, 8, 124, 138, 165, 60, 124, 139, 167, 155, 131, 147, 177, 255, 131, 147, 176, 153,
    64, 68, 73, 2, 36, 39, 45, 86, 41, 46, 54, 173, 60, 67, 80, 232, 75, 84, 101, 251, 89, 100, 120, 228, 105, 118, 142, 250, 110, 123, 148, 187, 118, 132, 158, 187, 126, 141, 169, 229, 134, 149, 177, 239, 136, 152, 179, 250, 136, 152, 181, 234, 139, 156, 186, 175, 130, 145, 173, 90, 124, 134, 151, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 71, 74, 79, 19, 60, 64, 73, 50, 92, 103, 124, 254, 86, 95, 111, 84, 90, 100, 117, 76, 126, 141, 168, 201, 113, 126, 150, 119, 99, 105, 117, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 93, 105, 125, 231, 135, 151, 181, 46, 0, 0, 0, 0, 137, 154, 184, 212, 123, 137, 164, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 74, 83, 98, 191, 133, 149, 179, 102, 111, 121, 139, 17, 134, 150, 180, 252, 126, 140, 166, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 48, 57, 132, 121, 136, 164, 197, 121, 135, 161, 115, 130, 146, 175, 221, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 47, 52, 46, 87, 98, 118, 254, 126, 142, 170, 254, 124, 139, 166, 135, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 51, 57, 67, 118, 115, 128, 152, 170, 127, 140, 164, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  Ihandle* image = IupImageRGBA(16, 16, imgdata);
  return image;
}

#define TEST_IMAGE_SIZE 16

static Ihandle* load_image_TestImage(void)
{
  unsigned char image_data_8 [TEST_IMAGE_SIZE*TEST_IMAGE_SIZE] = 
  {
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
    5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
    5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
    5,0,0,0,1,1,1,1,2,2,2,2,0,0,0,5, 
    5,0,0,0,1,1,1,1,2,2,2,2,0,0,0,5, 
    5,0,0,0,1,1,1,1,2,2,2,2,0,0,0,5, 
    5,0,0,0,1,1,1,1,2,2,2,2,0,0,0,5, 
    5,0,0,0,3,3,3,3,4,4,4,4,0,0,0,5, 
    5,0,0,0,3,3,3,3,4,4,4,4,0,0,0,5, 
    5,0,0,0,3,3,3,3,4,4,4,4,0,0,0,5, 
    5,0,0,0,3,3,3,3,4,4,4,4,0,0,0,5,
    5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
    5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
    5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
  };

  Ihandle* image = IupImage(TEST_IMAGE_SIZE, TEST_IMAGE_SIZE, image_data_8);
  IupSetAttribute(image, "0", "BGCOLOR");
  IupSetAttribute(image, "1", "255 0 0"); 
  IupSetAttribute(image, "2", "0 255 0");
  IupSetAttribute(image, "3", "0 0 255"); 
  IupSetAttribute(image, "4", "255 255 255"); 
  IupSetAttribute(image, "5", "0 0 0"); 

  return image;
}

static int addleaf(void)
{
  Ihandle* tree = IupGetHandle("tree");
  int id = IupGetInt(tree, "VALUE");
  IupTreeSetAttribute(tree, "ADDLEAF", id, "");
  return IUP_DEFAULT;
}

static int insertleaf(void)
{
  Ihandle* tree = IupGetHandle("tree");
  int id = IupGetInt(tree, "VALUE");
  IupTreeSetAttribute(tree, "INSERTLEAF", id, "");
  return IUP_DEFAULT;
}

static int addbranch(void)
{
  Ihandle* tree = IupGetHandle("tree");
  int id = IupGetInt(tree, "VALUE");
  IupTreeSetAttribute(tree, "ADDBRANCH", id, "");
  return IUP_DEFAULT;
}

static int insertbranch(void)
{
  Ihandle* tree = IupGetHandle("tree");
  int id = IupGetInt(tree, "VALUE");
  IupTreeSetAttribute(tree, "INSERTBRANCH", id, "");
  return IUP_DEFAULT;
}

static int togglestate(void)
{
  char *value;
  Ihandle* tree = IupGetHandle("tree");
  int id = IupGetInt(tree, "VALUE");
  value = IupTreeGetAttribute(tree, "STATE", id);
  if (value)
  {
    if (strcmp(value, "EXPANDED")==0)
      IupTreeSetAttribute(tree, "STATE", id, "COLLAPSED");
    else
      IupTreeSetAttribute(tree, "STATE", id, "EXPANDED");
  }
  return IUP_DEFAULT;
}

static int text_cb(Ihandle* ih, int c, char *after)
{
  if (c == K_ESC)
    return IUP_CLOSE;

  if (c == K_CR)
  {
    Ihandle *tree = IupGetHandle("tree");
    IupSetAttribute(tree, "NAME",   after);
    return IUP_CLOSE;
  }
  
  return IUP_DEFAULT;
}

static int removenode(void)
{
  Ihandle* tree = IupGetHandle("tree");
  IupSetAttribute(tree, "DELNODE", "SELECTED");
  return IUP_DEFAULT;
}

static int removechild(void)
{
  Ihandle* tree = IupGetHandle("tree");
  IupSetAttribute(tree, "DELNODE", "CHILDREN");
  return IUP_DEFAULT;
}

static int removemarked(void)
{
  Ihandle* tree = IupGetHandle("tree");
  IupSetAttribute(tree, "DELNODE", "MARKED");
  return IUP_DEFAULT;
}

static int removeall(void)
{
  Ihandle* tree = IupGetHandle("tree");
  IupSetAttribute(tree, "DELNODE", "ALL");
  return IUP_DEFAULT;
}

static int expandall(void)
{
  Ihandle* tree = IupGetHandle("tree");
  IupSetAttribute(tree, "EXPANDALL", "YES");
  return IUP_DEFAULT;
}

static int contractall(void)
{
  Ihandle* tree = IupGetHandle("tree");
  IupSetAttribute(tree, "EXPANDALL", "NO");
  return IUP_DEFAULT;
}

int renamenode(void)
{
  Ihandle* tree = IupGetHandle("tree");
  IupSetAttribute(tree, "RENAME", "YES");
  return IUP_DEFAULT;
}

static int button_cb(Ihandle *ih,int but,int pressed,int x,int y,char* status)
{
  printf("BUTTON_CB(but=%c (%d), x=%d, y=%d [%s]) - [id=%d]\n",(char)but,pressed,x,y, status, IupConvertXYToPos(ih, x, y));
  return IUP_DEFAULT;
}

static int motion_cb(Ihandle *ih,int x,int y,char* status)
{
  printf("MOTION_CB(x=%d, y=%d [%s]) - [id=%d]\n",x,y, status,IupConvertXYToPos(ih, x, y));
  return IUP_DEFAULT;
}

static int showrename_cb(Ihandle* ih, int id)
{
  (void)ih;
  printf("SHOWRENAME_CB(%d)\n", id);
  if (id == 6)
    return IUP_IGNORE;
  return IUP_DEFAULT;
}

static int togglevalue_cb(Ihandle* ih, int id, int status)
{
  (void)ih;
  printf("TOGGLEVALUE_CB(%d, %d)\n", id, status);
  return IUP_DEFAULT;
}

static int selection_cb(Ihandle *ih, int id, int status)
{
  (void)ih;
  printf("SELECTION_CB(%d, %d)\n", id, status);
  return IUP_DEFAULT;
}

static int multiselection_cb(Ihandle *ih, int* ids, int n)
{
  int i;
  (void)ih;
  printf("MULTISELECTION_CB(");
  for (i = 0; i < n; i++)
    printf("%d, ", ids[i]);
  printf("n=%d)\n", n);
  return IUP_DEFAULT;
}

static int multiunselection_cb(Ihandle *ih, int* ids, int n)
{
  int i;
  (void)ih;
  printf("MULTIUNSELECTION_CB(");
  for (i = 0; i < n; i++)
    printf("%d, ", ids[i]);
  printf("n=%d)\n", n);
  return IUP_DEFAULT;
}

static int executeleaf_cb(Ihandle* ih, int id)
{
  printf("EXECUTELEAF_CB (%d)\n", id);
  return IUP_DEFAULT;
}

static int rename_cb(Ihandle* ih, int id, char* title)
{
  printf("RENAME_CB (%d=%s)\n", id, title);
  if (strcmp(title, "fool") == 0)
    return IUP_IGNORE;
  return IUP_DEFAULT;
}

static int branchopen_cb(Ihandle* ih, int id)
{
  printf("BRANCHOPEN_CB (%d)\n", id);
//  if (id == 6)
//    return IUP_IGNORE;
  return IUP_DEFAULT;
}

static int branchclose_cb(Ihandle* ih, int id)
{
  printf("BRANCHCLOSE_CB (%d)\n", id);
//  if (id == 6)
//    return IUP_IGNORE;
  return IUP_DEFAULT;
}

static int noderemoved_cb(Ihandle* ih, void* data)
{
  printf("NODEREMOVED_CB(%p)\n", data);
  return IUP_DEFAULT;
}

static int dragdrop_cb(Ihandle* ih, int drag_id, int drop_id, int shift, int control)
{
  printf("DRAGDROP_CB (%d)->(%d)\n", drag_id, drop_id);
  return IUP_CONTINUE;
//  return IUP_DEFAULT;
}

static int getfocus_cb(Ihandle* ih)
{
  printf("GETFOCUS_CB()\n");
  return IUP_DEFAULT;
}

static int killfocus_cb(Ihandle* ih)
{
  printf("KILLFOCUS_CB()\n");
  return IUP_DEFAULT;
}

static int leavewindow_cb(Ihandle *ih)
{
  printf("LEAVEWINDOW_CB()\n");
  return IUP_DEFAULT;
}

static int enterwindow_cb(Ihandle* ih)
{
  printf("ENTERWINDOW_CB()\n");
  return IUP_DEFAULT;
}

static int k_any_cb(Ihandle* ih, int c)
{
  if (c == K_DEL) 
    IupSetAttribute(ih, "DELNODE", "MARKED");
  if (iup_isprint(c))
    printf("K_ANY(%s, %d = %s \'%c\')\n", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"), c, iupKeyCodeToName(c), (char)c);
  else
    printf("K_ANY(%s, %d = %s)\n", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"), c, iupKeyCodeToName(c));
  return IUP_CONTINUE;
}

static int help_cb(Ihandle* ih)
{
  printf("HELP_CB()\n");
  return IUP_DEFAULT;
}

static int selectnode(Ihandle* ih)
{
  Ihandle* tree = IupGetHandle("tree");
  IupSetAttribute(tree, "VALUE",  IupGetAttribute(ih, "TITLE"));
  return IUP_DEFAULT;
}

static int nodeinfo(Ihandle* ih)
{
  char attr[50], *kind;
  Ihandle* tree = IupGetHandle("tree");
  int branch = 0, id = IupGetInt(tree, "VALUE");
  printf("\nTree Info:\n");
  printf("  TOTALCOUNT=%s\n", IupGetAttribute(tree, "COUNT"));
  if (id == -1)
    return IUP_DEFAULT;
  printf("Node Info:\n");
  printf("  ID=%d\n", id);
  printf("  TITLE=%s\n", IupGetAttribute(tree, "TITLE"));
  printf("  DEPTH=%s\n", IupGetAttribute(tree, "DEPTH"));
  sprintf(attr, "KIND%d", id);
  kind = IupGetAttribute(tree, "KIND");
  printf("  KIND=%s\n", kind);
  if (strcmp(kind, "BRANCH")==0) branch = 1;
  if (branch)
    printf("  STATE=%s\n", IupGetAttribute(tree, "STATE"));
  printf("  IMAGE=%s\n", IupGetAttribute(tree, "IMAGE"));
  if (branch)
    printf("  IMAGEBRANCHEXPANDED=%s\n", IupGetAttribute(tree, "IMAGEBRANCHEXPANDED"));
  printf("  MARKED=%s\n", IupGetAttribute(tree, "MARKED"));
  printf("  COLOR=%s\n", IupGetAttribute(tree, "COLOR"));
  printf("  PARENT=%s\n", IupGetAttribute(tree, "PARENT"));
  printf("  CHILDCOUNT=%s\n", IupGetAttribute(tree, "CHILDCOUNT"));
  printf("  USERDATA=%p\n", IupGetAttribute(tree, "USERDATA"));
  return IUP_DEFAULT;
}

static int rightclick_cb(Ihandle* ih, int id)
{
  Ihandle *popup_menu;
  char attr[50];

  popup_menu = IupMenu(
    IupItem ("Node Info","nodeinfo"),
    IupItem ("Rename Node","renamenode"),
    IupSeparator(),
    IupItem ("Add Leaf","addleaf"),
    IupItem ("Add Branch","addbranch"),
    IupItem ("Insert Leaf","insertleaf"),
    IupItem ("Insert Branch","insertbranch"),
    IupItem ("Remove Node","removenode"),
    IupItem ("Remove Children","removechild"),
    IupItem ("Remove Marked","removemarked"),
    IupItem ("Remove All","removeall"),
    IupItem ("Toggle State","togglestate"),
    IupItem ("Expand All","expandall"),
    IupItem ("Contract All","contractall"),
    IupSubmenu("Focus", IupMenu(
      IupItem ("ROOT", "selectnode"),
      IupItem ("LAST", "selectnode"),
      IupItem ("PGUP", "selectnode"),
      IupItem ("PGDN", "selectnode"),
      IupItem ("NEXT", "selectnode"),
      IupItem ("PREVIOUS", "selectnode"),
      NULL)),
    IupSubmenu("Mark", IupMenu(
      IupItem ("INVERT", "selectnode"),
      IupItem ("BLOCK", "selectnode"),
      IupItem ("CLEARALL", "selectnode"),
      IupItem ("MARKALL", "selectnode"),
      IupItem ("INVERTALL", "selectnode"),
      NULL)),
    NULL);
    
  IupSetFunction("nodeinfo", (Icallback) nodeinfo);
  IupSetFunction("selectnode", (Icallback) selectnode);
  IupSetFunction("addleaf",    (Icallback) addleaf);
  IupSetFunction("addbranch",  (Icallback) addbranch);
  IupSetFunction("insertleaf",    (Icallback) insertleaf);
  IupSetFunction("insertbranch",  (Icallback) insertbranch);
  IupSetFunction("removenode", (Icallback) removenode);
  IupSetFunction("removechild", (Icallback) removechild);
  IupSetFunction("removemarked", (Icallback) removemarked);
  IupSetFunction("renamenode", (Icallback) renamenode);
  IupSetFunction("togglestate", (Icallback) togglestate);
  IupSetFunction("removeall", (Icallback) removeall);
  IupSetFunction("expandall", (Icallback) expandall);
  IupSetFunction("contractall", (Icallback) contractall);

//  sprintf(attr, "%d", id);
//  IupSetAttribute(ih, "VALUE", attr);
  IupPopup(popup_menu, IUP_MOUSEPOS, IUP_MOUSEPOS);

  IupDestroy(popup_menu);

  return IUP_DEFAULT;
}

static int active(Ihandle *ih)
{
  Ihandle* tree = IupGetHandle("tree");
  if (IupGetInt(tree, "ACTIVE"))
    IupSetAttribute(tree, "ACTIVE", "NO");
  else
    IupSetAttribute(tree, "ACTIVE", "YES");
  return IUP_DEFAULT;
}

static int next(Ihandle *ih)
{
  Ihandle* tree = IupGetHandle("tree");
  IupSetAttribute(tree, "VALUE",  "NEXT");
  return IUP_DEFAULT;
}

static int prev(Ihandle *ih)
{
  Ihandle* tree = IupGetHandle("tree");
  IupSetAttribute(tree, "VALUE",  "PREVIOUS");
  return IUP_DEFAULT;
}

static void init_tree_nodes(void)  
{
  Ihandle* tree = IupGetHandle("tree");

//  IupSetAttribute(tree, "AUTOREDRAW", "No");
#if 0
  /* create from bottom to top */
  /* the current node is the ROOT */
  //IupSetAttribute(tree, "VALUE", "0");
  IupSetAttribute(tree, "TITLE",         "Figures");  /* title of the root, id=0 */
  IupSetAttribute(tree, "ADDBRANCH",    "3D");    /* 3D=1 */
  IupSetAttribute(tree, "ADDLEAF",    "2D");    /* add to the root, so it will be before "3D", now 2D=1, 3D=2 */
  IupSetAttribute(tree, "ADDBRANCH",   "parallelogram"); /* id=1 */ 
  IupSetAttribute(tree, "ADDLEAF1",     "diamond");
  IupSetAttribute(tree, "ADDLEAF1",     "square");
  IupSetAttribute(tree, "ADDBRANCH",   "triangle");       
  IupSetAttribute(tree, "ADDLEAF1",     "scalenus");
  IupSetAttribute(tree, "ADDLEAF1",     "isoceles");
  IupSetAttribute(tree, "ADDLEAF1",     "equilateral");
  IupSetAttribute(tree, "ADDLEAF",      "Other (чущ)");
#else
  /* create from top to bottom */
  IupSetAttribute(tree, "TITLE0",         "Figures");  
  IupSetAttribute(tree, "ADDLEAF0",      "Other");     /* new id=1 */
  IupSetAttribute(tree, "ADDBRANCH1",   "triangle");  /* new id=2 */     
  IupSetAttribute(tree, "ADDLEAF2",     "equilateral");  /* ... */
  IupSetAttribute(tree, "ADDLEAF3",     "isoceles");
  IupSetAttribute(tree, "ADDLEAF4",     "scalenus");
  IupSetAttribute(tree, "STATE2",     "collapsed");
  IupSetAttribute(tree, "INSERTBRANCH2","parallelogram");  /* same depth as id=2, new id=6 */
  IupSetAttribute(tree, "ADDLEAF6",     "square");
  IupSetAttribute(tree, "ADDLEAF7",     "diamond");
  IupSetAttribute(tree, "INSERTLEAF6","2D");  /* new id=9 */
  IupSetAttribute(tree, "INSERTBRANCH9","3D");
#endif
  IupSetAttribute(tree, "TOGGLEVALUE2", "ON");
  IupSetAttribute(tree, "TOGGLEVALUE6", "ON");
  IupSetAttribute(tree, "TOGGLEVALUE9", "NOTDEF");
  IupSetAttribute(tree, "TOGGLEVALUE2", "OFF");
//  IupSetAttribute(tree, "AUTOREDRAW", "Yes");

  IupSetAttribute(tree, "MARKED1", "Yes");
  IupSetAttribute(tree, "MARKED8", "Yes");
  IupSetAttribute(tree, "VALUE",        "6");

  IupSetAttribute(tree, "RASTERSIZE", NULL);   /* remove the minimum size limitation */
  IupSetAttribute(tree, "COLOR8", "92 92 255");
//  IupSetAttribute(tree, "TITLEFONT8", "Courier, 14");
  IupSetAttributeHandle(tree, "IMAGE8", load_image_LogoTecgraf());
  IupSetAttributeHandle(tree, "IMAGE7", load_image_TestImage());
  IupSetAttribute(tree, "IMAGE6", IupGetAttribute(tree, "IMAGE8"));
}

/* Initializes IupTree and registers callbacks */
static void init_tree(void)
{
  Ihandle* tree = IupTree(); 

  IupSetCallback(tree, "EXECUTELEAF_CB", (Icallback) executeleaf_cb);
  IupSetCallback(tree, "RENAME_CB",      (Icallback) rename_cb);
  IupSetCallback(tree, "BRANCHCLOSE_CB", (Icallback) branchclose_cb);
  IupSetCallback(tree, "BRANCHOPEN_CB",  (Icallback) branchopen_cb);
  IupSetCallback(tree, "DRAGDROP_CB",    (Icallback) dragdrop_cb);
  IupSetCallback(tree, "RIGHTCLICK_CB",  (Icallback) rightclick_cb);
  IupSetCallback(tree, "K_ANY",          (Icallback) k_any_cb);
  IupSetCallback(tree, "SHOWRENAME_CB", (Icallback) showrename_cb);
  IupSetCallback(tree, "SELECTION_CB", (Icallback) selection_cb);
//  IupSetCallback(tree, "MULTISELECTION_CB", (Icallback) multiselection_cb);
//  IupSetCallback(tree, "MULTIUNSELECTION_CB", (Icallback) multiunselection_cb);
  IupSetCallback(tree, "GETFOCUS_CB", (Icallback) getfocus_cb);
  IupSetCallback(tree, "KILLFOCUS_CB", (Icallback) killfocus_cb);
  //IupSetCallback(tree, "ENTERWINDOW_CB", (Icallback) enterwindow_cb);
  //IupSetCallback(tree, "LEAVEWINDOW_CB", (Icallback)leavewindow_cb);
  //IupSetCallback(tree, "BUTTON_CB",    (Icallback)button_cb);
  //IupSetCallback(tree, "MOTION_CB",    (Icallback)motion_cb);
  IupSetCallback(tree, "NODEREMOVED_CB", (Icallback)noderemoved_cb);
  IupSetCallback(tree, "TOGGLEVALUE_CB", (Icallback)togglevalue_cb);

  IupSetCallback(tree, "HELP_CB", (Icallback)help_cb);

//  IupSetAttribute(tree, "FONT", "COURIER_NORMAL_14");
//  IupSetAttribute(tree, "FGCOLOR", "255 0 0");
//  IupSetAttribute(tree, "SPACING",   "10");
//  IupSetAttribute(tree, "BGCOLOR", "255 255 255");
//  IupSetAttribute(tree, "BGCOLOR", "128 0 255");

//  IupSetAttribute(tree, "MARKMODE",     "MULTIPLE");
//  IupSetAttribute(tree, "SHOWRENAME",   "YES");
  IupSetAttribute(tree, "SHOWDRAGDROP", "YES");
//  IupSetAttribute(tree, "SHOWTOGGLE",   "YES");
//  IupSetAttribute(tree, "SHOWTOGGLE",   "3STATE");
//  IupSetAttribute(tree, "DROPEQUALDRAG", "YES");

  IupSetAttribute(tree, "ADDEXPANDED",  "YES");
//  IupSetAttribute(tree, "HIDELINES",    "YES");
//  IupSetAttribute(tree, "HIDEBUTTONS",    "YES");
//  IupSetAttribute(tree, "INDENTATION",   "40");
//  IupSetAttribute(tree, "CANFOCUS", "NO");
  IupSetAttribute(tree, "TIP", "Tree Tip");
//  IupSetAttribute(tree, "TIPBGCOLOR", "255 128 128");
//  IupSetAttribute(tree, "TIPFGCOLOR", "0 92 255");

  // Windows Only  
//  IupSetAttribute(tree, "TIPBALLOON", "YES");
//  IupSetAttribute(tree, "TIPBALLOONTITLE", "Tip Title");
//  IupSetAttribute(tree, "TIPBALLOONTITLEICON", "2");

//  IupSetAttribute(tree, "ADDROOT", "NO");

  IupSetHandle("tree", tree);
}

/* Initializes the dlg */
static void init_dlg(void)
{
  Ihandle* butactv, *butnext, *butprev, *butmenu;
  Ihandle* tree = IupGetHandle("tree");
  Ihandle* box = IupHbox(tree, IupVbox(butactv = IupButton("Active", NULL), 
                                       butnext = IupButton("Next", NULL), 
                                       butprev = IupButton("Prev", NULL), 
                                       butmenu = IupButton("Menu", NULL), 
                                       NULL), NULL);
  Ihandle* dlg = IupDialog(box) ;
  IupSetAttribute(dlg,  "TITLE",   "IupTree");
  IupSetAttribute(box,  "MARGIN",  "10x10");
  IupSetAttribute(box,  "GAP",  "10");
//  IupSetAttribute(box, "BGCOLOR", "92 92 255");
//  IupSetAttribute(dlg, "BGCOLOR", "92 92 255");
//  IupSetAttribute(dlg, "BACKGROUND", "200 10 80");
//  IupSetAttribute(dlg, "BGCOLOR", "173 177 194");  // Motif BGCOLOR for documentation
  IupSetCallback(butactv, "ACTION", active);
  IupSetCallback(butnext, "ACTION", next);
  IupSetCallback(butprev, "ACTION", prev);
  IupSetCallback(butmenu, "ACTION", (Icallback)rightclick_cb);

  IupSetHandle("dlg", dlg);
}

void TreeTest(void)
{
  Ihandle* dlg;
  
  init_tree();                            /* Initializes IupTree */
  init_dlg();                             /* Initializes the dlg */
  dlg = IupGetHandle("dlg");              /* Retrieves the dlg handle */
  IupShowXY(dlg, IUP_CENTER, IUP_CENTER); /* Displays the dlg */
  init_tree_nodes();                  /* Initializes attributes, can be done here or anywhere */
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);
  IupControlsOpen();

  TreeTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
