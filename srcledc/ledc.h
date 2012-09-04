/** \file
 * \brief LEDC definitions.
 *
 * See Copyright Notice in iup.h
 * $Id: ledc.h,v 1.4 2012/03/30 16:26:20 scuri Exp $
 */
 
#ifndef __LEDC_H 
#define __LEDC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _elemlist {
  struct _elemlist *next;
  void         *data;
} Telemlist;

typedef struct {
  Telemlist* first;
  int size;
} Tlist;

typedef struct {
  char *name;
  char *value;
} Tattr;

typedef struct _Telem  Telem;
typedef struct _Tparam Tparam;

struct _Telem {
  char*   name;
  char*   elemname;
  Tlist*  attrs;
  Tparam**params;
  int     nparams;
  int     elemidx;
  char   *codename;
  union {
    struct { int w, h, d; } image;
  } data;
};

struct _Tparam {
  enum { NAME_PARAM, STRING_PARAM, ELEM_PARAM } tag;
  union {
    char  *name;
    Telem *elem;
  } data;
};

Tattr*  attr( char* name, char* value );
Tparam* param( int tag, void* value );
Telem*  elem( char* name, Tlist* attrs, Tlist* params );
Tlist*  list( void );
Tlist*  addlist( Tlist* l, void* data );
Tlist*  revertlist( Tlist* l );
Tparam** list2paramvector( Tlist* l );
void    cleanlist( Tlist* l );

void decl( Telem* e );

void checkused( char* name );
void named( char* name );

void init(void);
void finish(void);

void error( char* fmt, ... );

extern int yylineno;

extern char* filename;
extern char* outname;
extern char* funcname;
extern int   nocode;
extern int   static_image;

#ifdef __cplusplus
}
#endif

#endif
