#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <iup.h>

#include "../src/iup_class.h"
#include "../src/iup_register.h"

#define MAX_ITEMS 200

static int compare(const void *a, const void *b)
{
  return strcmp( * ( char** ) a, * ( char** ) b );
}

static char* getParameters(const char* format)
{
  static char str[100], *pstr;
  pstr = &str[0];

  if (format)
  {
    int i, count = strlen(format);

    for (i=0; i<count; i++)
    {
      char* fstr = NULL;

      switch(tolower(format[i]))
      {
      case 'b': fstr = "unsigned char"; break;
      case 'c': fstr = "unsigned char*"; break;
      case 'i': fstr = "int"; break;
      case 'j': fstr = "int*"; break;
      case 'f': fstr = "float"; break;
      case 'd': fstr = "double"; break;
      case 's': fstr = "char*"; break;
      case 'a': fstr = "char*"; break;
      case 'h': fstr = "Ihandle*"; break;
      case 'g': fstr = "Ihandle**"; break;
      case 'v': fstr = "void*"; break;
      }

      if (fstr)
      {
        pstr += sprintf(pstr, fstr);
        if (i != count-1 && count>1)
          pstr += sprintf(pstr, ", ");
      }
    }
    
  }
  else
    pstr += sprintf(pstr, "void");

  return str;
}

/* Update callback labels */
static int callbacksList_ActionCB (Ihandle *ih, char *callName, int pos, int state)
{
  if (state == 1)
  {
    Ihandle* listClasses = IupGetDialogChild(ih, "listClasses");
    Ihandle* labelInfo = IupGetDialogChild(ih, "labelInfo");
    char* className = IupGetAttribute(listClasses, IupGetAttribute(listClasses, "VALUE"));
    Iclass* ic = iupRegisterFindClass(className);
    char* format = iupClassCallbackGetFormat(ic, callName);
    const char* ret = strchr(format, '=');
    if (ret!=NULL)
      IupSetfAttribute(labelInfo, "TITLE", "%s %s(%s)", callName, getParameters(ret+1));
    else
      IupSetfAttribute(labelInfo, "TITLE", "Ihandle* %s(%s)", callName, getParameters(format));
  }

  (void)pos;
  return IUP_DEFAULT;
}

/* Update attribute labels */
static int attributesList_ActionCB (Ihandle *ih, char *attribName, int pos, int state)
{
  if (state == 1)
  {
    Ihandle* listClasses = IupGetDialogChild(ih, "listClasses");
    Ihandle* labelInfo = IupGetDialogChild(ih, "labelInfo");
    char* className = IupGetAttribute(listClasses, IupGetAttribute(listClasses, "VALUE"));
    Iclass* ic = iupRegisterFindClass(className);

    if (iupClassAttribIsRegistered(ic, attribName))
    {
      char* def_value;
      int flags;
      iupClassGetAttribNameInfo(ic, attribName, &def_value, &flags);

      IupSetfAttribute(labelInfo, "TITLE", "Attribute Name: %s\n"
                                           "Default Value: %s\n"
                                           "%s\n"
                                           "%s"
                                           "%s"
                                           "%s"
                                           "%s"
                                           "%s",
                                           attribName,
                                           def_value==NULL? "NULL": def_value,
                                           flags&(IUPAF_NO_INHERIT|IUPAF_NO_STRING)? "Is Inheritable": "NON Inheritable",
                                           flags&IUPAF_NO_STRING? "NOT a String\n": "",
                                           flags&IUPAF_HAS_ID? "Has ID\n": "",
                                           flags&IUPAF_READONLY? "Read-Only\n": (flags&IUPAF_WRITEONLY? "Write-Only\n": ""),
                                           flags&IUPAF_IHANDLENAME? "Ihandle* name\n": "",
                                           flags&IUPAF_NOT_SUPPORTED? "NOT SUPPORTED in this driver": "");
    }
    else
      IupSetAttribute(labelInfo, "TITLE", "Custom Attribute");
  }

  (void)pos;
  return IUP_DEFAULT;
}

static char* getNativeType(InativeType nativetype)
{
  char* str[] = {"VOID", "CONTROL", "CANVAS", "DIALOG", "IMAGE", "MENU"}; 
  return str[nativetype];
}

static char* getChildType(int childtype)
{
  char* str[] = {"NO CHILD", "MANY CHILDREN"}; 
  if (childtype > IUP_CHILDMANY)
  {
    static char buf[20];
    sprintf(buf, "%d CHILDREN", childtype-IUP_CHILDMANY);
    return buf;
  }
  else
    return str[childtype];
}

