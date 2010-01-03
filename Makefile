
CPPFLAGS=-O2 -Wall -g -DUSE_SQLITE

default: soimport

xmldb.o: xmldb.cpp xmldb.hpp
	g++ ${CPPFLAGS} -c xmldb.cpp

sqlitebuilder.o: sqlitebuilder.cpp sqlitebuilder.hpp tablebuilder.hpp
	g++ ${CPPFLAGS} -c sqlitebuilder.cpp

soimport.o: soimport.cpp soschema.hpp sqlitebuilder.hpp tablebuilder.hpp
	g++ ${CPPFLAGS} -c soimport.cpp

soimport: soimport.o xmldb.o sqlitebuilder.o
	g++ ${CPPFLAGS} -lexpat -lsqlite3 -o soimport xmldb.o sqlitebuilder.o soimport.o

clean:
	rm -f *.o sqliteimport

