#define NAME 257
#define STRING 258
typedef union {
   char*    fString;
   Tattr*   fAttr;
   Tlist*   fList;
   Telem*   fElem;
   Tparam*  fParam;
} YYSTYPE;
extern YYSTYPE yylval;
