/** \file
 * \brief LED to C converter
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>

#include "ledc.h"

#define alloc(type)  ((type*)malloc(sizeof(type)))

static FILE* outfile = NULL;
static long  vetsizepos;

/* used in y.tab.c */
char *filename;
char *outname = 0;
char *funcname = 0;
int   nocode = 0;
int   static_image = 0;

typedef struct {
  char* name;
  int used;
} Tname;

static Tlist* all_names;
static Tlist* all_elems;
static Tlist* all_images;

static Tlist* all_named;
static Tlist* all_late;

static int nerrors = 0;

static struct {
  char* name;
  int used;
} headerfile[] = {
  { "iup",        1 },
  { "iupcontrols",0 },
  { "iupgl",      0 },
  { "iupole",     0 },
  { "iupweb",     0 },
  { "iup_pplot",  0 },
  { "iup_mglplot",  0 }
};
#define nheaders (sizeof(headerfile)/sizeof(headerfile[0]))

enum headers { 
  IUP_H, 
  IUPCONTROLS_H, 
  IUPGL_H, 
  IUPOLE_H,
  IUPWEB_H,
  IUPPPLOT_H,
  IUPMGLPLOT_H
};

static void check_empty( Telem* elem );
static void check_image( Telem* elem );
static void check_imagergb( Telem* elem );
static void check_imagergba( Telem* elem );
static void check_string( Telem* elem );
static void check_cb( Telem* elem );
static void check_elem( Telem* elem );
static void check_elemlist( Telem* elem );
static void check_elemlist2( Telem* elem );
static void check_elemlist_rep( Telem* elem );
static void check_string_cb( Telem* elem );
static void check_string_elem( Telem* elem );
static void check_iupCpi( Telem* elem );

static void code_image( Telem* elem );
static void code_iupCpi( Telem* elem );
static void code_empty( Telem* elem );
static void code_string( Telem* elem );
static void code_string_cb( Telem* elem );
static void code_elem( Telem* elem );
static void code_elemlist( Telem* elem );
static void code_elemlist2( Telem* elem );
static void code_string_elem( Telem* elem );

typedef void(*function)(Telem*);

#define IMAGE     0
#define IMAGERGB  1
#define IMAGERGBA 2

static struct {
  char* name;
  void (*code)(Telem*);
  void (*check)(Telem*);
  int header;
}
elems[] =
{
  { "Image",        code_image,        check_image,       0  },    
  { "ImageRGB",     code_image,        check_imagergb,    0  },    
  { "ImageRGBA",    code_image,        check_imagergba,   0  },    
  { "User",         code_empty,        check_empty,       0  },
  { "Button",       code_string_cb,    check_string_cb,   0  },
  { "Canvas",       code_string,       check_cb,          0  },
  { "Colorbar",     code_empty,        check_empty,       IUPCONTROLS_H  },
  { "ColorBrowser", code_empty,        check_empty,       IUPCONTROLS_H  },
  { "Dial",         code_string,       check_string,      IUPCONTROLS_H  },
  { "Dialog",       code_elem,         check_elem,        0  },
  { "Fill",         code_empty,        check_empty,       0  },
  { "FileDlg",      code_empty,        check_empty,       0  },
  { "MessageDlg",   code_empty,        check_empty,       0  },
  { "ColorDlg",     code_empty,        check_empty,       0  },
  { "FontDlg",      code_empty,        check_empty,       0  },
  { "ProgressBar",  code_empty,        check_empty,       0  },
  { "Frame",        code_elem,         check_elem,        0  },
  { "Gauge",        code_empty,        check_empty,       IUPCONTROLS_H  },
  { "GLCanvas",     code_string,       check_cb,          IUPGL_H  },
  { "Hbox",         code_elemlist,     check_elemlist,    0  },
  { "Item",         code_string_cb,    check_string_cb,   0  },
  { "Label",        code_string,       check_string,      0  },
  { "List",         code_string,       check_cb,          0  },
  { "Matrix",       code_string,       check_cb,          IUPCONTROLS_H  },
  { "Sbox",         code_elem,         check_elem,        0  },
  { "Menu",         code_elemlist,     check_elemlist,    0  },
  { "MultiLine",    code_string,       check_cb,          0  },
  { "Radio",        code_elem,         check_elem,        0  },
  { "Separator",    code_empty,        check_empty,       0  },
  { "Submenu",      code_string_elem,  check_string_elem, 0  },
  { "Text",         code_string,       check_cb,          0  },
  { "Val",          code_string,       check_string,      0  },
  { "Tree",         code_empty,        check_empty,       0  },
  { "Tabs",         code_elemlist,     check_elemlist,    0  },
  { "Toggle",       code_string_cb,    check_string_cb,   0  },
  { "Vbox",         code_elemlist,     check_elemlist,    0  },
  { "Zbox",         code_elemlist,     check_elemlist,    0  },
  { "Normalizer",   code_elemlist,     check_elemlist_rep,    0  },
  { "OleControl",   code_string,       check_cb,          IUPOLE_H  },
  { "Cbox",         code_elemlist,     check_elemlist,    0  },
  { "Cells",        code_empty,        check_empty,       IUPCONTROLS_H  },
  { "Spin",         code_empty,        check_empty,       0  },
  { "Spinbox",      code_elem,         check_elem,        0  },
  { "Split",        code_elemlist2,    check_elemlist2,   0  },
  { "PPlot",        code_empty,        check_empty,       IUPPPLOT_H  },
  { "MglPlot",      code_empty,        check_empty,       IUPMGLPLOT_H  },
  { "WebBrowser",   code_empty,        check_empty,       IUPWEB_H  },
  { "@@@",          code_iupCpi,       check_iupCpi,      0  }
};
#define nelems (sizeof(elems)/sizeof(elems[0]))

