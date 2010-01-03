
AddOption('--sqlite', action='store_true', dest='use_sqlite', default=True,
        help="Add sqlite support [default]")
AddOption('--no-sqlite', action='store_false', dest='use_sqlite',
        help="Don't add sqlite support")
AddOption('--pgsql', action='store_true', dest='use_pgsql', default=False,
        help="Add postgresql support")
AddOption('--no-pgsql', action='store_false', dest='use_sqlite',
        help="Don't add postresql support [default]")


env = Environment()
env.Append(CXXFLAGS = Split('-Wall -g -O2'), LIBS = ['expat'])

commonobjs = env.Object(['xmldb.cpp', 'soimport.cpp', 'dbspec.cpp'])

sqliteenv = env.Clone()
sqliteenv.Append(LIBS = ['sqlite3'])
sqliteobjs = sqliteenv.Object(['sqlitebuilder.cpp', 'sqliteimport.cpp'])

pgenv = env.Clone()
pgenv.Append(LIBS = ['pqxx'])
pgobjs = pgenv.Object(['csvbuilder.cpp', 'pgbuilder.cpp', 'pgimport.cpp'])

sqliteenv.Program('sqliteimport', commonobjs + sqliteobjs)
pgenv.Program('pgimport', commonobjs + pgobjs)

