# - Try to find GlibmmUtils-2.4
# Once done, this will define
#
#  GlibmmUtils_FOUND - system has GlibmmUtils
#  GlibmmUtils_INCLUDE_DIRS - the GlibmmUtils include directories
#  GlibmmUtils_LIBRARIES - link these to use GlibmmUtils

include(LibFindMacros)

# Dependencies
libfind_package(GlibmmUtils Glib)
libfind_package(GlibmmUtils SigC++)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(GlibmmUtils_PKGCONF glibmm-utils)

# Main include dir
find_path(GlibmmUtils_INCLUDE_DIR
  NAMES glibmm-utils/glibmm-utils.h
  PATHS ${GlibmmUtils_PKGCONF_INCLUDE_DIRS}
  PATH_SUFFIXES glibmm-utils-1.0
)

find_library(GlibmmUtils_LIBRARY
  NAMES glibmm-utils
  PATHS ${GlibmmUtils_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(GlibmmUtils_PROCESS_INCLUDES GlibmmUtils_INCLUDE_DIR GlibmmUtilsConfig_INCLUDE_DIR Glib_INCLUDE_DIRS SigC++_INCLUDE_DIRS)
set(GlibmmUtils_PROCESS_LIBS GlibmmUtils_LIBRARY Glib_LIBRARIES SigC++_LIBRARIES)
libfind_process(GlibmmUtils)