static int mystricmp(char *s1, char *s2)
{
   int i = 0;
   if (s1 == s2) return 0;
   while ((int)s1[i] && (int)s2[i] && tolower((int)s1[i])==tolower((int)s2[i])) i++;
   if (s1[i] == s2[i]) return 0;
   else if (s1[i]<s2[i]) return -1;
   return 1;
}

void error( char* fmt, ... )
{
  va_list args;
  fprintf(stderr, "%s:%d: error: ", filename, yylineno );
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n" );
  nerrors++;
}

static void warning( char* fmt, ... )
{
  va_list args;
  fprintf(stderr, "%s:%d: warning: ", filename, yylineno );
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n" );
  va_end(args);
}

/* used in y.tab.c */
void named( char* name )
{
  Tname *n = alloc(Tname);
  n->name = name;
  n->used = 0;
  addlist( all_names, n );
}

/* used also in y.tab.c */
void checkused( char* name )
{
  Telemlist* p = all_names->first;
  if (!name) return;
  while (p)
  {
    Tname* n = (Tname*)p->data;

    if (!strcmp(name,n->name)) 
    {
      if (n->used) 
        error("element '%s' already used in line %d", name, n->used );
      else 
        n->used = yylineno;
      break;  
    }
    p = p->next;
  }
}

static int iselem( char* name )
{
  Telemlist* p = all_names->first;
  while (p)
  {
    Tname* n = (Tname*)p->data;
    if (!strcmp(name,n->name)) return 1;
    p = p->next;
  }
  return 0;
}

static int verify_nparams( int min, int max, Telem* elem )
{
  if (min!=-1 && elem->nparams < min )
  {
    error("too few arguments for %s", elem->elemname );
    return 0;
  }
  if (max!=-1 && elem->nparams >max )
  {
    error("too many arguments for %s", elem->elemname );
    return 0;
  }
  return 1;
}

static void param_elem( Tparam* p[], int n, int rep )
{
  switch (p[n-1]->tag)
  {
    case ELEM_PARAM:
      if (p[n-1]->data.elem->elemidx == IMAGE ||
          p[n-1]->data.elem->elemidx == IMAGERGB ||
          p[n-1]->data.elem->elemidx == IMAGERGBA )
        error( "%s is not a valid child", p[n-1]->data.elem->elemname ); 
      break;
    case NAME_PARAM:
      if (!iselem(p[n-1]->data.name))
        warning( "undeclared control '%s' (argument #%d)", p[n-1]->data.name, n );
      else if (!rep)
        checkused( p[n-1]->data.name );
      break;
    case STRING_PARAM:
      error( "control expected (argument #%d)", n );
      break;
  }
}

