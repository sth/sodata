
CPPFLAGS=-O2 -Wall

default: sqliteimport

xmldb.o: xmldb.cpp xmldb.hpp
	g++ ${CPPFLAGS} -c xmldb.cpp

sqliteify.o: sqliteify.cpp sqliteify.hpp
	g++ ${CPPFLAGS} -c sqliteify.cpp

sqliteimport: sqliteimport.cpp soschema.hpp xmldb.o sqliteify.o
	g++ ${CPPFLAGS} -lexpat -lsqlite3 -o sqliteimport xmldb.o sqliteify.o sqliteimport.cpp

clean:
	rm -f *.o sqliteimport

