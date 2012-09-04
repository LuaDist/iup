/** \file
 * \brief lexical analysis manager for LED.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_LEX_H 
#define __IUP_LEX_H

#ifdef __cplusplus
extern "C" {
#endif

/* TOKENS */
#define IUPLEX_TK_END          -1
#define IUPLEX_TK_BEGP         1
#define IUPLEX_TK_ENDP         2
#define IUPLEX_TK_ATTR         3
#define IUPLEX_TK_STR          4
#define IUPLEX_TK_NAME         5
#define IUPLEX_TK_NUMB         6
#define IUPLEX_TK_SET          7
#define IUPLEX_TK_COMMA        8
#define IUPLEX_TK_FUNC         9
#define IUPLEX_TK_ENDATTR     10

/* ERRORS */
#define IUPLEX_FILENOTOPENED   1
#define IUPLEX_NOTMATCH        2
#define IUPLEX_NOTENDATTR      3
#define IUPLEX_PARSEERROR      4

char*   iupLexGetError   (void);
int     iupLexStart      (const char *filename, int is_file);
void    iupLexClose      (void);
int     iupLexLookAhead  (void);
int     iupLexAdvance    (void);
int     iupLexFollowedBy (int t);
int     iupLexMatch      (int t);
int     iupLexSeenMatch  (int t, int *erro);
unsigned char iupLexByte (void);
int     iupLexInt        (void);
float   iupLexFloat      (void);
char*   iupLexGetName    (void);
char*   iupLexName       (void);
float   iupLexGetNumber  (void);
int     iupLexError      (int n, ...);
Iclass* iupLexGetClass   (void);

#ifdef __cplusplus
}
#endif

#endif
