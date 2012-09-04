# Copyright (C) 2012 LuaDist.
# Created by Peter Kapec <kapecp@gmail.com>
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
#	Note:
#		Searching headers and libraries is very simple and is NOT as powerful as scripts
#		distributed with CMake, because LuaDist defines directories to search for.
#		Everyone is encouraged to contact the author with improvements. Maybe this file
#		becomes part of CMake distribution sometimes.

# - Find CD
# Find the native CD headers and libraries.
#
# CD_INCLUDE_DIRS	- where to find im.h.
# CD_LIBRARIES	- List of libraries when using im.
# CD_FOUND	- True if CD found.

# Look for the header file.
FIND_PATH( CD_INCLUDE_DIR NAMES cd.h )

# Look for the library.
FIND_LIBRARY( CD_LIBRARY NAMES cd )

FIND_LIBRARY( CD_LIBRARY_LUA NAMES cdlua )

FIND_LIBRARY( CD_LIBRARY NAMES cd)
FIND_LIBRARY( CDPDF_LIBRARY NAMES cdpdf)
FIND_LIBRARY( CDCONTEXTPLUS_LIBRARY NAMES cdcontextplus)
FIND_LIBRARY( CDCAIRO_LIBRARY NAMES cdcairo)
FIND_LIBRARY( CDGL_LIBRARY NAMES cdgl)

# Handle the QUIETLY and REQUIRED arguments and set CD_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CD DEFAULT_MSG CD_LIBRARY CD_INCLUDE_DIR)

# Copy the results to the output variables.
IF(CD_FOUND)
	SET(CD_LIBRARIES ${CD_LIBRARY})
	SET(CD_INCLUDE_DIRS ${CD_INCLUDE_DIR})
ENDIF(CD_FOUND)

MARK_AS_ADVANCED(CD_INCLUDE_DIRS CD_LIBRARIES)
