
#if !defined(XMLDB_HPP_INCLUDED)
#define XMLDB_HPP_INCLUDED

#include <expat.h>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// xmltable class

// internal
extern "C"
void wrp_handle_element(void *data, const XML_Char *tag, const XML_Char **attrs);

class xmltable {
protected:
	const std::string name;
	typedef std::vector<std::string> columns_t;
	columns_t columns;

	int count;

public:
	int progress;

	xmltable(const char *a_name, const char * const *a_columns);
	void load();

protected:
	int column_index(const std::string &name);

	virtual void beginrow() = 0;
	virtual void setcolumn(int idx, const char *value) = 0;
	virtual void endrow() = 0;

private:
	friend void wrp_handle_element(void *data, const XML_Char *tag, const XML_Char **attrs);
	void handle_element(const XML_Char *tag, const XML_Char **attrs);
};

#endif // XMLDB_HPP_INCLUDED

