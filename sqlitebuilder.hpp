
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
	int cur_index;
	int row_count;

	std::string name;
	typedef std::vector<std::string> columns_t;
	columns_t columns;

public:
	sqlitebuilder(sqlite3 *a_db, const char *a_name, const columns_t &a_columns);
	~sqlitebuilder();

	virtual void open_table();
	virtual void table_complete();

	virtual void open_row();
	virtual void row_complete();

	virtual void add_column(const char *value);
	virtual void add_column(const std::string &value);
	virtual void add_column(int value);
	virtual void add_column(double value);

	virtual void add_index(const std::string &column);

private:
	template<typename T>
	void add_column_tmpl(T value);
	void begin_transaction();
	void end_transaction();
};

#endif // SQLITEBUILDER_HPP_INCLUDED

