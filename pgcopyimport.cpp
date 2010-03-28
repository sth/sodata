
// (c) 2009, 2010 Stephan Hohe

#include "soimport.hpp"
#include "pgbuilder.hpp"
#include "soschema.hpp"
#include <iostream>
#include <cstring>

// ---------------------------------------------------------------------------
// main

int main(int argc, char *argv[]) {
	parse_config(CS_PG, argc, argv);

	pqxx::connection db(config.connect);

	pgcopybuilder builder(db, config.tempdie);
	import_tables(builder);

	return 0;
}


