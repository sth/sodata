
#include "xmldb.hpp"
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <cstring>

// ---------------------------------------------------------------------------
// xmltable class

#define BLOCKSIZE (4*1024*1024)

xmltable::xmltable(const char *a_name, const std::vector<column_spec> &a_columns)
		: name(a_name), columns(a_columns), count(0), progress(100000) {
}

void xmltable::load() {
	XML_Parser parser = XML_ParserCreate("UTF-8");
	if (!parser)
		throw std::runtime_error("Can't create parser");
	XML_SetUserData(parser, this);
	XML_SetElementHandler(parser, &wrp_handle_element, 0);

	std::ifstream f((name + ".xml").c_str());
	char buffer[BLOCKSIZE];
	do {
		f.read(buffer, BLOCKSIZE);
		XML_Parse(parser, buffer, f.gcount(), (f ? 0 : 1));
	} while (f);

	XML_ParserFree(parser);
	std::cout << "  (got " << count << " datasets)" << std::endl;
}

void xmltable::handle_element(const XML_Char *tag, const XML_Char **attrs) {
	if (std::strcmp(tag, "row") != 0)
		return;
	count++;
	if (count % progress == 0)
		std::cout << "  (... " << count << " datasets)" << std::endl;

	open_row();
	size_t idx = 0;
	for (const XML_Char **a = attrs; *a; a+=2) {
		if (idx >= columns.size()) {
			std::cerr << "unknown column in " << name << ": " << *a << std::endl;
			return;
		}
		while (strcmp(*a, columns[idx].name) != 0) {
			add_column(columns[idx++], NULL);
			if (idx >= columns.size()) {
				std::cerr << "unknown column in " << name << ": " << *a << std::endl;
				return;
			}
		}
		add_column(columns[idx++], *(a+1));
	}
	while (idx < columns.size()) {
		add_column(columns[idx++], NULL);
	}
	row_complete();
}

int xmltable::column_index(const std::string &name) {
	for (size_t i=0; i<columns.size(); i++) {
		if (columns[i].name == name)
			return i;
	}
	throw std::logic_error("invalid column name: " + name);
}

extern "C"
void wrp_handle_element(void *data, const XML_Char *tag, const XML_Char **attrs) {
	reinterpret_cast<xmltable*>(data)->handle_element(tag, attrs);
}

