
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

objs = env.Object(['xmldb.cpp', 'soimport.cpp', 'dbspec.cpp'])

if GetOption('use_sqlite'):
    print "using sqlite"
    objs.extend(
            env.Object(['sqlitebuilder.cpp'])
        )
    env.Append(CXXFLAGS = ['-DUSE_SQLITE'], LIBS = ['sqlite3'])
if GetOption('use_pgsql'):
    print "using pgsql"
    objs.extend(
            env.Object(['pgbuilder.cpp', 'csvbuilder.cpp'])
        )
    env.Append(CXXFLAGS = ['-DUSE_PGSQL'], LIBS = ['pqxx'])

env.Program('soimport', objs)