static void param_string( Tparam* p[], int n )
{
  switch (p[n-1]->tag)
  {
    case ELEM_PARAM:
      error( "string expected (argument #%d)", n );
      break;
    case NAME_PARAM:
      warning( "string expected (argument #%d)", n );
      break;
    case STRING_PARAM:
      break;
  }
}

static void param_number( Tparam* p[], int n )
{
  switch (p[n-1]->tag)
  {
    case ELEM_PARAM:
      error( "number expected (argument #%d)", n );
      break;
    case NAME_PARAM:
      break;
    case STRING_PARAM:
      error( "number expected (argument #%d)", n );
      break;
  }
}

static void param_callback( Tparam* p[], int n )
{
  switch (p[n-1]->tag)
  {
    case ELEM_PARAM:
      error( "callback expected (argument #%d)", n );
      break;
    case NAME_PARAM:
      break;
    case STRING_PARAM:
      warning( "callback expected (argument #%d)", n );
      break;
  }
}

/****************************************************************/

static void check_empty( Telem* elem )
{
  verify_nparams( 0, 0, elem );
}

static void check_image( Telem* elem )
{
  int i, w, h, size;
  if (!verify_nparams( 2, -1, elem )) return;
  param_number( elem->params, 1 );
  param_number( elem->params, 2 );
  w = atoi( elem->params[0]->data.name );
  h = atoi( elem->params[1]->data.name );
  size = w*h;
  if (!verify_nparams( size+2, size+2, elem )) return;
  for (i=0; i<size; i++)
    param_number( elem->params, i+3 );

  elem->data.image.w = w;
  elem->data.image.h = h;
  elem->data.image.d = 1;
}

static void check_imagergb( Telem* elem )
{
  int i, w, h, size;
  if (!verify_nparams( 2, -1, elem )) return;
  param_number( elem->params, 1 );
  param_number( elem->params, 2 );
  w = atoi( elem->params[0]->data.name );
  h = atoi( elem->params[1]->data.name );
  size = w*h*3;
  if (!verify_nparams( size+2, size+2, elem )) return;
  for (i=0; i<size; i++)
    param_number( elem->params, i+3 );

  elem->data.image.w = w;
  elem->data.image.h = h;
  elem->data.image.d = 3;
}

static void check_imagergba( Telem* elem )
{
  int i, w, h, size;
  if (!verify_nparams( 2, -1, elem )) return;
  param_number( elem->params, 1 );
  param_number( elem->params, 2 );
  w = atoi( elem->params[0]->data.name );
  h = atoi( elem->params[1]->data.name );
  size = w*h*4;
  if (!verify_nparams( size+2, size+2, elem )) return;
  for (i=0; i<size; i++)
    param_number( elem->params, i+3 );

  elem->data.image.w = w;
  elem->data.image.h = h;
  elem->data.image.d = 4;
}

static void check_string( Telem* elem )
{
  if (!verify_nparams( 1, 1, elem )) return;
  param_string( elem->params, 1 );
}

static void check_cb( Telem* elem )
{
  if (!verify_nparams( 1, 1, elem )) return;
  param_callback( elem->params, 1 );
}

static void check_elem( Telem* elem )
{
  if (!verify_nparams( 1, 1, elem )) return;
  param_elem( elem->params, 1, 0 );
}

static void check_elemlist( Telem* elem )
{
  int i;
  if (!verify_nparams( 1, -1, elem )) return;
  for (i=0; i<elem->nparams; i++)
    param_elem( elem->params, i+1, 0 );
}

static void check_elemlist_rep( Telem* elem )
{
  int i;
  if (!verify_nparams( 1, -1, elem )) return;
  for (i=0; i<elem->nparams; i++)
    param_elem( elem->params, i+1, 1);
}

static void check_elemlist2( Telem* elem )
{
  int i;
  if (!verify_nparams( 1, 2, elem )) return;
  for (i=0; i<elem->nparams; i++)
    param_elem( elem->params, i+1, 0 );
}

static void check_string_cb( Telem* elem )
{
  if (!verify_nparams( 2, 2, elem )) return;
  param_string( elem->params, 1 );
  param_callback( elem->params, 2 );
}

static void check_string_elem( Telem* elem )
{
  if (!verify_nparams( 2, 2, elem )) return;
  param_string( elem->params, 1 );
  param_elem( elem->params, 2, 0 );
}

