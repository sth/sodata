
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

csvbuilder::csvbuilder(const std::string &a_outdir)
		: outdir(a_outdir), csv(), first_column(true) {
}

std::string csvbuilder::filename(const table_spec &spec) const {
	return outdir + "/" + spec.name + ".pgdata";
}

void csvbuilder::open_table(const table_spec &spec) {
	csv.open(filename(spec).c_str());
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

void csvbuilder::add_column(const column_spec &spec, const char *value) {
	if (value)
		add_column_tmpl(csvescape(value));
	else
		add_column_tmpl(nullstr);
}

