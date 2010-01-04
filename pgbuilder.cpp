
#include "pgbuilder.hpp"
#include <iostream>

// ---------------------------------------------------------------------------
// pgcommon

pgcommon::pgcommon(pqxx::connection &a_db, const char *a_name, const columns_t &a_columns)
		: db(a_db), name(a_name), columns(a_columns) {

	try {
		pqxx::work w(db);
		w.exec("DROP TABLE " + name + ";");
		w.commit();
	}
	catch (const pqxx::undefined_table &) {
		// Ignore if this fails (for example table didn't exist)
	}

	std::string create = "CREATE TABLE " + name + " (";
	for (columns_t::iterator it = columns.begin(); it != columns.end(); ++it) {
		create += std::string(it->name) + " ";
		switch (it->type) {
		case CT_VCHR64:
			create += "varchar(64)";
			break;
		case CT_TEXT:
			create += "text";
			break;
		case CT_INT:
		case CT_DATE:
			create += "int";
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
	w.exec("CREATE INDEX " + name + "_" + column + " ON " + name + " (" + column + ");");
	w.commit();
}

// ---------------------------------------------------------------------------
// pgbuilder

const std::string pgbuilder::stmt_insert("insertrow");

pgbuilder::pgbuilder(pqxx::connection &a_db, const char *a_name, const columns_t &a_columns)
		: tablebuilder(), pgcommon(a_db, a_name, a_columns) {
	std::ostringstream query;
	query << "INSERT INTO " << a_name << " VALUES (";
	for (size_t i=0; i<columns.size(); i++) {
		if (i)
			query << ", ";
		query << "$" << (i+1);
	}
	query << ");";
	const pqxx::prepare::declaration prep = db.prepare(stmt_insert, query.str());
	for (columns_t::iterator it = columns.begin(); it != columns.end(); ++it) {
		switch (it->type) {
		case CT_VCHR64:
		case CT_TEXT:
			prep("varchar", pqxx::prepare::treat_string);
			break;
		case CT_INT:
		case CT_DATE:
			prep("int", pqxx::prepare::treat_direct);
			break;
		default:
			throw std::runtime_error("unsupported column type");
		}
	}
}

void pgbuilder::open_table() {
	cur_work.reset(new pqxx::work(db));
}

void pgbuilder::table_complete() {
	cur_work->commit();
	cur_work.reset();
	db.unprepare(stmt_insert);
}

void pgbuilder::open_row() {
	cur_insert.reset(new pqxx::prepare::invocation(*cur_work, stmt_insert));
}

void pgbuilder::row_complete() {
	cur_insert->exec();
	cur_insert.reset();
}

void pgbuilder::add_column(const char *value) {
	if (value)
		(*cur_insert)(value);
	else
		(*cur_insert)();
}

void pgbuilder::add_column(const std::string &value) {
	(*cur_insert)(value);
}

void pgbuilder::add_column(int value) {
	(*cur_insert)(value);
}

void pgbuilder::add_column(double value) {
	(*cur_insert)(value);
}

void pgbuilder::add_index(const std::string &column) {
	add_index_impl(column);
}


// ---------------------------------------------------------------------------
// pgcopybuilder

pgcopybuilder::pgcopybuilder(pqxx::connection &a_db, const char *a_name, const columns_t &a_columns)
		: csvbuilder((std::string("/var/tmp/") + a_name + ".pgdata").c_str(), a_columns.size()),
		  pgcommon(a_db, a_name, a_columns) {
}

void pgcopybuilder::table_complete() {
	csvbuilder::table_complete();
	std::cout << "  (pushing to db...)" << std::endl;
	pqxx::work w(db);
	w.exec("COPY " + name + " FROM '/var/tmp/" + name + ".pgdata' WITH DELIMITER AS ',';");
	w.commit();
	unlink((std::string("/var/tmp/") + name + ".pgdata").c_str());
}

void pgcopybuilder::add_index(const std::string &column) {
	add_index_impl(column);
}

