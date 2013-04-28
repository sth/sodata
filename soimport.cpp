
#include "soimport.hpp"
#include "soschema.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <cassert>

// ---------------------------------------------------------------------------
// soloader class

soloader::soloader(tablebuilder &a_builder, const table_spec &a_table)
		: xmltable(a_table.name, a_table.columns()),
		  builder(a_builder), table(a_table) {
}

void soloader::load() {
	std::cout << "loading " << name << std::endl;
	builder.open_table(table);
	xmltable::load();
	builder.table_complete();
}

void soloader::open_row() {
	builder.open_row();
}

void soloader::add_column(const column_spec &col, const char *value) {
	builder.add_column(col, value);
}

void soloader::row_complete() {
	builder.row_complete();
}

void soloader::add_indexes() {
	const column_spec *columns = table.column_defs;
	for (size_t idx=0; columns[idx].name; idx++) {
		switch (columns[idx].type) {
		case CT_INT:
		case CT_DATE:
			std::cout << "  (indexing " << columns[idx].name << "...)" << std::endl;
			builder.add_index(columns[idx].name);
		default:
			break;
		}
	}
}

// ---------------------------------------------------------------------------
// sopostloader class

class sopostloader : public soloader {
private:
	typedef std::vector< std::pair<int, std::string> > tags_t;
	tags_t tags;

	int plidx_cur, plidx_id, plidx_posttypeid, plidx_tags, plidx_body;
	int pl_curid, pl_curposttype;

public:
	sopostloader(tablebuilder &a_builder, const table_spec &a_table);
	virtual void open_row();
	virtual void add_column(const column_spec &col, const char *value);

	void write_tags(const table_spec &spec);
	//void write_posttags(const table_spec &spec);
};

sopostloader::sopostloader(tablebuilder &a_builder, const table_spec &a_table)
		: soloader(a_builder, a_table), tags() {
	plidx_id = a_table.column_index("Id");
	plidx_posttypeid = a_table.column_index("PostTypeId");
	plidx_tags = a_table.column_index("Tags");
	plidx_body = a_table.column_index("Body");
}

void sopostloader::open_row() {
	soloader::open_row();
	plidx_cur = -1;
	pl_curid = 0;
	pl_curposttype = 0;
}

void sopostloader::add_column(const column_spec &col, const char *value) {
	soloader::add_column(col, value);

	plidx_cur++;
	if (!value)
		return;

	if (plidx_cur == plidx_id) {
		pl_curid = atoi(value);
	}
	else if (plidx_cur == plidx_posttypeid) {
		pl_curposttype = atoi(value);
	}
	else if (config.tags && (plidx_cur == plidx_tags) && (pl_curposttype == 1)) {
		std::string tname(value);
		size_t s, e;
		s = 0;
		while (s < tname.length()) {
			e = tname.find('>', s);
			if ((tname[s] != '<') || (e == tname.npos)) {
				std::cerr << "invalid tags: " << tname << std::endl;
				break;
			}
			std::string tag(tname.substr(s+1, e-s-1));
			tags.push_back(std::make_pair(pl_curid, tag));
			s = e + 1;
		}
	}
}

class dbvalue {
	std::string value;
public:
	template<typename T>
	dbvalue(T val) {
		std::ostringstream fmt;
		fmt << val;
		value = fmt.str();
	}
	const char* get() {
		return value.c_str();
	}
};

void sopostloader::write_tags(const table_spec &spec) {
	std::cout << "writing " << spec.name << std::endl;
	std::vector<column_spec> columns = spec.columns();
	builder.open_table(spec);
	int counter(0);
	for (tags_t::iterator it = tags.begin(); it != tags.end(); ++it) {
		if (++counter % 100000 == 0)
			std::cout << "  (" << counter << " datasets)" << std::endl;
		builder.open_row();
		dbvalue id(it->first);
		builder.add_column(columns[0], id.get());
		builder.add_column(columns[1], it->second.c_str());
		builder.row_complete();
	}
	std::cout << "  (" << counter << " datasets)" << std::endl;
	builder.table_complete();
	if (config.indexes) {
		std::cout << "  (indexing " << columns[0].name << "...)" << std::endl;
		builder.add_index(columns[0].name);
		std::cout << "  (indexing " << columns[1].name << "...)" << std::endl;
		builder.add_index(columns[1].name);
	}
}

// ---------------------------------------------------------------------------
// utility functions

void load_standard_table(tablebuilder &builder, const table_spec &table) {
	soloader loader(builder, table);
	loader.load();
	if (config.indexes) {
		loader.add_indexes();
	}
}

void import_tables(tablebuilder &builder) {
	// import all tables
	load_standard_table(builder, users_table);
	load_standard_table(builder, badges_table);
	load_standard_table(builder, votes_table);
	load_standard_table(builder, comments_table);

	sopostloader loader(builder, posts_table);
	loader.load();
	if (config.indexes) {
		loader.add_indexes();
	}
	if (config.tags) {
		loader.write_tags(tags_table);
	}

	load_standard_table(builder, posthistory_table);
}


// ---------------------------------------------------------------------------
// configuration

config_t config;

void parse_config(configset_t cs, int argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		if      (strcmp("-h", argv[i]) == 0) {
			std::cout <<
					"Start from directory with Stack Overflow database dump" <<
					"Xml files to create a dump.db sqlite database." << std::endl <<
					std::endl <<
					"Options:" << std::endl <<
					"  -h           Display this help message" << std::endl <<
					"  -I           Don't add indexes" << std::endl <<
					"  -T           Don't add a tags table" << std::endl;
			switch (cs) {
			case CS_PG:
				std::cout <<
					"  -c CONNECT   Database connect string/filename" << std::endl <<
					"  -d DIRNAME   Use temporary files in this directory" << std::endl;
			case CS_SQLITE:
				std::cout <<
					"  -f FILENAME  Name of database file" << std::endl;
				break;
			case CS_CSV:
				std::cout <<
					"  -d DIRNAME   Put CSV files in this directory" << std::endl;
			default:
				assert(0);
			}
			exit(0);
		}
		else if (strcmp("-I", argv[i]) == 0) {
			config.indexes = false;
		}
		else if (strcmp("-T", argv[i]) == 0) {
			config.tags = false;
		}
		else if (strcmp("-c", argv[i]) == 0) {
			if (i+1 >= argc) {
				std::cerr << "missing argument after '-c'" << std::endl;
				exit(1);
			}
			config.connect = argv[++i];
		}
		else if (strcmp("-d", argv[i]) == 0) {
			if (i+1 >= argc) {
				std::cerr << "missing argument after '-d'" << std::endl;
				exit(1);
			}
			config.tempdir = argv[++i];
		}
		else if (strcmp("-f", argv[i]) == 0) {
			if (i+1 >= argc) {
				std::cerr << "missing argument after '-f'" << std::endl;
				exit(1);
			}
			config.dbfile = argv[++i];
		}
		else {
			std::cerr << "unrecognized option: " << argv[i] << std::endl;
			std::cerr << "try -h for help" << std::endl;
			exit(1);
		}
	}
}

