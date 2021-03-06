# Find libdsdcc

if(NOT LIBDSDCC_FOUND)

  pkg_check_modules (LIBDSDCC_PKG libdsdcc)
  find_path(LIBDSDCC_INCLUDE_DIR NAMES dsd_decoder.h
    PATHS
    ${LIBDSDCC_PKG_INCLUDE_DIRS}
    /usr/include
    /usr/local/include
  )

  find_library(LIBDSDCC_LIBRARIES NAMES dsdcc
    PATHS
    ${LIBDSDCC_PKG_LIBRARY_DIRS}
    /usr/lib
    /usr/local/lib
  )

  if(LIBDSDCC_INCLUDE_DIR AND LIBDSDCC_LIBRARIES)
    set(LIBDSDCC_FOUND TRUE CACHE INTERNAL "libdsdcc found")
    message(STATUS "Found libdsdcc: ${LIBDSDCC_INCLUDE_DIR}, ${LIBDSDCC_LIBRARIES}")
  else(LIBDSDCC_INCLUDE_DIR AND LIBDSDCC_LIBRARIES)
    set(LIBDSDCC_FOUND FALSE CACHE INTERNAL "libdsdcc found")
    message(STATUS "libdsdcc not found.")
  endif(LIBDSDCC_INCLUDE_DIR AND LIBDSDCC_LIBRARIES)

  mark_as_advanced(LIBDSDCC_INCLUDE_DIR LIBDSDCC_LIBRARIES)

endif(NOT LIBDSDCC_FOUND)