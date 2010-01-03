
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
		: csv(), cur_column(0), column_count(a_column_count) {
	csv.open(name);
}

void csvbuilder::beginrow() {
	cur_column = 0;
}

template<typename T>
void csvbuilder::setcolumn_tmpl(int idx, const T &data) {
	assert(cur_column <= idx);
	while (cur_column < idx) {
		csv << ",";
		cur_column++;
	}
	csv << data;
}

void csvbuilder::setcolumn(int idx, const std::string &value) {
	setcolumn_tmpl(idx, csvescape(value));
}

void csvbuilder::setcolumn(int idx, const char *value) {
	if (value)
		setcolumn(idx, std::string(value));
	else
		setcolumn_tmpl(idx, nullstr);
}

void csvbuilder::setcolumn(int idx, int value) {
	setcolumn_tmpl(idx, value);
}

void csvbuilder::setcolumn(int idx, double value) {
	setcolumn_tmpl(idx, value);
}

void csvbuilder::commitrow() {
	while (cur_column+1 < column_count) {
		setcolumn_tmpl(cur_column+1, nullstr);
	}
	csv << "\n";
}

void csvbuilder::committable() {
	csv.close();
}

