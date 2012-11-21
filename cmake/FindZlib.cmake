# Copyright (C) 2007-2012 LuaDist.
# Created by Peter Drahos <drahosp@gmail.com>
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
#	Note:
#		Searching headers and libraries is very simple and is NOT as powerful as scripts
#		distributed with CMake, because LuaDist defines directories to search for.
#		Everyone is encouraged to contact the author with improvements. Maybe this file
#		becomes part of CMake distribution sometimes.

# - Find zlib
# Find the native zlib headers and libraries.
#
# ZLIB_INCLUDE_DIRS	- where to find z/z.h, etc.
# ZLIB_LIBRARIES	- List of libraries when using zlib.
# ZLIB_FOUND	- True if zlib found.

# Look for the header file.
FIND_PATH(ZLIB_INCLUDE_DIR NAMES zlib.h)

# Look for the library.
FIND_LIBRARY(ZLIB_LIBRARY NAMES zlib z)

# Handle the QUIETLY and REQUIRED arguments and set ZLIB_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(zlib DEFAULT_MSG ZLIB_LIBRARY ZLIB_INCLUDE_DIR)

# Copy the results to the output variables.
IF(ZLIB_FOUND)
	SET(ZLIB_LIBRARIES ${ZLIB_LIBRARY})
	SET(ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIR})
ELSE(ZLIB_FOUND)
	SET(ZLIB_LIBRARIES)
	SET(ZLIB_INCLUDE_DIRS)
ENDIF(ZLIB_FOUND)

MARK_AS_ADVANCED(ZLIB_INCLUDE_DIRS ZLIB_LIBRARIES)
