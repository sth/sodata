
#if !defined(TABLEBUILDER_HPP_INCLUDED)
#define TABLEBUILDER_HPP_INCLUDED

#include <string>

class tablebuilder {
public:
	tablebuilder() {}
	virtual ~tablebuilder() {}

	//int column_index(const std::string &name);
	virtual void begin_transaction() {}
	virtual void end_transaction() {}

	virtual void beginrow() {}

	virtual void setcolumn(int idx, const char *value) = 0;
	virtual void setcolumn(int idx, const std::string &value) = 0;
	virtual void setcolumn(int idx, int value) = 0;
	virtual void setcolumn(int idx, double value) = 0;

	virtual void commitrow() = 0;
	virtual void committable() {}

	virtual void add_index(const std::string &column) {}
};

#endif // TABLEBUILDER_HPP_INCLUDED

