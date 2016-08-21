
#include <pqxx/strconv> // Needs including first
#include "pgbuilder.hpp"
#include <iostream>
#include <unistd.h>

// ---------------------------------------------------------------------------
// pgcommon

pgcommon::pgcommon(pqxx::connection &a_db)
		: db(a_db), spec() {
}

void pgcommon::open_table_impl(const table_spec &a_spec) {
	spec = a_spec;

	try {
		pqxx::work w(db);
		w.exec("DROP TABLE " + std::string(spec.name) + ";");
		w.commit();
	}
	catch (const pqxx::undefined_table &) {
		// Ignore if this fails (for example table didn't exist)
	}

	std::string create = std::string("CREATE TABLE " )+ spec.name + " (";
	std::vector<column_spec> columns = spec.columns();
	for (std::vector<column_spec>::iterator it = columns.begin(); it != columns.end(); ++it) {
		create += std::string(it->name) + " ";
		switch (it->type) {
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
	w.exec(std::string("CREATE INDEX ") + spec.name + "_" + column + " ON " + spec.name + " (" + column + ");");
	w.commit();
}

// ---------------------------------------------------------------------------
// pgbuilder

pgbuilder::pgbuilder(pqxx::connection &a_db)
		: pgcommon(a_db), tablebuilder() {
}

void pgbuilder::open_table(const table_spec &a_spec) {
	pgcommon::open_table_impl(a_spec);

	cur_row.resize(spec.columns().size());
	cur_work.reset(new pqxx::work(db));
	cur_writer.reset(new pqxx::tablewriter(*cur_work, spec.name));
}

void pgbuilder::table_complete() {
	cur_writer->complete();
	cur_writer.reset();
	cur_work->commit();
	cur_work.reset();
}

void pgbuilder::open_row() {
	cur_idx = 0;
}

void pgbuilder::row_complete() {
	cur_writer->push_back(cur_row.begin(), cur_row.end());
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
	w.exec(std::string("COPY ") + spec.name + " FROM '" + filename(spec) + "' WITH DELIMITER AS ',';");
	w.commit();
	unlink(filename(spec).c_str());
}

void pgcopybuilder::add_index(const std::string &column) {
	add_index_impl(column);
}

