
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
	table_spec table;

public:
	sqliteloader(sqlite3 *a_db, const table_spec &a_table);
	void load();

protected:
	virtual void beginrow();
	virtual void setcolumn(int idx, const char *value);
	virtual void endrow();
};

sqliteloader::sqliteloader(sqlite3 *a_db, const table_spec &a_table)
		: xmltable(a_table.name, a_table.column_names()),
		  builder(a_db, a_table.name, a_table.column_names()), table(a_table) {
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

time_t parsedate(const char *value) {
	// 2009-07-12T22:51:42.563
	if (!value) {
		std::cerr << "warning: date parse failed for NULL" << std::endl;
		return 0;
	}
	tm t = {0};
	const char *rem = strptime(value, "%Y-%m-%dT%H:%M:%S", &t);
	if (!rem) {
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

void sqliteloader::setcolumn(int idx, const char *value) {
	if (!value) {
		// NULL
		builder.setcolumn(idx, value);
		return;
	}
	switch (table.columns[idx].type) {
	case CT_STR:
		builder.setcolumn(idx, value);
		break;
	case CT_INT:
		builder.setcolumn(idx, atoi(value));
		break;
	case CT_DATE:
		builder.setcolumn(idx, static_cast<int>(parsedate(value)));
		break;
	default:
		throw std::logic_error("invalid column type");
		break;
	}
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
		sqliteloader t(db, tables[i]);
		t.load();
	}

	rv = sqlite3_close(db);
	if (rv != SQLITE_OK)
		std::cerr << "cannot close db (" << sqlite3_errmsg(db) << ")" << std::endl;
	return 0;
}

