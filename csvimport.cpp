
// (c) 2013 Stephan Hohe

#include "soimport.hpp"
#include "csvbuilder.hpp"
#include "soschema.hpp"

int main(int argc, char *argv[]) {
	parse_config(CS_CSV, argc, argv);

	if (config.tempdir.empty())
		config.tempdir = ".";

	csvbuilder builder(config.tempdir);
	import_tables(builder);
}