static void check_iupCpi( Telem* elem )
{
  warning( "Unknown control %s used", elem->elemname );
}

/****************************************************************/

static int indentcol = 0;
static int nameds    = 0;

static void indent(void)   { indentcol++; }
static void unindent(void) { indentcol--; }

static void codeindent(void)
{
  int i;
  for (i=0; i<indentcol; i++)
    fprintf( outfile, "  " );
}

static void generatename( Telem *e )
{
  static char name[15];
  if (e->name)
  {
    sprintf( name, "named[%d]", nameds++ );
    e->codename = strdup( name );
    addlist( all_named, e );
    addlist( all_late, e );
  }
  else
  {
    e->codename = 0;
  }
}

static int codename( Telem* elem )
{
  if (elem->elemidx == IMAGE ||
      elem->elemidx == IMAGERGB ||
      elem->elemidx == IMAGERGBA )
  {
    return 0;
  }

  if (elem->name && elem->codename)
  {
    fprintf( outfile, "%s", elem->codename );
    return 1;
  }
  else
    return 0;
}

static void codeelemname( Telem *elem )
{
  if (elem->elemidx < nelems-1)
    fprintf( outfile, "Iup%s", elems[elem->elemidx].name );
  else
  {
    fprintf( outfile, "Iup%c%s", (char)toupper((int)elem->elemname[0]), elem->elemname+1 );
  }
}

static void code_attrs( Telem* elem )
{
  fprintf( outfile, ", " );
  if (elem->attrs)
  {
    Telemlist *p = elem->attrs->first;
    indent();
    while (p)
    {
      fprintf( outfile, "\n" );
      codeindent();
      fprintf( outfile, "\"%s\", \"%s\", ",
               ((Tattr*)(p->data))->name, ((Tattr*)(p->data))->value );
      p = p ->next;
    }
    unindent();
  }
  fprintf( outfile, "NULL )" );
}

static void code_start( Telem* elem )
{
  codeindent();
  if (elem->name && 
      elem->elemidx != IMAGE && 
      elem->elemidx != IMAGERGB &&
      elem->elemidx != IMAGERGBA)
    generatename( elem );
  if ( codename( elem ) )
    fprintf( outfile, " = " );
  if (elem->name)
    fprintf( outfile, "IupSetAtt( \"%s\", ", elem->name );
  else
    fprintf( outfile, "IupSetAtt( NULL, " );
  codeelemname( elem );
}

static void code_c_name( char* name )
{
  char *ant = name;
  char *p = name;
  while ( (p = strpbrk( p, "+-." )) != NULL )
  {
    char c = p[0];
    p[0] = 0;
    fprintf( outfile, "%s", ant );
    switch (c)
    {
      case '+': fprintf( outfile, "%s", "_plus_" ); break;
      case '-': fprintf( outfile, "%s", "_minus_" ); break;
      case '.': fprintf( outfile, "%s", "_dot_" ); break;
    }
    p[0] = c;
    ant = ++p;
  }
  fprintf( outfile, "%s", ant );
}

static void code_decl( Telem *e )
{
  if (e->name || e->attrs)
  {
    code_start( e );
    elems[e->elemidx].code( e ); 
    code_attrs( e );
  }
  else
  {
    codeindent();
    if (e->name && 
        e->elemidx != IMAGE && 
        e->elemidx != IMAGERGB &&
        e->elemidx != IMAGERGBA)
      generatename( e );
    if ( codename( e ) )
      fprintf( outfile, " = " );
    codeelemname( e );
    elems[e->elemidx].code( e ); 
  }
}

static Telem* namedelem( char *name )
{
  Telemlist* p = all_named->first;
  while (p)
  {
    Telem* n = (Telem*)p->data;
    if (!strcmp(n->name,name))
    {
      return n;
    }
    p = p->next;
  }
  return 0;
}

static void codeelemparam( Tparam* param )
{
  if (param->tag == ELEM_PARAM)
  {
    code_decl( param->data.elem );
  }
  else
  {
    Telem *e = namedelem( param->data.name );
    codeindent();
    if (e)
    {
      codename( e );
      fprintf( outfile, " /* %s */", param->data.name );
    }
    else
    {
      fprintf( outfile, "IupGetHandle( \"%s\" )", param->data.name );
    }
  }
}

