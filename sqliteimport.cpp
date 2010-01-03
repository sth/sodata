
// (c) 2009, 2010 Stephan Hohe

#include "soimport.hpp"
#include "sqlitebuilder.hpp"
#include "soschema.hpp"
#include <iostream>
#include <cstring>

// ---------------------------------------------------------------------------
// global config

static struct {
	bool indexes;
} config = { true };


// ---------------------------------------------------------------------------
// main

int main(int argc, char *argv[]) {
	for (int i = 1; i < argc; i++) {
		if      (strcmp("-h", argv[i]) == 0) {
			std::cout <<
					"Start from directory with Stack Overflow database dump" <<
					"Xml files to create a dump.db sqlite database." << std::endl <<
					std::endl <<
					"Options:" << std::endl <<
					"  -h           Display this help message" << std::endl <<
					"  -I           Don't add indexes" << std::endl;
			return 0;
		}
		else if (strcmp("-I", argv[i]) == 0) {
			config.indexes = false;
		}
		else {
			std::cerr << "unrecognized option: " << argv[i] << std::endl;
			std::cerr << "try -h for help" << std::endl;
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
		soloader t(builder, table);
		t.load();
		if (config.indexes)
			t.add_indexes();
	}

	rv = sqlite3_close(db);
	if (rv != SQLITE_OK)
		std::cerr << "cannot close db (" << sqlite3_errmsg(db) << ")" << std::endl;
	return 0;
}

