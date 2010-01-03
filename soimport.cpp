
#include "xmldb.hpp"
#include "soschema.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>

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
#if defined(USE_PGSQL)
	const char *connect;
#endif
} config = {
	true,
#if defined(USE_PGSQL)
	""
#endif
};

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
// soloader class

class soloader : private xmltable {
private:
	tablebuilder &builder;
	table_spec table;

public:
	soloader(tablebuilder &a_builder, const table_spec &a_table);
	void load();

protected:
	virtual void beginrow();
	virtual void setcolumn(int idx, const char *value);
	virtual void endrow();
	void add_indexes();
};

soloader::soloader(tablebuilder &a_builder, const table_spec &a_table)
		: xmltable(a_table.name, a_table.column_names()),
		  builder(a_builder), table(a_table) {
}

void soloader::load() {
	std::cout << "loading " << name << std::endl;
	builder.begin_transaction();
	xmltable::load();
	builder.committable();
	builder.end_transaction();
	if (config.indexes)
		add_indexes();
}

void soloader::beginrow() {
	if (count % 100000 == 0) {
		builder.end_transaction();
		builder.begin_transaction();
	}
	builder.beginrow();
}

void soloader::setcolumn(int idx, const char *value) {
	if (!value) {
		// NULL
		builder.setcolumn(idx, value);
		return;
	}
	switch (table.column_defs[idx].type) {
	case CT_VCHR64:
	case CT_TEXT:
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

void soloader::endrow() {
	builder.commitrow();
}

void soloader::add_indexes() {
	const column_spec *columns = table.column_defs;
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
	for (int i = 1; i < argc; i++) {
		if      (strcmp("-h", argv[i]) == 0) {
			std::cout <<
					"Start from directory with Stack Overflow database dump" <<
					"Xml files to create a dump.db sqlite database." << std::endl <<
					std::endl <<
					"Options:" << std::endl <<
					"  -h           Display this help message" << std::endl <<
					"  -I           Don't add indexes" << std::endl;
#if defined(USE_PGSQL)
			std::cout <<
					"  -c CONNECT   Database connect string" << std::endl;
#endif
			return 0;
		}
		else if (strcmp("-I", argv[i]) == 0) {
			config.indexes = false;
		}
#if defined(USE_PGSQL)
		else if (strcmp("-c", argv[i]) == 0) {
			if (i+1 >= argc) {
				std::cerr << "missing argument after '-c'" << std::endl;
				return 1;
			}
			config.connect = argv[++i];
		}
#endif
		else {
			std::cerr << "unrecognized option: " << argv[i] << std::endl;
			std::cerr << "try -h for help" << std::endl;
			return 1;
		}
	}

#if defined(USE_SQLITE)
	int rv;
	sqlite3 *db;
	
	rv = sqlite3_open("dump.db", &db);
	if (rv != SQLITE_OK) {
		std::cerr << "cannot open db (" << sqlite3_errmsg(db) << ")" << std::endl;
		return 1;
	}
#elif defined(USE_PGSQL)
	pqxx::connection db(config.connect);
#else
#	error "No database engine specified"
#endif

	// import all tables
	for (size_t i=0; i<table_count; i++) {
		const table_spec &table = tables[i];
#if   defined(USE_SQLITE)
		sqlitebuilder builder(db, table.name, table.column_names());
#elif defined(USE_PGSQL)
		pgbuilder builder(&db, table.name, table.columns());
#endif
		soloader t(builder, table);
		t.load();
	}

#if defined(USE_SQLITE)
	rv = sqlite3_close(db);
	if (rv != SQLITE_OK)
		std::cerr << "cannot close db (" << sqlite3_errmsg(db) << ")" << std::endl;
#endif
	return 0;
}

