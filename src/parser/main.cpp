// main.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include "parser.tab.h"   // Bison-generated
#include "lexer.h"        // Flex-generated
#include "ast_nodes.h"

using namespace ast;

// This is where parsed statements will go:
Program parsed_program;

// Parse from a C‐string:
int parseString(const std::string& input) {
    // Create a new buffer for the string
    YY_BUFFER_STATE buf = dln_scan_string(input.c_str());
    int result = dlnparse();
    dln_delete_buffer(buf);
    return result;
}

int main(int argc, char** argv) {
    // If an argument is given, treat it as a filename; otherwise parse stdin.
    if (argc == 2) {
        std::ifstream in(argv[1]);
        if (!in) {
            std::cerr << "Failed to open file: " << argv[1] << "\n";
            return EXIT_FAILURE;
        }
        std::stringstream ss;
        ss << in.rdbuf();
        if (parseString(ss.str()) != 0) {
            std::cerr << "Parse errors.\n";
            return EXIT_FAILURE;
        }
    }
    else {
        std::cout << "Type commands followed by semicolons, Ctrl-D to finish:\n";
        if (dlnparse() != 0) {
            std::cerr << "Parse errors.\n";
            return EXIT_FAILURE;
        }
    }

    // At this point parsed_program holds all your statements
    for (auto& stmt : parsed_program) {
        switch (stmt->kind) {
            case Stmt::CREATE: std::cout << "CREATE\n"; break;
            case Stmt::DROP:   std::cout << "DROP\n";   break;
            case Stmt::COPY:   std::cout << "COPY\n";   break;
            case Stmt::MOVE:   std::cout << "MOVE\n";   break;
        }
    }

    return EXIT_SUCCESS;
}