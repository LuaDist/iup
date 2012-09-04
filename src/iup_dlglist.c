/** \file
 * \brief list of all created dialogs
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>

#include "iup.h"

#include "iup_dlglist.h"
#include "iup_object.h"
#include "iup_assert.h"


typedef struct Idiallst_
{
  Ihandle *ih;
  struct Idiallst_ *next;
} Idiallst;

static Idiallst *idlglist = NULL;  /* list of all created dialogs */
static int idlg_count = 0;

void iupDlgListAdd(Ihandle *ih)
{
  if (ih)
  {
    Idiallst *p=(Idiallst *)malloc(sizeof(Idiallst));
    if (!p)
      return;
    p->ih = ih;
    p->next = idlglist;
    idlglist = p;
    idlg_count++;
  }
}

void iupDlgListRemove(Ihandle *ih)
{
  if (!idlglist || !ih)
    return;

  if (idlglist->ih == ih)    /* ih is header */
  {
    Idiallst *p = idlglist->next;
    free(idlglist);
    idlglist = p;
    idlg_count--;
  }
  else
  {
    Idiallst *p;    /* current pointer */
    Idiallst *b;    /* before pointer */
    for (b = idlglist, p = idlglist->next; p; b = p, p = p->next)
    {
      if (p->ih == ih)
      {
        b->next = p->next;
        free (p);
        idlg_count--;
        return;
      }
    }
  }
}

static Idiallst *idlg_first = NULL;

int iupDlgListCount(void)
{
  return idlg_count;
}

Ihandle *iupDlgListFirst (void)
{
  idlg_first = idlglist;
  return iupDlgListNext();
}

Ihandle *iupDlgListNext (void)
{
  Ihandle *ih = NULL;
  if (idlg_first)
  {
    ih = idlg_first->ih;
    idlg_first = idlg_first->next;
  }
  return ih;
}

static int idlg_nvisiblewin = 0;

void iupDlgListVisibleInc(void)
{
  iupASSERT(idlg_nvisiblewin < idlg_count);
  if (idlg_nvisiblewin == idlg_count)
    return;
  idlg_nvisiblewin++;
}

void iupDlgListVisibleDec(void)
{
  iupASSERT(idlg_nvisiblewin > 0);
  idlg_nvisiblewin--;
}

int iupDlgListVisibleCount(void)
{
  return idlg_nvisiblewin;
}

void iupDlgListDestroyAll(void)
{
  int i = 0, count;
  Ihandle** ih_array = (Ihandle**)malloc(idlg_count * sizeof(Ihandle*));
  Idiallst *list;
  for (list = idlglist; list; list = list->next)
  {
    if (iupObjectCheck(list->ih))
    {
      ih_array[i] = list->ih;
      i++;
    }
  }

  count = i;
  for (i = 0; i < count; i++)
  {
    if (iupObjectCheck(ih_array[i]))
      IupDestroy(ih_array[i]);   /* this will also destroy the list */
  }

  free(ih_array);
}
