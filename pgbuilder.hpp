
#if !defined(PGBUILDER_HPP_INCLUDED)
#define PGBUILDER_HPP_INCLUDED

#include "csvbuilder.hpp"
#include "dbspec.hpp"
#include <string>
#include <vector>
#include <pqxx/connection>
#include <pqxx/transaction>
#include <pqxx/prepared_statement>

class pgcommon {
protected:
	pqxx::connection &db;
	std::string name;
	typedef std::vector<column_spec> columns_t;
	columns_t columns;

	pgcommon(pqxx::connection &db, const char *a_name, const columns_t &a_columns);
	void add_index_impl(const std::string &column);
};

class pgbuilder : public tablebuilder, pgcommon {
private:
	static const std::string stmt_insert;
	std::auto_ptr<pqxx::work> cur_work;
	std::auto_ptr<pqxx::prepare::invocation> cur_insert;

public:
	pgbuilder(pqxx::connection &db, const char *a_name, const columns_t &a_columns);

	virtual void open_table();
	virtual void table_complete();
	virtual void open_row();
	virtual void row_complete();
	virtual void add_column(const char *value);
	virtual void add_column(const std::string &value);
	virtual void add_column(int value);
	virtual void add_column(double value);

	virtual void add_index(const std::string &column);
};

class pgcopybuilder : public csvbuilder, pgcommon {
public:
	pgcopybuilder(pqxx::connection &db, const char *a_name, const columns_t &a_columns);

	void table_complete();
	void add_index(const std::string &column);
};

#endif // PGBUILDER_HPP_INCLUDED

