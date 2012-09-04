%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ledc.h"

%}

%union {
   char*    fString;
   Tattr*   fAttr;
   Tlist*   fList;
   Telem*   fElem;
   Tparam*  fParam;
}

%type <fAttr>   attr_def
%type <fString> name_str
%type <fList>   attr_list opt_attr param_list param_decl
%type <fElem>   iupelem iupelem_decl
%type <fParam>  param 

%token '(' ')' '=' '[' ']' ','
%token <fString> NAME STRING 

%start led

%%

led		: /* empty */
		| decl_list
		;

decl_list	: decl
		| decl_list  decl
		;

decl            : iupelem_decl
	{ decl( $1 ); }
		;

iupelem_decl    : iupelem
	{ $$ = $1; }
                | NAME '=' iupelem
	{ $$ = $3; $3->name = $1; named($1); }
                ;

iupelem         : NAME opt_attr '(' param_decl ')'
	{ $$ = elem( $1, $2, $4 ); }

param_decl      : /* empty */
	{ $$ = 0; }
                | param_list
	{ $$ = $1; }
                ;

param_list	: param
	{ $$ = list(); addlist( $$, $1 ); }
		| param_list ',' param
	{ $$ = addlist( $1, $3 ); }
		;

param           : NAME
	{ $$ = param( NAME_PARAM, $1 ); }
                | STRING
	{ $$ = param( STRING_PARAM, $1 ); }
                | iupelem_decl
	{ $$ = param( ELEM_PARAM, $1 ); use($1->name); }
                ;

opt_attr	: /* empty */
	{ $$ = 0; }
		| '[' attr_list ']'
	{ $$ = revertlist( $2 ); }
		;

attr_list	: attr_def
	{ $$ = list(); addlist( $$, $1 );  }
		| attr_list ',' attr_def
	{ addlist( $1, $3 ); }
		;

attr_def	: name_str '=' name_str
	{ $$ = attr( $1, $3 ); }
		;

name_str	: NAME
                | STRING
                ;

%%

extern int yylineno;
extern FILE *yyin;

int yywrap(void) { return 1; }

void yyerror(char *str)
{
   error(" %s; last token read: %s",
         str, yylval.fString);
}

static int usage(void)
{
  fprintf( stderr, 
    "ledc [-v] [-c] [-f funcname] [-o file] files\n"
    "  -v            print the version number\n"
    "  -c            only perform checking, do not generate code\n"
    "  -f funcname   generate exported function <funcname> (default: led_load)\n"
    "  -o file       place output in file <file> (default: led.c)\n"
  );
  return -1;
}

int main(int argc, char *argv[])
{
  if (argc < 2) 
  {
    return usage();
  }

  while (argc > 1)
  {
    char *op = *(argv+1);
    if (op[0] != '-') break;
    argc--;
    argv++;
    switch (op[1])
    {
      case 'v':
        printf("Iup 3.0 LED Compiler to C\n");
        return 0;
        break;
      case 'o':
        if (argc>1) 
        {
          argc--;
          argv++;
          outname = *argv;
        }
        else return usage();
        break;
      case 'f':
        if (argc>1) 
        {
          argc--;
          argv++;
          funcname = *argv;
        }
        else return usage();
        break;
      case 'c':
        nocode = 1;
        break;
      default:
        return usage();
    }
  }

  if (!outname)  outname  = "led.c";
  if (!funcname) funcname = "led_load";

  init();
  while (--argc > 0)
  {
    char * name = *++argv;
    filename = name;
    if ((yyin = fopen(name, "r")) == NULL)
    {
      fprintf( stderr, "ledc: %s: %s\n", name,
#ifdef SunOS
        "cannot open file"
#else
        strerror(errno)
#endif
      );
      continue;
    }
    yylineno = 1;
    yyparse();
    fclose(yyin);
  }
  finish();
  return 0;
}

