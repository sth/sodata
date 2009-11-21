
#if !defined(SQLITEIFY_HPP_INCLUDED)
#define SQLITEIFY_HPP_INCLUDED

#include <sqlite3.h>
#include <string>
#include <vector>

class tablebuilder {
private:
	sqlite3 *db;
	sqlite3_stmt *prepared;

	std::string name;
	typedef std::vector<std::string> columns_t;
	columns_t columns;

	int count;

public:
	tablebuilder(sqlite3 *a_db, const char *a_name, const columns_t &a_columns);
	~tablebuilder();

	int column_index(const std::string &name);
	void begin_transaction();
	void end_transaction();

	void beginrow();

	void setcolumn(int idx, const char *value);
	void setcolumn(int idx, const std::string &value);
	void setcolumn(int idx, int value);
	void setcolumn(int idx, double value);

	void commitrow();
	void rollbackrow();

private:
	template<typename T>
	void setcolumn_tmpl(int idx, T value);
	void init();
	void done();
};

#endif // SQLITEIFY_HPP_INCLUDED

