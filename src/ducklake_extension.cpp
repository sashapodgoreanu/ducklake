#ifndef DUCKDB_BUILD_LOADABLE_EXTENSION
#define DUCKDB_BUILD_LOADABLE_EXTENSION
#endif
#include "ducklake_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "storage/ducklake_storage.hpp"
#include "functions/ducklake_table_functions.hpp"
#include "duckdb/main/extension_util.hpp"
#include "duckdb/parser/sql_statement.hpp"
#include "duckdb/parser/statement/create_statement.hpp"
#include "duckdb/parser/parsed_data/create_schema_info.hpp"
#include "duckdb/parser/statement/extension_statement.hpp"
#include "storage/ducklake_catalog.hpp"

#include <regex>
#include <memory>

namespace duckdb {

class PsqlState : public ClientContextState {
public:
	explicit PsqlState(unique_ptr<ParserExtensionParseData> parse_data) : parse_data(std::move(parse_data)) {
	}

	void QueryEnd() override {
		parse_data.reset();
	}

	unique_ptr<ParserExtensionParseData> parse_data;
};

struct PsqlParseData : ParserExtensionParseData {
	unique_ptr<SQLStatement> statement;

	unique_ptr<ParserExtensionParseData> Copy() const override {
		return make_uniq_base<ParserExtensionParseData, PsqlParseData>(statement->Copy());
	}

	virtual string ToString() const override {
		return "PsqlParseData";
	}

	PsqlParseData(unique_ptr<SQLStatement> statement) : statement(std::move(statement)) {
	}
};

static BoundStatement BindCreateDataBox(ClientContext &context, Binder &binder, OperatorExtensionInfo *info_p,
                                        SQLStatement &stmt) {
	// down‐cast to see if it’s ours:
	// auto *create = dynamic_cast<CreateDataBoxStatement*>(&stmt);
	// if (!create) {
	// not ours: let someone else bind it
	//    return BoundStatement();
	//}

	return binder.Bind(stmt);

	// CREATE statements return no rows
	return BoundStatement();
}

class DataBoxOperatorExtension : public OperatorExtension {
	DataBoxOperatorExtension() {
		Bind = BindCreateDataBox;
	};
};

ParserExtensionParseResult lakeshelf_parse(ParserExtensionInfo *, const string &query) {
	// Regex to match: CREATE DATABOX <catalog> . <shelf> . <schema> (optional ;) case-insensitive
	// static const std::regex pattern(
	//     R"(^\s*create\s+databox\s+\"?([A-Za-z_][A-Za-z0-9_]*)\"?\.\"?([A-Za-z_][A-Za-z0-9_]*)\"?\.\"?([A-Za-z_][A-Za-z0-9_]*)\"?\s*;?\s*$)",
	//     std::regex_constants::icase);
	static const std::regex pattern(
    R"(^\s*create\s+databox\s+(?:\"?([A-Za-z_][A-Za-z0-9_]*)\"?\.)?(?:\"?([A-Za-z_][A-Za-z0-9_]*)\"?\.)?\"?([A-Za-z_][A-Za-z0-9_]*)\"?\s*;?\s*$)",
	    std::regex_constants::icase);

	std::smatch matches;

	if (!std::regex_match(query, matches, pattern)) {
		// return error here
	}
	if (!matches[3].matched) throw ParserException(StringUtil::Format("No databox matching pattern for query '%s'", query));

	auto stm = make_uniq<CreateStatement>();
	auto create_info = make_uniq<CreateDataBoxInfo>();

	create_info->catalog = matches[1].matched ? matches[1].str() : "__current__";
	create_info->shelf = matches[2].matched ? matches[2].str() : "__current__";


	create_info->schema = matches[3].str();

	stm->info = std::move(create_info);
	stm->stmt_location = 0; // offset in the original string
	stm->stmt_length = query.length();
	stm->query = query;
	return ParserExtensionParseResult(make_uniq_base<ParserExtensionParseData, PsqlParseData>(std::move(stm)));
}

ParserExtensionPlanResult lakeshelf_plan(ParserExtensionInfo *, ClientContext &context,
                                         unique_ptr<ParserExtensionParseData> parse_data) {

	auto prql_state = make_shared_ptr<PsqlState>(std::move(parse_data));
	context.registered_state->Remove("lakeshelf_plan");
	context.registered_state->Insert("lakeshelf_plan", prql_state);

	throw BinderException("Use bind instead");
}

struct PrqlParserExtension : public ParserExtension {
	PrqlParserExtension() : ParserExtension() {
		parse_function = lakeshelf_parse;
		plan_function = lakeshelf_plan;
	}
};

BoundStatement prql_bind(ClientContext &context, Binder &binder, OperatorExtensionInfo *info, SQLStatement &statement) {

	auto lookup = context.registered_state->Get<PsqlState>("lakeshelf_plan");
	if (lookup) {
		auto prql_state = (PsqlState *)lookup.get();
		auto prql_parse_data = dynamic_cast<PsqlParseData *>(prql_state->parse_data.get());

		auto shelf_binder = Binder::CreateBinder(context, &binder);
		auto bound_stmt = shelf_binder->Bind(*(prql_parse_data->statement));
		return bound_stmt;
	}

	return {};
}

struct PrqlOperatorExtension : public OperatorExtension {
	PrqlOperatorExtension() : OperatorExtension() {
		Bind = prql_bind;
	}

