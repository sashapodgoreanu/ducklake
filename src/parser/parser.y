/* parser.y (Bison grammar, C interface) */
// Use a unique prefix to avoid symbol conflicts:
%define api.prefix {dln}

// Declarations needed in the generated header (.tab.h):
%code requires {
    #include <vector>
    #include <string>
    #include "ast_nodes.h"
}

%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "ast_nodes.h"
using namespace ast;
extern Program parsed_program;
%}

%union {
    ast::Stmt*                   stmt;
    std::vector<std::string>*    names;
    char*                        str;
}

%token CREATE DROP COPY MOVE TO DATABOX
%token <str> IDENT
%token DOT

%type <stmt> statement create_stmt drop_stmt copy_stmt move_stmt
%type <names> name_list

%%

program:
      /* empty */
    | program statement     { parsed_program.emplace_back(StmtPtr($2)); }
;

statement:
      create_stmt
    | drop_stmt
    | copy_stmt
    | move_stmt
;

create_stmt:
    CREATE DATABOX name_list {
        auto s = new CreateStmt(CreateStmt::DataBox);
        s->name_parts = *$3;
        delete $3;
        $$ = s;
    }
;

drop_stmt:
    DROP DATABOX name_list {
        auto s = new DropStmt(DropStmt::DataBox);
        s->name_parts = *$3;
        delete $3;
        $$ = s;
    }
;

copy_stmt:
    COPY DATABOX name_list TO name_list {
        auto s = new CopyStmt();
        s->src = *$3;
        s->dst = *$5;
        delete $3;
        delete $5;
        $$ = s;
    }
;

move_stmt:
    MOVE DATABOX name_list TO name_list {
        auto s = new MoveStmt();
        s->src = *$3;
        s->dst = *$5;
        delete $3;
        delete $5;
        $$ = s;
    }
;

name_list:
    IDENT DOT IDENT DOT IDENT {
        auto v = new std::vector<std::string>{ std::string($1), std::string($3), std::string($5) };
        free($1); free($3); free($5);
        $$ = v;
    }
;

%%


int dlnerror(const char* s) {
    std::fprintf(stderr, "Parse error: %s", s);
    return 0;
}