cmake_minimum_required(VERSION 3.13)
project(SoData VERSION 1.5 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(EXPAT REQUIRED)

add_library(SoDataCommon xmldb.cpp soimport.cpp dbspec.cpp)

add_executable(csvimport csvbuilder.cpp csvimport.cpp)
target_compile_features(csvimport PUBLIC cxx_std_11)
target_link_libraries(csvimport PRIVATE SoDataCommon)
target_link_libraries(csvimport PRIVATE EXPAT::EXPAT)

find_package(SQLite3)
if (SQLite3_FOUND)
	add_executable(sqliteimport sqlitebuilder.cpp sqliteimport.cpp)
	target_compile_features(sqliteimport PUBLIC cxx_std_11)
	target_link_libraries(sqliteimport PRIVATE SoDataCommon)
	target_link_libraries(sqliteimport PRIVATE SQLite::SQLite3)
	target_link_libraries(sqliteimport PRIVATE EXPAT::EXPAT)
endif (SQLite3_FOUND)

find_package(PQXX)
if (PQXX_FOUND)
	add_executable(pgimport csvbuilder.cpp pgbuilder.cpp pgimport.cpp)
	target_compile_features(pgimport PUBLIC cxx_std_11)
	target_compile_definitions(pgimport PUBLIC PQXX_HIDE_EXP_OPTIONAL=1)
	target_link_libraries(pgimport PRIVATE SoDataCommon)
	target_link_libraries(pgimport PRIVATE PQXX::PQXX)
	target_link_libraries(pgimport PRIVATE EXPAT::EXPAT)
endif (PQXX_FOUND)
