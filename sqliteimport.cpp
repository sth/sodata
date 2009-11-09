
#include "xmldb.hpp"
#include "sqliteify.hpp"
#include "soschema.hpp"
#include <iostream>
#include <stdexcept>

// ---------------------------------------------------------------------------
// sqliteloader class

class sqliteloader : private xmltable {
private:
	tablebuilder builder;

public:
	sqliteloader(sqlite3 *a_db, const char *a_name, const char * const *a_columns);
	void load();

protected:
	virtual void beginrow();
	virtual void setcolumn(int idx, const char *value);
	virtual void endrow();
};


sqliteloader::sqliteloader(sqlite3 *a_db, const char *a_name, const char * const *a_columns)
		: xmltable(a_name, a_columns), builder(a_db, a_name, a_columns) {
}

void sqliteloader::load() {
	builder.begin_transaction();
	xmltable::load();
	builder.end_transaction();
}

void sqliteloader::beginrow() {
	if (count % 100000 == 0) {
		builder.end_transaction();
		builder.begin_transaction();
	}
	builder.beginrow();
}

void sqliteloader::setcolumn(int idx, const char *value) {
	builder.setcolumn(idx, value);
}

void sqliteloader::endrow() {
	builder.commitrow();
}


// ---------------------------------------------------------------------------
// main

int main(int argc, char *argv[]) {
	int rv;
	sqlite3 *db;
	
	rv = sqlite3_open("dump.db", &db);
	if (rv != SQLITE_OK) {
		std::cerr << "cannot open db (" << sqlite3_errmsg(db) << ")" << std::endl;
		return 1;
	}

	// import all tables
	for (size_t i=0; i<table_count; i++) {
		sqliteloader t(db, tables[i].name, tables[i].columns);
		t.load();
	}

	rv = sqlite3_close(db);
	if (rv != SQLITE_OK)
		std::cerr << "cannot close db (" << sqlite3_errmsg(db) << ")" << std::endl;
	return 0;
}

