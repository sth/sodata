
// (c) 2009, 2010  Stephan Hohe

#include "dbspec.hpp"
#include <stdexcept>

std::vector<std::string> table_spec::column_names() const {
	std::vector<std::string> result;
	for (auto &column : columns)
		result.push_back(column.name);
	return result;
}

size_t table_spec::column_index(const std::string &name) const {
	for (size_t i=0; i < columns.size(); i++) {
		if (columns[i].name == name)
			return i;
	}
	throw std::runtime_error("unknown column name: '" + name + "'");
}

