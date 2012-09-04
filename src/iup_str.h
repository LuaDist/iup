/** \file
 * \brief String Utilities
 *
 * See Copyright Notice in "iup.h"
 */

 
#ifndef __IUP_STR_H 
#define __IUP_STR_H

#ifdef __cplusplus
extern "C" {
#endif


/** \defgroup str String Utilities
 * \par
 * See \ref iup_str.h
 * \ingroup util */

/** Returns a non zero value if the two strings are equal.
 * str1 or str2 can be NULL.
 * \ingroup str */
int iupStrEqual(const char* str1, const char* str2);

/** Returns a non zero value if the two strings are equal but ignores case.
 * str1 or str2 can be NULL.
 * \ingroup str */
int iupStrEqualNoCase(const char* str1, const char* str2);

/** Returns a non zero value if the two strings are equal 
 * up to a number of characters defined by the strlen of the second string.
 * str1 or str2 can be NULL.
 * \ingroup str */
int iupStrEqualPartial(const char* str1, const char* str2);

/** Returns a non zero value if the two strings are equal but ignores case 
 * up to a number of characters defined by the strlen of the second string.
 * str1 or str2 can be NULL.
 * \ingroup str */
int iupStrEqualNoCasePartial(const char* str1, const char* str2);

/** Returns 1 if the string is "1", "YES", "ON" or "TRUE". \n
 * Returns 0 otherwise.
 * \ingroup str */
int iupStrBoolean(const char* str);

/** Returns 1 if the string is "NO", "OFF" or "FALSE". \n
 * Returns 0 otherwise.
 * \ingroup str */
int iupStrFalse(const char* str);

/** Returns the number of lines in a string.
 * It works for UNIX, DOS and MAC line ends.
 * \ingroup str */
int iupStrLineCount(const char* str);

/** Returns the a pointer to the next line and the size of the current line.
 * It works for UNIX, DOS and MAC line ends. The size does not includes the line end.
 * If str is NULL it will return NULL.
 * \ingroup str */
const char* iupStrNextLine(const char* str, int *len);

/** Returns the number of repetitions of the character occours in the string.
 * \ingroup str */
int iupStrCountChar(const char *str, int c);

/** Returns a copy of the given string.
 * If str is NULL it will return NULL.
 * \ingroup str */
char* iupStrDup(const char* str); 

/** Returns a new string containing a copy of the string up to the character.
 * The string is then incremented to after the position of the character.
 * \ingroup str */
char *iupStrCopyUntil(char **str, int c);

/** Copy the string to the buffer, but limited to the max_size of the buffer.
 * buffer is always properly ended.
 * \ingroup str */
void iupStrCopyN(char* dst_str, int dst_max_size, const char* src_str);

/** Returns a buffer with the specified size+1. \n
 * The buffer is resused after 50 calls. It must NOT be freed.
 * Use size=-1 to free all the internal buffers.
 * \ingroup str */
char *iupStrGetMemory(int size);

/** Returns a buffer that contains a copy of the given buffer using \ref iupStrGetMemory.
 * \ingroup str */
char *iupStrGetMemoryCopy(const char* str);

/** Returns a very large buffer to be used in unknown size string construction.
 * Use snprintf or vsnprintf with the given size.
 * \ingroup str */
char *iupStrGetLargeMem(int *size);

/** Converts a string into lower case. Can be used in-place.
 * \ingroup str */
void iupStrLower(char* dstr, const char* sstr);

/** Converts a string into upper case. Can be used in-place.
 * \ingroup str */
void iupStrUpper(char* dstr, const char* sstr);

/** Checks if the string has at least 1 space character.
 * \ingroup str */
int iupStrHasSpace(const char* str);

/** Extract a RGB triple from the string. Returns 0 or 1.
 * \ingroup str */
int iupStrToRGB(const char *str, unsigned char *r, unsigned char *g, unsigned char *b);

/** Extract a RGBA quad from the string, alpha is optional. Returns 0, 3 or 4.
 * \ingroup str */
int iupStrToRGBA(const char *str, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a);

/** Converts the string to an int. The string must contains only the integer value.
 * Returns a a non zero value if sucessfull.
 * \ingroup str */
int iupStrToInt(const char *str, int *i);

/** Converts the string to two int. The string must contains two integer values in sequence, 
 * separated by the given character (usually 'x' or ':').
 * Returns the number of converted values.
 * Values not extracted are not changed.
 * \ingroup str */
int iupStrToIntInt(const char *str, int *i1, int *i2, char sep);

/** Converts the string to an float. The string must contains only the real value.
 * Returns a a non zero value if sucessfull.
 * \ingroup str */
int iupStrToFloat(const char *str, float *f);

/** Converts the string to two float. The string must contains two real values in sequence, 
 * separated by the given character (usually 'x' or ':').
 * Returns the number of converted values.
 * Values not extracted are not changed.
 * \ingroup str */
int iupStrToFloatFloat(const char *str, float *f1, float *f2, char sep);

/** Extract two strings from the string.
 * separated by the given character (usually 'x' or ':').
 * Returns the number of converted values.
 * Values not extracted are not changed.
 * \ingroup str */
int iupStrToStrStr(const char *str, char *str1, char *str2, char sep);

/** Returns the file extension of a file name.
 * Supports UNIX and Windows directory separators.
 * \ingroup str */
char* iupStrFileGetExt(const char *file_name);

/** Returns the file title of a file name.
 * Supports UNIX and Windows directory separators.
 * \ingroup str */
char* iupStrFileGetTitle(const char *file_name);

/** Returns the file path of a file name.
 * Supports UNIX and Windows directory separators.
 * \ingroup str */
char* iupStrFileGetPath(const char *file_name);

/** Concat path and title addind '/' between if path does not have it.
 * \ingroup str */
char* iupStrFileMakeFileName(const char* path, const char* title);

/** Split the filename in path and title using pre-alocated strings.
 * \ingroup str */
void iupStrFileNameSplit(const char* filename, char* path, char* title);

/** Replace a character in a string.
 * Returns the number of occurrences.
 * \ingroup str */
int iupStrReplace(char* str, char src, char dst);

/** Convert line ends to UNIX format in-place (one \n per line).
 * \ingroup str */
void iupStrToUnix(char* str);

/** Convert line ends to MAC format in-place (one \r per line).
 * \ingroup str */
void iupStrToMac(char* str);

/** Convert line ends to DOS/Windows format (the sequence \r\n per line).
 * If returned pointer different the input, it must be freed.
 * \ingroup str */
char* iupStrToDos(const char* str);

/** Convert string to C format. Process \n, \r and \t.
 * If returned pointer different the input, it must be freed.
 * \ingroup str */
char* iupStrConvertToC(const char* str);

/** Remove the interval from the string. Done in-place.
 * \ingroup str */
void iupStrRemove(char* value, int start, int end, int dir);

/** Remove the interval from the string and insert the new string at the start.
 * \ingroup str */
char* iupStrInsert(const char* value, const char* insert_value, int start, int end);

/** Process the mnemonic in the string. If not found returns str.
 * If found returns a new string. Action can be:
- 1: replace & by c
- -1: remove & and return in c
- 0: remove &
 * \ingroup str */
char* iupStrProcessMnemonic(const char* str, char *c, int action);

#ifdef __cplusplus
}
#endif

#endif
