
# Basic compilation environment
benv = Environment()
benv.Append(
      CXXFLAGS = ['-Wall', '-O2', '-ggdb'],
      LIBS = ['expat']
   )

# Environment for sqliteimport
senv = benv.Clone()
senv.Append(
      LIBS = ['sqlite3']
   )

# Environment for pgimport
penv = benv.Clone()
penv.Append(
      LIBS = ['pqxx']
   )

common = ['xmldb.cpp', 'soimport.cpp', 'dbspec.cpp']

benv.Program('csvimport', benv.Object(common + ['csvbuilder.cpp', 'csvimport.cpp']))

senv.Program('sqliteimport', benv.Object(common + ['sqlitebuilder.cpp', 'sqliteimport.cpp']))

penv.Program('pgimport', benv.Object(common + ['csvbuilder.cpp', 'pgbuilder.cpp', 'pgimport.cpp']))

benv.Append(TARFLAGS = ['-z'])
benv.Tar('sodata.tar.gz', ['csvimport', 'sqliteimport', 'pgimport'])