static int classesList_ActionCB (Ihandle *ih, char *className, int pos, int state)
{
  if (state == 1)
  {
    Iclass* ic;
    int i, attr_n, cb_n;
    Ihandle* listAttributes = IupGetDialogChild(ih, "listAttributes");
    Ihandle* listCallbacks = IupGetDialogChild(ih, "listCallbacks");
    Ihandle* labelInfo = IupGetDialogChild(ih, "labelInfo");
    char **attr_names = (char **) malloc(MAX_ITEMS * sizeof(char *));
    char **cb_names;
    
    attr_n = IupGetClassAttributes(className, attr_names, MAX_ITEMS);
    qsort(attr_names, attr_n, sizeof(char*), compare);

    cb_names = attr_names + attr_n;
    cb_n = IupGetClassCallbacks(className, cb_names, MAX_ITEMS);
    qsort(cb_names, cb_n, sizeof(char*), compare);

    /* Clear lists */
    IupSetAttribute(listAttributes, "REMOVEITEM", NULL);
    IupSetAttribute(listCallbacks,  "REMOVEITEM", NULL);

    /* Populate lists */
    for (i = 0; i < attr_n; i++)
      IupSetAttribute(listAttributes, "APPENDITEM", attr_names[i]);
    for (i = 0; i < cb_n; i++)
      IupSetAttribute(listCallbacks, "APPENDITEM", cb_names[i]);

    ic = iupRegisterFindClass(className);

    /* Update labels (values) */
    IupSetfAttribute(labelInfo, "TITLE", "Class Name: %s\n"
                                         "Creation Parameters: (%s)\n"
                                         "Native Type: %s\n"
                                         "Container Type: %s\n"
                                         "%s\n"
                                         "%s\n",
                                         ic->name,
                                         getParameters(ic->format),
                                         getNativeType(ic->nativetype),
                                         getChildType(ic->childtype),
                                         ic->is_interactive? "Is Interactive": "NOT Interactive",
                                         ic->has_attrib_id? "Has Id Attributes": "");

    free(attr_names);
  }

  (void)pos;
  return IUP_DEFAULT;
}

static void PopulateListOfClasses(Ihandle* ih)
{
  Ihandle* listClasses = IupGetDialogChild(ih, "listClasses");
  int i, n;
  char **list = (char **) malloc(MAX_ITEMS * sizeof(char *));

  n = IupGetAllClasses(list, MAX_ITEMS);
  
  qsort(list, n, sizeof(char*), compare);

  for(i = 0; i < n; i++)
    IupSetAttribute(listClasses, "APPENDITEM", list[i]);

  free(list);
}

void ClassInfo(void)
{
  Ihandle *dialog, *box, *lists, *listClasses, *listAttributes, *listCallbacks, *labelInfo;
  
  listClasses    = IupList(NULL);  /* list of registered classes */
  listAttributes = IupList(NULL);  /* list of attributes of the selected class */
  listCallbacks  = IupList(NULL);  /* list of  callbacks of the selected class */

  IupSetAttributes(listClasses,    "NAME=listClasses, SIZE= 70x85, EXPAND=VERTICAL");
  IupSetAttributes(listAttributes, "NAME=listAttributes, SIZE=120x85, EXPAND=VERTICAL");
  IupSetAttributes(listCallbacks,  "NAME=listCallbacks, SIZE=120x85, EXPAND=VERTICAL");

  IupSetCallback(listClasses,    "ACTION", (Icallback)    classesList_ActionCB);
  IupSetCallback(listAttributes, "ACTION", (Icallback) attributesList_ActionCB);
  IupSetCallback(listCallbacks,  "ACTION", (Icallback)  callbacksList_ActionCB);

  labelInfo = IupLabel(NULL);
  IupSetAttribute(labelInfo, "SIZE", "x50");
  IupSetAttribute(labelInfo, "EXPAND", "HORIZONTAL");
  IupSetAttribute(labelInfo, "NAME", "labelInfo");

  lists = IupVbox(
            IupHbox(
              IupSetAttributes(IupFrame(IupVbox(listClasses,    NULL)), "TITLE=Classes"),
              IupSetAttributes(IupFrame(IupVbox(listAttributes, NULL)), "TITLE=Attributes"),
              IupSetAttributes(IupFrame(IupVbox(listCallbacks,  NULL)), "TITLE=Callbacks"),
              NULL),
            IupHbox(
              IupSetAttributes(IupFrame(IupHbox(labelInfo, NULL)), "TITLE=Info, MARGIN=3x3"),
              NULL),
            NULL);

  box = IupHbox(lists, NULL);

	IupSetAttributes(lists,"MARGIN=10x10, GAP=10");

  dialog = IupDialog(box);

	IupSetAttribute(dialog, "TITLE", "Iup Classes Information");

  IupShowXY(dialog, IUP_CENTER, IUP_CENTER);

  PopulateListOfClasses(dialog);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  ClassInfo();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
