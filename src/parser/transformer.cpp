#include "ast_nodes.h"
#include "duckdb/parser/statement/create_statement.hpp"
#include "MyCreateInfo.h"
#include "duckdb/common/unique_ptr.hpp"  // for duckdb::unique_ptr, make_uniq

#include <vector>
#include <string>

using duckdb::unique_ptr;
using duckdb::make_uniq;

/// Transform an AST Program into DuckDB SQLStatement objects
std::vector<unique_ptr<duckdb::SQLStatement>> Transform(const ast::Program &prog) {
    std::vector<unique_ptr<duckdb::SQLStatement>> out;
    out.reserve(prog.size());

    for (auto &st : prog) {
        switch (st->kind) {
        case ast::Stmt::CREATE: {
            auto cst = static_cast<ast::CreateStmt*>(st.get());
            // Create the base CreateStatement
            auto stmt = make_uniq<duckdb::CreateStatement>();
            if (cst->obj == ast::CreateStmt::DataBox) {
                // Instantiate CreateDataBoxInfo
                auto info = make_uniq<duckdb::CreateDataBoxInfo>();
                if (!cst->name_parts.empty()) info->catalog = cst->name_parts[0];
                if (cst->name_parts.size() > 1) info->schema = cst->name_parts[1];
                info->name = cst->name_parts.back();
                stmt->info = std::move(info);
            } else {
                // Instantiate CreateShelfInfo
                auto info = make_uniq<duckdb::CreateShelfInfo>();
                if (!cst->name_parts.empty()) info->catalog = cst->name_parts[0];
                if (cst->name_parts.size() > 1) info->schema = cst->name_parts[1];
                info->name = cst->name_parts.back();
                stmt->info = std::move(info);
            }
            out.push_back(std::move(stmt));
            break;
        }
        case ast::Stmt::DROP: {
            auto dst = static_cast<ast::DropStmt*>(st.get());
            auto stmt = make_uniq<duckdb::CreateStatement>();
            if (dst->obj == ast::DropStmt::DataBox) {
                auto info = make_uniq<duckdb::DropDataBoxInfo>();
                if (!dst->name_parts.empty()) info->catalog = dst->name_parts[0];
                if (dst->name_parts.size() > 1) info->schema = dst->name_parts[1];
                info->name = dst->name_parts.back();
                stmt->info = std::move(info);
            } else {
                auto info = make_uniq<duckdb::DropShelfInfo>();
                if (!dst->name_parts.empty()) info->catalog = dst->name_parts[0];
                if (dst->name_parts.size() > 1) info->schema = dst->name_parts[1];
                info->name = dst->name_parts.back();
                stmt->info = std::move(info);
            }
            out.push_back(std::move(stmt));
            break;
        }
        case ast::Stmt::COPY: {
            auto cp = static_cast<ast::CopyStmt*>(st.get());
            auto stmt = make_uniq<duckdb::CreateStatement>();
            auto info = make_uniq<duckdb::CopyDataBoxInfo>();
            // join src and dst parts
            auto join = [&](const std::vector<std::string> &v) {
                std::string s = v.empty() ? "" : v[0];
                for (size_t i = 1; i < v.size(); i++) s += "." + v[i];
                return s;
            };
            info->src_name = join(cp->src);
            info->dst_name = join(cp->dst);
            stmt->info = std::move(info);
            out.push_back(std::move(stmt));
            break;
        }
        case ast::Stmt::MOVE: {
            auto mv = static_cast<ast::MoveStmt*>(st.get());
            auto stmt = make_uniq<duckdb::CreateStatement>();
            auto info = make_uniq<duckdb::MoveDataBoxInfo>();
            auto join = [&](const std::vector<std::string> &v) {
                std::string s = v.empty() ? "" : v[0];
                for (size_t i = 1; i < v.size(); i++) s += "." + v[i];
                return s;
            };
            info->src_name = join(mv->src);
            info->dst_name = join(mv->dst);
            stmt->info = std::move(info);
            out.push_back(std::move(stmt));
            break;
        }
        }
    }
    return out;
}