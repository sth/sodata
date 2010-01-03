
#if !defined(CSVBUILDER_HPP_INCLUDED)
#define CSVBUILDER_HPP_INCLUDED

#include "tablebuilder.hpp"
#include <string>
#include <fstream>

class csvbuilder : public tablebuilder {
private:
	std::ofstream csv;
	int cur_column;
	int column_count;

public:
	csvbuilder(const char *name, const int column_count);

	void beginrow();
	void setcolumn(int idx, const char *value);
	void setcolumn(int idx, const std::string &value);
	void setcolumn(int idx, int value);
	void setcolumn(int idx, double value);

	void commitrow();
	void committable();

private:
	template<typename T>
	void setcolumn_tmpl(int idx, const T &data);
};

#endif // SQLITEIFY_HPP_INCLUDED


