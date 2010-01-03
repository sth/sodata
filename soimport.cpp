
#include "xmldb.hpp"
#include "soschema.hpp"
#include <iostream>
#include <stdexcept>

#if defined(USE_SQLITE)
#include "sqlitebuilder.hpp"
#endif
#if defined(USE_PGSQL)
#include "pgbuilder.hpp"
#endif

// ---------------------------------------------------------------------------
// global config

static struct {
	bool indexes;
} config = {true};

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

// ---------------------------------------------------------------------------
// sqliteloader class

class sqliteloader : private xmltable {
private:
	tablebuilder &builder;
	table_spec table;

public:
	sqliteloader(tablebuilder &a_builder, const table_spec &a_table);
	void load();

protected:
	virtual void beginrow();
	virtual void setcolumn(int idx, const char *value);
	virtual void endrow();
	void add_indexes();
};

sqliteloader::sqliteloader(tablebuilder &a_builder, const table_spec &a_table)
		: xmltable(a_table.name, a_table.column_names()),
		  builder(a_builder), table(a_table) {
}

void sqliteloader::load() {
	std::cout << "loading " << name << std::endl;
	builder.begin_transaction();
	xmltable::load();
	builder.end_transaction();
	if (config.indexes)
		add_indexes();
}

void sqliteloader::beginrow() {
	if (count % 100000 == 0) {
		builder.end_transaction();
		builder.begin_transaction();
	}
	builder.beginrow();
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

void sqliteloader::add_indexes() {
	const column_spec *columns = table.columns;
	for (size_t idx=0; columns[idx].name; idx++) {
		switch (columns[idx].type) {
		case CT_INT:
		case CT_DATE:
			builder.add_index(columns[idx].name);
		default:
			break;
		}
	}
}

// ---------------------------------------------------------------------------
// main

int main(int argc, char *argv[]) {
	if (argc > 1) {
		if (std::string("-I") == argv[1]) {
			config.indexes = false;
		}
		else {
			std::cerr << "unrecognized option: " << argv[1] << std::endl;
			std::cout <<
					"Start from directory with Stack Overflow database dump" <<
					"Xml files to create a dump.db sqlite database." << std::endl <<
					std::endl <<
					"Options:" << std::endl <<
					"  -I  Don't add indexes" << std::endl;
			return 1;
		}
	}

	int rv;
	sqlite3 *db;
	
	rv = sqlite3_open("dump.db", &db);
	if (rv != SQLITE_OK) {
		std::cerr << "cannot open db (" << sqlite3_errmsg(db) << ")" << std::endl;
		return 1;
	}

	// import all tables
	for (size_t i=0; i<table_count; i++) {
		const table_spec &table = tables[i];
		sqlitebuilder builder(db, table.name, table.column_names());
		sqliteloader t(builder, table);
		t.load();
	}

	rv = sqlite3_close(db);
	if (rv != SQLITE_OK)
		std::cerr << "cannot close db (" << sqlite3_errmsg(db) << ")" << std::endl;
	return 0;
}

