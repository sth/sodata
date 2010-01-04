
#include "csvbuilder.hpp"
#include <cassert>

const std::string nullstr("\\N");

std::string csvescape(std::string value) {
	size_t pos = 0;
	while ((pos = value.find_first_of("\\\n\r,", pos)) != value.npos) {
		value.insert(pos, 1, '\\');
		pos += 2;
	}
	return value;
}

csvbuilder::csvbuilder(const char *name, int a_column_count)
		: csv(), first_column(true) {

	csv.open(name);
}

void csvbuilder::open_table() {
}

void csvbuilder::table_complete() {
	csv.close();
}

void csvbuilder::open_row() {
	first_column = true;
}

void csvbuilder::row_complete() {
	csv << "\n";
}


template<typename T>
void csvbuilder::add_column_tmpl(const T &data) {
	if (first_column)
		first_column = false;
	else
		csv << ",";
	csv << data;
}

void csvbuilder::add_column(const std::string &value) {
	add_column_tmpl(csvescape(value));
}

void csvbuilder::add_column(const char *value) {
	if (value)
		add_column(std::string(value));
	else
		add_column_tmpl(nullstr);
}

void csvbuilder::add_column(int value) {
	add_column_tmpl(value);
}

void csvbuilder::add_column(double value) {
	add_column_tmpl(value);
}

