/** \file
 * \brief Ihandle <-> Name table manager.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>

#include "iup.h"

#include "iup_str.h"
#include "iup_table.h"
#include "iup_names.h"
#include "iup_object.h"
#include "iup_class.h"
#include "iup_assert.h"
#include "iup_attrib.h"
#include "iup_str.h"


static Itable *inames_strtable = NULL;   /* table indexed by name containing Ihandle* address */

void iupNamesInit(void)
{
  inames_strtable = iupTableCreate(IUPTABLE_STRINGINDEXED);
}

void iupNamesFinish(void)
{
  iupTableDestroy(inames_strtable);
  inames_strtable = NULL;
}

static Ihandle* iNameGetTopParent(Ihandle* ih)
{
  Ihandle* parent = ih;
  while (parent->parent)
    parent = parent->parent;
  return parent;
}

static int iNameCheckArray(Ihandle** ih_array, int count, Ihandle* ih)
{
  int i;
  for (i = 0; i < count; i++)
  {
    if (ih_array[i] == ih)
      return 0;
  }
  return 1;
}

void iupNamesDestroyHandles(void)
{
  char *name;
  Ihandle** ih_array, *ih;
  int count, i = 0;

  count = iupTableCount(inames_strtable);
  if (!count)
    return;

  ih_array = (Ihandle**)malloc(count * sizeof(Ihandle*));

  /* store the names before updating so we can remove elements in the loop */
  name = iupTableFirst(inames_strtable);
  while (name)
  {
    ih = (Ihandle*)iupTableGetCurr(inames_strtable);
    if (iupObjectCheck(ih))   /* here must be a handle */
    {
      ih = iNameGetTopParent(ih);
      if (iNameCheckArray(ih_array, i, ih))
      {
        ih_array[i] = ih;
        i++;
      }
    }
    name = iupTableNext(inames_strtable);
  }

  count = i;
  for (i = 0; i < count; i++)
  {
    if (iupObjectCheck(ih_array[i]))  /* here must be a handle */
      IupDestroy(ih_array[i]);
  }

  free(ih_array);
}

void iupRemoveNames(Ihandle* ih)
{
  char *name;

  /* clear the cache */
  name = iupAttribGet(ih, "_IUP_LASTHANDLENAME");
  if (name)
    iupTableRemove(inames_strtable, name);

  /* check for an internal name */
  name = iupAttribGetHandleName(ih);
  if (name)
    iupTableRemove(inames_strtable, name);

  /* Do NOT search for other names */
}

Ihandle *IupGetHandle(const char *name)
{
  if (!name) /* no iupASSERT needed here */
    return NULL;
  return (Ihandle*)iupTableGet (inames_strtable, name);
}

Ihandle* IupSetHandle(const char *name, Ihandle *ih)
{
  Ihandle *old_ih;

  iupASSERT(name!=NULL);
  if (!name)
    return NULL;

  old_ih = iupTableGet(inames_strtable, name);

  if (ih != NULL)
  {
    iupTableSet(inames_strtable, name, ih, IUPTABLE_POINTER);

    /* save the name in the cache if it is a valid handle */
    if (iupObjectCheck(ih))
      iupAttribStoreStr(ih, "_IUP_LASTHANDLENAME", name);
  }
  else
  {
    iupTableRemove(inames_strtable, name);

    /* clear the name from the cache if it is a valid handle */
    if (iupObjectCheck(old_ih))
      iupAttribSetStr(old_ih, "_IUP_LASTHANDLENAME", NULL);
  }

  return old_ih;
}

int IupGetAllNames(char** names, int n)
{
  int i = 0;
  char* name;

  if (!names || !n)
    return iupTableCount(inames_strtable);

  name = iupTableFirst(inames_strtable);
  while (name)
  {
    names[i] = name;
    i++;
    if (i == n)
      break;

    name = iupTableNext(inames_strtable);
  }
  return i;
}

static int iNamesCountDialogs(void)
{
  int i = 0;
  char* name = iupTableFirst(inames_strtable);
  while (name)
  {
    Ihandle* dlg = (Ihandle*)iupTableGetCurr(inames_strtable);
    if (iupObjectCheck(dlg) &&  /* here must be a handle */
        dlg->iclass->nativetype == IUP_TYPEDIALOG)
      i++;

    name = iupTableNext(inames_strtable);
  }
  return i;
}

int IupGetAllDialogs(char** names, int n)
{
  int i = 0;
  char* name;

  if (!names || !n)
    return iNamesCountDialogs();

  name = iupTableFirst(inames_strtable);
  while (name)
  {
    Ihandle* dlg = (Ihandle*)iupTableGetCurr(inames_strtable);
    if (iupObjectCheck(dlg) &&  /* here must be a handle */
        dlg->iclass->nativetype == IUP_TYPEDIALOG)
    {
      names[i] = name;
      i++;
      if (i == n)
        break;
    }

    name = iupTableNext(inames_strtable);
  }
  return i;
}

char* IupGetName(Ihandle* ih)
{
  char *name;
  if (!ih) /* no iupASSERT needed here */
    return NULL;

  if (iupObjectCheck(ih))
  {
    /* check the cache first, but must be a handle */
    name = iupAttribGet(ih, "_IUP_LASTHANDLENAME");
    if (name)
      return name;
  }

  /* check for an internal name */
  name = iupAttribGetHandleName(ih);
  if (name)
    return name;
                               
  /* search for the name */
  name = iupTableFirst(inames_strtable);
  while (name)
  {
    if ((Ihandle*)iupTableGetCurr(inames_strtable) == ih)
      return name;

    name = iupTableNext(inames_strtable);
  }

  return NULL;
}
