/** \file
 * \brief IUPLua5 internal Functions
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IL_H 
#define __IL_H

#ifdef __cplusplus
extern "C" {
#endif

int iuplistlua_open (lua_State * L);
int iuplabellua_open (lua_State * L);
int iupitemlua_open (lua_State * L);
int iupimagelua_open (lua_State * L);
int iuphboxlua_open (lua_State * L);
int iupframelua_open (lua_State * L);
int iupfilllua_open (lua_State * L);
int iupfiledlglua_open (lua_State * L);
int iupdialoglua_open (lua_State * L);
int iupcanvaslua_open (lua_State * L);
int iupbuttonlua_open (lua_State * L);
int iupzboxlua_open (lua_State * L);
int iupvboxlua_open (lua_State * L);
int iuptogglelua_open (lua_State * L);
int iuptimerlua_open (lua_State * L);
int iuptextlua_open (lua_State * L);
int iupsubmenulua_open (lua_State * L);
int iupseparatorlua_open (lua_State * L);
int iupradiolua_open (lua_State * L);
int iupmultilinelua_open (lua_State * L);
int iupmenulua_open (lua_State * L);
int iupcboxlua_open (lua_State * L);
int iupspinboxlua_open (lua_State * L);
int iupspinlua_open (lua_State * L);
int iupsboxlua_open (lua_State * L);
int iupsplitlua_open (lua_State * L);
int iupgclua_open (lua_State * L);
int iupvallua_open (lua_State * L);
int iuptabslua_open (lua_State * L);
int iupfontdlglua_open(lua_State * L);
int iupmessagedlglua_open(lua_State * L);
int iupcolordlglua_open(lua_State * L);
int iupimagergbalua_open(lua_State * L);
int iupimagergblua_open(lua_State * L);
int iupprogressbarlua_open(lua_State * L);
int iupnormalizerlua_open(lua_State * L);
int iupuserlua_open(lua_State * L);
int iuptreelua_open(lua_State * L);
int iupclipboardlua_open(lua_State * L);

void iupgetparamlua_open (lua_State * L);

int iupluaScanf(lua_State *L);
void iupluaapi_open(lua_State * L);


            /*     Registration    */

/** Gets the global enviroment "iup".
    Used by secondary iuplua modules.
 */
void iuplua_get_env(lua_State *L);

/** Returns true if IupOpen was called from inside the IupLua initialization function (iuplua_open).
 * All the other C intialization functions are called from the respective IupLua initialization functions.
 */
int iuplua_opencall_internal(lua_State *L);

/** Register a function.
 */
void iuplua_register(lua_State *L, lua_CFunction func, const char* name);

/** Register a string.
 */
void iuplua_regstring(lua_State *L, const char* str, const char* name);


          /*              Callbacks             */

/** Stores the Lua State inside the IUP element,
 * so it can be retreived from inside a callback.
 * Used in the creation of the element.
 */
void iuplua_plugstate(lua_State *L, Ihandle *ih);

/** Retrieve the Lua State. Used inside a callback.
 */
lua_State* iuplua_getstate(Ihandle *ih);

/** First function called inside a callback. 
 * It prepares the stack for the call and returns the Lua state.
 */
lua_State * iuplua_call_start(Ihandle *ih, const char* name);

/** Called when a callback in Lua should be called. 
 * nargs is the number of arguments after the first Ihandle*.
 * Returns the callback return value. If nil returns IUP_DEFAULT.
 * At the end clears the Lua stack. 
 */
int iuplua_call(lua_State *L, int nargs);

/** Same as /ref iuplua_call, but returns a string.
 */
char* iuplua_call_rs(lua_State *L, int nargs);

/** Same as lua_pcall, but if an error occour then
 * _ERRORMESSAGE is called. 
 */
int iuplua_call_raw(lua_State* L, int nargs, int nresults);

/** Register the callback in the iup.callbaks table.
 * If type is not NULL, register in a sub table with the controls class name.
 */
void iuplua_register_cb(lua_State *L, const char* name, lua_CFunction func, const char* type);


            /*     Functions                     */

/** Pushes a raw Ihandle into the stack (an userdata).
 * Used by the control creation functions.
 */
void iuplua_pushihandle_raw(lua_State *L, Ihandle *h);

/** Returns an array of string stored in a Lua table. 
 If count is 0, table size is used. Else table size must match count. */
char** iuplua_checkstring_array(lua_State *L, int pos, int count);

/** Returns an array of int stored in a Lua table.
 If count is 0, table size is used. Else table size must match count. */
int* iuplua_checkint_array(lua_State *L, int pos, int count);

/** Returns an array of float stored in a Lua table.
 If count is 0, table size is used. Else table size must match count. */
float* iuplua_checkfloat_array(lua_State *L, int pos, int count);

/** Returns an array of unsigned char stored in a Lua table.
 If count is 0, table size is used. Else table size must match count. */
unsigned char* iuplua_checkuchar_array(lua_State *L, int pos, int count);

/** Returns an array of ihandle stored in a Lua table.
 If count is 0, table size is used. Else table size must match count. */
Ihandle** iuplua_checkihandle_array(lua_State *L, int pos, int count);

/** Same as iuplua_checkihandle but also accepts nil. */
Ihandle *iuplua_checkihandleornil(lua_State *L, int pos);

#if LUA_VERSION_NUM > 501
#define iuplua_getn(L,i)          ((int)lua_rawlen(L, i))
#else
#define LUA_OK		0
#define iuplua_getn(L,i)          ((int)lua_objlen(L, i))
#endif


#ifdef __cplusplus
}
#endif

#endif
