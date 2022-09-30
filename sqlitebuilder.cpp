
#include "sqlitebuilder.hpp"
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>


// ---------------------------------------------------------------------------
// helper functions

time_t parsedate(const char *value) {
	if (!value) {
		std::cerr << "warning: date parse failed for NULL" << std::endl;
		return 0;
	}
	tm t = {0};
	// try 2009-07-12T22:51:42.563
	const char *rem = strptime(value, "%Y-%m-%dT%H:%M:%S", &t);
	if (!rem) {
		// try 2009-07-12
		rem = strptime(value, "%Y-%m-%d", &t);
		if (!rem) {
			std::cerr << "warning: date parse failed for '" << value << "'" << std::endl;
			return 0;
		}
		else if (*rem != '\0') {
			// error
			std::cerr << "warning: date parse has unknown trailing data for '" << value << "'" << std::endl;
			return 0;
		}
	}
	else if (*rem != '.') {
		// error
		std::cerr << "warning: date parse has unknown trailing data for '" << value << "'" << std::endl;
		return 0;
	}
	return timegm(&t);
}

int parsebool(const char *value) {
	if (std::strcmp(value, "True") == 0)
		return 1;
	else if (std::strcmp(value, "False") == 0)
		return 0;
	else
		throw std::runtime_error(std::string("cannot parse value as bool: ") + value);
}


// ---------------------------------------------------------------------------
// sqlitebuilder

sqlitebuilder::sqlitebuilder(sqlite3 *a_db)
		: db(a_db), prepared(NULL), cur_index(0), row_count(0), name() {
}

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

void sqlitebuilder::open_table(const table_spec &spec) {
	int rv;

	name = spec.name;
	columns_t columns = spec.column_names();

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

	row_count = 0;
	begin_transaction();
}

void sqlitebuilder::table_complete() {
	sqlite3_finalize(prepared);
	prepared = NULL;
	end_transaction();
}

int sqlite3_bind_any(sqlite3_stmt* s, int i, const char *v) {
	if (v)
		return sqlite3_bind_text(s, i, v, -1, SQLITE_STATIC);
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

void sqlitebuilder::add_column(const column_spec &col, const char *value) {
	if (!value) {
		// NULL
		add_column_tmpl(value);
		return;
	}
	switch (col.type) {
	case CT_VCHR12:
	case CT_VCHR64:
	case CT_TEXT:
		add_column_tmpl(value);
		break;
	case CT_INT:
		add_column_tmpl(atoi(value));
		break;
	case CT_DATE:
		add_column_tmpl(static_cast<int>(parsedate(value)));
		break;
	case CT_BOOL:
		add_column_tmpl(parsebool(value));
		break;
	default:
		throw std::logic_error("invalid column type");
		break;
	}
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