	std::string GetName() override {
		return "prql";
	}

	unique_ptr<LogicalExtensionOperator> Deserialize(Deserializer &deserializer) override {
		throw InternalException("prql operator should not be serialized");
	}
};

static void LoadInternal(DatabaseInstance &instance) {
	ExtensionUtil::RegisterExtension(instance, "ducklake", {"Adds support for DuckLake, SQL as a Lakehouse Format"});

	auto &config = DBConfig::GetConfig(instance);
	config.storage_extensions["ducklake"] = make_uniq<DuckLakeStorageExtension>();

	PrqlParserExtension op;
	config.parser_extensions.push_back(op);
	config.operator_extensions.push_back(make_uniq<PrqlOperatorExtension>());

	DuckLakeSnapshotsFunction snapshots;
	ExtensionUtil::RegisterFunction(instance, snapshots);

	DuckLakeTableInfoFunction table_info;
	ExtensionUtil::RegisterFunction(instance, table_info);

	auto table_insertions = DuckLakeTableInsertionsFunction::GetFunctions();
	ExtensionUtil::RegisterFunction(instance, table_insertions);

	auto table_deletions = DuckLakeTableDeletionsFunction::GetFunctions();
	ExtensionUtil::RegisterFunction(instance, table_deletions);

	DuckLakeMergeAdjacentFilesFunction merge_adjacent_files;
	ExtensionUtil::RegisterFunction(instance, merge_adjacent_files);

	DuckLakeCleanupOldFilesFunction cleanup_old_files;
	ExtensionUtil::RegisterFunction(instance, cleanup_old_files);

	DuckLakeExpireSnapshotsFunction expire_snapshots;
	ExtensionUtil::RegisterFunction(instance, expire_snapshots);

	DuckLakeSetOptionFunction set_options;
	ExtensionUtil::RegisterFunction(instance, set_options);

	auto table_changes = DuckLakeTableInsertionsFunction::GetDuckLakeTableChanges();
	ExtensionUtil::RegisterFunction(instance, *table_changes);

	DuckLakeListFilesFunction list_files;
	ExtensionUtil::RegisterFunction(instance, list_files);
}

void DucklakeExtension::Load(DuckDB &db) {
	LoadInternal(*db.instance);
}
std::string DucklakeExtension::Name() {
	return "ducklake";
}

std::string DucklakeExtension::Version() const {
#ifdef EXT_VERSION_DUCKLAKE
	return EXT_VERSION_DUCKLAKE;
#else
	return "";
#endif
}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void ducklake_init(duckdb::DatabaseInstance &db) {
	duckdb::DuckDB db_wrapper(db);
	db_wrapper.LoadExtension<duckdb::DucklakeExtension>();
}

DUCKDB_EXTENSION_API const char *ducklake_version() {
	return duckdb::DuckDB::LibraryVersion();
}
}
