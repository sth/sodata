
CXX=g++
CPPFLAGS=-O2 -Wall
HEADS=csvbuilder.hpp pgbuilder.hpp soschema.hpp tablebuilder.hpp dbspec.hpp \
		soimport.hpp sqlitebuilder.hpp xmldb.hpp 

default: sqliteimport pgimport pgcopyimport

%.o: %.cpp ${HEADS}
	${CXX} ${CPPFLAGS} -c $<

SQLITEOBJS=soimport.o dbspec.o xmldb.o sqlitebuilder.o sqliteimport.o
sqliteimport: ${SQLITEOBJS}
	${CXX} ${CPPFLAGS} -lexpat -lsqlite3 -o sqliteimport ${SQLITEOBJS}

PGOBJS=soimport.o dbspec.o xmldb.o csvbuilder.o pgbuilder.o pgimport.o
pgimport: ${PGOBJS}
	${CXX} ${CPPFLAGS} -lexpat -lpqxx -o pgimport ${PGOBJS}

PGCOBJS=soimport.o dbspec.o xmldb.o csvbuilder.o pgbuilder.o pgcopyimport.o
pgcopyimport: ${PGCOBJS}
	${CXX} ${CPPFLAGS} -lexpat -lpqxx -o pgcopyimport ${PGCOBJS}

clean:
	rm -f *.o sqliteimport pgimport pgcopyimport

