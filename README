Start in a directory containing the stackoverflow data dump xml files.

- 'sqliteimport' will create a sqlite database 'dump.db' containing the data.
- 'pgimport -c "dbname=... ..."' will directly import the data into an existing,
  empty Postgresql database. This is rather slow.
- 'pgcopyimport -c "..."' will create temporary files in /var/tmp containing
  the dump data in suitable format, and than import that data into an existing,
  empty PostreSql database. This is rather fast and therefore the recommended way
  to import the data. Make sure you have ~2 GB of free space in /var/tmp.

With the '-I' switch generating of indexes can be disabled. This makes the import
faster.

