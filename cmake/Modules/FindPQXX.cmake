# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.CMake.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindPQXX
-----------

Find the PQXX libraries

IMPORTED targets
^^^^^^^^^^^^^^^^

This module defines the following :prop_tgt:`IMPORTED` target:

``PQXX::PQXX``

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables if found:

``PQXX_INCLUDE_DIRS``
  where to find sqlite3.h, etc.
``PQXX_LIBRARIES``
  the libraries to link against to use PQXX.
``PQXX_VERSION``
  version of the PQXX library found
``PQXX_FOUND``
  TRUE if found

#]=======================================================================]

# Look for the necessary header
find_path(PQXX_INCLUDE_DIR NAMES pqxx/pqxx)
mark_as_advanced(PQXX_INCLUDE_DIR)

# Look for the necessary library
find_library(PQXX_LIBRARY NAMES pqxx)
mark_as_advanced(PQXX_LIBRARY)

# Extract version information from the header file
if(PQXX_INCLUDE_DIR)
    file(STRINGS ${PQXX_INCLUDE_DIR}/pqxx/version.hxx _ver_line
         REGEX "^#define PQXX_VERSION  *\"[0-9]+\\.[0-9]+\\.[0-9]+\""
         LIMIT_COUNT 1)
    string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+"
           PQXX_VERSION "${_ver_line}")
    unset(_ver_line)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(PQXX
    REQUIRED_VARS PQXX_INCLUDE_DIR PQXX_LIBRARY
    VERSION_VAR PQXX_VERSION)

# Create the imported target
if(PQXX_FOUND)
    set(PQXX_INCLUDE_DIRS ${PQXX_INCLUDE_DIR})
    set(PQXX_LIBRARIES ${PQXX_LIBRARY})
    if(NOT TARGET PQXX::PQXX)
        add_library(PQXX::PQXX UNKNOWN IMPORTED)
        set_target_properties(PQXX::PQXX PROPERTIES
            IMPORTED_LOCATION             "${PQXX_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${PQXX_INCLUDE_DIR}")
    endif()
endif()
