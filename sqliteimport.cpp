
// (c) 2009, 2010 Stephan Hohe

#include "soimport.hpp"
#include "sqlitebuilder.hpp"
#include "soschema.hpp"
#include <iostream>
#include <cstring>

// ---------------------------------------------------------------------------
// main

int main(int argc, const char *argv[]) {
	int rv;
	sqlite3 *db;

	parse_config(CS_SQLITE, argc, argv);
	
	if (config.dbfile.empty()) {
		config.dbfile = "dump.db";
	}
	rv = sqlite3_open(config.dbfile.c_str(), &db);
	if (rv != SQLITE_OK) {
		std::cerr << "cannot open db (" << sqlite3_errmsg(db) << ")" << std::endl;
		return 1;
	}

	sqlitebuilder builder(db);
	import_tables(builder);

	rv = sqlite3_close(db);
	if (rv != SQLITE_OK)
		std::cerr << "cannot close db (" << sqlite3_errmsg(db) << ")" << std::endl;
	return 0;
}

