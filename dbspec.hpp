
// (c) 2009, 2010  Stephan Hohe

#if !defined(DBSPEC_HPP_INCLUDED)
#define DBSPEC_HPP_INCLUDED

#include <string>
#include <vector>

enum column_type_t {
	CT_VCHR12,
	CT_VCHR64,
	CT_TEXT,
	CT_INT,
	CT_DATE,
	CT_BOOL,
};

struct column_spec {
	const char *name;
	column_type_t type;
};

struct table_spec {
	const char *name;
	const std::vector<column_spec> columns;
	std::vector<std::string> column_names() const;
	size_t column_index(const std::string &name) const;
};

#endif // DBSPEC_HPP_INCLUDED

