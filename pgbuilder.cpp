
#include "pgbuilder.hpp"
#include <pqxx/transaction.hxx>
#include <iostream>

/*
typedef pqxx::transaction<> transaction_t;
typedef pqxx::transactor<transaction_t> transactor_t;

class table_transactor : public transactor_t {
protected:
	const std::string table;

public:
	tabletransactor(const std::string &a_table) : table(a_table) {
	}
};

class drop_table : public table_transactor {
public:
	table_copy(const std::string &a_table) : table_transactor(a_table) {
	}

	void operator()(transaction_t &tr) {
		std::string sql = "DROP TABLE " + table + ";";
		tr.exec(sql);
	}
};

class copy_table : public table_transactor {
public:
	copy_table(const std::string &a_table) : table_transactor(a_table) {
	}

	void operator()(transaction_t &tr) {
		std::string sql = "CREATE TABLE " + table + ";";
		sql += "COPY " + table " FROM '" + table + ".pgdata' WITH DELIMITER AS ',';";
		tr.exec(sql);
	}
};

class add_index : public table_transactor {
private:
	std::string column;

public:
	add_index(const std::string &a_table, const std::string &a_column)
			: table_transactor(a_table), column(a_column) {
	}

	void operator()(transaction_t &tr) {
		std::string indexname = table + "_" + column;
		std::string sql = "CREATE INDEX " + indexname + " ON " + table + " (" + column + ");";
		tr.exec(sql);
	}
};
*/

pgbuilder::pgbuilder(pqxx::connection *a_db, const char *a_name, const columns_t &a_columns)
		: csvbuilder((std::string("/var/tmp/") + a_name + ".pgdata").c_str(), a_columns.size()),
		  db(a_db), name(a_name), columns(a_columns) {

	try {
		pqxx::work w(*db);
		w.exec("DROP TABLE " + name + ";");
		w.commit();
	}
	catch (const pqxx::undefined_table &) {
		// Ignore if this fails (for example table didn't exist)
	}

	std::string create = "CREATE TABLE " + name + " (";
	for (columns_t::iterator it = columns.begin(); it != columns.end(); ++it) {
		create += std::string(it->name) + " ";
		switch (it->type) {
		case CT_VCHR64:
			create += "varchar(64)";
			break;
		case CT_TEXT:
			create += "text";
			break;
		case CT_INT:
		case CT_DATE:
			create += "int";
			break;
		default:
			throw std::runtime_error("Unsupported column type");
		}
		create += ", ";
	}
	create.erase(create.size() - 2);
	create += ");";
	pqxx::work w(*db);
	w.exec(create);
	w.commit();
}

void pgbuilder::committable() {
	csvbuilder::committable();
	std::cout << "  (pushing to db...)" << std::endl;
	pqxx::work w(*db);
	w.exec("COPY " + name + " FROM '/var/tmp/" + name + ".pgdata' WITH DELIMITER AS ',';");
	w.commit();
	unlink((std::string("/var/tmp/") + name + ".pgdata").c_str());
}

void pgbuilder::add_index(const std::string &column) {
	pqxx::work w(*db);
	w.exec("CREATE INDEX " + name + "_" + column + " ON " + name + " (" + column + ");");
	w.commit();
}

