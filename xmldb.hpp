
#if !defined(XMLDB_HPP_INCLUDED)
#define XMLDB_HPP_INCLUDED

#include "dbspec.hpp"
#include <expat.h>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// xmltable class

// internal
extern "C"
void wrp_handle_element(void *data, const XML_Char *tag, const XML_Char **attrs);

class xmltable {
protected:
	const std::string name;
	typedef std::vector<column_spec> columns_t;
	columns_t columns;

	int count;

public:
	int progress;

	xmltable(const char *a_name, const std::vector<column_spec> &a_columns);
	void load();

protected:
	int column_index(const std::string &name);

	virtual void open_row() = 0;
	virtual void add_column(const column_spec &col, const char *value) = 0;
	virtual void row_complete() = 0;

private:
	friend void wrp_handle_element(void *data, const XML_Char *tag, const XML_Char **attrs);
	void handle_element(const XML_Char *tag, const XML_Char **attrs);
};

#endif // XMLDB_HPP_INCLUDED

