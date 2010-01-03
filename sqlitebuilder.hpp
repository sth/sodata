
#if !defined(SQLITEBUILDER_HPP_INCLUDED)
#define SQLITEBUILDER_HPP_INCLUDED

#include "tablebuilder.hpp"
#include <sqlite3.h>
#include <string>
#include <vector>

class sqlitebuilder : public tablebuilder {
private:
	sqlite3 *db;
	sqlite3_stmt *prepared;

	std::string name;
	typedef std::vector<std::string> columns_t;
	columns_t columns;

public:
	sqlitebuilder(sqlite3 *a_db, const char *a_name, const columns_t &a_columns);
	~sqlitebuilder();

	int column_index(const std::string &name);
	void begin_transaction();
	void end_transaction();

	void beginrow();

	void setcolumn(int idx, const char *value);
	void setcolumn(int idx, const std::string &value);
	void setcolumn(int idx, int value);
	void setcolumn(int idx, double value);

	void commitrow();

	void add_index(const std::string &column);

private:
	template<typename T>
	void setcolumn_tmpl(int idx, T value);
};

#endif // SQLITEBUILDER_HPP_INCLUDED

