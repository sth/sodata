
// (c) 2009, 2010 Stephan Hohe

#include "soimport.hpp"
#include "pgbuilder.hpp"
#include "soschema.hpp"
#include <iostream>
#include <cstring>

#if defined(__cpp_lib_filesystem)
#include <filesystem>
using std::filesystem::absolute;

#elif defined(__cpp_lib_experimental_filesystem)
#include <experimental/filesystem>
using std::experimental::filesystem::absolute;

#else

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdexcept>
std::string absolute(const std::string &relpath) {
	char* abspath = realpath(relpath.c_str(), nullptr);
	if (!abspath) {
		throw std::runtime_error(relpath + ": cannot resolve absolute path name: " + strerror(errno));
	}
	std::string absstr(abspath);
	free(abspath);
	return absstr;
}

#endif // filesystem


// ---------------------------------------------------------------------------
// main

int main(int argc, const char *argv[]) {
	parse_config(CS_PG, argc, argv);

	pqxx::connection db(config.connect);

	if (config.simple) {
		pgbuilder builder(db);
		import_tables(builder);
	}
	else {
		// We need an absolute path for this to work.
		std::string absdir = absolute(config.dir);
		pgcopybuilder builder(db, absdir);
		import_tables(builder);
	}

	return 0;
}

