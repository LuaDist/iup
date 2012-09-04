/** \file
 * \brief function table manager
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h> 

#include "iup.h"

#include "iup_str.h"
#include "iup_table.h"
#include "iup_func.h"
#include "iup_drv.h"
#include "iup_assert.h"


static Itable *ifunc_table = NULL;   /* the function hast table indexed by the name string */
static const char *ifunc_action_name = NULL;  /* name of the action being retrieved in IupGetFunction */

void iupFuncInit(void)
{
  ifunc_table = iupTableCreate(IUPTABLE_STRINGINDEXED);
}

void iupFuncFinish(void)
{
  iupTableDestroy(ifunc_table);
  ifunc_table = NULL;
}

const char *IupGetActionName(void)
{
  return ifunc_action_name;
}

Icallback IupGetFunction(const char *name)
{
  void* value;
  Icallback func;

  iupASSERT(name!=NULL);
  if (!name)
    return NULL;

  ifunc_action_name = name; /* store the retrieved name */

  func = (Icallback)iupTableGetFunc(ifunc_table, name, &value);

  /* if not defined and not the idle, then check for the DEFAULT_ACTION */
  if (!func && !iupStrEqual(name, "IDLE_ACTION"))
    func = (Icallback)iupTableGetFunc(ifunc_table, "DEFAULT_ACTION", &value);

  return func;
}

Icallback IupSetFunction(const char *name, Icallback func)
{
  void* value;
  Icallback old_func;

  iupASSERT(name!=NULL);
  if (!name)
    return NULL;

  old_func = (Icallback)iupTableGetFunc(ifunc_table, name, &value);

  if (!func)
    iupTableRemove(ifunc_table, name);
  else
    iupTableSetFunc(ifunc_table, name, (Ifunc)func);

  /* notifies the driver if changing the Idle */
  if (iupStrEqual(name, "IDLE_ACTION"))
    iupdrvSetIdleFunction(func);

  return old_func;
}
