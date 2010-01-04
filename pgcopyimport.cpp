
// (c) 2009, 2010 Stephan Hohe

#include "soimport.hpp"
#include "pgbuilder.hpp"
#include "soschema.hpp"
#include <iostream>
#include <cstring>


// ---------------------------------------------------------------------------
// global config

static struct {
	bool indexes;
	const char *connect;
} config = { true, "" };


// ---------------------------------------------------------------------------
// main

int main(int argc, char *argv[]) {
	for (int i = 1; i < argc; i++) {
		if      (strcmp("-h", argv[i]) == 0) {
			std::cout <<
					"Start from directory with Stack Overflow database dump " <<
					"Xml files to fill a postgresql database with the dump.\n" <<
					"This program creates temporary text files in /var/tmp "
					"contining the data and then loads it into the dpecified " <<
					"database with \"COPY FROM 'file'\"." << std::endl <<
					std::endl <<
					"Options:\n" << std::endl <<
					"  -h           Display this help message" << std::endl <<
					"  -I           Don't add indexes" << std::endl <<
					"  -c CONNECT   Database connect string" << std::endl;
			return 0;
		}
		else if (strcmp("-I", argv[i]) == 0) {
			config.indexes = false;
		}
		else if (strcmp("-c", argv[i]) == 0) {
			if (i+1 >= argc) {
				std::cerr << "missing argument after '-c'" << std::endl;
				return 1;
			}
			config.connect = argv[++i];
		}
		else {
			std::cerr << "unrecognized option: " << argv[i] << std::endl;
			std::cerr << "try -h for help" << std::endl;
			return 1;
		}
	}

	pqxx::connection db(config.connect);

	// import all tables
	for (size_t i=0; i<table_count; i++) {
		const table_spec &table = tables[i];
		pgcopybuilder builder(db, table.name, table.columns());
		soloader t(builder, table);
		t.load();
		if (config.indexes)
			t.add_indexes();
	}

	return 0;
}