/****************************************************************/

static void code_iupCpi( Telem* elem )
{
  int i=0;
  indent();
  fprintf( outfile, "( " );
  for (i=0; i<elem->nparams; i++)
  {
    if (elem->params[i]->tag == ELEM_PARAM)
    {
      fprintf( outfile, "\n" );
      codeindent();
      codeelemparam( elem->params[i] );
    }
    else
    {
      fprintf( outfile, "\"%s\"", elem->params[i]->data.name );
    }
    if (i+1<elem->nparams) fprintf( outfile, ", " ); 
  }
  fprintf( outfile, ")" );
  unindent();
}

static void code_image( Telem* elem )
{
  int i,
      d = elem->data.image.d,
      w = elem->data.image.w,
      h = elem->data.image.h;
  
  fprintf( outfile, "static void image_" );
  code_c_name( elem->name );
  fprintf( outfile,
    " (void)\n"
    "{\n"
    "  %sunsigned char map[] = {", static_image? "static ": "" );

  for (i=0; i<w*h*d; i++)
  {
    if (i%(w*d) == 0) fprintf( outfile, "\n    " );
    fprintf( outfile, "%2d,", atoi(elem->params[i+2]->data.name) );
  }

  fprintf( outfile,
    "\n"
    "  0 };\n\n" );
  indent();
  code_start( elem );
  fprintf( outfile, "( %d, %d, map )", elem->data.image.w, elem->data.image.h );
  code_attrs( elem );
  fprintf( outfile, ";\n}\n\n" );
  unindent();
}

static void code_empty( Telem* elem )
{
  (void) elem;
  fprintf( outfile, "()" );
}

static void code_string( Telem* elem )
{
  fprintf( outfile, "( \"%s\" )", elem->params[0]->data.name );
}

static void code_string_cb( Telem* elem )
{
  fprintf( outfile, "( \"%s\", \"%s\" )", elem->params[0]->data.name, elem->params[1]->data.name );
}

static void code_elem( Telem* elem )
{
  fprintf( outfile, "(\n" );
  indent();
  codeelemparam( elem->params[0] );
  fprintf( outfile, "\n" );
  unindent();
  codeindent();
  fprintf( outfile, ")" );
}


static void code_elemlist( Telem* elem )
{
  int i=0;
  fprintf( outfile, "(\n" );
  indent();
  for (i=0; i<elem->nparams; i++)
  {
    codeelemparam( elem->params[i] );
    fprintf( outfile, ",\n" ); 
  }
  unindent();
  codeindent();
  fprintf( outfile, "NULL)" );
}

static void code_elemlist2( Telem* elem )
{
  int i=0;
  fprintf( outfile, "(\n" );
  indent();
  for (i=0; i<2; i++)
  {
    if (i < elem->nparams)
    {
      codeelemparam( elem->params[i] );
      fprintf( outfile, ",\n" ); 
    }
    else
      fprintf( outfile, "NULL,\n" ); 
  }
  unindent();
  codeindent();
  fprintf( outfile, ")" );
}

static void code_string_elem( Telem* elem )
{
  fprintf( outfile, "( \"%s\",\n", elem->params[0]->data.name );
  indent();
  codeelemparam( elem->params[1] );
  fprintf( outfile, "\n" );
  unindent();
  codeindent();
  fprintf( outfile, ")" );
}

/****************************************************************/

static char* strlower( char* str )
{
  int i=0;
  while (str[i]) { str[i]=(char)tolower((int)str[i]); i++; }
  return str;
}

static int lookupelem( char* name )
{
  int i;
  for (i=0; i<nelems && mystricmp(name, elems[i].name); i++ );
  return i<nelems ? i : nelems-1;
}

Tattr*  attr( char* name, char* value )
{
  Tattr* ret = alloc(Tattr);
  ret->name  = name;
  ret->value = value;
  return ret;
}

Tparam* param( int tag, void* value )
{
  Tparam* ret = alloc(Tparam);
  ret->tag = tag;
  switch (tag)
  {
    case NAME_PARAM:
    case STRING_PARAM:
      ret->data.name = (char*)value;
      break;
    case ELEM_PARAM:
      ret->data.elem = (Telem*)value;
      break;
  }
  return ret;
}

