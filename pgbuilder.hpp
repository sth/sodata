
#if !defined(PGBUILDER_HPP_INCLUDED)
#define PGBUILDER_HPP_INCLUDED

#include "csvbuilder.hpp"
#include "dbspec.hpp"
#include <string>
#include <vector>
#include <pqxx/connection.hxx>

class pgbuilder : public csvbuilder {
private:
	pqxx::connection *db;

	std::string name;
	typedef std::vector<column_spec> columns_t;
	columns_t columns;

public:
	pgbuilder(pqxx::connection *db, const char *a_name, const columns_t &a_columns);

	void committable();
	void add_index(const std::string &column);
};

#endif // PGBUILDER_HPP_INCLUDED

