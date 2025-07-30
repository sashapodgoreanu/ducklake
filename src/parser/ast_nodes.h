// ast_nodes.h
#pragma once
#include <memory>
#include <string>
#include <vector>

namespace ast {

/// Base class for all statements
struct Stmt {
    enum Kind { CREATE, DROP, COPY, MOVE } kind;
    explicit Stmt(Kind k) : kind(k) { }
    virtual ~Stmt() {}
};
using StmtPtr = std::unique_ptr<Stmt>;

/// CREATE DATABOX / CREATE SHELF
struct CreateStmt : Stmt {
    enum ObjType { DataBox, Shelf } obj;
    std::vector<std::string> name_parts;
    CreateStmt(ObjType o) : Stmt(CREATE), obj(o) {}
};

/// DROP DATABOX / DROP SHELF
struct DropStmt : Stmt {
    enum ObjType { DataBox, Shelf } obj;
    std::vector<std::string> name_parts;
    DropStmt(ObjType o) : Stmt(DROP), obj(o) {}
};

/// COPY DATABOX src TO dst
struct CopyStmt : Stmt {
    std::vector<std::string> src, dst;
    CopyStmt() : Stmt(COPY) {}
};

/// MOVE DATABOX src TO dst
struct MoveStmt : Stmt {
    std::vector<std::string> src, dst;
    MoveStmt() : Stmt(MOVE) {}
};

/// Top‐level program holding all statements
using Program = std::vector<StmtPtr>;

} // namespace ast
