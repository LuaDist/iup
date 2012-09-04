#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define YYPREFIX "yy"
#line 1 "ledc.y"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ledc.h"

static int yyparse(void);

#line 11 "ledc.y"
typedef union {
   char*    fString;
   Tattr*   fAttr;
   Tlist*   fList;
   Telem*   fElem;
   Tparam*  fParam;
} YYSTYPE;
#line 29 "y.tab.c"
#define NAME 257
#define STRING 258
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    0,   10,   10,   11,    8,    8,    7,    6,    6,
    5,    5,    9,    9,    9,    4,    4,    3,    3,    1,
    2,    2,
};
short yylen[] = {                                         2,
    0,    1,    1,    2,    1,    1,    3,    5,    0,    1,
    1,    3,    1,    1,    1,    0,    3,    1,    3,    3,
    1,    1,
};
short yydefred[] = {                                      0,
    0,    0,    6,    5,    0,    3,    0,    0,    0,    4,
    0,    7,   21,   22,   18,    0,    0,    0,    0,   17,
    0,    0,   14,    0,    0,   15,   11,   20,   19,    0,
    8,   12,
};
short yydgoto[] = {                                       2,
   15,   16,   17,    9,   24,   25,    3,    4,   27,    5,
    6,
};
short yysindex[] = {                                   -247,
  -60,    0,    0,    0, -247,    0, -246, -252,  -28,    0,
  -78,    0,    0,    0,    0,  -46,  -44, -249, -252,    0,
 -252,  -60,    0,  -27,  -25,    0,    0,    0,    0, -249,
    0,    0,
};
short yyrindex[] = {                                     18,
  -21,    0,    0,    0,   20,    0,    0,    0,    0,    0,
  -21,    0,    0,    0,    0,    0,    0,  -20,    0,    0,
    0,  -37,    0,  -19,    0,    0,    0,    0,    0,    0,
    0,    0,
};
short yygindex[] = {                                      0,
    2,    5,    0,    0,    0,    0,   19,  -16,   -5,    0,
   22,
};
#define YYTABLESIZE 49
short yytable[] = {                                      21,
    7,   26,   16,   13,   13,   14,   13,   22,   23,    1,
   11,   18,    8,   26,   19,   31,   30,    1,   16,    2,
    9,   10,   29,   28,   32,   12,   10,    0,    0,    0,
    8,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   20,
};
short yycheck[] = {                                      44,
   61,   18,   40,   41,  257,  258,   44,  257,  258,  257,
  257,   40,   91,   30,   61,   41,   44,    0,   40,    0,
   41,   41,   21,   19,   30,    7,    5,   -1,   -1,   -1,
   91,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   93,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 258
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'='",0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,"']'",0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"NAME",
"STRING",
};
char *yyrule[] = {
"$accept : led",
"led :",
"led : decl_list",
"decl_list : decl",
"decl_list : decl_list decl",
"decl : iupelem_decl",
"iupelem_decl : iupelem",
"iupelem_decl : NAME '=' iupelem",
"iupelem : NAME opt_attr '(' param_decl ')'",
"param_decl :",
"param_decl : param_list",
"param_list : param",
"param_list : param_list ',' param",
"param : NAME",
"param : STRING",
"param : iupelem_decl",
"opt_attr :",
"opt_attr : '[' attr_list ']'",
"attr_list : attr_def",
"attr_list : attr_list ',' attr_def",
"attr_def : name_str '=' name_str",
"name_str : NAME",
"name_str : STRING",
};
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
#line 93 "ledc.y"


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
    "  -s            declarate image data as static\n"
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
        printf("Iup 3 LED Compiler to C\n");
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
      case 's':
        static_image = 1;
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

#line 246 "y.tab.c"
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
static int yyparse(void)
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
#endif
yynewerror:
    yyerror("invalid syntax");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 5:
#line 41 "ledc.y"
{ decl( yyvsp[0].fElem ); }
break;
case 6:
#line 45 "ledc.y"
{ yyval.fElem = yyvsp[0].fElem; }
break;
case 7:
#line 47 "ledc.y"
{ yyval.fElem = yyvsp[0].fElem; yyvsp[0].fElem->name = yyvsp[-2].fString; named(yyvsp[-2].fString); }
break;
case 8:
#line 51 "ledc.y"
{ yyval.fElem = elem( yyvsp[-4].fString, yyvsp[-3].fList, yyvsp[-1].fList ); }
break;
case 9:
#line 54 "ledc.y"
{ yyval.fList = 0; }
break;
case 10:
#line 56 "ledc.y"
{ yyval.fList = yyvsp[0].fList; }
break;
case 11:
#line 60 "ledc.y"
{ yyval.fList = list(); addlist( yyval.fList, yyvsp[0].fParam ); }
break;
case 12:
#line 62 "ledc.y"
{ yyval.fList = addlist( yyvsp[-2].fList, yyvsp[0].fParam ); }
break;
case 13:
#line 66 "ledc.y"
{ yyval.fParam = param( NAME_PARAM, yyvsp[0].fString ); }
break;
case 14:
#line 68 "ledc.y"
{ yyval.fParam = param( STRING_PARAM, yyvsp[0].fString ); }
break;
case 15:
#line 70 "ledc.y"
{ yyval.fParam = param( ELEM_PARAM, yyvsp[0].fElem ); checkused(yyvsp[0].fElem->name); }
break;
case 16:
#line 74 "ledc.y"
{ yyval.fList = 0; }
break;
case 17:
#line 76 "ledc.y"
{ yyval.fList = revertlist( yyvsp[-1].fList ); }
break;
case 18:
#line 80 "ledc.y"
{ yyval.fList = list(); addlist( yyval.fList, yyvsp[0].fAttr );  }
break;
case 19:
#line 82 "ledc.y"
{ addlist( yyvsp[-2].fList, yyvsp[0].fAttr ); }
break;
case 20:
#line 86 "ledc.y"
{ yyval.fAttr = attr( yyvsp[-2].fString, yyvsp[0].fString ); }
break;
#line 451 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
