
#if !defined(TABLEBUILDER_HPP_INCLUDED)
#define TABLEBUILDER_HPP_INCLUDED

#include <string>

class tablebuilder {
public:
	tablebuilder() {}
	virtual ~tablebuilder() {}

	virtual void open_table() = 0;
	virtual void table_complete() = 0;

	virtual void open_row() = 0;
	virtual void row_complete() = 0;

	virtual void add_column(const char *value) = 0;
	virtual void add_column(const std::string &value) = 0;
	virtual void add_column(int value) = 0;
	virtual void add_column(double value) = 0;

	virtual void add_index(const std::string &column) {}
};

#endif // TABLEBUILDER_HPP_INCLUDED

