
// (c) 2009, 2010  Stephan Hohe

#include "dbspec.hpp"

std::vector<column_spec> table_spec::columns() const {
	std::vector<column_spec> result;
	for (size_t i=0; column_defs[i].name; i++)
		result.push_back(column_defs[i]);
	return result;
}

std::vector<std::string> table_spec::column_names() const {
	std::vector<std::string> result;
	for (size_t i=0; column_defs[i].name; i++)
		result.push_back(column_defs[i].name);
	return result;
}

