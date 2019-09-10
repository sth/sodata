
CXX=g++
CPPFLAGS=-std=c++11 -O2 -ggdb -Wall -DPQXX_HIDE_EXP_OPTIONAL
HEADS=csvbuilder.hpp pgbuilder.hpp soschema.hpp tablebuilder.hpp dbspec.hpp \
		soimport.hpp sqlitebuilder.hpp xmldb.hpp 

default: sqliteimport pgimport csvimport

%.o: %.cpp ${HEADS}
	${CXX} ${CPPFLAGS} -c $<

SQLITEOBJS=soimport.o dbspec.o xmldb.o sqlitebuilder.o sqliteimport.o
sqliteimport: ${SQLITEOBJS}
	${CXX} ${CPPFLAGS} -o sqliteimport ${SQLITEOBJS} -lexpat -lsqlite3

PGOBJS=soimport.o dbspec.o xmldb.o csvbuilder.o pgbuilder.o pgimport.o
pgimport: ${PGOBJS}
	${CXX} ${CPPFLAGS} -o pgimport ${PGOBJS} -lexpat -lpqxx

CSVOBJS=soimport.o dbspec.o xmldb.o csvbuilder.o csvimport.o
csvimport: ${CSVOBJS}
	${CXX} ${CPPFLAGS} -o csvimport ${CSVOBJS} -lexpat

clean:
	rm -f *.o sqliteimport pgimport csvimport

