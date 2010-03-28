
#if !defined(CSVBUILDER_HPP_INCLUDED)
#define CSVBUILDER_HPP_INCLUDED

#include "tablebuilder.hpp"
#include <string>
#include <fstream>

class csvbuilder : public tablebuilder {
private:
	std::string outdir;
	std::ofstream csv;
	bool first_column;

public:
	csvbuilder(const std::string &a_outdir);

	virtual void open_table(const table_spec &a_spec);
	virtual void table_complete();
	virtual void open_row();
	virtual void row_complete();
	virtual void add_column(const column_spec &spec, const char *value);

protected:
	std::string filename(const table_spec &spec) const;

private:
	template<typename T>
	void add_column_tmpl(const T &data);
};

#endif // CSVBUILDER_HPP_INCLUDED


