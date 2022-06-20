
// (c) 2013 Stephan Hohe

#include "soimport.hpp"
#include "csvbuilder.hpp"
#include "soschema.hpp"

int main(int argc, const char *argv[]) {
	parse_config(CS_CSV, argc, argv);

	if (config.dir.empty()) {
		config.dir = ".";
	}

	csvbuilder builder(config.dir);
	import_tables(builder);
	return 0;
}

