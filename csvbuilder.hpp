
#if !defined(CSVBUILDER_HPP_INCLUDED)
#define CSVBUILDER_HPP_INCLUDED

#include "tablebuilder.hpp"
#include <string>
#include <fstream>

class csvbuilder : public tablebuilder {
private:
	std::ofstream csv;
	bool first_column;

public:
	csvbuilder(const char *name, const int column_count);

	virtual void open_table();
	virtual void table_complete();
	virtual void open_row();
	virtual void row_complete();
	virtual void add_column(const char *value);
	virtual void add_column(const std::string &value);
	virtual void add_column(int value);
	virtual void add_column(double value);

private:
	template<typename T>
	void add_column_tmpl(const T &data);
};

#endif // SQLITEIFY_HPP_INCLUDED


