/** \file
 * \brief Attributes Environment Management
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_ATTRIB_H 
#define __IUP_ATTRIB_H

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup attrib Attribute Environment 
 * \par
 * When attributes are not stored at the control  
 * they are stored in a hash table (see \ref table).
 * \par
 * As a general rule use:
 * - IupGetAttribute, IupSetAttribute, ... : when care about control implementation, hash table, inheritance and default value
 * - iupAttribGetStr,Int,Float: when care about inheritance, hash table and default value
 * - iupAttribGet,... : ONLY access the hash table
 * These different functions have very different performances and results. So use them wiselly.
 * \par
 * See \ref iup_attrib.h 
 * \ingroup cpi */


/** Returns true if the attribute name if in the internal format "_IUP...".
 * \ingroup attrib */
#define iupATTRIB_ISINTERNAL(_name) ((_name[0] == '_' && _name[1] == 'I' && _name[2] == 'U' && _name[3] == 'P')? 1: 0)

/** Returns true if the attribute name is a known pointer.
 * \ingroup attrib */
int iupAttribIsPointer(Ihandle* ih, const char *name);

/** Sets the attribute only in the hash table as a pointer.
 * It ignores children.
 * \ingroup attrib */
void iupAttribSetStr(Ihandle* ih, const char* name, const char* value);

/** Sets the attribute only in the hash table as a string. 
 * The string is internally duplicated.
 * It ignores children.
 * \ingroup attrib */
void iupAttribStoreStr(Ihandle* ih, const char* name, const char* value);

/** Sets the attribute only in the hash table as a string. 
 * The string is internally duplicated. Use same format as sprintf.
 * It ignores children.
 * \ingroup attrib */
void iupAttribSetStrf(Ihandle *ih, const char* name, const char* format, ...);

/** Sets an integer attribute only in the hash table.
 * It will be stored as a string.
 * It ignores children.
 * \ingroup attrib */
void iupAttribSetInt(Ihandle *ih, const char* name, int num);

/** Sets an floating point attribute only in the hash table.
 * It will be stored as a string.
 * It ignores children.
 * \ingroup attrib */
void iupAttribSetFloat(Ihandle *ih, const char* name, float num);

/** Returns the attribute from the hash table only. 
 * NO inheritance, NO control implementation, NO defalt value here.
 * \ingroup attrib */
char* iupAttribGet(Ihandle* ih, const char* name);

/** Returns the attribute from the hash table only, 
 * but if not defined then checks in its parent tree.
 * NO control implementation, NO defalt value here.
 * \ingroup attrib */
char* iupAttribGetInherit(Ihandle* ih, const char* name);

/** Returns the attribute from the hash table of a native parent.
 * Don't check for default values. Don't check at the element.
 * Used for BGCOLOR and BACKGROUND attributes.
 * \ingroup attrib */
char* iupAttribGetInheritNativeParent(Ihandle* ih, const char* name);

/** Returns the attribute from the hash table as a string, 
 * but if not defined then checks in its parent tree if allowed by the control implementation, 
 * if still not defined then returns the registered default value if any.
 * NO control implementation, only checks inheritance and default value from it.
 * \ingroup attrib */
char* iupAttribGetStr(Ihandle* ih, const char* name);   

/** Returns the attribute from the hash table as a string, 
 * but if not defined then checks in the control implementation, 
 * if still not defined then returns the registered default value if any. \n
 * NO inheritance here. Used only in the IupLayoutDialog.
 * \ingroup attrib */
char* iupAttribGetLocal(Ihandle* ih, const char* name);

/** Same as \ref iupAttribGetStr but returns an integer number.
 * Checks also for boolean values.
 * \ingroup attrib */
int iupAttribGetInt(Ihandle* ih, const char* name);

/** Same as \ref iupAttribGetStr but checks for boolean values.
 * Use \ref iupStrBoolean.
 * \ingroup attrib */
int iupAttribGetBoolean(Ihandle* ih, const char* name);

/** Same as \ref iupAttribGetStr but returns an floating point number.
 * \ingroup attrib */
float iupAttribGetFloat(Ihandle* ih, const char* name);

/** Set an internal name to a handle.
 * \ingroup attrib */
void iupAttribSetHandleName(Ihandle *ih);

/** Returns the internal name if set.
 * \ingroup attrib */
char* iupAttribGetHandleName(Ihandle *ih);


/* For all attributes in the evironment, call the class SetAttribute only.
 * Called only after the element is mapped, but before the children are mapped. */
void iupAttribUpdate(Ihandle* ih); 

/* For all registered inherited attributes, checks the parent tree and 
 * call the class SetAttribute if the attribute is defined.
 * Called only after the element is mapped, but before the children are mapped. */
void iupAttribUpdateFromParent(Ihandle* ih);

/* For all attributes in the evironment, call the class SetAttribute only for the children.
 * Called only after the element is mapped, and after the children are mapped. */
void iupAttribUpdateChildren(Ihandle* ih);


/* Other functions declared in <iup.h> and implemented here. 
IupGetAllAttributes
IupGetAttributes
IupSetAttributes
IupSetAttribute
IupStoreAttribute
IupGetAttribute
IupGetFloat
IupGetInt
IupGetInt2
IupSetfAttribute
IupSetAttributeHandle
IupGetAttributeHandle
*/


#ifdef __cplusplus
}
#endif

#endif
