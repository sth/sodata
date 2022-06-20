
#if !defined(SOIMPORT_HPP_INCLUDED)
#define SOIMPORT_HPP_INCLUDED

#include "xmldb.hpp"
#include "tablebuilder.hpp"
#include "dbspec.hpp"

class soloader : private xmltable {
protected:
	tablebuilder &builder;
	table_spec table;

public:
	soloader(tablebuilder &a_builder, const table_spec &a_table);
	void load();
	void add_indexes();

protected:
	virtual void open_row();
	virtual void add_column(const column_spec &col, const char *value);
	virtual void row_complete();
};

struct config_t {
	bool indexes;
	bool posttags;
	std::string only;
	bool simple;
	std::string connect;
	std::string dir;
	std::string dbfile;
	config_t() : indexes(true), posttags(true), only(), simple(false),
			connect(), dir(), dbfile() {
	}
};

extern config_t config;

enum configset_t {
	CS_PG,
	CS_SQLITE,
	CS_CSV,
};

void parse_config(configset_t cs, int argc, const char **argv);
void import_tables(tablebuilder &builder);

#endif // SOIMPORT_HPP_INCLUDED

