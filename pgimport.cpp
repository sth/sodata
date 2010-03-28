
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

	//std::auto_ptr<tablebuilder> builder;
	//builder.reset(new pgbuilder(db));
	if (config.tempdir.empty()) {
		pgbuilder builder(db);
		import_tables(builder);
	}
	else {
		pgcopybuilder builder(db, config.tempdir);
		import_tables(builder);
	}

	return 0;
}


