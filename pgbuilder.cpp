
#include <pqxx/strconv> // Needs including first
#include "pgbuilder.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>

// ---------------------------------------------------------------------------
// pgcommon

pgcommon::pgcommon(pqxx::connection &a_db)
		: db(a_db), tablename() {
}

void pgcommon::open_table_impl(const table_spec &spec) {
	tablename = spec.name;

	try {
		pqxx::work w(db);
		w.exec("DROP TABLE " + std::string(spec.name) + ";");
		w.commit();
	}
	catch (const pqxx::undefined_table &) {
		// Ignore if this fails (for example table didn't exist)
	}

	std::string create = std::string("CREATE TABLE " )+ spec.name + " (";
	const std::vector<column_spec> &columns = spec.columns;
	for (auto it = columns.begin(); it != columns.end(); ++it) {
		create += std::string(it->name) + " ";
		switch (it->type) {
		case CT_VCHR12:
			create += "varchar(12)";
			break;
		case CT_VCHR64:
			create += "varchar(64)";
			break;
		case CT_TEXT:
			create += "text";
			break;
		case CT_INT:
			create += "int";
			break;
		case CT_DATE:
			create += "timestamp";
			break;
		case CT_BOOL:
			create += "boolean";
			break;
		default:
			throw std::runtime_error("Unsupported column type");
		}
		create += ", ";
	}
	create.erase(create.size() - 2);
	create += ");";
	pqxx::work w(db);
	w.exec(create);
	w.commit();
}

void pgcommon::add_index_impl(const std::string &column) {
	pqxx::work w(db);
	w.exec(std::string("CREATE INDEX ") + tablename + "_" + column + " ON " + tablename + " (" + column + ");");
	w.commit();
}

// ---------------------------------------------------------------------------
// pgbuilder

pgbuilder::pgbuilder(pqxx::connection &a_db)
		: pgcommon(a_db), tablebuilder() {
}

std::string insert_stmt(const std::string &table, size_t count) {
	if (!count) {
		throw std::logic_error("cannot insert into table without columns");
	}
	std::ostringstream sql;
	sql << "INSERT INTO " << table << " VALUES ($1";
	for (size_t i = 1; i < count; ++i) {
		sql << ", $" << i+1;
	}
	sql << ")";
	return sql.str();
}

void pgbuilder::open_table(const table_spec &spec) {
	pgcommon::open_table_impl(spec);

	cur_row.resize(spec.columns.size());
	cur_insert = std::string("insert_") + spec.name;
	db.prepare(cur_insert, insert_stmt(spec.name, spec.columns.size()));
	cur_work.reset(new pqxx::work(db));
}

void pgbuilder::table_complete() {
	cur_work->commit();
	cur_work.reset();
}

void pgbuilder::open_row() {
	cur_idx = 0;
}

void pgbuilder::row_complete() {
	// This usage of `prepared` is deprecated. We should use `exec_prepared` and pass all
	// the parameters as template parameters.
	//
	// But we don't know the number of parameters. And the same pgbuilder is used to
	// build multiple tables, which need a different number of parameters. So we don't know
	// the required number of parameters statically and cannot create a corresponding call
	// with the required parameters.
	pqxx::prepare::invocation &&inv = cur_work->prepared(cur_insert);
	for (auto str : cur_row) {
		inv(str);
	}
	inv.exec();
}

void pgbuilder::add_column(const column_spec &col, const char *value) {
	// TODO: For general usage, this should store a copy
	cur_row[cur_idx++] = value;
}

void pgbuilder::add_index(const std::string &column) {
	add_index_impl(column);
}


// ---------------------------------------------------------------------------
// pgcopybuilder

pgcopybuilder::pgcopybuilder(pqxx::connection &a_db, const std::string &tempdir)
		: pgcommon(a_db), csvbuilder(tempdir) {
}

void pgcopybuilder::open_table(const table_spec &a_spec) {
	pgcommon::open_table_impl(a_spec);
	csvbuilder::open_table(a_spec);
}

void pgcopybuilder::table_complete() {
	csvbuilder::table_complete();
	std::cout << "  (pushing to db...)" << std::endl;
	pqxx::work w(db);
	w.exec(std::string("COPY ") + tablename + " FROM '" + filename(tablename) + "' WITH DELIMITER AS ',';");
	w.commit();
	unlink(filename(tablename).c_str());
}

void pgcopybuilder::add_index(const std::string &column) {
	add_index_impl(column);
}

