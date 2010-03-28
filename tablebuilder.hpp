
#if !defined(TABLEBUILDER_HPP_INCLUDED)
#define TABLEBUILDER_HPP_INCLUDED

#include <string>
#include "dbspec.hpp"

class tablebuilder {
public:
	tablebuilder() {}
	virtual ~tablebuilder() {}

	virtual void open_table(const table_spec &spec) = 0;
	virtual void table_complete() = 0;

	virtual void open_row() = 0;
	virtual void row_complete() = 0;

	// pointer must be valid until row_complete() is called
	virtual void add_column(const column_spec &col, const char *value) = 0;

	virtual void add_index(const std::string &column) {}
};

#endif // TABLEBUILDER_HPP_INCLUDED

