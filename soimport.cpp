
#include "soimport.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>

// ---------------------------------------------------------------------------
// helper functions

time_t parsedate(const char *value) {
	if (!value) {
		std::cerr << "warning: date parse failed for NULL" << std::endl;
		return 0;
	}
	tm t = {0};
	// try 2009-07-12T22:51:42.563
	const char *rem = strptime(value, "%Y-%m-%dT%H:%M:%S", &t);
	if (!rem) {
		// try 2009-07-12
		rem = strptime(value, "%Y-%m-%d", &t);
		if (!rem) {
			std::cerr << "warning: date parse failed for '" << value << "'" << std::endl;
			return 0;
		}
		else if (*rem != '\0') {
			// error
			std::cerr << "warning: date parse has unknown trailing data for '" << value << "'" << std::endl;
			return 0;
		}
	}
	else if (*rem != '.') {
		// error
		std::cerr << "warning: date parse has unknown trailing data for '" << value << "'" << std::endl;
		return 0;
	}
	return timegm(&t);
}

// ---------------------------------------------------------------------------
// soloader class


soloader::soloader(tablebuilder &a_builder, const table_spec &a_table)
		: xmltable(a_table.name, a_table.column_names()),
		  builder(a_builder), table(a_table) {
}

void soloader::load() {
	std::cout << "loading " << name << std::endl;
	builder.begin_transaction();
	xmltable::load();
	builder.committable();
	builder.end_transaction();
}

void soloader::beginrow() {
	if (count % 100000 == 0) {
		builder.end_transaction();
		builder.begin_transaction();
	}
	builder.beginrow();
}

void soloader::setcolumn(int idx, const char *value) {
	if (!value) {
		// NULL
		builder.setcolumn(idx, value);
		return;
	}
	switch (table.column_defs[idx].type) {
	case CT_VCHR64:
	case CT_TEXT:
		builder.setcolumn(idx, value);
		break;
	case CT_INT:
		builder.setcolumn(idx, atoi(value));
		break;
	case CT_DATE:
		builder.setcolumn(idx, static_cast<int>(parsedate(value)));
		break;
	default:
		throw std::logic_error("invalid column type");
		break;
	}
}

void soloader::endrow() {
	builder.commitrow();
}

void soloader::add_indexes() {
	const column_spec *columns = table.column_defs;
	for (size_t idx=0; columns[idx].name; idx++) {
		switch (columns[idx].type) {
		case CT_INT:
		case CT_DATE:
			builder.add_index(columns[idx].name);
		default:
			break;
		}
	}
}

