
env = Environment()
env.Append(CXXFLAGS = Split('-Wall -O2'), LIBS = ['expat'])

commonobjs = env.Object(['xmldb.cpp', 'soimport.cpp', 'dbspec.cpp'])

sqliteenv = env.Clone()
sqliteenv.Append(LIBS = ['sqlite3'])
sqliteobjs = sqliteenv.Object(['sqlitebuilder.cpp', 'sqliteimport.cpp'])

sqliteenv.Program('sqliteimport', commonobjs + sqliteobjs)

pgenv = env.Clone()
pgenv.Append(LIBS = ['pqxx'])
pgobjs = pgenv.Object(['csvbuilder.cpp', 'pgbuilder.cpp'])

pgiobjs = pgenv.Object('pgimport.cpp')
pgenv.Program('pgimport', commonobjs + pgobjs + pgiobjs)

env.Append(TARFLAGS = ['-z'])
env.Tar('soimport.tar.gz', ['sqliteimport', 'pgimport'])

