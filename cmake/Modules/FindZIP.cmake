# - Try to find LIBZIP
# Once done this will define
#
#  ZIP_FOUND - system has LIBZIP
#  ZIP_INCLUDE_DIRS - the LIBZIP include directory
#  ZIP_LIBRARIES - Link these to use LIBZIP
#  ZIP_DEFINITIONS - Compiler switches required for using LIBZIP
#
#  Copyright (c) 2009 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (ZIP_LIBRARIES AND ZIP_INCLUDE_DIRS)
  # in cache already
  set(ZIP_FOUND TRUE)
else (ZIP_LIBRARIES AND ZIP_INCLUDE_DIRS)

  find_path(ZIP_INCLUDE_DIR
    NAMES
      zip.h
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
        C:/Programme/FWTools2.4.7/include
    PATH_SUFFIXES
        proj4

  )
  mark_as_advanced(ZIP_INCLUDE_DIR)

  find_library(LIBZIP_LIBRARY
    NAMES
        zip
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        C:/Programme/FWTools2.4.7/lib     
  )
  mark_as_advanced(LIBZIP_LIBRARY)

  if (LIBZIP_LIBRARY)
    set(LIBZIP_FOUND TRUE)
  endif (LIBZIP_LIBRARY)

  set(ZIP_INCLUDE_DIRS
    ${ZIP_INCLUDE_DIR}
  )

  if (LIBZIP_FOUND)
    set(ZIP_LIBRARIES
      ${ZIP_LIBRARIES}
      ${LIBZIP_LIBRARY}
    )
  endif (LIBZIP_FOUND)

  if (ZIP_INCLUDE_DIRS AND ZIP_LIBRARIES)
     set(ZIP_FOUND TRUE)
  endif (ZIP_INCLUDE_DIRS AND ZIP_LIBRARIES)

  if (ZIP_FOUND)
    if (NOT ZIP_FIND_QUIETLY)
      message(STATUS "Found LIBZIP: ${ZIP_LIBRARIES}")
    endif (NOT ZIP_FIND_QUIETLY)
  else (ZIP_FOUND)
    if (ZIP_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find LIBZIP")
    endif (ZIP_FIND_REQUIRED)
  endif (ZIP_FOUND)

  # show the ZIP_INCLUDE_DIRS and ZIP_LIBRARIES variables only in the advanced view
  mark_as_advanced(ZIP_INCLUDE_DIRS ZIP_LIBRARIES)

endif (ZIP_LIBRARIES AND ZIP_INCLUDE_DIRS)