/* used in y.tab.c */
Telem* elem( char* name, Tlist* attrs, Tlist* params )
{
  Telem* ret = alloc(Telem);
  ret->name = 0;
  ret->elemname = strlower(name); 
  ret->attrs = attrs;
  if (params)
  {
    ret->nparams = params->size;
    ret->params = list2paramvector( params );
  }
  else
  {
    ret->nparams = 0;
    ret->params = 0;
  }
  ret->elemidx = lookupelem( name );
  elems[ret->elemidx].check(ret);
  headerfile[elems[ret->elemidx].header].used = 1;
  return ret;
}

Tlist*  list( void )
{
  Tlist* ret = alloc(Tlist);
  ret->first = 0;
  ret->size = 0;
  return ret;
}

Tlist*  addlist( Tlist* l, void* data )
{
  Telemlist* ne = alloc(Telemlist);
  ne->data = data;
  ne->next = l->first;
  l->first = ne;
  l->size++;
  return l;
}

void cleanlist( Tlist* l )
{
  l->first = 0;
  l->size = 0;
}


void decl( Telem* e )
{
  if (!e->name) warning("%s declared without a name", e->elemname );
  else
  {
    if (nerrors || nocode) return;
    if (e->elemidx == IMAGE || e->elemidx == IMAGERGB || e->elemidx == IMAGERGBA)
    {
      code_image( e );
      addlist( all_images, e );
    }
    else
    {
      addlist( all_elems, e );
    }
  }
}


static Telemlist* revert( Telemlist* elem )
{
  if (!elem->next)
  {
    return elem;
  }
  else
  {
    Telemlist *ret;
    ret = revert( elem->next );
    elem->next->next = elem;
    elem->next = 0;
    return ret;
  }
}

Tlist* revertlist( Tlist* l )
{
  if (l->first)
    l->first = revert( l->first );
  return l;
}

Tparam** list2paramvector( Tlist* params )
{
  int i = params->size;
  if (i)
  {
    Tparam** ret = (Tparam**)malloc(i*sizeof(Tparam*));
    Telemlist *p = params->first;
    while (p)
    {
      ret[--i] = (Tparam*)p->data;
      p = p->next;
    }
    return ret;
  }
  else
    return 0;
}


void init(void)
{
  all_names = list();
  all_elems = list();
  all_images = list();
  all_named = list();
  all_late = list();

  if (nocode) return;
  outfile = stdout;

  outfile = fopen( outname, "w" );
  if (!outfile) 
  {
    perror( outname );
    exit(-1);
  }

  fprintf( outfile,
    "/*   Automatically generated by Iup 3 LED Compiler to C.   */\n"
    "\n"
    "#include <stdlib.h>\n"
    "#include <stdarg.h>\n"
    "#include <iup.h>\n"
    "\n" 
    "static Ihandle* named[ " );
  vetsizepos = ftell( outfile );
  fprintf( outfile, 
    "       ];\n"
    "\n" );
}

void finish(void)
{
  int i;
  Telemlist *p;

  if (nerrors || nocode) return;

  for (i=1; i<nheaders; i++)
  {
    if (headerfile[i].used)
    {
      fprintf( outfile, "#include <%s.h>\n", headerfile[i].name );
    }
  }

  fprintf( outfile, "\nvoid %s (void)\n{\n", funcname );

  revertlist( all_images );
  p = all_images->first;
  while (p)
  {
    Telem* e = (Telem*)(p->data);
    fprintf(outfile, "  image_" );
    code_c_name( e->name );
    fprintf(outfile, " ();\n" );
    p = p->next;
  }

  indent();
  revertlist( all_elems );
  p = all_elems->first;
  while (p)
  {
    Telem* e = (Telem*)(p->data);
    code_decl( e );
    fprintf( outfile, ";\n" );
    p = p->next;
  }
  unindent();

  fprintf( outfile, "}\n" );
  fseek( outfile, vetsizepos, SEEK_SET );
  fprintf( outfile, "%6d", nameds+1 );
  
  fclose( outfile );
}

/* necessario para o Linux ?? */
int gettxt( char *v1, char *v2);
int gettxt( char *v1, char *v2)
{
  fprintf(stderr, "%s: %s\n", v1, v2);
  return 0;
}

