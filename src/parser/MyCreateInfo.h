#pragma once

#include "duckdb/parser/parsed_data/create_info.hpp"
#include "duckdb/common/unique_ptr.hpp"         // duckdb::unique_ptr & make_uniq
#include "duckdb/common/constants.hpp"          // DEFAULT_SCHEMA, INVALID_CATALOG
#include "duckdb/common/enums/catalog_type.hpp" // CatalogType enum

#include <string>
#include <memory>

namespace duckdb {

using std::string;

/// CREATE DATABOX
struct CreateDataBoxInfo : public CreateInfo {
    string name;
    CreateDataBoxInfo()
        : CreateInfo(CatalogType::SCHEMA_ENTRY) {
    }

    unique_ptr<CreateInfo> Copy() const override {
        auto result = make_uniq<CreateDataBoxInfo>();
        CopyProperties(*result);
        result->name = name;
        return result;
    }

    string ToString() const override {
        return "CREATE DATABOX " + catalog + "." + schema + "." + name;
    }
};

/// CREATE SHELF
struct CreateShelfInfo : public CreateInfo {
    string name;
    CreateShelfInfo()
        : CreateInfo(CatalogType::SCHEMA_ENTRY) {
    }

    unique_ptr<CreateInfo> Copy() const override {
        auto result = make_uniq<CreateShelfInfo>();
        CopyProperties(*result);
        result->name = name;
        return result;
    }

    string ToString() const override {
        return "CREATE SHELF " + catalog + "." + schema + "." + name;
    }
};

/// DROP DATABOX
struct DropDataBoxInfo : public CreateInfo {
    string name;
    DropDataBoxInfo()
        : CreateInfo(CatalogType::SCHEMA_ENTRY) {
    }

    unique_ptr<CreateInfo> Copy() const override {
        auto result = make_uniq<DropDataBoxInfo>();
        CopyProperties(*result);
        result->name = name;
        return result;
    }

    string ToString() const override {
        return "DROP DATABOX " + catalog + "." + schema + "." + name;
    }
};

/// DROP SHELF
struct DropShelfInfo : public CreateInfo {
    string name;
    DropShelfInfo()
        : CreateInfo(CatalogType::SCHEMA_ENTRY) {
    }

    unique_ptr<CreateInfo> Copy() const override {
        auto result = make_uniq<DropShelfInfo>();
        CopyProperties(*result);
        result->name = name;
        return result;
    }

    string ToString() const override {
        return "DROP SHELF " + catalog + "." + schema + "." + name;
    }
};

/// COPY DATABOX src TO dst
struct CopyDataBoxInfo : public CreateInfo {
    string src_name;
    string dst_name;
    CopyDataBoxInfo()
        : CreateInfo(CatalogType::SCHEMA_ENTRY) {
    }

    unique_ptr<CreateInfo> Copy() const override {
        auto result = make_uniq<CopyDataBoxInfo>();
        CopyProperties(*result);
        result->src_name = src_name;
        result->dst_name = dst_name;
        return result;
    }

    string ToString() const override {
        return "COPY DATABOX " + src_name + " TO " + dst_name;
    }
};

/// MOVE DATABOX src TO dst
struct MoveDataBoxInfo : public CreateInfo {
    string src_name;
    string dst_name;
    MoveDataBoxInfo()
        : CreateInfo(CatalogType::SCHEMA_ENTRY) {
    }

    unique_ptr<CreateInfo> Copy() const override {
        auto result = make_uniq<MoveDataBoxInfo>();
        CopyProperties(*result);
        result->src_name = src_name;
        result->dst_name = dst_name;
        return result;
    }

    string ToString() const override {
        return "MOVE DATABOX " + src_name + " TO " + dst_name;
    }
};

} // namespace duckdb