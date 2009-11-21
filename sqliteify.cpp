
#include "sqliteify.hpp"
#include <iostream>
#include <stdexcept>

tablebuilder::tablebuilder(sqlite3 *a_db, const char *a_name, const columns_t &a_columns)
		: db(a_db), prepared(NULL), name(a_name), columns(a_columns), count(0) {
	init();
}

tablebuilder::~tablebuilder() {
	done();
}

int tablebuilder::column_index(const std::string &name) {
	for (size_t i=0; i<columns.size(); i++) {
		if (columns[i] == name)
			return i;
	}
	throw std::logic_error("invalid column name: " + name);
}

void tablebuilder::begin_transaction() {
	int rv = sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, 0);
	if (rv != SQLITE_OK)
		std::cerr << "can't begin transaction: (" << sqlite3_errmsg(db) << ")" << std::endl;
}

void tablebuilder::end_transaction() {
	int rv = sqlite3_exec(db, "END TRANSACTION;", 0, 0, 0);
	if (rv != SQLITE_OK)
		std::cerr << "can't end transaction: (" << sqlite3_errmsg(db) << ")" << std::endl;
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
void tablebuilder::setcolumn_tmpl(int idx, T value){
	int rv;
	rv = sqlite3_bind_any(prepared, idx+1, value);
	if (rv != SQLITE_OK)
		std::cerr << "can't bind parameter in " << name <<
				" (" << sqlite3_errmsg(db) << "), value '" << value << "'" <<
				std::endl;
}

void tablebuilder::setcolumn(int idx, const char *value) {
	return setcolumn_tmpl(idx, value);
}

void tablebuilder::setcolumn(int idx, const std::string &value) {
	return setcolumn_tmpl(idx, value);
}

void tablebuilder::setcolumn(int idx, int value) {
	return setcolumn_tmpl(idx, value);
}

void tablebuilder::setcolumn(int idx, double value) {
	return setcolumn_tmpl(idx, value);
}

void tablebuilder::init() {
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

void tablebuilder::beginrow() {
	int rv;
	rv = sqlite3_reset(prepared);
	if (rv != SQLITE_OK)
		std::cerr << "sqlite3_reset: (" << sqlite3_errmsg(db) << ")" << std::endl;
	rv = sqlite3_clear_bindings(prepared);
	if (rv != SQLITE_OK)
		std::cerr << "sqlite3_clear_bindings: (" << sqlite3_errmsg(db) << ")" << std::endl;
}

void tablebuilder::commitrow() {
	int rv = sqlite3_step(prepared);
	if (rv != SQLITE_DONE)
		std::cerr << "sqlite3_step: (" << sqlite3_errmsg(db) << ")" << std::endl;
}

void tablebuilder::done() {
	sqlite3_finalize(prepared);
}

