%option yylineno

%{
#include <string.h>
#include "ledc.h"
#include "y.tab.h"
%}

a       [aA]
b       [bB]
c       [cC]
d       [dD]
e       [eE]
f       [fF]
g       [gG]
h       [hH]
i       [iI]
j       [jJ]
k       [kK]
l       [lL]
m       [mM]
n       [nN]
o       [oO]
p       [pP]
q       [qQ]
r       [rR]
s       [sS]
t       [tT]
u       [uU]
v       [vV]
w       [wW]
x       [xX]
y       [yY]
z       [zZ]

%%

[ \t\n]		;

"("		{ return '('; }
")"		{ return ')'; }
"="		{ return '='; }
"["		{ return '['; }
"]"		{ return ']'; }
","		{ return ','; }

["](\\.|[^"])*["] {
		   int len = strlen(yytext)-2;
		   char *str = (char*)malloc(sizeof(char)*len+1);
		   strncpy(str, yytext+1, len);
		   str[len] = 0;
		   yylval.fString = str;
		   return STRING;
		}

[A-Za-z0-9_.+-][A-Za-z0-9_.+-]* {
		   yylval.fString = strdup(yytext);
		   return NAME;
		}

"#".*$		;

%%


