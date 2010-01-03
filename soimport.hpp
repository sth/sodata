
#if !defined(SOIMPORT_HPP_INCLUDED)
#define SOIMPORT_HPP_INCLUDED

#include "xmldb.hpp"
#include "tablebuilder.hpp"
#include "dbspec.hpp"

class soloader : private xmltable {
private:
	tablebuilder &builder;
	table_spec table;

public:
	soloader(tablebuilder &a_builder, const table_spec &a_table);
	void load();
	void add_indexes();

protected:
	virtual void beginrow();
	virtual void setcolumn(int idx, const char *value);
	virtual void endrow();
};

#endif // SOIMPORT_HPP_INCLUDED

