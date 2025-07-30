%option noyywrap
%option prefix="dln"
%option outfile="lexer.cc"
%option header-file="lexer.h"
%{
#include "parser.tab.h"  /* defines dlnparse(), dlnerror, dlnSTYPE, dlnTOKENS */
#include <cstring>
%}

%%
"create"        { return CREATE; }
"drop"          { return DROP; }
"copy"          { return COPY; }
"move"          { return MOVE; }
"databox"       { return DATABOX; }
"to"            { return TO; }
"\."           { return DOT; }
[ \t\r\n]+      { /* skip whitespace */ }
[a-zA-Z_][a-zA-Z0-9_]* {
    dlnlval.str = strdup(yytext);
    return IDENT;
}
.                { return yytext[0]; }
%%