
#include "sqlitebuilder.hpp"
#include <iostream>
#include <stdexcept>

sqlitebuilder::sqlitebuilder(sqlite3 *a_db, const char *a_name, const columns_t &a_columns)
		: db(a_db), prepared(NULL), cur_index(0), row_count(0), name(a_name), columns(a_columns) {
	int rv;

	std::string drop = "DROP TABLE IF EXISTS " + name + ";";
	std::string create = "CREATE TABLE " + name + " (";
	std::string prepare = "INSERT INTO " + name + " VALUES (";
	for (columns_t::iterator it = columns.begin(); it != columns.end(); ++it) {
		create += *it + ", ";
		prepare += "?, ";
	}
	create.erase(create.size() - 2);
	prepare.erase(prepare.size() - 2);
	create += ");";
	prepare += ");";

	rv = sqlite3_exec(db, drop.c_str(), 0, 0, 0);
	if (rv != SQLITE_OK)
		std::cerr << "can't drop existing table: (" << sqlite3_errmsg(db) << ")" << std::endl;

	rv = sqlite3_exec(db, create.c_str(), 0, 0, 0);
	if (rv != SQLITE_OK)
		throw std::runtime_error("can't create table " + name + " (" + sqlite3_errmsg(db) + ")");

	rv = sqlite3_prepare_v2(db, prepare.c_str(), -1, &prepared, 0);
	if (rv != SQLITE_OK)
		throw std::runtime_error("can't prepare insert for table " + name + " (" + sqlite3_errmsg(db) + ")");
}

sqlitebuilder::~sqlitebuilder() {
	sqlite3_finalize(prepared);
}

/*
int sqlitebuilder::column_index(const std::string &name) {
	for (size_t i=0; i<columns.size(); i++) {
		if (columns[i] == name)
			return i;
	}
	throw std::logic_error("invalid column name: " + name);
}
*/

void sqlitebuilder::begin_transaction() {
	int rv = sqlite3_exec(db, "BEGIN;", 0, 0, 0);
	if (rv != SQLITE_OK)
		std::cerr << "can't begin transaction: (" << sqlite3_errmsg(db) << ")" << std::endl;
}

void sqlitebuilder::end_transaction() {
	int rv = sqlite3_exec(db, "COMMIT;", 0, 0, 0);
	if (rv != SQLITE_OK)
		std::cerr << "can't commit transaction: (" << sqlite3_errmsg(db) << ")" << std::endl;
}

int sqlite3_bind_any(sqlite3_stmt* s, int i, const char *v) {
	if (v)
		return sqlite3_bind_text(s, i, v, -1, SQLITE_TRANSIENT);
	else
		return sqlite3_bind_null(s, i);
}

int sqlite3_bind_any(sqlite3_stmt* s, int i, const std::string &v) {
	return sqlite3_bind_any(s, i, v.c_str());
}

int sqlite3_bind_any(sqlite3_stmt* s, int i, int v) {
	return sqlite3_bind_int(s, i, v);
}

int sqlite3_bind_any(sqlite3_stmt* s, int i, double v) {
	return sqlite3_bind_double(s, i, v);
}

// now that's a case where a simple copy & paste would probably have been
// better...
template<typename T>
void sqlitebuilder::add_column_tmpl(T value){
	int rv;
	rv = sqlite3_bind_any(prepared, ++cur_index, value);
	if (rv != SQLITE_OK)
		std::cerr << "can't bind parameter in " << name <<
				" (" << sqlite3_errmsg(db) << "), value '" << value << "'" <<
				std::endl;
}

void sqlitebuilder::add_column(const char *value) {
	return add_column_tmpl(value);
}

void sqlitebuilder::add_column(const std::string &value) {
	return add_column_tmpl(value);
}

void sqlitebuilder::add_column(int value) {
	return add_column_tmpl(value);
}

void sqlitebuilder::add_column(double value) {
	return add_column_tmpl(value);
}

void sqlitebuilder::open_table() {
	begin_transaction();
}

void sqlitebuilder::table_complete() {
	end_transaction();
}

void sqlitebuilder::open_row() {
	int rv;
	if (++row_count % 100000 == 0) {
		end_transaction();
		begin_transaction();
	}
	rv = sqlite3_reset(prepared);
	if (rv != SQLITE_OK)
		std::cerr << "sqlite3_reset: (" << sqlite3_errmsg(db) << ")" << std::endl;
	rv = sqlite3_clear_bindings(prepared);
	if (rv != SQLITE_OK)
		std::cerr << "sqlite3_clear_bindings: (" << sqlite3_errmsg(db) << ")" << std::endl;
	cur_index = 0;
}

void sqlitebuilder::row_complete() {
	int rv = sqlite3_step(prepared);
	if (rv != SQLITE_DONE)
		std::cerr << "sqlite3_step: (" << sqlite3_errmsg(db) << ")" << std::endl;
}

void sqlitebuilder::add_index(const std::string &column) {
	std::string indexname = name + "_" + column;
	std::string sql = "CREATE INDEX " + indexname + " ON " + name + " (" + column + ");";
	int rv = sqlite3_exec(db, sql.c_str(), 0, 0, 0);
	if (rv != SQLITE_OK)
		std::cerr << "sqlite3_exec: (" << sqlite3_errmsg(db) << ")" << std::endl;
}

