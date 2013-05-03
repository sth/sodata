# Stack Overflow / Stack Exchange data dump import tool

## Stack Exchange Data Dumps

The user generated content on Stack Overflow and the whole Stack Exchange
network is licensed under the Creative Commons [CC BY-SA 3.0][1] license. There
are regular (more or less) data dumps provided in XML format that contain
all the questions/answers/comments/vote counts/... of the Stack Exchange
sites.

This sodata tool creates a real PostgreSQL or Sqlite3 database from the
XML files.

For more information about the data dumps and where to download them, see:

- [Original Stack Overflow data dump announcement][2]
- [Torrents with all the data on Clear Bits][3] 
- [Other Stack Exchange blog posts anout the data dump][4]

If you just want to play around with the database a little without all the effort
of downloading GBs of data and importing them into your own database, you might
also want to look at the online [Stack Exchange Data Explorer][dbexplore].

## Usage

This code can be compiled to generate three import tools:

- `sqliteimport`, which imports the XML files into a Sqlite3 database
- `pgimport`, which imports the XML files into a PostgreSQL database
- `csvimport`, which converts the XML files into CSV files

Generally it is recommended to use a new, empty database to import into, but
the tools don't really care if there are existing tables, as long as there are
no name conflicts.

All the tools expect to be started in a directory that contains the extracted
data dump XML files which should be imported.

### sqliteimport

Creates a Sqlite3 database. By default it creates a new database file called
`dump.db`, but this can be changed with the `-f` flag:

    sqliteimport -f myimport.db

### pgimport

Expects a existing and preferable empty PostgreSQL database and fills it with
data. You should specify connect options with the `-c` flag, like this:

    pgimport -c "host=localhost dbname=so user=soimporter password=abc"

By default, this will use a temporary file and SQL `COPY` to import the data.
This means that you'll need enough free diskspace to store a temporary copy
of the data. Also the importing database user will need to have superuser
privileges in the database to be allowed to use `COPY`.

The directory used for the temporary data can be specified with the `-d` option:

    pgimport -d tmpdir -c ...

There has to be enough free diskspace to store a temporary copy of each table in
that directory. Having this on a different hard disk than the database or the
input files will speed up the import process.

The import can also be done without temporary files and without requiring a
database superuser. For this use the `-s` flag:

    pgimport -s -c ...

### csvimport

Generates CSV (Comma separated value) files form the XML data. The `-d` parameter
can be used to specify a target directory, by default the current working
directory is used.

The resulting CSV files use escaping as understood by PostgreSQL. This means
that the characters `\\` (backslash), `\n` (newline), `\r` (aarrige retrun),
and `,` (comma) will be escaped by a preceding backslash when they occur in
a text field. NULL values will be represented by `\\N` (a backslash character
followed by a capital N).

### Common options

With the `-I` switch generating of indexes can be disabled. This makes the import
faster.

## Created Database

The import tools create tables with the same names as the XML files, so they
will be called `posts`, `comments`, `users` and so on. The column names in
those tables correspond to the attribute names in the XML. For more detail
see [`soschema.hpp`][schema], which defines the tables and columns that get
imported.

## Compiling

All programs use `libexpat` to parse the input files, it has to be available.

To compile `sqliteimport` you'll need to have libsqlite3 installed, including
it's header files.

To compile `pgimport` you'll need to have `libpqxx` installed, including it's
header files.

To build all tools, simply type:

    make

You can also just compile the tools individually:

    make sqliteimport
    make pgimport
    make csvimport

 [1]: http://creativecommons.org/licenses/by-sa/3.0/
 [2]: http://blog.stackoverflow.com/2009/06/stack-overflow-creative-commons-data-dump/
 [3]: http://www.clearbits.net/creators/146-stack-exchange-data-dump
 [4]: http://blog.stackexchange.com/category/cc-wiki-dump/
 [dbexplore]: http://data.stackexchange.com/
 [schema]: https://github.com/sth/sodata/blob/master/soschema.hpp

