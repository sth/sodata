
#if !defined(PGBUILDER_HPP_INCLUDED)
#define PGBUILDER_HPP_INCLUDED

#include "csvbuilder.hpp"
#include "dbspec.hpp"
#include <string>
#include <vector>
#include <memory>
#include <pqxx/connection>
#include <pqxx/transaction>
#include <pqxx/tablewriter>

class pgcommon {
protected:
	pqxx::connection &db;
	table_spec spec;

	pgcommon(pqxx::connection &db);
	void add_index_impl(const std::string &column);
	void open_table_impl(const table_spec &a_spec);
};


class pgbuilder : private pgcommon, public tablebuilder {
private:
	std::vector<const char *> cur_row;
	std::unique_ptr<pqxx::work> cur_work;
	std::unique_ptr<pqxx::tablewriter> cur_writer;
	int cur_idx;

public:
	pgbuilder(pqxx::connection &db);

	virtual void open_table(const table_spec &spec);
	virtual void table_complete();
	virtual void open_row();
	virtual void row_complete();
	virtual void add_column(const column_spec &col, const char *value);

	virtual void add_index(const std::string &column);
};


class pgcopybuilder : private pgcommon, public csvbuilder {
public:
	pgcopybuilder(pqxx::connection &db, const std::string &tempdir);

	virtual void open_table(const table_spec &a_spec);
	virtual void table_complete();
	virtual void add_index(const std::string &column);
};

#endif // PGBUILDER_HPP_INCLUDED

